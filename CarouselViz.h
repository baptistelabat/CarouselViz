/*
*   Carousel Visualizer
*   This program shows a 3-D HIGHWIND-carousel with BabyBetty attached
*
*   Copyright 2015, University of Freiburg
*   Systems Theory Lab
*   Authors: Elias Rosch <eliasrosch@googlemail.com> , Ben Schleusener <ben.schleusener@gmail.com>
*   
*/

#ifndef CAROUSELVIZ_H
#define CAROUSELVIZ_H


// OTHER_IP should be the address of the BeagleBone Black
//#define OTHER_IP "10.42.0.42"
//#define OTHER_PORT 8080
//#define OWN_PORT 8080
//#define WINDOW_LEN 500

using namespace std;


class CarouselViz {
    public:
        //Constructor/Destructor
        CarouselViz();
        ~CarouselViz();
        //Called when a key is pressed
        static void handleKeypress(unsigned char key, int x, int y);
 
        //Initializes the CarouselViz
        static void initFunc();

        //Initializes 3D rendering
        static void initRendering();
 
        //Called when the window is resized
        static void handleResize(int w, int h);

        //Draws the 3D scene
        static void drawScene();

        //Called when there is nothing to draw or update
        static void idleFunc(void);
 
        //Called to update the animation
        static void updateFunc(int value);

    private:
};

#endif // CAROUSELVIZ_H
