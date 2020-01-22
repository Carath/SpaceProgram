#ifndef USER_INPUTS_H
#define USER_INPUTS_H


#include "SDLA.h"


typedef enum {X_NO_INPUT = 0, LEFT = -1, RIGHT = 1} XaxisInput;
typedef enum {Y_NO_INPUT = 0, UP = -1, DOWN = 1} YaxisInput;

typedef struct
{
	XaxisInput Xinput;
	YaxisInput Yinput;
} Input;


extern const Uint8 *Keyboard_state;

extern int Quit;
extern int RenderScene;
extern int SimulationRunning;
extern int CameraFollowing;
extern int IndexFollowedBody;
extern int DrawAllNames;
extern int CollisionsEnabled;


// Main function for handling user inputs:
void input_control(Input *input, int bodies_number);


// Returns 1 if the given key is actually pressed, and if so updates 'last_pressed_key' and 'key_still_down'.
int key_pressed(SDL_Keycode key);


// Prints the supported hotkeys in the console:
void printHotkeys(void);


#endif
