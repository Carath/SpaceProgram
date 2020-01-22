#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "simulations.h"
#include "physics.h"


#ifndef _WIN32
#include <sys/time.h>
double realTime(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (double) tv.tv_sec + (double) tv.tv_usec / 1000000.;
}

#else
#include <windows.h> // to be replaced!
double realTime(void)
{
	return (double) GetTickCount() / 1000.;
}
#endif


double unif_rand(double min, double max)
{
	return ((double) rand() / RAND_MAX) * (max - min) + min;
}


// Removes NULL bodies from the body array, modifies
// the number of bodies, and updates 'IndexFollowedBody'.
void refreshBodyArray(Body ***oldArray, int *oldLength)
{
	int old_length = *oldLength, new_length = 0;

	for (int i = 0; i < old_length; ++i)
		new_length += (*oldArray)[i] != NULL;

	if (new_length == old_length)
		return;

	// Creating a new array:

	*oldLength = new_length;

	Body **newArray = (Body**) calloc(new_length, sizeof(Body*));

	if (newArray == NULL)
	{
		printf("\nNot enough memory to create a new bodies array.\n");
		exit(EXIT_FAILURE);
	}

	int index = 0;

	for (int i = 0; i < old_length; ++i)
	{
		if ((*oldArray)[i] != NULL)
		{
			if (i == IndexFollowedBody)
				IndexFollowedBody = index;

			newArray[index] = (*oldArray)[i];

			++index;
		}
	}

	free(*oldArray);

	*oldArray = newArray;
}


// Benchmarking 'UPDATES_PER_FRAME':
void benchmarkUpdatesNumber(Body **bodies, int bodies_number)
{
	printf("Updates number benchmark.\nNumber of bodies: %d. Actual 'UPDATES_PER_FRAME': %d\n\n",
		bodies_number, UPDATES_PER_FRAME);

	int updates_per_frame_multiplier = 1;

	double time = 0.;

	while (time < 0.010) // 10 ms
	{
		double start = realTime();

		for (int i = 0; i < updates_per_frame_multiplier; ++i)
			moveBodies(bodies, bodies_number, NULL, NULL, 0.);

		time = realTime() - start;

		printf("updates_per_frame_multiplier = %d => time = %.2f ms\n", updates_per_frame_multiplier, 1000. * time);

		updates_per_frame_multiplier *= 2;
	}

	Quit = 1;
}


Body** simul_EarthMoonShip(int *bodies_number, Body **ship)
{
	*bodies_number = 3;

	Body **bodies = (Body**) calloc(*bodies_number, sizeof(Body*));

	bodies[0] = createBody("Nostromo", Spaceship, 15., 1.0e3, -1.0e8, -1.0e8, 500., -500.);
	bodies[1] = createBody("Earth", Planet, 6.360e6, 5.97e24, 0, 0, 0, 0);
	bodies[2] = createBody("Moon", Moon, 1.736e6, 7.35e22, 3.85e8, 0, 0, 1023.2);

	*ship = bodies[0];

	// printBodyInfo(bodies[0]);
	// printBodyInfo(bodies[1]);
	// printBodyInfo(bodies[2]);

	return bodies;
}


Body** simul_3Earths(int *bodies_number, Body **ship)
{
	*bodies_number = 3;

	Body **bodies = (Body**) calloc(*bodies_number, sizeof(Body*));

	bodies[0] = createBody("Earth_0", Planet, 6.360e6, 5.97e24, 0, 0, 0, 0);
	bodies[1] = createBody("Earth_1", Planet, 6.360e6, 5.97e24, -1e8, 0, 0, -1500.);
	bodies[2] = createBody("Earth_2", Planet, 6.360e6, 5.97e24, 1e8, 0, 0, 1500.);

	*ship = NULL;

	return bodies;
}


Body** simul_manyBodies(int *bodies_number, Body **ship)
{
	double radius = 5e6;
	double mass = 6e24;
	double init_dist_bound = 7e8;
	double init_speed_bound = 100.;

	*bodies_number = 256;

	printf("\nNumber of bodies: %d\n", *bodies_number);

	Body **bodies = (Body**) calloc(*bodies_number, sizeof(Body*));

	if (bodies == NULL)
	{
		printf("\nNot enough memory to store all the bodies.\n\n");
		exit(EXIT_FAILURE);
	}

	char name_string[20];

	for (int i = 0; i < *bodies_number; ++i)
	{
		sprintf(name_string, "Earth_%d", i);
		double posX = unif_rand(-init_dist_bound, init_dist_bound);
		double posY = unif_rand(-init_dist_bound, init_dist_bound);
		double speedX = unif_rand(-init_speed_bound, init_speed_bound);
		double speedY = unif_rand(-init_speed_bound, init_speed_bound);

		bodies[i] = createBody(name_string, Planet, radius, mass, posX, posY, speedX, speedY); // Earth-like planet.
	}

	*ship = NULL;

	return bodies;
}
