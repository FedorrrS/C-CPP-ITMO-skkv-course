//
// Created by selischev on 16.04.23.
//

#ifndef ITMO_C_PNG_FEDORRRS_STRUCTS_H
#define ITMO_C_PNG_FEDORRRS_STRUCTS_H

#include "../return_codes.h"

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* chunks */
#define IHDR "IHDR"
#define IEND "IEND"
#define IDAT "IDAT"
#define PLTE "PLTE"

/* error msg's */
#define MSG_SUCCESS ""
#define MSG_FNF "File not found"
#define MSG_ALREADY_EXISTS "File already exists"
#define MSG_NOT_ENOUGH_MEMORY "Not enough memory"
#define MSG_INVALID_DATA "Invalid data"
#define MSG_INVALID_PARAMETER "Expected 2 arguments to command line: <PNG> <PNM>"
#define MSG_UNKNOWN "Unknown result"
#define MSG_UNSUPPORTED "Unsupported functionality"
#define MSG_CHUNK_NOT_ALLOWED "This chunk is not allowed here"

typedef unsigned char byte;
typedef unsigned int uint;
typedef unsigned long uLong;

typedef struct
{
	int code;
	char *msg;
} exit_r;

typedef struct
{
	byte *data;
	size_t size;
	uint offset;
} comp_data;

typedef struct
{
	char *type;
	byte *data;
	/* bytes per pixel */ int bpp;
	size_t w, h;
} PNM;

typedef void (*filter_func)(size_t, PNM *);

#endif	  // ITMO_C_PNG_FEDORRRS_STRUCTS_H
