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

#ifndef _ICQ_LINK_BASE_H
#define _ICQ_LINK_BASE_H

#include "icqtypes.h"

class ContactInfo;
class TcpSessionBase;
class TcpSessionListener;

class IcqLinkBase {
public:
	// Create a tcp session connected to the address (ip, port)
	virtual TcpSessionBase *createTcpSession(TcpSessionListener *l, uint32 ip, uint16 port) = 0;

	// Get a contact's information
	virtual ContactInfo *getContactInfo(QID *qid) = 0;

	// Get an icon by a contact's face and status
	virtual void *getFaceIcon(int face, int status) = 0;

	// Pop up a message.
	// t is the duration time you want the message to be shown.
	// t = 0 means infinite until the user clicks on it.
	virtual void popupMsg(void *icon, const char *text, uint32 t) = 0;
};

#endif
