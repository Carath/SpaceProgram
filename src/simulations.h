#ifndef SIMULATIONS_H
#define SIMULATIONS_H


#include "bodies.h"


extern int Quit;
extern int IndexFollowedBody;


double realTime(void);


double unif_rand(double min, double max);


// Removes NULL bodies from the body array, modifies
// the number of bodies, and updates 'indexFollowedBody'.
void refreshBodyArray(Body ***oldArray, int *oldLength);


// Benchmarking 'UPDATES_PER_FRAME':
void benchmarkUpdatesNumber(Body **bodies, int bodies_number);


Body** simul_EarthMoonShip(int *bodies_number, Body **ship);


Body** simul_3Earths(int *bodies_number, Body **ship);


Body** simul_manyBodies(int *bodies_number, Body **ship);


#endif
