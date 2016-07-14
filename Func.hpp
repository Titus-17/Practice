#ifndef Myclass_h
#define Myclass_h

void create_window(IplImage* image,char *name, rect in);
void create_image(IplImage* image,char *name, rect in);
void set_status(const char *img_name, const char *dir, RGB dev, const char *status, const char *developer);

extern rect END[20];

#endif
