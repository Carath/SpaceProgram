#ifndef DRAWING_H
#define DRAWING_H


#include "bodies.h"
#include "user_inputs.h"


extern SDL_Renderer *renderer;

extern TTF_Font *font_medium;
extern CachedFont *cached_font_medium;

extern SDL_Color Black;
extern SDL_Color Red;
extern SDL_Color Lime;
extern SDL_Color Blue;
extern SDL_Color Yellow;
extern SDL_Color White;
extern SDL_Color HUDcolor;

extern const double CenterX;
extern const double CenterY;
extern double Xorigin;
extern double Yorigin;
extern int IndexFollowedBody;
extern int DrawAllNames;
extern int CollisionsEnabled;

extern double FrameBatchTime;


// Draws a set of bodies, along with the user inputs for a spaceship. To not draw them, pass NULL as 'input'.
void drawBodies(Body **bodies, int bodies_number, Input *input);


// Draws the Head-Up Display:
void drawHUD(Body **bodies, int bodies_number);


// Draws a compass showing where the object is, when it is not on-screen.
// Double precision rescaled coordinates must be passed for this to work properly.
void drawCompass(double targetX, double targetY);


// Draw a cross where the spaceship is:
void drawCross(int x, int y);


// 'ratio' is the half-basis length divided by the arrow height.
void drawArrow(int startX, int startY, float directionX, float directionY, float ratio);


// Draws a disk, with the current color.
// Double precision rescaled coordinates must be passed for this to work properly.
void fillCircle(double x0, double y0, double radius);


#endif
