#include <stdio.h>			//Для sprintf
#include <unistd.h>			//Для chdir		
#include <iostream>
#include <highgui.h>		//Для функций OpenCV
#include "rect.hpp"
#include "RGB.hpp"
#include "Func.hpp"

using namespace std;

int main(int argc, char* argv[])
{

	char *filename;
	if (argc>1)
	{
		filename = argv[1];
		FILE *fp;
		fp = fopen(filename, "r");
		if (fp == NULL)
		{
			printf("Error: cannot open file\n");
			return -1;
			fclose(fp);
		}
		fclose(fp);
	}
	else
	{ 
		printf("Error: empty file name\n");
		return -2;
	}


	IplImage* image = 0;
	IplImage* src = 0;

	image = cvLoadImage(filename,1);
	src = cvCloneImage(image);

	assert(src != 0);

	RGB pool(100, 40, 25, 30, 15, 25);
	RGB dev1(45, 115, 65, 15, 35, 25); 		//ptr[3*x];		// B - синий
	RGB dev2(40, 105, 132, 30, 35, 50);		//ptr[3*x+1];	// G - зелёный
	RGB dev3(175, 45, 20, 45, 25, 15);		//ptr[3*x+2];  	// R - красный
	
	point start;
	start.x = 0;
	start.y = 0;

	rect pool_out(start, 0, 0);
	rect pool_left(start, 0, 0);
	rect pool_right(start, 0, 0);

	rect dev1_out(start, 0, 0);
	rect dev1_left(start, 0, 0);
	rect dev1_right(start, 0, 0);

	rect dev2_out(start, 0, 0);
	rect dev2_left(start, 0, 0);
	rect dev2_right(start, 0, 0);

	rect dev3_out(start, 0,0);
	rect dev3_left(start, 0, 0);
	rect dev3_right(start, 0, 0);

	if (chdir("dev1_Zone_Stickers")) system("mkdir dev1_Zone_Stickers");
	else chdir("..");
	if (chdir("pool_Zone_Stickers")) system("mkdir pool_Zone_Stickers");
	else chdir("..");
	if (chdir("dev2_Zone_Stickers")) system("mkdir dev2_Zone_Stickers");
	else chdir("..");
	if (chdir("dev3_Zone_Stickers")) system("mkdir dev3_Zone_Stickers");
	else chdir("..");
	pool_out = pool.find_zone_edges(image);
	
	dev1_out = dev1.find_zone_edges(image);
	dev1_left = dev1_out.find_left_part();
	dev1_right = dev1_out.find_right_part();

	dev2_out = dev2.find_zone_edges(image);
	dev2_left = dev2_out.find_left_part();
	dev2_right = dev2_out.find_right_part();

	dev3_out = dev3.find_zone_edges(image);
	dev3_left = dev3_out.find_left_part();
	dev3_right = dev3_out.find_right_part();
	
	create_image(image,(char* )"pool_zone.jpg", pool_out);
	create_image(image,(char* )"dev1_left_zone.jpg", dev1_left);
	create_image(image,(char* )"dev1_right_zone.jpg", dev1_right);

	create_image(image,(char* )"dev2_left_zone.jpg", dev2_left);
	create_image(image,(char* )"dev2_right_zone.jpg", dev2_right);

	create_image(image,(char* )"dev3_left_zone.jpg", dev3_left);
	
	create_image(image,(char* )"dev3_right_zone.jpg", dev3_right);

	system("rm status.csv");
	set_status("pool_zone.jpg", "pool_Zone_Stickers", pool, ",Status: in pool", ",Developer: none\n");
	set_status("dev1_left_zone.jpg","dev1_Zone_Stickers", dev1, ",Status: in progress", ",Developer: dev1\n");
	set_status("dev2_left_zone.jpg", "dev2_Zone_Stickers", dev2, ",Status: in progress,", "Developer: dev2\n");
	set_status("dev3_left_zone.jpg", "dev3_Zone_Stickers", dev3, ",Status: in progress,", "Developer: dev3\n");
	set_status("dev1_right_zone.jpg", "dev1_Zone_Stickers", dev1, ",Status: done,", "Developer: dev1\n");
	set_status("dev2_right_zone.jpg", "dev2_Zone_Stickers", dev2, ",Status: done,", "Developer: dev2\n");
	set_status("dev3_right_zone.jpg", "dev3_Zone_Stickers", dev3, ",Status: done,", "Developer: dev3\n");

	cvReleaseImage(&image);
	cvReleaseImage(&src);
	return 0;
}
