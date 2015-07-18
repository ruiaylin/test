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

#include "tcppacket.h"


int TcpOutPacket::send(int sock)
{
	uint16 len = cursor - data - sizeof(len);
	*((uint16 *) data) = htons(len);
	return ::send(sock, data, len + sizeof(len), 0);
}
