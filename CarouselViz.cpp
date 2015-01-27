/*
*   Carousel Visualizer
*   This program shows a 3-D HIGHWIND-carousel with BabyBetty attached
*
*   Copyright 2015, University of Freiburg
*   Systems Theory Lab
*   Authors: Elias Rosch <eliasrosch@googlemail.com> , Ben Schleusener <ben.schleusener@gmail.com>
*   
*/

#include <math.h>
#include "./CarouselViz.h"
#include "./UDP.h"
#include "./Sensor.h"
#include "./glm.h"

// Define UDP variables
// OTHER_IP should be the address of the BeagleBone Black
#define OTHER_IP "10.42.0.42"
#define OTHER_PORT 8080
#define OWN_PORT 8080

// WINDOW_LEN defines the sliding window size
// Large Window --> smooth behavior
// Small Window --> agressive behavior
#define WINDOW_LEN 250

using namespace std;

bool view_mode = false;

// Current angle of the CarouselArm
float arm_angle = 0.0f;
float arm_angle_time = 0.0f;
float arm_len = 5.9f;
// Elevation angle between CarouselArm & Line
float line_angle = 0.0f;
float line_angle_time = 0.0f;
float line_len = 6.0f;
// Pitch angle of the plane
float plane_angle = 0.0f;
// azimuth and elevation for bubble
float bubble_elevation = -20.0f;
float bubble_azimuth = -90.0f;
float bubble_radius = 0.5f;

// Prepare space for the .obj models
GLMmodel* plane_model;
GLMmodel* plane_canope_model;
GLMmodel* arm_model;
GLMmodel* trailer_model;
GLMmodel* wheels_model;
GLMmodel* plate_model;

// Start struct receiving process
UDP my_udp;
SensorValues *my_values;

// This variables are to do a sliding window average of the angles
float window_line_angles[WINDOW_LEN];
float window_line_sum;
float window_plane_angles[WINDOW_LEN];
float window_plane_sum;
int window_len = WINDOW_LEN;



void CarouselViz::initFunc() {
    // Initialize UDP
    my_udp.initUDP(OTHER_IP, OTHER_PORT, OWN_PORT);
    // Load .obj files
    plane_model = glmReadOBJ((char *)"./objects/baby_betty_cularis.obj");
    glmScale(plane_model, 0.1f);
    plane_canope_model = glmReadOBJ((char *)"./objects/baby_betty_cularis_canope.obj");
    glmScale(plane_canope_model, 0.1f);
    arm_model = glmReadOBJ((char *)"./objects/arm.obj");
    glmScale(arm_model, 0.2f);
    trailer_model = glmReadOBJ((char *)"./objects/trailer.obj");
    glmScale(trailer_model, 0.2f);
    wheels_model = glmReadOBJ((char *)"./objects/wheels.obj");
    glmScale(wheels_model, 0.2f);
    plate_model = glmReadOBJ((char *)"./objects/plate.obj");
    glmScale(plate_model, 0.2f);
    // init the rendering
    initRendering();
}
 
//Initializes 3D rendering
void CarouselViz::initRendering() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING); //Enable lighting
    glEnable(GL_LIGHT0); //Enable light #0
    glEnable(GL_LIGHT1); //Enable light #1
    glEnable(GL_NORMALIZE); //Automatically normalize normals
    //glShadeModel(GL_SMOOTH); //Enable smooth shading
}
 
//Called when the window is resized
void CarouselViz::handleResize(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)w / (double)h, 1.0, 200.0);
}

//Called when a key is pressed
void CarouselViz::handleKeypress(unsigned char key, int x, int y) {
    switch (key) {
        case 27: //Escape key
            glmDelete(plane_model);
            exit(0);
        case 99: //Escape key
            view_mode = !view_mode;
    }
}

