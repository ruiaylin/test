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

#ifndef _SPEECH_CODEC_H
#define _SPEECH_CODEC_H

class SpeechCodec {
public:
	virtual const char *getName() = 0;
	virtual int getBitsPerSample() = 0;
	virtual int getSamplesPerFrame() = 0;
	virtual int encode(const char *in, char *out, int n) = 0;
	virtual int decode(const char *in, char *out, int n) = 0;
};

#endif
