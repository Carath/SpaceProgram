#ifndef PHYSICS_H
#define PHYSICS_H


#include "bodies.h"
#include "user_inputs.h"


extern double *DistArray;
extern int IndexFollowedBody;
extern int CollisionsEnabled;
extern unsigned int SimulationFrameIndex;


// Returns the amount of simulation time a user second represents:
double getTimeScale(void);


// Updates the internal value storing the elapsed simulation time.
// This can be called several time without issues:
void updateSimulationTime(void);


// Returns the elapsed time in the simulation, in seconds:
double getSimulationTime(void);


void changeSimulationSpeed(double time_multiplier);


double distance(double x1, double y1, double x2, double y2);


int collision(Body **bodies, int indexBody1, int indexBody2, double dist);


void update_accel_input(Body *ship, Input *input, double thrust);


// Updating each positions simultaneously!
void moveBodies(Body **bodies, int bodies_number, Body *ship, Input *input, double thrust);


#endif
