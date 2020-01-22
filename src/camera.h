#ifndef CAMERA_H
#define CAMERA_H


#include "bodies.h"


extern TTF_Font *font_medium;
extern int RenderScene;


// Public function for getting the 'Scale' value:
double getScale(void);


// Translates real X-coordinates to displaying ones.
// Double precision is conserved for the compass.
double Xrescale(double x);


// Translates real Y-coordinates to displaying ones.
// Double precision is conserved for the compass.
double Yrescale(double x);


// Translates real lengths to on-screen ones:
double getLength(double length);


// Translates a numerical value to a number of pixels. Used for on-screen coordinates, but also objects size.
// This is more accurate when RAW_PIXELS_DRAWING = 0.
int getPixel(double value);


void getCameraInfo(double *scale, double *x_origin, double *y_origin);


void moveCamera(double scale_multiplier, int Xdirection, int Ydirection);


void followBody(Body *body);


int isInWindow(double x, double y);


// Checks if the body can appear on-screen, and if the whole screen is covered by said body.
// Double precision rescaled coordinates and radius must be passed for this to work properly.
void bodyScreenCheck(double x, double y, double radius, int *may_appear, int *is_covered);


// Find the intersection between a line passing through the center of the screen
// and the target, assuming that said target isn't in the window. Double precision
// rescaled coordinates must be passed for this to work properly.
void findFrameIntersection(double targetX, double targetY, int *intersX, int *intersY);


#endif
