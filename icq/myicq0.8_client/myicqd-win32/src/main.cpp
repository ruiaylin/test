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

#include <stdlib.h>
#include <iostream.h>
#include "sessionhash.h"


int main(int argc, char *argv[])
{
#ifdef _WIN32
	WSADATA wsaData;
	WORD version = MAKEWORD(2, 2);
	if (WSAStartup(version, &wsaData) != 0) {
		cerr << "WSAStartup() failed." << endl;
		return 1;
	}
#endif

	if (!UdpSession::initialize())
		return 1;

	cout << "myicqd is now started." << endl << endl;

	time_t sendTime, keepAliveTime, now;
	sendTime = keepAliveTime = time(NULL);

	while (true) {
		UdpSession::onReceive();

		now = time(NULL);
		if (now - sendTime > SEND_TIMEOUT) {
			sendTime = now;
			UdpSession::checkSendQueue();
		}
		if (now - keepAliveTime > KEEPALIVE_TIMEOUT) {
			keepAliveTime = now;
			UdpSession::checkKeepAlive();
		}
		UdpSession::broadcastMessages();
	}

	UdpSession::cleanUp();
#ifdef _WIN32
	WSACleanup();
#endif
	return 0;
}
