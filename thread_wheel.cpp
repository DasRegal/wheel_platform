#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include "thread_wheel.h"

int g_isCycle = 1;

int g_leftHigh;
int g_rightHigh;
int g_isHighLevel;

CWheelThread *tLeftWheel = new CWheelThread(SET_VALUE(100));
CWheelThread *tRightWheel = new CWheelThread(SET_VALUE(100));

CWheelThread::CWheelThread(int value)
{
	m_period = value;
	m_isHighLevel = 1;
}

CWheelThread::~CWheelThread()
{

}

int CWheelThread::getLevel(void)
{
	return m_isHighLevel;
}

int CWheelThread::getPeriod(void)
{
	return m_period;
}

void CWheelThread::setLevel(int isSet)
{
	m_isHighLevel = isSet;
}

void CWheelThread::setPeriod(int period)
{
	m_period = period;
}