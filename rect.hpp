#ifndef rect_h
#define rect_h

struct point
{
	int x;
	int y;
};

class rect
{
private:
	point a;
	int width;
	int height;
public:
	rect();
	rect(int q, int s, int w, int h);
	rect(point b, int w, int h);
	void set_value(int q, int s, int w, int h);
	void show_value();
	int get_x();
	int get_y();
	int get_width();
	int get_height();

	rect find_left_part();
	rect find_right_part();
};


#endif