// #include "stdafx.h"

#include "find.h"
// int Hmin = 110;
// int Hmax = 256;

// int Smin = 196;
// int Smax = 256;

// int Vmin = 180;
// int Vmax = 256;

int Hmin = 163;
int Hmax = 256;

int Smin = 13;
int Smax = 256;

int Vmin = 191;
int Vmax = 256;

int HSVmax = 256;
CAMERA::CAMERA(void)
{
	CaptureVideo (); //Функция захвата видео с камер.
	GenerationVideoFileName(); //Генерация названий видеофайлов и изображений.
	InitializationVideoEditors();//Инициализация структур редакторов для записи видео.
#if (defined (CAMERA1) || defined (FROM_FILE1)) 
	frame1=0;
	ExistObj=false, X=0, Y=0;
#endif
}
/* ======================================================================
** Функция поиска объекта.
** ======================================================================
**void find11 вычисляет:
//ExistObj - Есть или нет в кадре объект.
//X - Координата найденного объекта по Х.
//Y - Координата найденного объекта по Y.
** ======================================================================
*/
#if (defined (CAMERA1) || defined (FROM_FILE1))
void CAMERA::find11 (void)
{
	//--------------------------------Область алгоритма---------------------------------------------------
	
	hsv = cvCreateImage( cvGetSize(frame1), IPL_DEPTH_8U, 3 );
	h_plane = cvCreateImage( cvGetSize(frame1), IPL_DEPTH_8U, 1 );
	s_plane = cvCreateImage( cvGetSize(frame1), IPL_DEPTH_8U, 1 );
	v_plane = cvCreateImage( cvGetSize(frame1), IPL_DEPTH_8U, 1 );
	h_range = cvCreateImage( cvGetSize(frame1), IPL_DEPTH_8U, 1 );
	s_range = cvCreateImage( cvGetSize(frame1), IPL_DEPTH_8U, 1 );
	v_range = cvCreateImage( cvGetSize(frame1), IPL_DEPTH_8U, 1 );
	hsv_and = cvCreateImage( cvGetSize(frame1), IPL_DEPTH_8U, 1 );
	//  Конвертируем в HSV.
	cvCvtColor( frame1, hsv, CV_BGR2HSV ); 
	// Разбиваем на отдельные каналы.
	cvCvtPixToPlane( hsv, h_plane, s_plane, v_plane, 0 );

#ifdef SHOW_WINDOWS
	// Показываем картинку.
	cvShowImage("original", frame1);
#endif
	// Выполняем пороговое преобразование.
	cvInRangeS(h_plane, cvScalar(Hmin), cvScalar(Hmax), h_range);
	cvInRangeS(s_plane, cvScalar(Smin), cvScalar(Smax), s_range);
	cvInRangeS(v_plane, cvScalar(Vmin), cvScalar(Vmax), v_range);

#ifdef SHOW_WINDOWS
	// Показываем результат.
	cvShowImage("H range", h_range);
	cvShowImage("S range", s_range);
	cvShowImage("V range", v_range);
#endif

	// Обнуляем.
	cvZero(hsv_and);

	// Складываем.
	cvAnd(h_range, s_range, hsv_and);
	cvAnd(hsv_and, v_range, hsv_and);

#ifdef SHOW_WINDOWS
	cvShowImage( "hsv and", hsv_and );
#endif

	int counter = 0; // Счётчик числа белых пикселей.

	// Пробегаемся по пикселям изображения.
	for(int y=0; y<hsv_and->height; y++)
	{
		uchar* ptr = (uchar*) (hsv_and->imageData + y * hsv_and->widthStep);
		for(int x=0; x<hsv_and->width; x++)
		{
			if( ptr[x]>0 )
			{
				X += x;
				Y += y;
				counter++; 
			}
		}
	}
	if (counter>=10)
	{
		ExistObj=true;
		float fwx2 = (float)(frame1->width/2);
		float fwy2 = (float)(frame1->height/2);
		X = float(X)/counter;
		Y = float(Y)/counter;
		int Radius=10;
		CvPoint pt = cvPoint( cvRound( X ), cvRound( Y ) );
			CvSize   axes = cvSize(Radius, Radius);
			cvEllipse(
				frame1,
				pt, // Центр
				axes, // Длина большой и малой оси
				0, // Угол от оси X, против часовой стрелки
				0, // Начальный угол
				360, // Конечный угол
				CV_RGB(0,255,0), // Цвет
				3
				);
		X = (X - fwx2)/ fwx2;
		Y = (Y - fwy2)/ fwy2;
	}
	cvReleaseImage(&hsv);
	cvReleaseImage(&h_plane);
	cvReleaseImage(&s_plane);
	cvReleaseImage(&v_plane);
	cvReleaseImage(&h_range);
	cvReleaseImage(&s_range);
	cvReleaseImage(&v_range);
	cvReleaseImage(&hsv_and);
	//----------------------------------------------------------------------------------------------------
	return;
}
void CAMERA::find1 (void)
{
	X=0, Y=0, ExistObj=false;
	//--------------------Первая (верхняя) камера - поиск и при необходимости запись------------------
#if (defined (CAMERA1) || defined (FROM_FILE1))
	//Получение кадра.
	frame1 = cvQueryFrame(capture1);
	if(!frame1)
	{
#ifdef OUTPUT_CONSOLE
		printf("Frame for camera 1 is not received.");
#endif
		return;
	}
	//Запись необработанного кадра.
#if (defined(WRITE_CAMERA1) && (defined (CAMERA1) || defined (FROM_FILE1))) 
	cvWriteFrame(writer1, frame1);
#endif
	find11 ();

	//Вывод результатов поиска.
#ifdef OUTPUT_CONSOLE_FRAME_RES
	if (ExistObj)
		printf ("\nExist = TRUE, Ux = %.4lf, Uy = %.4lf \n", X, Y);
	else
		printf ("\nExist = FALSE, Ux = %.4lf, Uy = %.4lf \n", X, Y);
#endif
#endif
	//----------------------------------------------------------------------------------------------------
}
#endif



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//-------------------Функция проверки существования файла---------------------------------------------
bool CAMERA::exists(const char *fname)
{
	FILE *file;
	if (file = fopen(fname, "r"))
	{
		fclose(file);
		return true;
	}
	return false;
}
//----------------------------------------------------------------------------------------------------

