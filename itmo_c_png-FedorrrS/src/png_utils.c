//
// Created by selischev on 16.04.23.
//
#include "../include/png_utils.h"

uint byte_to_int(const byte *src, const int offset, const int n)
{
	uint res = 0;
	for (int i = offset; i < offset + n; ++i)
	{
		res = (res << 8) | src[i];
	}
	return res;
}

bool is_expected_chunk(const byte *cur, const char *expected)
{
	return !memcmp(cur, expected, 4);
}

uint get_chunk_length(FILE *png, exit_r *result)
{
	byte length[4];
	if (fread(length, sizeof(byte), 4, png) != 4)
	{
		result->code = ERROR_DATA_INVALID;
		result->msg = MSG_INVALID_DATA;
		return result->code;
	}
	return byte_to_int(length, 0, 4);
}

void get_chunk_type(FILE *in, byte *chunk_type, exit_r *result)
{
	if (fread(chunk_type, sizeof(byte), 4, in) != 4)
	{
		result->code = ERROR_DATA_INVALID;
		result->msg = MSG_INVALID_DATA;
	}
}

bool is_png(FILE *input_file)
{
	byte valid_png_signature[] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };
	byte signature[8];
	return fread(signature, sizeof(byte), 8, input_file) == 8 && !memcmp(signature, valid_png_signature, 8);
}

void parse_IHDR(FILE *png, PNM *pnm, exit_r *result)
{
	byte buff[17];
	if (fread(buff, sizeof(byte), 17, png) != 17)
	{
		result->code = ERROR_DATA_INVALID;
		result->msg = MSG_INVALID_DATA;
		return;
	}
	pnm->w = byte_to_int(buff, 0, 4);
	pnm->h = byte_to_int(buff, 4, 4);
	/* color type */
	switch (buff[9])
	{
	case 0:
		pnm->type = "P5";
		pnm->bpp = 1;
		break;
	case 2:
	case 3:
		pnm->type = "P6";
		pnm->bpp = 3;
		break;
	default:
		result->code = ERROR_DATA_INVALID;
		result->msg = MSG_INVALID_DATA;
		return;
	}
	/* bit depth should be 8;
	 * compression and filter methods */
	if (buff[8] != 8 || buff[10] != 0 || buff[11] != 0)
	{
		result->code = ERROR_DATA_INVALID;
		result->msg = MSG_INVALID_DATA;
	}
	/* interlace method Adam7 (= 1) not implemented */
	if (buff[12] == 1)
	{
		result->code = ERROR_UNSUPPORTED;
		result->msg = MSG_UNSUPPORTED;
	}
	/* buff[13..16] are skipped CRC */
}

void upd_extracted(comp_data *extracted, uint offset, size_t size)
{
	extracted->offset = offset;
	extracted->size += size;
}

void parse_IDAT(size_t length, FILE *in, comp_data *extracted, exit_r *result)
{
	/* if it's first IDAT allocate mem */
	if (!extracted->size)
	{
		upd_extracted(extracted, 0, length);
		extracted->data = malloc(sizeof(byte) * length);
		if (!extracted->data)
		{
			result->code = ERROR_DATA_INVALID;
			result->msg = MSG_NOT_ENOUGH_MEMORY;
			return;
		}
	}
	/* else reallocate mem */
	else
	{
		upd_extracted(extracted, extracted->size, length);
		void *reallocated_mem = realloc(extracted->data, extracted->size);
		if (!reallocated_mem)
		{
			result->code = ERROR_OUT_OF_MEMORY;
			result->msg = MSG_NOT_ENOUGH_MEMORY;
			return;
		}
	}
	byte *data = malloc(sizeof(byte) * length);
	if (!data)
	{
		result->code = ERROR_OUT_OF_MEMORY;
		result->msg = MSG_NOT_ENOUGH_MEMORY;
		goto free_extracted;
	}
	if (fread(data, sizeof(byte), length, in) != length)
	{
		result->code = ERROR_DATA_INVALID;
		result->msg = MSG_INVALID_DATA;
		goto free_extracted;
	}
	for (int i = 0; i < length; ++i)
	{
		extracted->data[i + extracted->offset] = data[i];
	}
	free(data);
	return;
free_extracted:
	free(extracted->data);
}

const filter_func FILTER_FUNCTIONS[] = {
	apply_none, apply_sub, apply_up, apply_average, apply_paeth,
};

void apply_filter(PNM *pnm, exit_r *result)
{
	const uint line = pnm->w * pnm->bpp + 1;
	for (size_t i = 0; i < pnm->h * line; i += line)
	{
		uint filter_type = pnm->data[i];
		if (filter_type >= sizeof(FILTER_FUNCTIONS) / sizeof(filter_func))
		{
			result->code = ERROR_DATA_INVALID;
			result->msg = MSG_INVALID_DATA;
			return;
		}
		FILTER_FUNCTIONS[filter_type](i, pnm);
	}
}

void apply_none(size_t offset, PNM *pnm) {}
void apply_sub(size_t offset, PNM *pnm)
{
	/* Sub(x) + Raw(x - bpp) */
	for (size_t i = pnm->bpp + 1 + offset; i < pnm->w * pnm->bpp + 1 + offset; ++i)
	{
		pnm->data[i] += pnm->data[i - pnm->bpp];
	}
}

