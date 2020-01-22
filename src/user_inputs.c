#include <stdio.h>

#include "settings.h"
#include "user_inputs.h"
#include "camera.h"
#include "physics.h"


static SDL_Keycode last_pressed_key;
static int key_still_down = 0; // must start with 0.

static char keynamesBuffer[1000];


// Main function for handling user inputs:
void input_control(Input *input, int bodies_number)
{
	SDL_Event event; // Better to not declare it as static, causes bugs...

	while (SDL_PollEvent(&event)) {} // 'while' in order to empty the event list (nails the mouse motion bug).
	// Nota Bene: SDL_PollEvent() implicitly calls SDL_PumpEvents().

	if (event.type == SDL_QUIT || Keyboard_state[SDL_GetScancodeFromKey(QUIT_KEY)])
	{
		Quit = 1;
		return;
	}

	else if (key_still_down) // Nothing happens if the last key is still down.
	{
		if (!Keyboard_state[SDL_GetScancodeFromKey(last_pressed_key)])
			key_still_down = 0;
		else
			return;
	}

	// Pausing/unpausing the simulation:

	if (key_pressed(PAUSE_KEY)) // no repeat
	{
		SimulationRunning = !SimulationRunning;
		RenderScene = 1; // For drawing the pause message.
	}

	// Toggling the drawing of all bodies name:

	if (key_pressed(TOGGLE_DRAWING_ALL_NAMES)) // no repeat
	{
		DrawAllNames = !DrawAllNames;
		RenderScene = 1; // For drawing the dedicated message.
	}

	// Toggling collisions:

	if (key_pressed(TOGGLE_COLLISIONS_KEY)) // no repeat
	{
		CollisionsEnabled = !CollisionsEnabled;
		RenderScene = 1; // For drawing the collision message.
	}

	// Camera movement. Note: calling moveCamera() sets RenderScene to 1.

	if (event.type == SDL_MOUSEWHEEL && event.wheel.y != 0)
	{
		if (event.wheel.y > 0)
			moveCamera(CAMERA_ZOOM, 0., 0.);

		else if (event.wheel.y < 0)
			moveCamera(1. / CAMERA_ZOOM, 0., 0.);

		else
			printf("Unsupported mousewheel input.\n");
	}

	if (Keyboard_state[SDL_GetScancodeFromKey(CAMERA_UP)])
	{
		moveCamera(1., 0, -1);
	}

	if (Keyboard_state[SDL_GetScancodeFromKey(CAMERA_DOWN)])
	{
		moveCamera(1., 0, 1);
	}

	if (Keyboard_state[SDL_GetScancodeFromKey(CAMERA_LEFT)])
	{
		moveCamera(1., -1, 0);
	}

	if (Keyboard_state[SDL_GetScancodeFromKey(CAMERA_RIGHT)])
	{
		moveCamera(1., 1, 0);
	}

	if (key_pressed(CAMERA_FOLLOW)) // no repeat
	{
		CameraFollowing = !CameraFollowing;
		RenderScene = 1;
	}

	if (key_pressed(CAMERA_NEXT_TARGET) && CameraFollowing) // no repeat
	{
		IndexFollowedBody = (IndexFollowedBody + 1) % bodies_number;
		RenderScene = 1;
	}

	if (key_pressed(CAMERA_PREVIOUS_TARGET) && CameraFollowing) // no repeat
	{
		IndexFollowedBody = (IndexFollowedBody + bodies_number - 1) % bodies_number;
		RenderScene = 1;
	}

	// Modifying the simulation speed:

	if (key_pressed(SLOW_DOWN_TIME)) // no repeat
	{
		changeSimulationSpeed(1. / TIME_SCALE_MULTIPLIER);
		RenderScene = 1; // For drawing the timescale update.
	}

	if (key_pressed(SPEED_UP_TIME)) // no repeat
	{
		changeSimulationSpeed(TIME_SCALE_MULTIPLIER);
		RenderScene = 1; // For drawing the timescale update.
	}

	// Moving a spaceship:

	// Resetting to default values!
	input -> Xinput = X_NO_INPUT;
	input -> Yinput = Y_NO_INPUT;

	if (Keyboard_state[SDL_GetScancodeFromKey(MOVE_UP_KEY)])
	{
		input -> Yinput += UP;
	}

	if (Keyboard_state[SDL_GetScancodeFromKey(MOVE_DOWN_KEY)])
	{
		input -> Yinput += DOWN;
	}

	if (Keyboard_state[SDL_GetScancodeFromKey(MOVE_LEFT_KEY)])
	{
		input -> Xinput += LEFT;
	}

	if (Keyboard_state[SDL_GetScancodeFromKey(MOVE_RIGHT_KEY)])
	{
		input -> Xinput += RIGHT;
	}
}


