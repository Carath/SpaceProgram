#include <stdio.h>
#include <stdlib.h>

#include "settings.h"
#include "drawing.h"
#include "camera.h"
#include "physics.h"


#define point(x, y) \
	SDL_RenderDrawPoint(renderer, x, y)

#define line(x1, y1, x2, y2) \
	SDL_RenderDrawLine(renderer, x1, y1, x2, y2)

#define setColor(color) \
	SDLA_SetDrawColor((color) -> r, (color) -> g, (color) -> b)


static const SDL_Rect HUDrect = {0, 0, LEFT_MARGIN, WINDOW_HEIGHT};
static const SDL_Rect FrameRect = {LEFT_MARGIN, 0, WINDOW_WIDTH, WINDOW_HEIGHT};

static const char* OnOffStrings[] = {"OFF", "ON"};

static char HUD_buffer_1[500];
static char HUD_buffer_2[500];
static char HUD_buffer_3[10];

static int HUDcounter = 0;


// Draws a set of bodies, along with the user inputs for a spaceship. To not draw them, pass NULL as 'input'.
void drawBodies(Body **bodies, int bodies_number, Input *input)
{
	for (int i = 0; i < bodies_number; ++i)
	{
		if (bodies[i] == NULL)
			continue;

		double x_rescaled = Xrescale(bodies[i] -> PosX);
		double y_rescaled = Yrescale(bodies[i] -> PosY);
		double r_onScreen;

		// On-screen coordinates used for drawing:
		int x_onScreen = getPixel(x_rescaled);
		int y_onScreen = getPixel(y_rescaled);

		if (bodies[i] -> Type == Spaceship)
		{
			setColor(&Red);

			r_onScreen = CROSS_SIZE;

			drawCross(x_onScreen, y_onScreen);

			drawCompass(x_rescaled, y_rescaled); // using double precision.

			// Drawing user inputs:

			if (input != NULL && (input -> Xinput != X_NO_INPUT || input -> Yinput != Y_NO_INPUT))
			{
				setColor(&Lime);

				float arrowShiftX = CROSS_SIZE * input -> Xinput;
				float arrowShiftY = CROSS_SIZE * input -> Yinput;

				drawArrow(x_onScreen + arrowShiftX, y_onScreen + arrowShiftY, 2 * arrowShiftX, 2 * arrowShiftY, 0.5);
			}
		}

		else
		{
			setColor(&Yellow);

			if (DRAW_COMPASS_ALL_OBJECTS)
				drawCompass(x_rescaled, y_rescaled); // using double precision.

			r_onScreen = getLength(bodies[i] -> Radius);

			fillCircle(x_rescaled, y_rescaled, r_onScreen); // using double precision.
		}

		if (DrawAllNames)
		{
			int texture_width;

			if (SDL_QueryTexture(bodies[i] -> TextureName, NULL, NULL, &texture_width, NULL))
				SDLA_ExitWithError("Cannot find the texture size.");

			// Shifting the text down for cosmetic effect:
			int vertical_shift = MAX(r_onScreen + 10, r_onScreen * 1.1);

			SDLA_DrawTexture(bodies[i] -> TextureName, x_onScreen - texture_width / 2, y_onScreen + vertical_shift);
		}
	}
}