void apply_up(size_t offset, PNM *pnm)
{
	/* Up(x) + Prior(x)
	 * NOTE: Prior means previous */
	const uint line = pnm->w * pnm->bpp + 1;
	for (size_t i = 1 + offset; i < line + offset; ++i)
	{
		pnm->data[i] += pnm->data[i - line];
	}
}

void apply_average(size_t offset, PNM *pnm)
{
	/* Average(x) + floor((Raw(x - bpp) + Prior(x)) / 2)
	 * NOTE: no need to use floor because unsigned char is an integer type */
	const uint line = pnm->w * pnm->bpp + 1;
	for (size_t i = 1 + offset; i < pnm->bpp + offset + 1; ++i)
	{
		/* first scanline assuming Prior(x) = 0 ∀x */
		pnm->data[i] += pnm->data[i - line] / 2;
	}
	for (size_t i = 1 + pnm->bpp + offset; i < line + offset; ++i)
	{
		pnm->data[i] += (pnm->data[i - pnm->bpp] + pnm->data[i - line]) / 2;
	}
}

byte paeth_predictor(byte a, byte b, byte c)
{
	int p = a + b - c;
	int pa = abs(p - a);
	int pb = abs(p - b);
	int pc = abs(p - c);
	if (pa <= pb && pa <= pc)
	{
		return a;
	}
	else if (pb <= pc)
	{
		return b;
	}
	else
	{
		return c;
	}
}

void apply_paeth(size_t offset, PNM *pnm)
{
	/* Paeth(x) + PaethPredictor(Raw(x - bpp), Prior(x), Prior(x - bpp)) */
	const uint line = pnm->w * pnm->bpp + 1;
	for (size_t i = 1 + offset; i < pnm->bpp + offset + 1; ++i)
	{
		/* first scanline assuming Prior(x) = 0 ∀x */
		pnm->data[i] += paeth_predictor(pnm->data[i - pnm->bpp], pnm->data[i - line], 0);
	}

	for (size_t i = 1 + pnm->bpp + offset; i < line + offset; ++i)
	{
		pnm->data[i] += paeth_predictor(pnm->data[i - pnm->bpp], pnm->data[i - line], pnm->data[i - line - pnm->bpp]);
	}
}

void decompress(comp_data *extracted, PNM *pnm, exit_r *result)
{
	const uint line = pnm->w * pnm->bpp + 1;
	pnm->data = malloc(sizeof(byte *) * pnm->h * line);
	if (!pnm->data)
	{
		result->code = ERROR_OUT_OF_MEMORY;
		result->msg = MSG_NOT_ENOUGH_MEMORY;
		goto extracted_free;
	}
#ifdef ZLIB
	uLong size = pnm->h * line;
	switch (uncompress(pnm->data, &size, extracted->data, extracted->size))
	{
	case (Z_ERRNO):
	case (Z_STREAM_END):
	case (Z_NEED_DICT):
	case (Z_VERSION_ERROR):
		result->code = ERROR_UNKNOWN;
		result->msg = MSG_UNKNOWN;
		break;
	case (Z_DATA_ERROR):
		result->code = ERROR_DATA_INVALID;
		result->msg = MSG_INVALID_DATA;
		break;
	case (Z_MEM_ERROR):
	case (Z_BUF_ERROR):
		result->code = ERROR_OUT_OF_MEMORY;
		result->msg = MSG_NOT_ENOUGH_MEMORY;
		break;
	case (Z_OK):
		break;
	}
#elif defined(LIBDEFLATE)
	struct libdeflate_decompressor *d = libdeflate_alloc_decompressor();
	switch (libdeflate_zlib_decompress(d, extracted->data, extracted->size, pnm->data, pnm->h * line, NULL))
	{
	case (LIBDEFLATE_BAD_DATA):
		result->code = ERROR_DATA_INVALID;
		result->msg = MSG_INVALID_DATA;
		libdeflate_free_decompressor(d);
		break;
	case (LIBDEFLATE_SHORT_OUTPUT):
	case (LIBDEFLATE_INSUFFICIENT_SPACE):
		result->code = ERROR_OUT_OF_MEMORY;
		result->msg = MSG_NOT_ENOUGH_MEMORY;
		libdeflate_free_decompressor(d);
		break;
	case LIBDEFLATE_SUCCESS:
		libdeflate_free_decompressor(d);
		break;
	}
#elif defined(ISAL)
	struct inflate_state state;
	isal_inflate_init(&state);
	state.next_in = extracted->data;
	state.avail_in = extracted->size;
	state.next_out = pnm->data;
	state.avail_out = pnm->h * line;
	state.crc_flag = IGZIP_ZLIB;
	switch (isal_inflate(&state))
	{
	case (ISAL_INVALID_BLOCK):
	case (ISAL_INVALID_SYMBOL):
	case (ISAL_INVALID_LOOKBACK):
	case (ISAL_INVALID_WRAPPER):
	case (ISAL_INCORRECT_CHECKSUM):
		result->code = ERROR_DATA_INVALID;
		result->msg = MSG_INVALID_DATA;
		break;
	case (ISAL_UNSUPPORTED_METHOD):
		result->code = ERROR_UNSUPPORTED;
		result->msg = MSG_UNSUPPORTED;
		break;
	case (ISAL_NEED_DICT):
		result->code = ERROR_UNKNOWN;
		result->msg = MSG_UNKNOWN;
		break;
	case (ISAL_DECOMP_OK):
		break;
	}
#endif
	if (result->code != SUCCESS)
	{
		free(pnm->data);
	}
extracted_free:
	free(extracted->data);
}
