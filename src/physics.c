#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "physics.h"


#define FRAMETIME (1000 / FRAMERATE) // integer
#define REAL_FRAMERATE (1000. / FRAMETIME) // double, != FRAMERATE
#define DELTA_TIME ((double) INIT_TIME_MULTIPLIER / (REAL_FRAMERATE * UPDATES_PER_FRAME)) // Do not modify.


const double GravitationalConst = 6.67430e-11; // m3 / (kg . s2)

const int FrameTime = FRAMETIME; // In ms. Integer!

// This can be changed during runtime:
static double dt = DELTA_TIME; // Time interval.
static double dt2s2 = DELTA_TIME * DELTA_TIME / (2 - CHEAT); // dt * dt / 2 if CHEAT = 0, dt * dt else.
static double FrameTimeMultiplier = (double) INIT_TIME_MULTIPLIER / REAL_FRAMERATE;
static double ElapsedSimulationTime = 0.;
static unsigned int LastSimulationFrameIndex = 0;


// Returns the amount of simulation time a user second represents:
inline double getTimeScale(void)
{
	return FrameTimeMultiplier * REAL_FRAMERATE;
}


// Updates the internal value storing the elapsed simulation time.
// This can be called several time without issues:
inline void updateSimulationTime(void)
{
	ElapsedSimulationTime += (SimulationFrameIndex - LastSimulationFrameIndex) * FrameTimeMultiplier;

	LastSimulationFrameIndex = SimulationFrameIndex;
}


// Returns the elapsed time in the simulation, in seconds:
inline double getSimulationTime(void)
{
	updateSimulationTime();

	return ElapsedSimulationTime;
}


inline void changeSimulationSpeed(double time_multiplier)
{
	updateSimulationTime();

	dt *= time_multiplier;
	dt2s2 = dt * dt / (2 - CHEAT);
	FrameTimeMultiplier *= time_multiplier;
}


inline double distance(double x1, double y1, double x2, double y2)
{
	double delta_x = x1 - x2;
	double delta_y = y1 - y2;

	return sqrt(delta_x * delta_x + delta_y * delta_y);
}


inline int collision(Body **bodies, int indexBody1, int indexBody2, double dist)
{
	Body *body1 = bodies[indexBody1], *body2 = bodies[indexBody2];

	if (body1 == NULL || body2 == NULL)
		return 1; // No gravity update must be done for this interaction.

	double r1 = body1 -> Radius, r2 = body2 -> Radius;

	int status = r1 + r2 >= dist;

	if (status && CollisionsEnabled)
	{
		int indexSurvivor = body1 -> Mass > body2 -> Mass ? indexBody1 : indexBody2;
		int indexLostOne = body1 -> Mass > body2 -> Mass ? indexBody2 : indexBody1;

		Body *survivor = bodies[indexSurvivor], *lostOne = bodies[indexLostOne];

		// Assuming both bodies have same density:
		double new_radius = pow(r1 * r1 * r1 + r2 * r2 * r2, 1./3.);
		double ratio = survivor -> Mass / (survivor -> Mass + lostOne -> Mass);

		*(double*) &(survivor -> Radius) = new_radius;
		*(double*) &(survivor -> Mass) += lostOne -> Mass;
		*(double*) &(survivor -> GravityFactor) = GravitationalConst * survivor -> Mass;

		// This is probably quite unrealistic:
		survivor -> PosX = ratio * survivor -> PosX + (1. - ratio) * lostOne -> PosX;
		survivor -> PosY = ratio * survivor -> PosY + (1. - ratio) * lostOne -> PosY;
		survivor -> SpeedX = ratio * survivor -> SpeedX + (1. - ratio) * lostOne -> SpeedX;
		survivor -> SpeedY = ratio * survivor -> SpeedY + (1. - ratio) * lostOne -> SpeedY;
		survivor -> AccelX = ratio * survivor -> AccelX + (1. - ratio) * lostOne -> AccelX;
		survivor -> AccelY = ratio * survivor -> AccelY + (1. - ratio) * lostOne -> AccelY;

		// Removing the absorbed object:

		freeBody(lostOne);
		bodies[indexLostOne] = NULL;

		if (IndexFollowedBody == indexLostOne)
			IndexFollowedBody = indexSurvivor;
	}

	return status;
}


