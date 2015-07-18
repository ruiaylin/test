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

#ifndef _WAVE_H
#define _WAVE_H

class WaveIn {
public:
	WaveIn();
	~WaveIn();

	BOOL open(int channels, int freq, int bits, HWND hWnd);
	void close();
	BOOL addBuffer(DWORD size);
	BOOL start();

private:
	HWAVEIN hWaveIn;
	WAVEFORMATEX wfex;
	CPtrList waveHeaders;
};

class WaveOut {
public:
	WaveOut();
	~WaveOut();

	BOOL open(int channels, int freq, int bits, HWND hWnd);
	void close();
	void play(LPCTSTR buf, DWORD size);

private:
	HWAVEOUT hWaveOut;
	WAVEFORMATEX wfex;
};

#endif