#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bodies.h"
#include "physics.h"


// Number of supported BodyType:
static const short BodyTypeNumber = APPLY_BODY(SUM_MACRO);


// Array of string of each BodyType:
static const char* BodyTypeStringArray[] = {APPLY_BODY(TO_STRING)};


// Returns the number of supported BodyType:
short getBodyTypeNumber(void)
{
	return BodyTypeNumber;
}


// Get the name of a BodyType:
const char* getBodyTypeName(BodyType type)
{
	if (type > BodyTypeNumber)
	{
		printf("\nInvalid Body.\n");
		return NULL;
	}

	return BodyTypeStringArray[type];
}


// Get a BodyType from its name:
BodyType getBodyID(char *string)
{
	short index = 0;

	while (index < BodyTypeNumber && strcmp(string, BodyTypeStringArray[index]) != 0)
		++index;

	if (index == BodyTypeNumber)
	{
		printf("\nNo matching Body found for the string: %s\n\n", string);
		exit(EXIT_FAILURE);
	}

	return index;
}


// Free with a regular free() call.
Body* createBody(char *name, BodyType type, double radius, double mass,
	double initPosX, double initPosY, double initSpeedX, double initSpeedY)
{
	Body *body = (Body*) calloc(1, sizeof(Body));

	// Copying the name, including the final '\0'. Name will be truncated if too big:
	snprintf(body -> Name, MAX_NAME_LENGTH + 1, "%s", name);

	*(BodyType*) &(body -> Type) = type;
	*(double*) &(body -> Radius) = radius;
	*(double*) &(body -> Mass) = mass;
	*(double*) &(body -> GravityFactor) = GravitationalConst * mass; // For optimization.

	body -> PosX = initPosX;
	body -> PosY = initPosY;
	body -> SpeedX = initSpeedX;
	body -> SpeedY = initSpeedY;
	body -> AccelX = 0.; // by default.
	body -> AccelY = 0.; // by default.

	// Must be done no matter the value of 'DrawAllNames':
	body -> TextureName = SDLA_CreateTextTexture(font_small, &White, body -> Name); // truncated name if needed.

	return body;
}


// Necessary to use this over a regular free() call, in case textures have been created.
void freeBody(Body *body)
{
	if (body == NULL)
		return;

	SDL_DestroyTexture(body -> TextureName);
	free(body);
}


void printBodyInfo(Body *body)
{
	if (body == NULL)
		return;

	printf("Name: %s, Type: %s\n", body -> Name, getBodyTypeName(body -> Type));
	printf("Radius: %.2e m, Mass: %.2e kg\n", body -> Radius, body -> Mass);
	printf("PosX: %.2e m, PosY: %.2e m\n", body -> PosX, body -> PosY);
	printf("SpeedX: %.2e m/s, SpeedY: %.2e m/s\n", body -> SpeedX, body -> SpeedY);
	printf("AccelX: %.2e m/s2, AccelY: %.2e m/s2\n\n", body -> AccelX, body -> AccelY);
}
