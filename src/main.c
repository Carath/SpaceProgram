#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "SDLA.h"
#include "settings.h"
#include "camera.h"
#include "drawing.h"
#include "physics.h"
#include "user_inputs.h"
#include "simulations.h"


////////////////////////////////////////////////////////////
// Global variables:

extern const int FrameTime;

SDL_Window *window;
SDL_Renderer *renderer;

TTF_Font *font_small;
TTF_Font *font_medium;

CachedFont *cached_font_medium;

SDL_Color Black = {0, 0, 0, 255};
SDL_Color Red = {255, 0, 0, 255};
SDL_Color Lime = {0, 255, 0, 255};
SDL_Color Blue = {0, 0, 0, 255};
SDL_Color Yellow = {255, 255, 0, 255};
SDL_Color White = {255, 255, 255, 255};
SDL_Color HUDcolor = {48, 48, 48, 255};

const Uint8 *Keyboard_state;

int Quit = 0;
int RenderScene = 1;
int SimulationRunning = 1;
int CameraFollowing = 0;
int IndexFollowedBody = 0;
int DrawAllNames = 1;
int CollisionsEnabled = 1;

unsigned int SimulationFrameIndex = 0; // For controlling the simulation elapsed time.

double FrameBatchTime = 0.; // Used to store the sum of the real frame times, per frame batch.


int main(int argc, char **argv)
{
	srand(time(NULL)); // Initializing randomness.

	////////////////////////////////////////////////////////////
	// Initializing SDLA - rendering:

	SDLA_Init(&window, &renderer, "Space Program", WINDOW_WIDTH, WINDOW_HEIGHT,
		USE_HARDWARE_ACCELERATION, SDLA_BLENDED);

	////////////////////////////////////////////////////////////
	// Loading fonts:

	char font_name[] = "/usr/share/fonts/truetype/dejavu/DejaVuSerif-Bold.ttf";

	font_small = TTF_OpenFont(font_name, FONT_SMALL_SIZE);
	font_medium = TTF_OpenFont(font_name, FONT_MEDIUM_SIZE);

	cached_font_medium = SDLA_CachingFontAll(font_name, FONT_MEDIUM_SIZE, &White);

	////////////////////////////////////////////////////////////
	// For multiple keyboard inputs:

	Keyboard_state = SDL_GetKeyboardState(NULL);

	////////////////////////////////////////////////////////////
	// Printing in the console the supported hotkeys:

	printHotkeys();

	////////////////////////////////////////////////////////////
	// Space simultation settings:

	Input current_input;
	int bodies_number;
	Body *ship = NULL;
	Body **bodies = NULL;

	if (argc <= 1 || atoi(argv[1]) <= 0)
	{
		// Earth, Moon, and a Spaceship:
		bodies = simul_EarthMoonShip(&bodies_number, &ship);
	}

	else if (argc <= 1 || atoi(argv[1]) == 1)
	{
		// 3 Earth-like planets:
		bodies = simul_3Earths(&bodies_number, &ship);
	}

	else
	{
		// Many Earth-like planets:
		bodies = simul_manyBodies(&bodies_number, &ship);

		DrawAllNames = 0; // More satisfying that way.
	}

	////////////////////////////////////////////////////////////
	// Benchmarking 'UPDATES_BY_FRAME':

	// benchmarkUpdatesNumber(bodies, bodies_number);

	////////////////////////////////////////////////////////////
	// Main loop:

	Uint32 lastTime = SDL_GetTicks();

	// For controlling the rendering:
	// unsigned int renderFrameIndex = 0;

	// For controlling the simulation elapsed time:

	double frameStartTime, start, end;
	double drawingTime = 0., simulationTime = 0.;

	while (!Quit)
	{
		frameStartTime = realTime();

		////////////////////////////////////////////////////////////
		// Input control:

		input_control(&current_input, bodies_number);

		////////////////////////////////////////////////////////////
		// Drawing:

		start = realTime();

		if (SimulationRunning)
			RenderScene = 1;

		if (RenderScene)
		{
			SDLA_ClearWindow(NULL);

			// This has to be done before drawing bodies:
			if (CameraFollowing)
				followBody(bodies[IndexFollowedBody]);

			drawBodies(bodies, bodies_number, &current_input);

			// After drawing bodies:
			drawHUD(bodies, bodies_number);

			// Rendering:
			SDL_RenderPresent(renderer);

			RenderScene = 0;

			// ++renderFrameIndex;
			// printf("renderFrameIndex: %d\n", renderFrameIndex);
		}

		end = realTime();
		drawingTime += SimulationRunning ? end - start : 0.;
		start = end;

		////////////////////////////////////////////////////////////
		// Bodies and ship movement:

		if (SimulationRunning) // Don't move things when paused!
		{
			double thrust = 5.;

			moveBodies(bodies, bodies_number, ship, &current_input, thrust);

			++SimulationFrameIndex;
		}

		simulationTime += SimulationRunning ? realTime() - start : 0.;

		// Removing absorbed bodies, both for performance improvement and for a correct following of bodies:
		refreshBodyArray(&bodies, &bodies_number);

		////////////////////////////////////////////////////////////
		// Refresh rate control:

		// Don't use an unsigned integer! Will wait an eternity if it overflows...
		int time_passed = SDL_GetTicks() - lastTime;

		int to_wait = MAX(0, FrameTime - time_passed);

		// printf("to_wait = %d\n", to_wait); // in SDL ticks. Must be > 0 in order to work well.

		SDL_Delay(to_wait); // In menus: 0% CPU usage.

		lastTime = SDL_GetTicks();

		end = realTime();

		FrameBatchTime += SimulationRunning ? end - frameStartTime : 0.;
	}

	if (BENCHMARK_SIMULATION && SimulationFrameIndex != 0)
	{
		printf("\nDrawing mean: %.2f ms\n", 1000. * drawingTime / SimulationFrameIndex);
		printf("Physics mean time: %.2f ms\n", 1000. * simulationTime / SimulationFrameIndex);
	}

	////////////////////////////////////////////////////////////
	// Freeing and Quitting:

	freePhysicsResources();

	for (int i = 0; i < bodies_number; ++i)
		freeBody(bodies[i]);

	free(bodies);

	SDLA_FreeCachedFont(cached_font_medium);

	TTF_CloseFont(font_medium);
	TTF_CloseFont(font_small);

	SDLA_Quit();

	return EXIT_SUCCESS;
}
