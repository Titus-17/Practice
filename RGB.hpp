#ifndef RGB_h
#define RGB_h

class RGB
{
private:
	int R;
	int G;
	int B;
public:
	RGB(int r,int g,int b);
	rect find_zone_edges(IplImage* image);
	int find_objects_edges(IplImage* image);	
};

#endif