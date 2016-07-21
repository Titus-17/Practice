#include <stdio.h>
#include <cstdio>
#include <unistd.h>
#include <cv.h>
#include <highgui.h>
#include <stdlib.h>
#include "rect.hpp"
#include "RGB.hpp"
#include "Func.hpp"

using namespace std;

void create_window(IplImage* image,char *name, rect in)	//Выпрезает из изображения image прямоугольник с атрибутами in, создает
{														//окно с вырезанным изображением и сохраняет под именем name
	cvSetImageROI(image, cvRect(in.get_x(), in.get_y(), in.get_width(), in.get_height()));
		
	IplImage *sub_img = cvCreateImage(cvGetSize(image), image->depth, image->nChannels);
	cvCopy(image, sub_img, NULL);
	cvSaveImage(name, sub_img);
	cvResetImageROI(image);
	cvNamedWindow(name ,CV_WINDOW_AUTOSIZE);
	cvShowImage(name,sub_img);
	cvWaitKey(0);
	cvReleaseImage(&sub_img);
	cvDestroyWindow(name);
}
void create_image(IplImage* image,char *name, rect in)
{
	cvSetImageROI(image, cvRect(in.get_x(), in.get_y(), in.get_width(), in.get_height()));

	IplImage *sub_img = cvCreateImage(cvGetSize(image), image->depth, image->nChannels);
	cvCopy(image, sub_img, NULL);
	cvSaveImage(name, sub_img);
	cvResetImageROI(image);
	cvReleaseImage(&sub_img);
}
void set_status(const char *img_name, const char *dir, RGB dev, const char *status, const char *developer)
{
	IplImage* image = 0;
	IplImage* src = 0;

	image = cvLoadImage(img_name, 1);
	
	int z = dev.find_objects_edges(image);
	if (z != 0)
	{
		chdir(dir);
		for (int i = 0; i < z; i ++)
		{
			FILE *fout;
			FILE *fin;

			char string[400];
			char zbar[200];
			int j = 0;
			fout = fopen("../text", "r");
			fin = fopen("../status.csv", "a");
			create_image(image,(char*)img_name, END[i]);
			sprintf(zbar, "zbarimg %s >../text", img_name);
			system(zbar);
			fgets(string, 200, fout);
			if (string[0] == 'Q')
			{	
				fseek(fout , 8 , SEEK_SET );				//Переносим курсор вперед, чтобы не считывать ненужные символы от zbar'а
				fgets(string, 200, fout);

				while(string[j] != '\0')
					j++;
				string[j-1] = '\0';							//Убираем перенос строки
				fputs("Task: ", fin);
				fputs(string, fin);
				fputs(status, fin);
				fputs(developer, fin);
				
				strcat(string, ".jpg");
				printf("%s\n", string);
				create_image(image,(char*)string,END[i]);
				fclose(fout);
				fclose(fin);
			}
			else printf("Ошибка!\n");
		}
		char del[100];
		sprintf(del, "rm %s",img_name);
		system(del);

		chdir("..");
	}
	else printf("Стикеров не найдено!\n");
}