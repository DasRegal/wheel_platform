//*****************************************************************************
//
// Имя файла    : 'motion.cpp'
// Заголовок    : Драйвер двигателей
// Автор        : Барышников Р. А.
// Контакты     : plexus_bra@rambler.ru
// Дата         : 27.09.2013
//
//*****************************************************************************

#include <stdio.h>

#include "motion.h"
#include "gpio.h"
#include "thread_wheel.h"

#include <mutex>              
#include <condition_variable> 
/*

std::mutex mtx;
std::condition_variable cv;
*/
// =============================================================================
//                                 Отладка
// =============================================================================

// =============================================================================
//                                Костанты
// =============================================================================

// // 27 вывод
// #define		WHEEL_RIGHT				88
// // 30 вывод
// #define		WHEEL_RIGHT_REVERSE		114
// // 18 вывод
// #define		WHEEL_LEFT				115
// // 31 вывод
// #define		WHEEL_LEFT_REVERSE		87

// 27 вывод
#define		WHEEL_RIGHT				90
// 30 вывод
#define		WHEEL_RIGHT_REVERSE		108
// 18 вывод
#define		WHEEL_LEFT				111
// 31 вывод
#define		WHEEL_LEFT_REVERSE		88

// =============================================================================
//                                 Макросы
// =============================================================================

// =============================================================================
//                           Глобальные переменные
// =============================================================================

int g_isCycleLeft;
int g_isCycleRight;

// =============================================================================
//                             Прототипы функций
// =============================================================================

int rightWheelMove(EDir isReverse);
int leftWheelMove(EDir isReverse);
int rightWheelStop(void);
int leftWheelStop(void);

void* rightPWMTick(void* vptr_args);
void* leftPWMTick(void* vptr_args);

pthread_t thrLeftWheel, thrRightWheel;

int motionInit (void)
{
	g_isCycleLeft = 0;
	g_isCycleRight = 0;
	GPIOInit(WHEEL_RIGHT);
	GPIOInit(WHEEL_RIGHT_REVERSE);
	GPIOInit(WHEEL_LEFT);
	GPIOInit(WHEEL_LEFT_REVERSE);

	if (pthread_create(&thrLeftWheel, NULL, leftPWMTick, NULL) != 0)
	{
		return EXIT_FAILURE;
	}
	
	if (pthread_create(&thrRightWheel, NULL, rightPWMTick, NULL) != 0)
	{
		return EXIT_FAILURE;
	}

	return 0;
}

int motionMove (EDir isReverse)
{
	rightWheelMove(isReverse);
	leftWheelMove(isReverse);
	return 0;
}

int motionStop (void)
{
	rightWheelStop();
	leftWheelStop();
	return 0;
}


int motionLeft (ETurn isAround, EDir isReverse)
{
	rightWheelMove(isReverse);
	if (isAround)
	{
		leftWheelMove(isReverse ? (isAround ? FORWARD:REVERSE):(isAround ? REVERSE:FORWARD));
	}
	else
	{
		leftWheelStop();
	}
}

int motionRight(ETurn isAround, EDir isReverse)
{
	leftWheelMove(isReverse);
	if (isAround)
	{
		rightWheelMove(isReverse ? (isAround ? FORWARD:REVERSE):(isAround ? REVERSE:FORWARD));
	}
	else
	{
		rightWheelStop();
	}
}

int rightWheelMove(EDir isReverse)
{
	// GPIOSet(WHEEL_RIGHT);
	g_isCycleRight = 1;
	if (isReverse)
	{
		GPIOSet(WHEEL_RIGHT_REVERSE);
	}
	else
	{
		GPIOClr(WHEEL_RIGHT_REVERSE);
	}

//	cv.notify_all()
}

int leftWheelMove(EDir isReverse)
{
	// GPIOSet(WHEEL_LEFT);
	g_isCycleLeft = 1;
	if (isReverse)
	{
		GPIOSet(WHEEL_LEFT_REVERSE);
	}
	else
	{
		GPIOClr(WHEEL_LEFT_REVERSE);
	}
//	cv.nofify_all();
}

int rightWheelStop(void)
{
	g_isCycleRight = 0;
	GPIOClr(WHEEL_RIGHT);
	GPIOClr(WHEEL_RIGHT_REVERSE);	
}

int leftWheelStop(void)
{
	g_isCycleLeft = 0;
	GPIOClr(WHEEL_LEFT);
	GPIOClr(WHEEL_LEFT_REVERSE);	
}
#include <iostream>
 void* leftPWMTick(void* vptr_args)
{
	// std::unique_lock<std::mutex> lck(mtx);
//	std::cerr << "value =  " << (int) g_isCycleLeft << "\n";
	while (true)
	{
		// cv.wait(lck);
		if(g_isCycleLeft)
		{
			// tLeftWheel->setLevel(1);
			GPIOSet(WHEEL_LEFT);
			usleep(tLeftWheel->getPeriod());
			// tLeftWheel->setLevel(0);
			GPIOClr(WHEEL_LEFT);
			usleep(20000 - tLeftWheel->getPeriod());
		}
	}
}

 void* rightPWMTick(void* vptr_args)
{
	// std::unique_lock<std::mutex> lck(mtx);

	while (true)
	{
		// cv.wait(lck);
		if(g_isCycleRight)
		{
			// tRightWheel->setLevel(1);
			GPIOSet(WHEEL_RIGHT);
			usleep(tRightWheel->getPeriod());
			// tRightWheel->setLevel(0);
			GPIOClr(WHEEL_RIGHT);
			usleep(20000 - tRightWheel->getPeriod());
		}
	}
}

void setSpeed(ESpeedWheel wheel, int value)
{
	if(wheel == LEFT_WHEEL)
	{
		tLeftWheel->setPeriod(SET_VALUE(value));
	}
	if(wheel == RIGHT_WHEEL)
	{
		tRightWheel->setPeriod(SET_VALUE(value));
	}
}