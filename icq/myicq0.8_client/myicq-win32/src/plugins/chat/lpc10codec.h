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

#include "speechcodec.h"
#include "lpc10.h"


class Lpc10Codec : public SpeechCodec {
public:
	Lpc10Codec();
	~Lpc10Codec();

	virtual const char *getName() {
		return "lpc10";
	}
	virtual int getBitsPerSample() {
		return 16;
	}
	virtual int getSamplesPerFrame() {
		return LPC10_SAMPLES_PER_FRAME;
	}

	virtual int encode(const char *in, char *out, int n);
	virtual int decode(const char *in, char *out, int n);

private:
	lpc10_encoder_state *encodeState;
	lpc10_decoder_state *decodeState;
};
