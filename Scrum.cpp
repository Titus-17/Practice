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
		}
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

	RGB pink(255,0,255);
	RGB red(255,0,0); 		//ptr[3*x];		// B - синий
	RGB green(0,255,0);		//ptr[3*x+1];	// G - зелёный
	RGB blue(0,0,255);		//ptr[3*x+2];  	// R - красный
	RGB yellow(255,255,0);

	point start;
	start.x = 0;
	start.y = 0;

	rect pink_out(start, 0, 0);
	rect pink_left(start, 0, 0);
	rect pink_right(start, 0, 0);

	rect red_out(start, 0, 0);
	rect red_left(start, 0, 0);
	rect red_right(start, 0, 0);

	rect green_out(start, 0, 0);
	rect green_left(start, 0, 0);
	rect green_right(start, 0, 0);

	rect blue_out(start, 0,0);
	rect blue_left(start, 0, 0);
	rect blue_right(start, 0, 0);

	if (chdir("Red_Zone_Stickers")) system("mkdir Red_Zone_Stickers");
	else chdir("..");
	if (chdir("Pink_Zone_Stickers")) system("mkdir Pink_Zone_Stickers");
	else chdir("..");
	if (chdir("Green_Zone_Stickers")) system("mkdir Green_Zone_Stickers");
	else chdir("..");
	if (chdir("Blue_Zone_Stickers")) system("mkdir Blue_Zone_Stickers");
	else chdir("..");

	pink_out = pink.find_zone_edges(image);

	red_out = red.find_zone_edges(image);
	red_left = red_out.find_left_part();
	red_right = red_out.find_right_part();

	green_out = green.find_zone_edges(image);
	green_left = green_out.find_left_part();
	green_right = green_out.find_right_part();

	blue_out = blue.find_zone_edges(image);
	blue_left = blue_out.find_left_part();
	blue_right = blue_out.find_right_part();

	create_image(image,(char* )"pink_zone.jpg", pink_out);

	create_image(image,(char* )"red_left_zone.jpg", red_left);
	create_image(image,(char* )"red_right_zone.jpg", red_right);

	create_image(image,(char* )"green_left_zone.jpg", green_left);
	create_image(image,(char* )"green_right_zone.jpg", green_right);

	create_image(image,(char* )"blue_left_zone.jpg", blue_left);
	create_image(image,(char* )"blue_right_zone.jpg", blue_right);

	system("rm status.csv");
	set_status("red_left_zone.jpg","Red_Zone_Stickers", red, ",Status: in progress", ",Developer: red\n");
	set_status("pink_zone.jpg", "Pink_Zone_Stickers", pink, ",Status: in pool", ",Developer: none\n");
	set_status("green_left_zone.jpg", "Green_Zone_Stickers", green, ",Status: in progress,", "Developer: green\n");
	set_status("blue_left_zone.jpg", "Blue_Zone_Stickers", blue, ",Status: in progress,", "Developer: blue\n");
	set_status("red_right_zone.jpg", "Red_Zone_Stickers", red, ",Status: done,", "Developer: red\n");
	set_status("green_right_zone.jpg", "Green_Zone_Stickers", green, ",Status: done,", "Developer: green\n");
	set_status("blue_right_zone.jpg", "Blue_Zone_Stickers", blue, ",Status: done,", "Developer: blue\n");

	cvReleaseImage(&image);
	cvReleaseImage(&src);
	return 0;
}