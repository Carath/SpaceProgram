#ifndef SETTINGS_H
#define SETTINGS_H


///////////////////////////////////////////////////////////////
// General declarations to be included everywhere:
///////////////////////////////////////////////////////////////


#define MAX_NAME_LENGTH 50


#define MIN(x, y) (x < y ? x : y)
#define MAX(x, y) (x < y ? y : x)


// Careful, do not use this inside a function:
#define ARRAY_SIZE(array) (sizeof(array) / sizeof(*array))


///////////////////////////////////////////////////////////////
// Graphical settings:
///////////////////////////////////////////////////////////////


#define WINDOW_WIDTH 1800
#define WINDOW_HEIGHT 1000

#define USE_HARDWARE_ACCELERATION 1

#define FRAMERATE 60


///////////////////////////////////////////////////////////////
// Simulation settings:
///////////////////////////////////////////////////////////////


#define INIT_TIME_MULTIPLIER (24. * 3600.) // 1 second of real time corresponds to 'INIT_TIME_MULTIPLIER' seconds of simulation.

#define TIME_SCALE_MULTIPLIER 1.25 // For slowing down / speeding up the simulation.

#define UPDATES_PER_FRAME 50 // Number of updates per frame. The larger the value, the more precise the simulation,
// but this has an impact on performance.

#define ENABLE_MULTITHREADING // Multithreading can improve performances when working with a large number of bodies.
// However, it can also be greatly better to disable it when the number of bodies is small and UPDATES_PER_FRAME is large.
// It may be useful to try different settings, by setting BENCHMARK_SIMULATION to 1.

#define THREAD_NUMBER 3 // Use a small value. For the machine this has been developed on,
// 3 works best, 2 also helps a bit. But things get worst up to 4...

#define CHEAT 0 // '1': allows lower values of 'UPDATES_PER_FRAME', for unknown reason. '0' otherwise.

#define BENCHMARK_SIMULATION 1 // Used to estimate the time spend on drawing or doing physics computations.


///////////////////////////////////////////////////////////////
// Drawing settings:
///////////////////////////////////////////////////////////////


#define MAX_RADIUS 5e8 // depends on the simulation.

// During the simulation, the Head-Up Display is updated approximately HUD_UPDATES_PER_SEC per second,
// more precisely once every HUD_UPDATES_FRAME_STEP frames.
#define HUD_UPDATES_PER_SEC 4
#define HUD_UPDATES_FRAME_STEP (FRAMERATE / HUD_UPDATES_PER_SEC)

#define CAMERA_STEP (0.75 / FRAMERATE) // FRAMERATE independent.
#define CAMERA_ZOOM 1.20

#define RAW_PIXELS_DRAWING 0 // A value of '0' improves the drawing accuracy (0.5 pixel vs 1 pixel error).

#define FONT_SMALL_SIZE 15
#define FONT_MEDIUM_SIZE 20

#define CROSS_SIZE 4
#define COMPASS_SIZE 10

#define COMPASS_MARGIN 25
#define LEFT_MARGIN 275
#define HUD_MARGIN 20

#define DRAW_COMPASS_ALL_OBJECTS 1


///////////////////////////////////////////////////////////////
// Hotkeys:
///////////////////////////////////////////////////////////////


#define QUIT_KEY SDLK_ESCAPE
#define PAUSE_KEY SDLK_SPACE

#define TOGGLE_DRAWING_ALL_NAMES SDLK_n
#define TOGGLE_COLLISIONS_KEY SDLK_c

#define CAMERA_UP SDLK_UP
#define CAMERA_DOWN SDLK_DOWN
#define CAMERA_LEFT SDLK_LEFT
#define CAMERA_RIGHT SDLK_RIGHT
#define CAMERA_FOLLOW SDLK_f
#define CAMERA_NEXT_TARGET SDLK_TAB
#define CAMERA_PREVIOUS_TARGET SDLK_LSHIFT

#define SLOW_DOWN_TIME SDLK_1
#define SPEED_UP_TIME SDLK_2

#define MOVE_UP_KEY SDLK_z
#define MOVE_DOWN_KEY SDLK_s
#define MOVE_LEFT_KEY SDLK_q
#define MOVE_RIGHT_KEY SDLK_d


#endif
