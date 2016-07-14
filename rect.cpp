#include <iostream>
#include "rect.hpp"

rect::rect()
{
	point zero;
	zero.x = 0;
	zero.y = 0;
	a = zero;
	width = 0;
	height = 0;
}
rect::rect(int q, int s, int w, int h)
{
	a.x = q;
	a.y = s;
	width = w;
	height = h;
}
rect::rect(point b, int w, int h):a(b), width(w), height(h){}
void rect::set_value(int q, int s, int w, int h)
{
	a.x = q;
	a.y = s;
	width = w;
	height = h;
}
void rect::show_value()
{
	std::cout << "a.x = " << a.x;
	std::cout << "\na.y = " << a.y;
	std::cout << "\nwidth = " << width;
	std::cout << "\nheight = " << height << "\n";
}
int rect::get_x()
{
	return a.x;
}
int rect::get_y()
{
	return a.y;
}
int rect::get_width()
{
	return width;
}
int rect::get_height()
{
	return height;
}
rect rect::find_left_part()								//Делит прямоугольник пополам и возвращает атрибуты его левой половины
{
	point s;
	s.x = a.x;
	s.y = a.y;

	rect alpha(s, width/2, height);
	return alpha;
}
rect rect::find_right_part()							//Делит прямоугольник пополам и возвращает атрибуты его правой половины
{
	point s;
	s.x = a.x + width/2;
	s.y = a.y;

	rect alpha(s,width/2,height);
	return alpha;
}