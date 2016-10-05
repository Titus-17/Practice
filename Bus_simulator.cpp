#include <GL/glut.h>    // Header File for The GLUT Library 
#include <GL/gl.h>      // Header File for The OpenGL32 Library
#include <GL/glu.h>	    // Header File for The GLu32 Library
#include <unistd.h>     // Header file for sleeping.
#include <stdio.h>      // Header file for standard file i/o.
#include <stdlib.h>     // Header file for malloc/free.
#include <fstream>
#include <math.h>
#include <iostream>
#include "House.hpp"
#include "GLFunc.hpp"

extern int window;

int main(int argc, char **argv) 
{
    
    filescan();
    
    glutInit(&argc, argv);  

    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);  

    /* get a 640 x 480 window */
    glutInitWindowSize(640, 480);  

    /* the window starts at the upper left corner of the screen */
    glutInitWindowPosition(0, 0);  

    /* Open a window */  
    window = glutCreateWindow("Window 1");  

    /* Register the function to do all our OpenGL drawing. */
    glutDisplayFunc(&DrawGLScene); 

    /* Go fullscreen.  This is as soon as possible. */
    // glutFullScreen();

    /* Even if there are no events, redraw our gl scene. */
    glutIdleFunc(&DrawGLScene);

    /* Register the function called when our window is resized. */
    glutReshapeFunc(&ReSizeGLScene);

    /* Register the function called when the keyboard is pressed. */
    glutKeyboardFunc(&keyPressed);

    /* Register the function called when special keys (arrows, page down, etc) are pressed. */
    glutSpecialFunc(&specialKeyPressed);

    /* Initialize our window. */
    InitGL(640, 480);
  
    /* Start Event Processing Engine */  
    glutMainLoop();  

    return 1;
}