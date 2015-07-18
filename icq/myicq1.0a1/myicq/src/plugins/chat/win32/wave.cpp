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

#include "stdafx.h"
#include "wave.h"


WaveIn::WaveIn()
{
	hWaveIn = NULL;
}

WaveIn::~WaveIn()
{
	close();
}

void WaveIn::close()
{
	waveInReset(hWaveIn);
	
	while (!waveHeaders.IsEmpty()) {
		LPWAVEHDR pwh = (LPWAVEHDR) waveHeaders.RemoveHead();
		waveInUnprepareHeader(hWaveIn, pwh, sizeof(WAVEHDR));
		delete []pwh->lpData;
		delete pwh;
	}

	waveInClose(hWaveIn);
	hWaveIn = NULL;
}

BOOL WaveIn::open(int channels, int freq, int bits, HWND hWnd)
{
	wfex.cbSize = sizeof(wfex);
	wfex.wFormatTag = WAVE_FORMAT_PCM;
	wfex.nChannels = channels;
	wfex.nSamplesPerSec = freq;
	wfex.wBitsPerSample = bits;
	wfex.nBlockAlign = wfex.nChannels * (wfex.wBitsPerSample / 8);
	wfex.nAvgBytesPerSec = wfex.nSamplesPerSec * wfex.nBlockAlign;

	MMRESULT res;
	res = waveInOpen(&hWaveIn, WAVE_MAPPER, &wfex, (DWORD) hWnd, 0, CALLBACK_WINDOW);
	return (res == MMSYSERR_NOERROR);
}

BOOL WaveIn::addBuffer(DWORD size)
{
	WAVEHDR *pwh = new WAVEHDR;
	pwh->dwFlags = 0;
	pwh->dwBufferLength = size;
	pwh->lpData = new char[size];

	waveHeaders.AddTail(pwh);

	if (waveInPrepareHeader(hWaveIn, pwh, sizeof(WAVEHDR)) ||
		waveInAddBuffer(hWaveIn, pwh, sizeof(WAVEHDR))) {
		close();
		return FALSE;
	}
	return TRUE;
}

BOOL WaveIn::start()
{
	return (waveInStart(hWaveIn) == MMSYSERR_NOERROR);
}


WaveOut::WaveOut()
{
	hWaveOut = NULL;
}

WaveOut::~WaveOut()
{
	close();
}

void WaveOut::close()
{
	waveOutReset(hWaveOut);
	waveOutClose(hWaveOut);
	hWaveOut = NULL;
}

BOOL WaveOut::open(int channels, int freq, int bits, HWND hWnd)
{
	wfex.cbSize = sizeof(wfex);
	wfex.wFormatTag = WAVE_FORMAT_PCM;
	wfex.nChannels = channels;
	wfex.nSamplesPerSec = freq;
	wfex.wBitsPerSample = bits;
	wfex.nBlockAlign = wfex.nChannels * (wfex.wBitsPerSample / 8);
	wfex.nAvgBytesPerSec = wfex.nSamplesPerSec * wfex.nBlockAlign;

	MMRESULT res;
	res = waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfex, (DWORD) hWnd, 0, CALLBACK_WINDOW);
	return (res == MMSYSERR_NOERROR);
}

void WaveOut::play(LPCTSTR buf, DWORD size)
{
	WAVEHDR *pwh = new WAVEHDR;
	pwh->dwFlags = 0;
	pwh->dwLoops = 0;
	pwh->dwBufferLength = size;
	pwh->lpData = new char[size];
	memcpy(pwh->lpData, buf, size);

	waveOutPrepareHeader(hWaveOut, pwh, sizeof(WAVEHDR));
	waveOutWrite(hWaveOut, pwh, sizeof(WAVEHDR));
}
