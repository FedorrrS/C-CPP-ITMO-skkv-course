//
// Created by selischev on 16.04.23.
//
#ifndef ITMO_C_PNG_FEDORRRS_PNG_UTILS_H
#define ITMO_C_PNG_FEDORRRS_PNG_UTILS_H

#include "lib_decider.h"
#include "structs.h"

/* cast byte to int */
uint byte_to_int(const byte *src, int offset, int n);
/* checks if cur chunk type is valid */
bool is_expected_chunk(const byte *cur, const char *expected);
/* gets length of next chunk */
uint get_chunk_length(FILE *png, exit_r *result);
/* gets name of next chunk */
void get_chunk_type(FILE *in, byte *chunk_type, exit_r *result);
/* update offset and size to extracted data */
void upd_extracted(comp_data *extracted, uint offset, size_t size);
/* checks if given file is input file */
bool is_png(FILE *input_file);
/* parse IHDR and PLTE
Width:              4 bytes
Height:             4 bytes
Bit depth:          1 byte
Color type:         1 byte
Compression method: 1 byte
Filter method:      1 byte
Interlace method:   1 byte

Red:   1 byte (0 = black, 255 = red)
Green: 1 byte (0 = black, 255 = green)
Blue:  1 byte (0 = black, 255 = blue) */
void parse_IHDR(FILE *png, PNM *pnm, exit_r *result);
/* parse PLTE not-yet-implemented */
void parse_PLTE(FILE *png, size_t len, PNM *pnm, exit_r *result);
/* parse IDAT */
void parse_IDAT(size_t length, FILE *in, comp_data *extracted, exit_r *result);
/* decompression using deflate */
void decompress(comp_data *extracted, PNM *pnm, exit_r *result);
/* Filter */
void apply_none(size_t offset, PNM *pnm);
void apply_filter(PNM *pnm, exit_r *result);
void apply_sub(size_t offset, PNM *pnm);
void apply_up(size_t offset, PNM *pnm);
void apply_average(size_t offset, PNM *pnm);
byte paeth_predictor(byte a, byte b, byte c);
void apply_paeth(size_t offset, PNM *pnm);

#endif	  // ITMO_C_PNG_FEDORRRS_PNG_UTILS_H
