//
// Created by selischev on 16.04.23.
//

#include "../include/pnm_utils.h"

void dump_pnm(PNM *pnm, FILE *output_file)
{
	/* maximum values equals 255 because for bit depth 8: 2^8 - 1 = 255 */
	fprintf(output_file, "%s\n%zu %zu\n%i\n", pnm->type, pnm->w, pnm->h, 255);
	for (int i = 0; i < pnm->h * (pnm->w * pnm->bpp + 1); ++i)
	{
		if (i % (pnm->w * pnm->bpp + 1) != 0)
		{
			fputc(pnm->data[i], output_file);
		}
	}
}