// Draws the Head-Up Display:
void drawHUD(Body **bodies, int bodies_number)
{
	setColor(&HUDcolor);

	SDL_RenderFillRect(renderer, &HUDrect);

	setColor(&White);

	if (!SimulationRunning) // paused.
	{
		HUDcounter = 0; // To be sure up to date information is drawn during a pause.

		FrameBatchTime = 0.; // To reset it, while the pause is on.

		sprintf(HUD_buffer_3, "Paused.");
		SDLA_DrawCachedFont(cached_font_medium, HUD_MARGIN, HUD_MARGIN + 800, HUD_buffer_3);
	}

	if (HUDcounter == 0)
	{
		double fps = FrameBatchTime == 0. ? 0. : (double) HUD_UPDATES_FRAME_STEP / FrameBatchTime;

		FrameBatchTime = 0.; // Resetting the sum of frame times, for a new frame batch.

		double secondsNumberPerHour = 3600.;
		double secondsNumberPerDay = secondsNumberPerHour * 24.;
		double secondsNumberPerYear = secondsNumberPerDay * 365.25;

		double simulationTime = getSimulationTime(); // in seconds.

		int year = simulationTime / secondsNumberPerYear;
		int day = (simulationTime - year * secondsNumberPerYear) / secondsNumberPerDay;
		int hour = (simulationTime - year * secondsNumberPerYear - day * secondsNumberPerDay) / secondsNumberPerHour;

		sprintf(HUD_buffer_1, "FPS:  %.1f\nDrawing names:  %s\nCollisions:  %s\n\nScale:  %.2e\nXorigin:  %9.2e m\n"
			"Yorigin:  %9.2e m\n\nTime scale:  %.2e\nYear:  %d\nDay:  %d\nHour:  %d\n\nBodies number:  %.d",
			fps, OnOffStrings[DrawAllNames], OnOffStrings[CollisionsEnabled], getScale(), Xorigin, Yorigin, getTimeScale(),
			year, day, hour, bodies_number);
	}

	SDLA_DrawCachedFont(cached_font_medium, HUD_MARGIN, HUD_MARGIN, HUD_buffer_1);

	if (CameraFollowing)
	{
		Body *body = bodies[IndexFollowedBody];

		if (body == NULL)
		{
			printf("Cannot draw info of a NULL body.\n");
			return;
		}

		if (HUDcounter == 0)
		{
			double speed = distance(0, body -> SpeedX, 0, body -> SpeedY);
			double accel = distance(0, body -> AccelX, 0, body -> AccelY);

			sprintf(HUD_buffer_2, "Camera following:\n%.15s\n\nType:  %s\nRadius:  %.2e m\nMass:  %.2e kg\nPosX:  %9.2e m\n"
				"PosY:  %9.2e m\nSpeed:  %.2e m/s\nAccel:   %.2e m/s2", body -> Name, getBodyTypeName(body -> Type),
				body -> Radius, body -> Mass, body -> PosX, body -> PosY, speed, accel);
		}

		SDLA_DrawCachedFont(cached_font_medium, HUD_MARGIN, HUD_MARGIN + 450, HUD_buffer_2);
	}

	HUDcounter = SimulationRunning ? (HUDcounter + 1) % HUD_UPDATES_FRAME_STEP : 0; // stays at 0 while paused.
}


// Draws a compass showing where the object is, when it is not on-screen.
// Double precision rescaled coordinates must be passed for this to work properly.
void drawCompass(double targetX, double targetY)
{
	if (isInWindow(targetX, targetY))
		return;

	int intersX, intersY;

	findFrameIntersection(targetX, targetY, &intersX, &intersY);

	double compSizeNormalized = COMPASS_SIZE / distance(CenterX, CenterY, intersX, intersY); // distance is > 0.

	double directionX = compSizeNormalized * (intersX - CenterX);
	double directionY = compSizeNormalized * (intersY - CenterY);

	drawArrow(intersX, intersY, directionX, directionY, 0.5);
}


// Draw a cross where the spaceship is:
void drawCross(int x, int y)
{
	line(x - CROSS_SIZE, y, x + CROSS_SIZE, y);
	line(x, y - CROSS_SIZE, x, y + CROSS_SIZE);
}


// 'ratio' is the half-basis length divided by the arrow height.
void drawArrow(int startX, int startY, float directionX, float directionY, float ratio)
{
	int endX = startX + directionX, endY = startY + directionY;

	line(startX + ratio * directionY, startY - ratio * directionX, endX, endY);
	line(startX - ratio * directionY, startY + ratio * directionX, endX, endY);
}


// Draws a disk, with the current color, using the Bresenham's circle algorithm.
// Double precision rescaled coordinates must be passed for this to work properly.
// This may be slow for very large circles intersecting the window...
void fillCircle(double x0, double y0, double radius)
{
	int may_appear, is_covered;

	bodyScreenCheck(x0, y0, radius, &may_appear, &is_covered); // for optimization!

	if (!may_appear)
		return;

	if (is_covered)
	{
		SDL_RenderFillRect(renderer, &FrameRect);
		return;
	}

	// Casting to integers:
	const int x0_ = getPixel(x0);
	const int y0_ = getPixel(y0);
	const int r = getPixel(radius);

	if (r <= 0)
	{
		point(x0_, y0_);
		return;
	}

	int x = 0, y = r, error = 5 - (r << 2);

	while (x <= y)
	{
		// From top to bottom:
		line(x0_ - x, y0_ - y, x0_ + x, y0_ - y);
		line(x0_ - y, y0_ - x, x0_ + y, y0_ - x);
		line(x0_ - y, y0_ + x, x0_ + y, y0_ + x);
		line(x0_ - x, y0_ + y, x0_ + x, y0_ + y);

		if (error > 0)
		{
			--y;
			error -= y << 3;
		}

		++x;
		error += 4 + (x << 3);
	}
}
