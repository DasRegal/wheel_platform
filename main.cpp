#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <csignal>

#include "main.h"
#include "motion.h"
#include "find.h"
#include "cameraConfig.h"
#include "buzzer.h"

CAMERA Cam; //Инициализация.

int Init(void)
{	
	BuzInit();
	return motionInit();
}

time_t now;

void uDelay(int time)
{
	for (int i = 0; i < 500; ++i)
	{
		usleep(time);
	}
}

void signal_callback_handler(int sig)
{
	printf("\nSTOP\n");
	motionStop();
	Cam.CVRelease ();//Освобождение памяти.
	exit(sig);
}

int main(int argc, char const *argv[])
{	
	signal(SIGINT, signal_callback_handler);
	signal(SIGKILL, signal_callback_handler);
	signal(SIGTERM, signal_callback_handler);

	if(Init() != 0)
	{
		printf("[!] Initialization error\n");
		return -1;
	}

	setSpeed(RIGHT_WHEEL, 50);
	setSpeed(LEFT_WHEEL, 50);

	//------------Цикл обработки кадров и поиска маркера--------------------------------------------------
	while(true) 
	{
		//----------------------------------------------------------------------------------------------------
#if (defined (CAMERA1) || defined (FROM_FILE1))
		Cam.find1(); //Функция поиска.
#endif
		//Реакция на результаты поиска по первой камере.
#if (defined (CAMERA1) || defined (FROM_FILE1))
		if (Cam.getExistObj())//Есть объект.
		{
			if (Cam.getY()>=0.8)//Стоп.
			{
				beep(50, 1);
				char textbuf31[]="TARGET IS ACHIEVED";
				char textbuf32[]="STOP";
				CvFont myFont;
				cvInitFont(&myFont,	CV_FONT_HERSHEY_PLAIN,Cam.frame1->height/160,Cam.frame1->width/213,0,Cam.frame1->width/100,8);
				cvPutText(Cam.frame1,textbuf31,cvPoint(Cam.frame1->width*1/8,Cam.frame1->height/3),&myFont,cvScalar(0,255,255));
				cvPutText(Cam.frame1,textbuf32,cvPoint(Cam.frame1->width*13/32,Cam.frame1->height/2),&myFont,cvScalar(0,0,255));
			}
			if (Cam.getX()<0.5 && Cam.getX()>-0.5)
			{
				setSpeed(RIGHT_WHEEL, 100);
				setSpeed(LEFT_WHEEL, 100);
				motionMove(FORWARD);
				// uDelay(1);
				// motionStop();
				// char textbuf31[]="FORWARD";
				// char textbuf32[]="NO TURN";
				// CvFont myFont;
				// cvInitFont(&myFont,	CV_FONT_HERSHEY_PLAIN,Cam.frame1->height/160,Cam.frame1->width/213,0,Cam.frame1->width/100,8);
				// cvPutText(Cam.frame1,textbuf31,cvPoint(Cam.frame1->width*1/8,Cam.frame1->height/3),&myFont,cvScalar(0,255,255));
				// cvPutText(Cam.frame1,textbuf32,cvPoint(Cam.frame1->width*13/32,Cam.frame1->height/2),&myFont,cvScalar(0,0,255));
			}
			else
			{
				setSpeed(RIGHT_WHEEL, 50);
				setSpeed(LEFT_WHEEL, 50);
				if (Cam.getX()>=0.5)//Влево.
				{
					motionRight(NORMAL, FORWARD);
					// uDelay(1);
					// motionStop();
					char textbuf1[]="TURN RIGHT";
					char textbuf11[]="->";
					CvFont myFont;
					cvInitFont(&myFont,	CV_FONT_HERSHEY_PLAIN,Cam.frame1->height/160,Cam.frame1->width/213,0,Cam.frame1->width/100,8);
					cvPutText(Cam.frame1,textbuf1,cvPoint(Cam.frame1->width*5/16,Cam.frame1->height/3),&myFont,cvScalar(0,255,255));
					cvPutText(Cam.frame1,textbuf11,cvPoint(Cam.frame1->width*15/32,Cam.frame1->height/2),&myFont,cvScalar(0,0,255));
				}
				else if (Cam.getX()<=-0.5)//Вправо.
				{
					motionLeft(NORMAL, FORWARD);
					// uDelay(1);
					// motionStop();
					char textbuf2[]="TURN LEFT";
					char textbuf22[]="<-";
					CvFont myFont;
					cvInitFont(&myFont,	CV_FONT_HERSHEY_PLAIN,Cam.frame1->height/160,Cam.frame1->width/213,0,Cam.frame1->width/100,8);
					cvPutText(Cam.frame1,textbuf2,cvPoint(Cam.frame1->width*9/32,Cam.frame1->height/3),&myFont,cvScalar(0,255,255));
					cvPutText(Cam.frame1,textbuf22,cvPoint(Cam.frame1->width*13/32,Cam.frame1->height/2),&myFont,cvScalar(0,0,255));
				}
			}
		}
		else
		{
			motionStop();
			char textbuf4[]="NO OBJECT";
			char textbuf5[]="STOP";
			CvFont myFont;
			cvInitFont(&myFont,	CV_FONT_HERSHEY_PLAIN,Cam.frame1->height/160,Cam.frame1->width/213,0,Cam.frame1->width/100,8);
			cvPutText(Cam.frame1,textbuf4,cvPoint(Cam.frame1->width*5/16,Cam.frame1->height/3),&myFont,cvScalar(0,255,255));
			cvPutText(Cam.frame1,textbuf5,cvPoint(Cam.frame1->width*13/32,Cam.frame1->height/2),&myFont,cvScalar(0,0,255));
		}
#endif
		//----------------------------------------------------------------------------------------------------

		Cam.WriteFrame();//Должно быть в конце while. Запись кадров с выполненным алгоритмом. +Показ окон, если объявлено в config.h

		char c = cvWaitKey(33);
		if (c == 27) 
			break;
			//while (c == 'p') {c = cvWaitKey(0);} //Пауза. Отмена любой другой клавишей, кроме 'p'.
	}
	//----------------------------------------------------------------------------------------------------
	
	motionStop();
	return 0;
}

void Delay(int time)
{
	sleep(time);
}