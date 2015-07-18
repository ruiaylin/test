/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   copyright            : (C) 2002 by Zhang Yong                         *
 *   email                : z-yong163@163.com                              *
 ***************************************************************************/

#include "lpc10codec.h"
#include <malloc.h>
#include <memory.h>

#define LPC10_BYTES_IN_COMPRESSED_FRAME		\
	((LPC10_BITS_IN_COMPRESSED_FRAME >> 3) + !!(LPC10_BITS_IN_COMPRESSED_FRAME & 7))


Lpc10Codec::Lpc10Codec()
{
	encodeState = create_lpc10_encoder_state();
	decodeState = create_lpc10_decoder_state();
}

Lpc10Codec::~Lpc10Codec()
{
	free(encodeState);
	free(decodeState);
}

int Lpc10Codec::encode(const char *in, char *out, int n)
{
	float speech[LPC10_SAMPLES_PER_FRAME];
	INT32 bits[LPC10_BITS_IN_COMPRESSED_FRAME];
	short *data = (short *) in;
	n /= sizeof(short);

	int i = n % LPC10_SAMPLES_PER_FRAME;
	if (i) {
		i = LPC10_SAMPLES_PER_FRAME - i;
		while (i-- > 0)
			data[n++] = 0;
	}

	char *p = out;
	n /= LPC10_SAMPLES_PER_FRAME;
	memset(out, 0, n * LPC10_BYTES_IN_COMPRESSED_FRAME);

	while (n-- > 0) {
		for (i = 0; i < LPC10_SAMPLES_PER_FRAME; ++i)
			speech[i] = data[i] / 32768.0f;

		lpc10_encode(speech, bits, encodeState);

		for (i = 0; i < LPC10_BITS_IN_COMPRESSED_FRAME; ++i)
			p[i >> 3] |= (!!bits[i] << (i & 7));

		p += LPC10_BYTES_IN_COMPRESSED_FRAME;
		data += LPC10_SAMPLES_PER_FRAME;
	}

	return (p - out);
}

int Lpc10Codec::decode(const char *in, char *out, int n)
{
	float speech[LPC10_SAMPLES_PER_FRAME];
	INT32 bits[LPC10_BITS_IN_COMPRESSED_FRAME];
	short *data = (short *) out;

	n /= LPC10_BYTES_IN_COMPRESSED_FRAME;
	while (n-- > 0) {
		int i;
		for (i = 0; i < LPC10_BITS_IN_COMPRESSED_FRAME; ++i)
			bits[i] = !!(in[i >> 3] & (1 << (i & 7)));

		lpc10_decode(bits, speech, decodeState);

		for (i = 0; i < LPC10_SAMPLES_PER_FRAME; ++i)
			*data++ = (short) (speech[i] * 32768);

		in += LPC10_BYTES_IN_COMPRESSED_FRAME;
	}

	return ((char *) data - out);
}
