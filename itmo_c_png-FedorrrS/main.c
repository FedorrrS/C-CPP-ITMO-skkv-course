#include "include/png_utils.h"
#include "include/pnm_utils.h"

int main(int argc, char *argv[])
{
	exit_r result = { SUCCESS, MSG_SUCCESS };
	if (argc != 3)
	{
		result.code = ERROR_PARAMETER_INVALID;
		result.msg = MSG_INVALID_PARAMETER;
		goto exit_program;
	}
	FILE *input_file = fopen(argv[1], "rb");
	if (!input_file)
	{
		result.code = ERROR_CANNOT_OPEN_FILE;
		result.msg = MSG_FNF;
		goto exit_program;
	}
	if (!is_png(input_file))
	{
		result.code = ERROR_DATA_INVALID;
		result.msg = MSG_INVALID_DATA;
		goto cleanup;
	}
	/* parsing IHDR */
	/* parsing length */
	get_chunk_length(input_file, &result);
	if (result.code != SUCCESS)
	{
		goto cleanup;
	}
	/* parsing chuck type */
	byte chunk_type[4];
	get_chunk_type(input_file, chunk_type, &result);
	if (result.code != SUCCESS)
	{
		goto cleanup;
	}
	if (!is_expected_chunk(chunk_type, IHDR))
	{
		result.code = ERROR_DATA_INVALID;
		result.msg = MSG_INVALID_DATA;
		goto cleanup;
	}
	/* parsing IHDR data */
	PNM pnm;
	parse_IHDR(input_file, &pnm, &result);
	if (result.code != SUCCESS)
	{
		goto cleanup;
	}
	/* end of parsing IHDR */
	size_t length;

	/* place for not-yet-implemented PLTE parsing */

	/* parsing IDAT */
	/* parsing length */
	length = get_chunk_length(input_file, &result);
	if (result.code != SUCCESS)
	{
		goto cleanup;
	}
	/* parsing chuck type */
	get_chunk_type(input_file, chunk_type, &result);
	if (result.code != SUCCESS)
	{
		goto cleanup;
	}
	comp_data extracted;
	while (!is_expected_chunk(chunk_type, IEND))
	{
		if (is_expected_chunk(chunk_type, IDAT))
		{
			parse_IDAT(length, input_file, &extracted, &result);
			if (result.code != SUCCESS)
			{
				goto cleanup;
			}
		}
		else
		{
			/* Critical chunks handling */
			/* handled cases:
			 * PLTE in gc
			 * IHDR
			 * seems to be enough... */
			if ((is_expected_chunk(chunk_type, PLTE) && pnm.bpp == 1) || is_expected_chunk(chunk_type, IHDR))
			{
				result.code = ERROR_DATA_INVALID;
				result.msg = MSG_CHUNK_NOT_ALLOWED;
				goto cleanup;
			}
			/* Ancillary chunk data skip */
			if (fseek(input_file, sizeof(byte) * length, SEEK_CUR))
			{
				result.code = ERROR_DATA_INVALID;
				result.msg = MSG_INVALID_DATA;
				free(extracted.data);
				goto cleanup;
			}
		}
		/* CRC skipped */
		if (fseek(input_file, sizeof(byte) * 4, SEEK_CUR))
		{
			result.code = ERROR_DATA_INVALID;
			result.msg = MSG_INVALID_DATA;
			free(extracted.data);
			goto cleanup;
		}
		/* switching to next chunk */
		/* parsing length */
		length = get_chunk_length(input_file, &result);
		if (result.code != SUCCESS)
		{
			goto cleanup;
		}
		/* parsing chunk type */
		get_chunk_type(input_file, chunk_type, &result);
		if (result.code != SUCCESS)
		{
			goto cleanup;
		}
	}
	/* IEND CRC skipped */
	if (fseek(input_file, sizeof(byte) * 4, SEEK_CUR))
	{
		result.code = ERROR_DATA_INVALID;
		result.msg = MSG_INVALID_DATA;
		free(extracted.data);
		goto cleanup;
	}
	decompress(&extracted, &pnm, &result);
	if (result.code != SUCCESS)
	{
		goto cleanup;
	}
	apply_filter(&pnm, &result);
	FILE *output_file = fopen(argv[2], "wb");
	if (!output_file)
	{
		result.code = ERROR_CANNOT_OPEN_FILE;
		result.msg = MSG_ALREADY_EXISTS;
		free(pnm.data);
		goto cleanup;
	}
	dump_pnm(&pnm, output_file);
	free(pnm.data);
	fclose(output_file);
cleanup:
	fclose(input_file);
	/* pnm closed in `dump_pnm` */
exit_program:
	fprintf(stderr, "%s", result.msg);
	return result.code;
}
