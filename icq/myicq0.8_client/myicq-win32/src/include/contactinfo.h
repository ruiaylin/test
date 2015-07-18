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

#ifndef _CONTACT_INFO_H
#define _CONTACT_INFO_H

#include "icqtypes.h"

enum {
	STATUS_ONLINE,
	STATUS_OFFLINE,
	STATUS_AWAY,
	STATUS_INVIS
};

class ContactInfo {
public:
	uint32 uin;
	string nick;
	uint8 face;
	uint8 gender;
	uint8 age;
	string country;
	string province;
	string city;

	string email;
	string address;
	string zipcode;
	string tel;

	string name;
	uint8 blood;
	string college;
	string profession;
	string homepage;
	string intro;

	uint32 status;
};

#endif