// Returns 1 if the given key is actually pressed, and if so updates 'last_pressed_key' and 'key_still_down'.
int key_pressed(SDL_Keycode key)
{
	if (Keyboard_state[SDL_GetScancodeFromKey(key)])
	{
		last_pressed_key = key;
		key_still_down = 1;
		return 1;
	}

	else
		return 0;
}


static void initKeynamesBuffer(void)
{
	if (keynamesBuffer[0] != '\0')
		return;

	char keynamesArray[17][25]; // 17: numbers of supported hotkeys.
	// 25: maximum name length of an hotkey. What follows is due to
	// a limitation of the SDL_GetKeyName() function, which uses a
	// unique buffer for every key...

	sprintf(keynamesArray[ 0], "%s", SDL_GetKeyName(QUIT_KEY));
	sprintf(keynamesArray[ 1], "%s", SDL_GetKeyName(PAUSE_KEY));
	sprintf(keynamesArray[ 2], "%s", SDL_GetKeyName(TOGGLE_DRAWING_ALL_NAMES));
	sprintf(keynamesArray[ 3], "%s", SDL_GetKeyName(TOGGLE_COLLISIONS_KEY));
	sprintf(keynamesArray[ 4], "%s", SDL_GetKeyName(CAMERA_UP));
	sprintf(keynamesArray[ 5], "%s", SDL_GetKeyName(CAMERA_DOWN));
	sprintf(keynamesArray[ 6], "%s", SDL_GetKeyName(CAMERA_LEFT));
	sprintf(keynamesArray[ 7], "%s", SDL_GetKeyName(CAMERA_RIGHT));
	sprintf(keynamesArray[ 8], "%s", SDL_GetKeyName(CAMERA_FOLLOW));
	sprintf(keynamesArray[ 9], "%s", SDL_GetKeyName(CAMERA_NEXT_TARGET));
	sprintf(keynamesArray[10], "%s", SDL_GetKeyName(CAMERA_PREVIOUS_TARGET));
	sprintf(keynamesArray[11], "%s", SDL_GetKeyName(SLOW_DOWN_TIME));
	sprintf(keynamesArray[12], "%s", SDL_GetKeyName(SPEED_UP_TIME));
	sprintf(keynamesArray[13], "%s", SDL_GetKeyName(MOVE_UP_KEY));
	sprintf(keynamesArray[14], "%s", SDL_GetKeyName(MOVE_DOWN_KEY));
	sprintf(keynamesArray[15], "%s", SDL_GetKeyName(MOVE_LEFT_KEY));
	sprintf(keynamesArray[16], "%s", SDL_GetKeyName(MOVE_RIGHT_KEY));

	sprintf(keynamesBuffer, "Quit: %s\nPause: %s\nToggle drawing names: %s\nToggle collisions: %s\n"
		"Camera up: %s arrow\nCamera down: %s arrow\nCamera left: %s arrow\nCamera right: %s arrow\n"
		"Camera toggle following: %s\nCamera next target: %s\nCamera previous target: %s\nSlow down time: %s\n"
		"Speed up time: %s\nMove up: %s\nMove down: %s\nMove left: %s\nMove right: %s\n",
		keynamesArray[0], keynamesArray[1], keynamesArray[2], keynamesArray[3], keynamesArray[4], keynamesArray[5],
		keynamesArray[6], keynamesArray[7], keynamesArray[8], keynamesArray[9], keynamesArray[10], keynamesArray[11],
		keynamesArray[12], keynamesArray[13], keynamesArray[14], keynamesArray[15], keynamesArray[16]);
}


// Prints the supported hotkeys in the console:
void printHotkeys(void)
{
	initKeynamesBuffer();

	printf("Supported hotkeys:\n\n%s\n", keynamesBuffer);
}
