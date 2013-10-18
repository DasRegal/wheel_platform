#pragma once


#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "cameraConfig.h"

class CAMERA
{
private:
	float X, Y; //Координаты по X (слева(-1)-справа(1) от центральной линии) и Y (вверху(-1)-внизу(1) от центральной линии) от -1 до 1.
	bool ExistObj;
	CvCapture* capture1;
	
	IplImage* hsv;
	IplImage* h_plane; //// для хранения каналов HSV до преобразования
	IplImage* s_plane;
	IplImage* v_plane;

	IplImage* h_range; //// для хранения каналов HSV после преобразования
	IplImage* s_range;
	IplImage* v_range; 
	IplImage* hsv_and;

#if (defined (WRITE_ALG_CAMERA1) || defined (WRITE_CAMERA1))
	//double fps1 = cvGetCaptureProperty (capture1, CV_CAP_PROP_FPS);
	//Почему-то запись работает только с частотой записи 15 кадров в секунду.
	double fps1;
	CvSize size1;
#ifdef WRITE_ALG_CAMERA1
	CvVideoWriter * writer_alg1;
#endif
#if defined WRITE_CAMERA1
	CvVideoWriter * writer1;
#endif
#endif

#define n 30
	char filename_alg_c1[n];
	char filename_c1[n];

	bool exists(const char *fname);
	
	void ShowWindow(void);//Показ окошек.

	void CaptureVideo (void);//Функция захвата видео с камер.
	void GenerationVideoFileName (void);//Генерация названий видеофайлов и изображений.
	void InitializationVideoEditors(void);//Инициализация структур редакторов для записи видео.
	
	void find11 (void);
	
public:
	CAMERA(void);

	//////////////////////////////Вызываемые пользователем////////////////////////////////////////////////////
#if (defined (CAMERA1) || defined (FROM_FILE1)) 
	IplImage* frame1;
	double getX(void);
	double getY(void);
	bool getExistObj(void);
	void find1 (void);
#endif

	void WriteFrame(void);//Запись кадров с алгоритмом. Вызывается в конце while-цикла обработки кадров.
	void CVRelease (void);//Освобождение памяти. Вызывается в конце программы, или когда можно освободить память после видео.
	//////////////////////////////////////////////////////////////////////////////////////////////////////////

};