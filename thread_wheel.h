#ifndef _THREAD_WHEEL_H
#define _THREAD_WHEEL_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

#define		T_PERIOD		20000
#define		SET_VALUE(x)	((x) * 200)

enum ESpeedWheel
{
	LEFT_WHEEL = 0,
	RIGHT_WHEEL
};

class CWheelThread
{
private:
	int m_isHighLevel;
	int m_period;
public:
	CWheelThread(int);
	~CWheelThread();

	int getLevel(void);
	int getPeriod(void);
	void setLevel(int);
	void setPeriod(int);
};

extern CWheelThread *tLeftWheel;
extern CWheelThread *tRightWheel;

#endif //_THREAD_WHEEL_H