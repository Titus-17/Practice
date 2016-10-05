#include <GL/glut.h>    // Header File For The GLUT Library 
#include <GL/gl.h>		// Header File For The OpenGL32 Library
#include <GL/glu.h>		// Header File For The GLu32 Library

#include <unistd.h>  // Header file for sleeping.
#include <stdio.h>    // Header file for standard file i/o.
#include <stdlib.h>  // Header file for malloc/free.

#include <fstream>
#include <iostream>

#include <ctime>

#include "GLFunc.hpp"
#include "House.hpp"

using namespace std;

 
/*ASCII коды для специальных кнопок*/
#define ESCAPE 27
#define PAGE_UP 73
#define PAGE_DOWN 81
#define UP_ARROW 72
#define DOWN_ARROW 80
#define LEFT_ARROW 75
#define RIGHT_ARROW 77

#define FPS_TIME_DELAY 0.0165       // Время, которое должно пройти, чтобы частота обновления кадров была 60/с: 1/60 = 0.0165.

int window;
int j;
float FPS = 0;

GLfloat yrot;                       // y rotation 
GLfloat yspeed;                     // y rotation speed
GLfloat z = -5;  		            // depth into the screen.
GLfloat ylr;
GLuint  filter;                     /* Which Filter To Use (nearest/linear/mipmapped) */
GLuint  texture[3];                 /* Storage for 3 textures. */
GLuint  texture2[3];

clock_t start_time =  clock();      // Начальное время для замера фпс
clock_t end_time;                   // Конечное время для замера фпс
clock_t start_wait;                 // Начальное время для ограничения фпс 
clock_t end_wait;                   // Конечное время для ограничения фпс

House *H = new House[getCount()]();

void filescan()
{
    int i = 0;
    fstream file;
    ifstream fin("map.csv");
    string mas[10];

    while(!fin.eof())
    {
        getline(fin, mas[0], ',');
        H[i].setx(atoi(mas[0].c_str()));

        getline(fin, mas[1], ',');
        H[i].sety(atoi(mas[1].c_str()));

        getline(fin, mas[2], ',');
        H[i].setz(atoi(mas[2].c_str()));

        getline(fin, mas[3], ',');
        H[i].setw(atoi(mas[3].c_str()));

        getline(fin, mas[4], ',');
        H[i].setl(atoi(mas[4].c_str()));

        getline(fin, mas[5], '\n');
        H[i].seth(atoi(mas[5].c_str()));
        i++;
    }
}

int getCount()
{
    fstream file;
    ifstream fin("map.csv");
    int i = 0;
    string temp;
    while(!fin.eof())
    {
        getline(fin, temp, '\n');
        i++;
    }
    
    fin.close();
    return i;   
}

float FPS_show()
{
    start_wait = clock();
    while ((float)(end_wait - start_wait)/CLOCKS_PER_SEC <= FPS_TIME_DELAY)         //Ограничение FPS до 60 кадров в секунду : 1/60 = 0.0165
        end_wait = clock();

    ++FPS;
    end_time = clock();
    if ((float)(end_time - start_time)/CLOCKS_PER_SEC >= 1)                         //Подсчет FPS
    {
        std::cout << (float)(end_time - start_time)/CLOCKS_PER_SEC << "\n";
        std::cout << "FPS = " << FPS << "\n";
        FPS = 0;
        start_time =  clock();
    }
    return FPS;
}

