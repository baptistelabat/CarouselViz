/*
*   Carousel Visualizer
*   This program shows a 3-D HIGHWIND-carousel with BabyBetty attached
*
*   Copyright 2015, University of Freiburg
*   Systems Theory Lab
*   Authors: Elias Rosch <eliasrosch@googlemail.com> , Ben Schleusener <ben.schleusener@gmail.com>
*   
*/


#include <GL/glut.h>
#include "./CarouselViz.h"

int main(int argc, char** argv) {
    //Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 800);
 
    //Create the window
    glutCreateWindow("The very-fun-to-play carousel game");
    CarouselViz::initFunc();
    //Set handler functions
    glutDisplayFunc(CarouselViz::drawScene);
    glutReshapeFunc(CarouselViz::handleResize);
    //Define what to do while idlephase (receive UDP-Packages)
    glutIdleFunc(CarouselViz::idleFunc);
    //Add a timer
    glutTimerFunc(25, CarouselViz::updateFunc, 0);
    glutKeyboardFunc(CarouselViz::handleKeypress);
 
    glutMainLoop();
}
