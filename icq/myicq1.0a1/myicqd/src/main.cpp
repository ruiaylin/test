/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <dlfcn.h>
#include <iostream.h>
#include "myicq.h"
#include "log.h"
#include "groupplugin.h"


GET_PLUGIN_INFO loadPlugin(const char *name)
{
	string s = name;
	s += ".so";

	void *module = dlopen(s.c_str(), RTLD_LAZY);
	if (!module)
		return NULL;
		
	return (GET_PLUGIN_INFO) dlsym(module, "getPluginInfo");
}

static void *dnsThread(void *)
{
	handleDNS();
	return NULL;
}

static void *dbUpdateThread(void *)
{
	handleDBUpdate();
	return NULL;
}

static void *dbQueryThread(void *data)
{
	int i = (int) data;
	handleDBQuery(i);
	return NULL;
}

static void *pulseThread(void *)
{
	pulse();
	return NULL;
}

static bool myicqStart()
{
	if (!myicqInit())
		return false;
	
	pthread_t thread;

	// Creating threads...
	pthread_create(&thread, NULL, pulseThread, NULL);

	// DNS
	pthread_create(&thread, NULL, dnsThread, NULL);

	pthread_create(&thread, NULL, dbUpdateThread, NULL);
	for (int i = 0; i < NR_DB_QUERY; ++i)
		pthread_create(&thread, NULL, dbQueryThread, (void *) i);

	return true;
}

int daemon_init()
{
	struct sigaction act;
	int i, maxfd;
	
	if (fork() != 0)
		exit(0);
	if (setsid() < 0)
		return -1;
	act.sa_handler = SIG_IGN;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	sigaction(SIGHUP, &act, 0);
	if (fork() != 0)
		exit(0);
	chdir("/");
	umask(0);
	maxfd = sysconf(_SC_OPEN_MAX);
	for (i = 0; i < maxfd; i++)
		close(i);
	open("/dev/null", O_RDWR);
	dup(0);
	dup(1);
	dup(2);
	return 0;
}

int main(int argc, char *argv[])
{
	if (daemon_init() < 0) {
		cerr << "daemon_init error." << endl;
		return 1;
	}

	initArgs(argc, argv);
	
	if (!myicqStart()) {
		LOG(1) ("myicqStart() failed.\n");
		return 1;
	}
		
	handlePacket();
	
	myicqDestroy();	
	return 0;
}