inline void update_accel_input(Body *ship, Input *input, double thrust)
{
	if (ship == NULL || ship -> Type != Spaceship || ship -> Mass == 0. || input == NULL)
		return;

	double force = thrust / ship -> Mass;

	if (input -> Yinput == UP)
	{
		ship -> AccelY -= force;
	}

	if (input -> Yinput == DOWN)
	{
		ship -> AccelY += force;
	}

	if (input -> Xinput == LEFT)
	{
		ship -> AccelX -= force;
	}

	if (input -> Xinput == RIGHT)
	{
		ship -> AccelX += force;
	}
}


// Initialise the buffer array containing the distance for each interaction:
static void init_DistArray(int interaction_number)
{
	if (DistArray != NULL)
		return;

	DistArray = (double*) calloc(interaction_number, sizeof(double));
}


// Updating each positions simultaneously!
void moveBodies(Body **bodies, int bodies_number, Body *ship, Input *input, double thrust)
{
	int interaction_number = bodies_number * (bodies_number - 1) / 2;

	init_DistArray(interaction_number);

	for (int u = 0; u < UPDATES_PER_FRAME; ++u)
	{
		// Resetting every accelerations:

		for (int i = 0; i < bodies_number; ++i)
		{
			if (bodies[i] == NULL)
				continue;

			bodies[i] -> AccelX = 0.;
			bodies[i] -> AccelY = 0.;
		}

		// Computing every gravity caused accelerations:

		#ifdef ENABLE_MULTITHREADING
			#pragma omp parallel for num_threads(THREAD_NUMBER)
		#endif
		for (int i = 0; i < bodies_number - 1; ++i)
		{
			if (bodies[i] == NULL)
				continue;

			int shift = (bodies_number - 1) * i - (i + 1) * i / 2 - 1; // Computed by hand. Do not factorize by i.

			for (int j = i + 1; j < bodies_number; ++j)
			{
				if (bodies[j] == NULL)
					continue;

				DistArray[shift + j] = distance(bodies[i] -> PosX, bodies[i] -> PosY, bodies[j] -> PosX, bodies[j] -> PosY);
			}
		}

		// Applying on each body the acceleration change due to the gravitational effect:

		int index = 0;

		for (int i = 0; i < bodies_number - 1; ++i)
		{
			for (int j = i + 1; j < bodies_number; ++j)
			{
				double dist = DistArray[index];

				++index;

				if (collision(bodies, i, j, dist))
					continue;

				// NULL checks have been done during the collision, and dist_cubed must be > 0,
				// therefore gravity updates can be done:

				double dist_cubed = dist * dist * dist;

				double scal_x = (bodies[j] -> PosX - bodies[i] -> PosX) / dist_cubed;
				double scal_y = (bodies[j] -> PosY - bodies[i] -> PosY) / dist_cubed;

				bodies[i] -> AccelX += bodies[j] -> GravityFactor * scal_x;
				bodies[i] -> AccelY += bodies[j] -> GravityFactor * scal_y;

				bodies[j] -> AccelX -= bodies[i] -> GravityFactor * scal_x;
				bodies[j] -> AccelY -= bodies[i] -> GravityFactor * scal_y;
			}
		}

		// Managing the ship thrust after the gravity effect, to not erase it:

		update_accel_input(ship, input, thrust);

		// Moving each body:

		for (int i = 0; i < bodies_number; ++i)
		{
			Body *body = bodies[i];

			if (body == NULL)
				continue;

			body -> PosX += body -> SpeedX * dt + body -> AccelX * dt2s2;
			body -> PosY += body -> SpeedY * dt + body -> AccelY * dt2s2;

			body -> SpeedX += body -> AccelX * dt;
			body -> SpeedY += body -> AccelY * dt;
		}
	}
}