void CAMERA::CaptureVideo (void)
{
	//-------------------------Захват видео с камер (или из файла)----------------------------------------
#if (defined (CAMERA1))
	capture1 = cvCaptureFromCAM(NCam1);
	if (!capture1)
	{
#ifdef OUTPUT_CONSOLE
		printf("Video from the first camera is not captured.");
#endif
		return;//Захвата видеопотока не произошло.
	}
#endif

#if defined (FROM_FILE1)
	char* file1 ="d:\\11Oct\\laser1\\1.avi";
#ifdef OUTPUT_CONSOLE
	printf("File1: %s\n", file1);
#endif
	capture1 = cvCreateFileCapture (file1);
	if (!capture1)
	{
#ifdef OUTPUT_CONSOLE
		printf("Video from the first camera is not captured.");
#endif
		return;//Захвата видеопотока не произошло.
	}
#endif
	//----------------------------------------------------------------------------------------------------
}

void CAMERA::GenerationVideoFileName (void)
{
	//--------------Генерация названий видеофайлов и калибровочных изображений для записи-----------------
#if (defined (WRITE_ALG_CAMERA1) || defined (WRITE_CAMERA1))

	int nVideoFile = 0;
	bool FileName = false;
	while(!FileName)
	{
		sprintf(filename_alg_c1, "/root/Flight_%d_cam1_alg.avi", nVideoFile);
		sprintf(filename_c1, "/root/Flight_%d_cam1.avi", nVideoFile);

		if (exists(filename_alg_c1) || exists(filename_c1))
		{
			nVideoFile++;
		}
		else 
		{
			FileName = true;
		}
	}
#endif
	//----------------------------------------------------------------------------------------------------
}

void CAMERA::InitializationVideoEditors(void)
{
	//-------------------------Инициализация структур редакторов для записи видео-------------------------
#if (defined (WRITE_ALG_CAMERA1) || defined (WRITE_CAMERA1))
	//double fps1 = cvGetCaptureProperty (capture1, CV_CAP_PROP_FPS);
	//Почему-то запись работает только с частотой записи 15 кадров в секунду.
	fps1 = 15;
	size1 = cvSize( (int)cvGetCaptureProperty( capture1, CV_CAP_PROP_FRAME_WIDTH), (int)cvGetCaptureProperty( capture1, CV_CAP_PROP_FRAME_HEIGHT));

#ifdef WRITE_ALG_CAMERA1
	writer_alg1 = cvCreateVideoWriter(filename_alg_c1, CV_FOURCC('X','V','I','D'), fps1, size1, 1);
	assert(writer_alg1!=0);
#endif
#if defined WRITE_CAMERA1
	writer1 = cvCreateVideoWriter(filename_c1, CV_FOURCC('X','V','I','D'), fps1, size1, 1);
	assert(writer1!=0);
#endif

#endif
	//----------------------------------------------------------------------------------------------------
}

void CAMERA::ShowWindow(void)
{
	//-------------------------------------Показ окон-----------------------------------------------------
#ifdef SHOW_WINDOWS
#if (defined( CAMERA1) || defined (FROM_FILE1))
	const char *windowName1 = "WindowCamera1";
	cvNamedWindow(windowName1, CV_WINDOW_AUTOSIZE);
	cvShowImage(windowName1, frame1);
#endif
#endif
	//----------------------------------------------------------------------------------------------------
}

void CAMERA::WriteFrame(void)
{
	//-------------------------------------Запись кадра с алгоритмом--------------------------------------
#ifdef WRITE_ALG_CAMERA1
	cvWriteFrame(writer_alg1, frame1);
#endif
#ifdef SHOW_WINDOWS
	ShowWindow(); //Показ окошек.
#endif
	//----------------------------------------------------------------------------------------------------
}

void CAMERA::CVRelease (void)
{
	//-------------Освобождение памяти, разрушение окон и т.д.--------------------------------------------
#if (defined (CAMERA1) || defined (FROM_FILE1))
	cvReleaseCapture( &capture1 );
#ifdef WRITE_ALG_CAMERA1
	cvReleaseVideoWriter(&writer_alg1);
#endif
#ifdef WRITE_CAMERA1
	cvReleaseVideoWriter(&writer1);
#endif
#endif

#ifdef SHOW_WINDOWS
	cvDestroyAllWindows();
#endif
	//----------------------------------------------------------------------------------------------------
}

//------------------------------Возврат полученных переменных-----------------------------------------
#if (defined (CAMERA1) || defined (FROM_FILE1)) 
double CAMERA::getX(void) { return X; }
double CAMERA::getY(void) { return Y; }
bool CAMERA::getExistObj(void) { return ExistObj; }
#endif
//----------------------------------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


