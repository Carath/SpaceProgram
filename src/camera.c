#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>

#include "settings.h"
#include "camera.h"


// Drawing frame size:
#define M1 (COMPASS_MARGIN + LEFT_MARGIN)
#define M2 COMPASS_MARGIN
#define M3 COMPASS_MARGIN
#define M4 COMPASS_MARGIN

#define W1 (WINDOW_WIDTH - M1 - M2)
#define H1 (WINDOW_HEIGHT - M3 - M4)

#define SLOPE ((double) H1 / W1)


// On-screen coordinates of the frame center:
const double CenterX = M1 + W1 / 2.;
const double CenterY = M3 + H1 / 2.;

// Simulation coordinates of the camera. This can be modified during runtime:
double Xorigin = 0;
double Yorigin = 0;


// Coefficient of the diagonal lines from the drawing frame:
static const double A_1 = SLOPE;
static const double B_1 = M3 - SLOPE * M1;

static const double A_2 = -SLOPE;
static const double B_2 = M3 + H1 + SLOPE * M1;

// Scale of the simulation:
static double Scale = MIN(WINDOW_WIDTH - LEFT_MARGIN, WINDOW_HEIGHT) / (2. * MAX_RADIUS);


// Public function for getting the 'Scale' value:
inline double getScale(void)
{
	return Scale;
}


// Translates real X-coordinates to displaying ones.
// Double precision is conserved for the compass.
inline double Xrescale(double x)
{
	return CenterX + Scale * (x - Xorigin);
}


// Translates real Y-coordinates to displaying ones.
// Double precision is conserved for the compass.
inline double Yrescale(double y)
{
	return CenterY + Scale * (y - Yorigin);
}


// Translates real lengths to on-screen ones:
inline double getLength(double length)
{
	return Scale * length;
}


// Translates a numerical value to a number of pixels. Used for on-screen coordinates, but also objects size.
// This is more accurate when RAW_PIXELS_DRAWING = 0.
inline int getPixel(double value)
{
	double used_value = RAW_PIXELS_DRAWING ? value : nearbyint(value);

	// Casting a floating point number to an integer, while said number is too big to be
	// representable as an integer is undefined behavior. To be sure everything works properly:

	if (used_value >= INT_MAX)
		return INT_MAX;

	if (used_value <= INT_MIN)
		return INT_MIN;

	return (int) used_value;
}


void getCameraInfo(double *scale, double *x_origin, double *y_origin)
{
	*scale = Scale;
	*x_origin = Xorigin;
	*y_origin = Yorigin;
}


void moveCamera(double scale_multiplier, int Xdirection, int Ydirection)
{
	double step = CAMERA_STEP * MIN(WINDOW_WIDTH - LEFT_MARGIN, WINDOW_HEIGHT) / (2. * Scale);

	Xorigin += step * Xdirection;
	Yorigin += step * Ydirection;

	double temp = Scale * scale_multiplier;

	if (temp != 0. && temp != +INFINITY)
		Scale = temp;

	RenderScene = 1;
}


void followBody(Body *body)
{
	if (body == NULL)
	{
		printf("Following a removed body...\n");
		return;
	}

	Xorigin = body -> PosX;
	Yorigin = body -> PosY;
}


inline int isInWindow(double x, double y)
{
	return (x >= LEFT_MARGIN && x < WINDOW_WIDTH) && (y >= 0 && y < WINDOW_HEIGHT);
}


inline int isInCircle(double centerX, double centerY, double radius, double x, double y)
{
	return (x - centerX) * (x - centerX) + (y - centerY) * (y - centerY) <= radius * radius;
}


// Checks if the body can appear on-screen, and if the whole screen is covered by said body.
// Double precision rescaled coordinates and radius must be passed for this to work properly.
void bodyScreenCheck(double x, double y, double radius, int *may_appear, int *is_covered)
{
	double xmin_1 = LEFT_MARGIN, xmax_1 = WINDOW_WIDTH, ymin_1 = 0, ymax_1 = WINDOW_HEIGHT; // window bounds.
	double xmin_2 = x - radius, xmax_2 = x + radius, ymin_2 = y - radius, ymax_2 = y + radius; // body bounds.

	*may_appear = (xmax_1 >= xmin_2 && xmin_1 <= xmax_2) && (ymax_1 >= ymin_2 && ymin_1 <= ymax_2);

	// *is_covered = (xmin_1 >= xmin_2 && xmax_1 <= xmax_2) && (ymin_1 >= ymin_2 && ymax_1 <= ymax_2); // box only.
	*is_covered = isInCircle(x, y, radius, xmin_2, ymin_2) && isInCircle(x, y, radius, xmin_2, ymax_2) &&
		isInCircle(x, y, radius, xmax_2, ymin_2) && isInCircle(x, y, radius, xmax_2, ymax_2); // circle only.
}


// Find the intersection between a line passing through the center of the screen
// and the target, assuming that said target isn't in the window. Double precision
// rescaled coordinates must be passed for this to work properly.
void findFrameIntersection(double targetX, double targetY, int *intersX, int *intersY)
{
	// Checking the target position compared to the frame diagonals:
	int diagCheck_1 = A_1 * targetX + B_1 > targetY;
	int diagCheck_2 = A_2 * targetX + B_2 > targetY;

	double ratio = (targetY - CenterY) / (targetX - CenterX);

	if (!diagCheck_1 && diagCheck_2) // Left side of the screen.
	{
		*intersX = M1;
		*intersY = CenterY + (*intersX - CenterX) * ratio;
	}

	else if (diagCheck_1 && !diagCheck_2) // Right side of the screen.
	{
		*intersX = WINDOW_WIDTH - M2;
		*intersY = CenterY + (*intersX - CenterX) * ratio;
	}

	else if (diagCheck_1 && diagCheck_2) // Top side of the screen.
	{
		*intersY = M3;
		*intersX = CenterX + (*intersY - CenterY) / ratio;
	}

	else // Bottom side of the screen.
	{
		*intersY = WINDOW_HEIGHT - M4;
		*intersX = CenterX + (*intersY - CenterY) / ratio;
	}
}
