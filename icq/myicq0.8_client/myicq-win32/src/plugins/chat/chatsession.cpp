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

#include "chatsession.h"
#include "packet.h"
#include "lpc10codec.h"

enum {
	CHAT_TEXT = 0x1000,
	CHAT_SPEECH_DATA,
};


ChatSession::ChatSession(TcpSessionBase *s)
{
	tcp = s;
	listener = NULL;

	currentCodec = new Lpc10Codec;
}

ChatSession::~ChatSession()
{
	if (currentCodec)
		delete currentCodec;
	if (tcp)
		tcp->destroy();
}

void ChatSession::sendChatText(const char *text)
{
	OutPacket *out = tcp->createPacket(CHAT_TEXT);
	*out << text;
	tcp->sendPacket(out);
}

void ChatSession::sendSpeechData(const char *speech, int n)
{
	char bits[4096];
	n = currentCodec->encode(speech, bits, n);
	OutPacket *out = tcp->createPacket(CHAT_SPEECH_DATA);
	out->writeData(bits, n);
	tcp->sendPacket(out);
}

void ChatSession::onChatText(InPacket &in)
{
	const char *text;
	in >> text;
	if (listener)
		listener->onChatText(text);
}

void ChatSession::onSpeechData(InPacket &in)
{
	int n;
	const char *bits = in.readData(n);

	char speech[4096];
	n = currentCodec->decode(bits, speech, n);
	if (listener)
		listener->onSpeechData(speech, n);
}

bool ChatSession::onPacketReceived(InPacket &in)
{
	uint16 cmd = in.getCmd();
	switch (cmd) {
	case CHAT_TEXT:
		onChatText(in);
		break;

	case CHAT_SPEECH_DATA:
		onSpeechData(in);
		break;

	default:
		return false;
	}

	return true;
}

void ChatSession::onClose()
{
	if (listener)
		listener->onClose();
}
