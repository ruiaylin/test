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

#include "myicq.h"
#include "icqsocket.h"
#include "udpsession.h"
#include "server.h"
#include "dbmanager.h"
#include "dnsmanager.h"
#include "groupplugin.h"
#include "profile.h"
#include "log.h"
#include "getopt.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef _WIN32

#include "win32\service.h"

#define CONFIG_FILE		"c:\\myicqd.cnf"
#define FILE_SEPARATOR	'\\'

inline void sleep(uint32 t)
{
	Sleep(t * 1000);
}

#else

#define CONFIG_FILE		"/etc/myicqd.cnf"
#define FILE_SEPARATOR	'/'

#endif


time_t curTime;
OPTIONS _ops;

GET_PLUGIN_INFO loadPlugin(const char *name);


static DBManager dbQuery[NR_DB_QUERY];


static bool parseConfig(Section &sec)
{
	const char *name = sec.getName();

	if (!strcmp(name, "myicq")) {
		_ops.myicqIP = inet_addr(sec.getString("ip", "0.0.0.0"));
		_ops.myicqPort = htons(sec.getInteger("port", MYICQ_PORT_DEFAULT));
		_ops.enableRegister = sec.getInteger("enable_register", 1);

	} else if (!strcmp(name, "log")) {
		_ops.logFile = sec.getString("file");
		_ops.logLevel = sec.getInteger("level", 1);

	} else if (!strcmp(name, "mysql")) {
		_ops.dbInfo.host = sec.getString("host");
		_ops.dbInfo.port = sec.getInteger("port", 0);
		_ops.dbInfo.db = sec.getString("db");
		_ops.dbInfo.user = sec.getString("user");
		_ops.dbInfo.passwd = sec.getString("passwd");

	} else if (!strcmp(name, "s2s")) {
		_ops.enableS2S = sec.getInteger("enable", 1);
		_ops.domain = strLowerCase((char *) sec.getString("domain"));
		_ops.desc = sec.getString("description");

		if (_ops.domain.empty()) {
			char name[256];
			if (gethostname(name, sizeof(name)) == 0)
				_ops.domain = strLowerCase(name);
		}

	} else if (!strcmp(name, "group"))
		_ops.pluginDir = sec.getString("dir");

	return true;
}

static bool parseGroupConfig(Section &sec)
{
	int enable = sec.getInteger("enable", 1);
	if (!enable)
		return false;

	const char *name = sec.getName();

	GET_PLUGIN_INFO getPluginInfo = NULL;
	if (strcmp(name, "chatroom") != 0) {
		string s = _ops.pluginDir + FILE_SEPARATOR + name;
		getPluginInfo = loadPlugin(s.c_str());
		if (!getPluginInfo) {
			LOG(1) ("Can not load module %s\n", name);
			return false;
		}

		LOG(1) ("module %s is loaded\n", name);
	}

	GroupPlugin *p = new GroupPlugin;

	if (getPluginInfo) {
		getPluginInfo(&p->info);
		p->name = name;
	}
	p->displayName = sec.getString("name", name);

	return GroupPlugin::registerPlugin(p);
}

static bool loadPlugins()
{
	string name = _ops.pluginDir + FILE_SEPARATOR + "groups.cnf";
	return parseProfile(name.c_str(), parseGroupConfig);
}

static void printVersion()
{
	printf("myicqd version 1.0-alpha1 published under GPL license\n");
}

static void printHelp()
{
}

void initArgs(int argc, char *argv[])
{
	static struct option longopts[] = {
#ifdef _WIN32
		{ "install", no_argument, NULL, 'i' },
		{ "remove", no_argument, NULL, 'e' },
#endif
		{ "version", no_argument, NULL, 'v' },
		{ NULL, 0, NULL, '\0' },
	};

	const char *shortops =
#ifdef _WIN32
		"ie"
#endif
		"v"
	;
	
	char c;
	while ((c = getopt_long(argc, argv, shortops, longopts, NULL)) != EOF) {
		switch (c) {
#ifdef _WIN32
		case 'i':
			installService();
			exit(0);
		case 'e':
			removeService();
			exit(0);
#endif
		case 'v':
			printVersion();
			exit(0);

		default:
			printHelp();
			exit(0);
		}
	}
}

static void checkTimeOuts()
{
	UdpSession::checkSendQueue();
	UdpSession::checkKeepAlive();
}

bool myicqInit()
{
	srand(time(&curTime));

	parseProfile(CONFIG_FILE, parseConfig);

	Log::open(_ops.logFile.c_str(), _ops.logLevel);

	loadPlugins();

	// Initialize database subsystem
	if (!DBManager::init(_ops.dbInfo)) {
		LOG(1) ("Cannot connect to mysql master\n");
		return false;
	}
	for (int i = 0; i < NR_DB_QUERY; ++i) {
		if (!dbQuery[i].create(_ops.dbInfo)) {
			LOG(1) ("Cannot connect to mysql slave\n");
			return false;
		}
	}

	if (!UdpSession::init())
		return false;

	if (_ops.enableS2S) {
		if (!Server::init())
			return false;

		LOG(1) ("domain name is %s\n", _ops.domain.c_str());
	}

	LOG(1) ("myicqd is now started.\n");
	return true;
}

void myicqDestroy()
{
	UdpSession::destroy();
	Server::destroy();
	DBManager::destroy();

	Log::closeLog();
}

void pulse()
{
	for (;;) {
		time((time_t *) &curTime);
		sleep(1);
	}
}

void handlePacket()
{
	int sock = UdpSession::sock;

	for (;;) {
		fd_set readfds;
		fd_set writefds;

		FD_ZERO(&readfds);
		FD_ZERO(&writefds);

		FD_SET(sock, &readfds);
		int maxfd = sock;

		if (_ops.enableS2S) {
			int n = Server::generateFds(readfds, writefds);
			if (n > maxfd)
				maxfd = n;
		}

		timeval to;
		to.tv_sec = 1;
		to.tv_usec = 0;

		int n = select(maxfd + 1, &readfds, &writefds, NULL, &to);

		if (n > 0) {
			if (FD_ISSET(sock, &readfds))
				UdpSession::onReceive();

			if (_ops.enableS2S)
				Server::examineFds(readfds, writefds);

		} else if (n < 0) {
			if (errno == EINTR)
				continue;

			LOG(1) ("select() failed\n");
			break;
		}

		DBManager::dispatch();
		DNSManager::dispatch();

		static time_t lastTime;
		if (curTime != lastTime) {
			lastTime = curTime;
			checkTimeOuts();
		}
	}
}

void handleDNS()
{
	DNSManager::process();
}

void handleDBQuery(int i)
{
	dbQuery[i].processQuery();
}

void handleDBUpdate()
{
	DBManager::processUpdate();
}
