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

#ifndef _SYNC_H
#define _SYNC_H

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#include <semaphore.h>
#endif

class Mutex {
public:
	Mutex();
	~Mutex();

	void lock();
	void unlock();

private:

#ifdef _WIN32
	CRITICAL_SECTION cs;
#else
	pthread_mutex_t m;
#endif
};

class Semaphore {
public:
	Semaphore();
	~Semaphore();

	void down();
	void up();

private:

#ifdef _WIN32
	HANDLE sem;
#else
	sem_t sem;
#endif
};


#ifdef _WIN32	// Win32 implementation

inline Semaphore::Semaphore()
{
	sem = CreateSemaphore(NULL, 0, 4096, NULL);
}

inline Semaphore::~Semaphore()
{
	if (sem)
		CloseHandle(sem);
}

inline void Semaphore::down()
{
	WaitForSingleObject(sem, INFINITE);
}

inline void Semaphore::up()
{
	ReleaseSemaphore(sem, 1, NULL);
}


inline Mutex::Mutex()
{
	InitializeCriticalSection(&cs);
}

inline Mutex::~Mutex()
{
	DeleteCriticalSection(&cs);
}

inline void Mutex::lock()
{
	EnterCriticalSection(&cs);
}

inline void Mutex::unlock()
{
	LeaveCriticalSection(&cs);
}

#else		// Linux implementation

inline Semaphore::Semaphore()
{
	sem_init(&sem, 0, 0);
}

inline Semaphore::~Semaphore()
{
	sem_destroy(&sem);
}

inline void Semaphore::down()
{
	sem_wait(&sem);
}

inline void Semaphore::up()
{
	sem_post(&sem);
}


inline Mutex::Mutex()
{
	pthread_mutex_init(&m, NULL);
}

inline Mutex::~Mutex()
{
	pthread_mutex_destroy(&m);
}

inline void Mutex::lock()
{
	pthread_mutex_lock(&m);
}

inline void Mutex::unlock()
{
	pthread_mutex_unlock(&m);
}

#endif		// _WIN32

#endif
