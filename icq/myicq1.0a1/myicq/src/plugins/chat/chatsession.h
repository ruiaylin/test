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

#ifndef _CHAT_SESSION_H
#define _CHAT_SESSION_H

#include "tcpsessionbase.h"
#include "speechcodec.h"

class ChatSession : public TcpSessionListener {
public:
	ChatSession(TcpSessionBase *s);
	virtual ~ChatSession();

protected:
	virtual bool onPacketReceived(InPacket &);
	virtual void onSend() {}

	virtual void onChatText(const char *text) = 0;
	virtual void onSpeechData(const char *frame, int n) = 0;

	int getBitsPerSample() {
		return currentCodec->getBitsPerSample();
	}
	int getFrameSize() {
		return (getBitsPerSample() * currentCodec->getSamplesPerFrame() / 8);
	}

	void sendChatText(const char *text);
	void sendSpeechData(const char *speech, int n);

	void onSpeechData(InPacket &in);
	void onChatText(InPacket &);

	TcpSessionBase *tcp;
	SpeechCodec *currentCodec;
};

#endif