int ImageLoad(const char *filename, Image *image)
{
    FILE *file;
    unsigned long size;             // Размер изображения в байтах
    unsigned long i;                // Счетчик
    unsigned short int planes;      // Количество слоев на изображении (должно быть = 1) 
    unsigned short int bpp;         // Количество байт на пиксель (должно быть = 24)
    char temp;                      // Используется для конвертирования bgr в rgb.

    // make sure the file is there.
    if ((file = fopen(filename, "rb")) == NULL)
    {
        cout << "File Not Found : " << filename << "\n";
        return 0;
    }
    
    // seek through the bmp header, up to the width/height:
    fseek(file, 18, SEEK_CUR);

    // read the width
    if ((i = fread(&image->sizeX, 4, 1, file)) != 1)
    {
        cout << "Error reading width from " << filename << "\n";
        return 0;
    }

    cout << "Width of " << filename << image->sizeX << "\n";
    // read the height 
    if ((i = fread(&image->sizeY, 4, 1, file)) != 1)
    {
        cout << "Error reading height from " << filename << "\n";
        return 0;
    }
    printf("Weight of %s: %lu\n", filename, image->sizeY);
    
    // calculate the size (assuming 24 bits or 3 bytes per pixel).
    size = image->sizeX * image->sizeY * 3;

    // read the planes
    if ((fread(&planes, 2, 1, file)) != 1)
    {
        cout << "Error reading planes from " << filename << "\n";
        return 0;
    }
    if (planes != 1)
    {
        cout << "Planes from " << filename << " is not 1: " << planes << "\n"; 
        return 0;
    }

    // read the bpp
    if ((i = fread(&bpp, 2, 1, file)) != 1)
    {
        cout << "Error reading bpp from" << filename << "\n";
        return 0;
    }

    if (bpp != 24) {
        cout << "Bpp from " << filename <<  "is not 24: " << bpp << "\n";
        return 0;
    }
    
    // seek past the rest of the bitmap header.
    fseek(file, 24, SEEK_CUR);

    // read the dat
    image->data = (char *) malloc(size);
    if (image->data == NULL)
    {
        cout << "Error allocating memory for color-corrected image data" << "\n";
        return 0;   
    }

    if ((i = fread(image->data, size, 1, file)) != 1)
    {
        cout << "Error reading image data from " << filename << "\n"; 
        return 0;
    }

    for (i = 0;i < size; i += 3)// reverse all of the colors. (bgr -> rgb)
    { 
        temp = image->data[i];
        image->data[i] = image->data[i+2];
        image->data[i+2] = temp;
    }
    return 1;
}

/* The function called when our window is resized (which shouldn't happen, because we're fullscreen) */
GLvoid ReSizeGLScene(GLsizei Width, GLsizei Height)
{
    if (Height == 0)              			// Prevent A Divide By Zero If The Window Is Too Small
    	Height = 1;

    glViewport(0, 0, Width, Height);        // Reset The Current Viewport And Perspective Transformation

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(45.0f,(GLfloat)Width/(GLfloat)Height,0.1f,100.0f);
    glMatrixMode(GL_MODELVIEW);
}

// Load Bitmaps And Convert To Textures
GLvoid LoadGLTextures()
{
    // Load Texture
    Image *image1;
    Image *image2;
    // allocate space for texture
    image1 = (Image *) malloc(sizeof(Image));
    image2 = (Image *) malloc(sizeof(Image));
    if (image1 == NULL)
    {
        cout << "Error allocating space for image" << endl;
        exit(0);
    }
    if (image2 == NULL)
    {
        cout << "Error allocating space for image" << endl;
        exit(0);
    }

    if (!ImageLoad("Data/square,attend_me.bmp", image2))
    {
        exit(1);
    }   
    if (!ImageLoad("Data/Asphalt.bmp", image1))
    {
        exit(1);
    }   
    // Create Textures  
    glGenTextures(3, &texture[0]);

    // texture 1 (poor quality scaling)
    glBindTexture(GL_TEXTURE_2D, texture[0]);   // 2d texture (x and y size)

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT); // cheap scaling when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT); // cheap scaling when image smalled than texture


    // 2d texture, level of detail 0 (normal), 3 components (red, green, blue), x size from image, y size from image, 
    // border 0 (normal), rgb color data, unsigned byte data, and finally the data itself.
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image1->sizeX, image1->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image1->data);

    // texture 2 (linear scaling)
    glBindTexture(GL_TEXTURE_2D, texture[1]);   // 2d texture (x and y size)
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image1->sizeX, image1->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image1->data);

    // texture 3 (mipmapped scaling)
    glBindTexture(GL_TEXTURE_2D, texture[2]);   // 2d texture (x and y size)
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image1->sizeX, image1->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image1->data);

    // 2d texture, 3 colors, width, height, RGB in that order, byte data, and the dat
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, image1->sizeX, image1->sizeY, GL_RGB, GL_UNSIGNED_BYTE, image1->data); 
    //_______________________________________________//
    // Create 2 Textures  

    glGenTextures(3, &texture2[0]);
    
    // texture 1 (poor quality scaling)
         glBindTexture(GL_TEXTURE_2D, texture2[0]);   // 2d texture (x and y size)
         glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT); // cheap scaling when image bigger than texture
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT); // cheap scaling when image smalled than texture

    // 2d texture, level of detail 0 (normal), 3 components (red, green, blue), x size from image, y size from image, 
    // border 0 (normal), rgb color data, unsigned byte data, and finally the data itself.
         glTexImage2D(GL_TEXTURE_2D, 0, 3, image2->sizeX, image2->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image2->data);

    // texture 2 (linear scaling)
        glBindTexture(GL_TEXTURE_2D, texture2[0]);   // 2d texture (x and y size)
         glTexImage2D(GL_TEXTURE_2D, 0, 3, image2->sizeX, image2->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image2->data);

    // texture 3 (mipmapped scaling)
        glBindTexture(GL_TEXTURE_2D, texture2[0]);   // 2d texture (x and y size)
        glTexImage2D(GL_TEXTURE_2D, 0, 3, image2->sizeX, image2->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image2->data);

    // 2d texture, 3 colors, width, height, RGB in that order, byte data, and the data
        gluBuild2DMipmaps(GL_TEXTURE_2D, 3, image2->sizeX, image2->sizeY, GL_RGB, GL_UNSIGNED_BYTE, image2->data); 
}