//Draws the 3D scene
void CarouselViz::drawScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
    glMatrixMode(GL_MODELVIEW);
 
    //glTranslatef(0.0f, 2.0f, 0.0f);
 
    //Add ambient light
    GLfloat ambientColor[] = {0.2f, 0.2f, 0.2f, 1.0f}; //Color (0.2, 0.2, 0.2)
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);
 
    //Add positioned light
    GLfloat lightColor0[] = {0.5f, 0.5f, 0.5f, 1.0f}; //Color (0.5, 0.5, 0.5)
    GLfloat lightPos0[] = {4.0f, 0.0f, -8.0f, 1.0f}; //Positioned at (4, 0, 8)
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
 
    
    //Add directed light
    GLfloat lightColor1[] = {0.35f, 0.35f, 0.2f, 1.0f}; //Color (0.5, 0.2, 0.2)
    //Coming from the direction (-1, 0.5, 0.5)
    GLfloat lightPos1[] = {-1.0f, 0.5f, 0.5f, 0.0f};
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor1);
    glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);


    glBegin(GL_QUADS);

    //Ground
    glColor3f(0.45f, 1.0f, 0.45f);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-40.0f, -8.0f, 40.0f);
    glVertex3f(40.0f, -8.0f, 40.0f);
    glVertex3f(40.0f, -8.0f, -40.0f);
    glVertex3f(-40.0f, -8.0f, -40.0f);
    glEnd();
    
    glPushMatrix();

    // Trailer, Wheels & Plate
    glPushMatrix();
    glRotatef(-90.0,1.0,0.0,0.0);
    glRotatef(90.0,0.0,0.0,1.0);
    glTranslatef(0.0,0.0,-5.5);
    glColor3f(0.9f, 0.9f, 0.9f);
    glmDraw(trailer_model,GLM_SMOOTH);
    glColor3f(0.2f, 0.2f, 0.2f);
    glmDraw(wheels_model,GLM_SMOOTH);
    glColor3f(0.54f, 0.27f, 0.07f);
    glTranslatef(0.0f,0.0f,0.01f);
    glmDraw(plate_model,GLM_SMOOTH);
    glPopMatrix();

    glRotatef(arm_angle, 0.0f, 1.0f, 0.0f);
    /*
    //Set the colour here
    glBegin(GL_QUADS);
    glColor3f(0.0f, 0.0f, 1.0f);
    
    // Draw the top cuboid
    //Back
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-3.5f, -0.2f, 0.2f);
    glVertex3f(3.5f, -0.2f, 0.2f);
    glVertex3f(3.5f, 0.2f, 0.2f);
    glVertex3f(-3.5f, 0.2f, 0.2f);
 
    //Right
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(3.5f, -0.2f, -0.2f);
    glVertex3f(3.5f, 0.2f, -0.2f);
    glVertex3f(3.5f, 0.2f, 0.2f);
    glVertex3f(3.5f, -0.2f, 0.2f);
 
    //Front
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(-3.5f, -0.2f, -0.2f);
    glVertex3f(-3.5f, 0.2f, -0.2f);
    glVertex3f(3.5f, 0.2f, -0.2f);
    glVertex3f(3.5f, -0.2f, -0.2f);

    //Left
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(-3.5f, -0.2f, -0.2f);
    glVertex3f(-3.5f, -0.2f, 0.2f);
    glVertex3f(-3.5f, 0.2f, 0.2f);
    glVertex3f(-3.5f, 0.2f, -0.2f);

    //Top
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-3.5f, 0.2f, -0.2f); 
    glVertex3f(3.5f, 0.2f, -0.2f);
    glVertex3f(3.5f, 0.2f, 0.2f);
    glVertex3f(-3.5f, 0.2f, 0.2f);

    //Draw the column cuboid

    //Back
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-0.2f, 0.0f, 0.2f);
    glVertex3f(0.2f, 0.0f, 0.2f);
    glVertex3f(0.2f, -8.0f, 0.2f);
    glVertex3f(-0.2f, -8.0f, 0.2f);
    //Right
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0.2f, 0.0f, -0.2f);
    glVertex3f(0.2f, 0.0f, 0.2f);
    glVertex3f(0.2f, -8.0f, 0.2f);
    glVertex3f(0.2f, -8.0f, -0.2f);
 
    //Front
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(-0.2f, 0.0f, -0.2f);
    glVertex3f(0.2f, 0.0f, -0.2f);
    glVertex3f(0.2f, -8.0f, -0.2f);
    glVertex3f(-0.2f, -8.0f, -0.2f);
    //Left
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(-0.2f, 0.0f, -0.2f);
    glVertex3f(-0.2f, 0.0f, 0.2f);
    glVertex3f(-0.2f, -8.0f, 0.2f);
    glVertex3f(-0.2f, -8.0f, -0.2f);
    glEnd();
    */

    glPushMatrix();
    glRotatef(-90.0,1.0,0.0,0.0);
    glRotatef(90.0,0.0,0.0,1.0);
    glTranslatef(0.0,0.0,-5.5);
    glColor3f(0.7f, 0.7f, 0.7f);
    glmDraw(arm_model,GLM_SMOOTH);
    glPopMatrix();

    // Draw the lines
    // Line 1
    glColor3f(1.0f, 0.0f, 0.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.2f, 0.0f);
    glVertex3f(arm_len, 0.2f, 0.0f);
    glEnd();
    // Line 2
    glTranslatef(arm_len, 0.2, 0.0);
    glRotatef(line_angle, 0.0f, 0.0f, 1.0f);
    glLineWidth(2.0f);
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(line_len, 0.0f, 0.0f);
    glEnd();

    // Draw the Plane
    glTranslatef(line_len,0.0f,0.0f);
    glTranslatef(0.0f, 0.0f, -0.1f);
    glRotatef(180.0f,0.0f,1.0f,0.0f);
    glRotatef(plane_angle,1.0f,0.0f,0.0f);

    glColor3f(0.9f, 0.9f, 0.9f);
    glmDraw(plane_model,GLM_SMOOTH);
    glTranslatef(0.0f, 0.01f, 0.0f);
    glColor3f(0.2f, 0.2f, 0.2f);
    glmDraw(plane_canope_model,GLM_SMOOTH);

    
    // Draw the Bubble
    glPopMatrix();
    glRotatef(bubble_azimuth, 0.0f,1.0f,0.0f);
    glTranslatef(arm_len,0.2f,0.0f);
    glRotatef(bubble_elevation, 0.0f,0.0f,1.0f);
    glTranslatef(line_len,0.0f,0.0f);
    glColor3f(0.89f, 0.32f, 0.0f);
    glutSolidSphere(bubble_radius, 20, 20);

    glutSwapBuffers();
}