/* A general OpenGL initialization function.  Sets all of the initial parameters. */
GLvoid InitGL(GLsizei Width, GLsizei Height)			// We call this right after our OpenGL window is created.
{
    LoadGLTextures();                     		// load the textures.
    glEnable(GL_TEXTURE_2D);                    		// Enable texture mapping.

    glClearColor(0.5, 0.78, 1, 0);						// This Will Clear The Background Color To Black
    glClearDepth(1);									// Enables Clearing Of The Depth Buffer
    glDepthFunc(GL_LESS);								// The Type Of Depth Test To Do
    glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();									// Reset The Projection Matrix
    gluLookAt(0, 0, 0, 1, 1, 1, 0, 1, 0); 
    
    gluPerspective(90, (GLfloat)Width/(GLfloat)Height, 0, 100);	// Calculate The Aspect Ratio Of The Window
    
    glMatrixMode(GL_MODELVIEW);
}

/* The main drawing function. */
GLvoid DrawGLScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
    glLoadIdentity();									// Reset The View
    GLfloat yl = 360 - yrot;
    glTranslatef(0, 0, z);								// move z units out from the screen.
    // glTranslatef(ylr, 0, 0);
    // glTranslatef(yspeed, 0, 0);						// move z units out from the screen.
    glRotatef(yl, 0, 1, 0);								// Rotate On The Y Axis
    glBindTexture(GL_TEXTURE_2D, texture2[filter]);		// choose the texture to use.
   
	for (j = 0; j < getCount(); j++)
		H[j].BuildHouse();

    glBindTexture(GL_TEXTURE_2D, texture[filter]);    // choose the texture to use.
    
    glBegin(GL_QUADS);									//Отрисовка сцены
    	glColor3f(0.36, 0.63, 0.18);
        glTexCoord2f(1, 1); glVertex3f(75, -1, 75);		// Top Right Of The Texture and Quad
        glTexCoord2f(0, 1); glVertex3f(-75, -1, 75);	// Top Left Of The Texture and Quad
        glTexCoord2f(0, 0); glVertex3f(-75, -1, -75);	// Bottom Left Of The Texture and Quad
        glTexCoord2f(1, 0); glVertex3f(75, -1, -75);	// Bottom Right Of The Texture and Quad
    glEnd();											// done with the polygon.
    yrot += yspeed;										// Y Axis Rotation

    
    // since this is double buffered, swap the buffers to display what just got drawn.
    glutSwapBuffers();
    FPS_show();
}

/* The function called whenever a normal key is pressed. */
void keyPressed(unsigned char key, int x, int y) 
{
    switch (key)
    {   
        case ESCAPE:
    	   glutDestroyWindow(window); 	
    	   exit(1);                   	
    	break;

        default:
    	break;
    }
}

/* The function called whenever a normal key is pressed. */
void specialKeyPressed(int key, int x, int y)
{
    /* avoid thrashing this procedure */
    usleep(100);

    switch (key)
    {   
        case GLUT_KEY_DOWN: // move the cube into the distance.
    	   z -= 0.2f;
    	break;
        
        case GLUT_KEY_UP: // move the cube closer.
    	   z += 0.2f;
    	break;
        
       /* case GLUT_KEY_RIGHT: // rotate the cube left.
        if (yspeed < 0)
            yspeed = 0;
        else yspeed=+0.5f;
        
        break;

        case GLUT_KEY_LEFT: // rotate the cube right.
        if (yspeed > 0)
            yspeed = 0;
        else yspeed=-0.5f;
        break;
        */
        case GLUT_KEY_LEFT: // rotate the cube right.
            if (yspeed < 0)
                yspeed = 0;
            else
                yspeed += 0.1;
        break;

        case GLUT_KEY_RIGHT: // rotate the cube right.
            if (yspeed > 0)
                yspeed = 0;
            else
                yspeed -= 0.1;
        break;
       
        default:
            yspeed = 0;
    	break;
    }	
}