void CarouselViz::proveCollision() {
    float azimuth_diff = fabs(arm_angle - bubble_azimuth);
    if (azimuth_diff > 360.0) {
        azimuth_diff -= 360.0;
    }
    float elevation_diff = fabs(line_angle - bubble_elevation);
    if (elevation_diff > 360.0) {
        elevation_diff -= 360.0;
    }
    // printf("difference in angles: %f\n ", azimuth_diff);
    // printf("ballradiusangle: %f\n ", 180*atan2(bubble_radius,3.5+cos(3.1415*bubble_elevation/180.0)*line_len)/3.1415);
    if (((azimuth_diff < (180*atan2(bubble_radius,arm_len+cos(3.1415*bubble_elevation/180.0)*line_len)/3.1415)) ||
         azimuth_diff > 360.0 - (180*atan2(bubble_radius,arm_len+cos(3.1415*bubble_elevation/180.0)*line_len)/3.1415)) &&
       ((elevation_diff < (180*atan2(bubble_radius,line_len)/3.1415)) ||
         elevation_diff > 360.0 - (180*atan2(bubble_radius,line_len)/3.1415))) {
            //printf("HUAAA!!");
            bubble_azimuth += 90.0;
            if (bubble_azimuth > 360.0) {
                bubble_azimuth -= 360;
            }
    }
}

void CarouselViz::idleFunc(void) {
    // Receive the new udp Sensorvalues
    my_values = my_udp.receiveUDPstruct();
    
    window_line_sum = 0.0;
    window_plane_sum = 0.0;
    for (int i = 0; i < window_len-1;i++) {
        window_line_angles[i] = window_line_angles[i+1];
        window_line_sum += window_line_angles[i+1];
        window_plane_angles[i] = window_plane_angles[i+1];
        window_plane_sum += window_plane_angles[i+1];
    }
    window_line_angles[window_len - 1] = 90 -180* atan2(my_values->compZ, my_values->compX)/3.1415;
    window_plane_angles[window_len - 1] = 90 -180* atan2(my_values->compZ, my_values->compY)/3.1415;
    window_line_sum += window_line_angles[window_len - 1];
    window_plane_sum += window_plane_angles[window_len - 1];
    line_angle = window_line_sum/(float)window_len; 
    plane_angle = window_plane_sum/(float)window_len; 
}
 
void CarouselViz::updateFunc(int value) {
    arm_angle_time += 1.0f;
    line_angle_time += 1.0f;
    arm_angle += 3.0f + 2.0*sin(3.1415*arm_angle_time/180.0);
    if (arm_angle > 360.0)
        arm_angle -= 360.0;
    line_angle = 30*sin(3.1415*line_angle_time/180.0)-45.0;
    proveCollision();
 
    // Set the camera position depending on the view_mode
    if (!view_mode) {
        glLoadIdentity();
        gluLookAt(-12.0, 3.0, -20.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    } else {
        glLoadIdentity(); 
        glTranslatef(0.0f, 0.0f, -5.0f);
        glRotatef(30.0f,1.0f,0.0f,0.0f);
        glTranslatef(0.0f, -2.0f, -4.0f);
        glRotatef(-90.0f,0.0f,1.0f,0.0f);
        glTranslatef(0.0f, 0.0f, line_len);
        glRotatef(line_angle,-1.0f,0.0f,0.0f);
        glTranslatef(0.0f, 0.0f, arm_len);
        glRotatef(arm_angle,0.0f,-1.0f,0.0f);
        /*
        glTranslatef(4.0f, 0.0f, 0.2f);
        */
        gluLookAt(0.0, 0.0, 0.0, 0.1, 0.0, 0.0, 0.0, 1.0, 0.0);
    }
    glutPostRedisplay();
    glutTimerFunc(25, updateFunc, 0);

}
