#ifndef BODIES_H
#define BODIES_H


#include "SDLA.h"
#include "settings.h"


extern TTF_Font *font_small;
extern SDL_Color White;
extern const double GravitationalConst;


// Macro used for creating the list of Body:
#define APPLY_BODY(FUN)			\
	FUN(Black_Hole)				\
	FUN(Neutron_Star)			\
	FUN(Star)					\
	FUN(Planet)					\
	FUN(Moon)					\
	FUN(Asteroid)				\
	FUN(Comet)					\
	FUN(Spaceship)				\


#define TO_STRING(STRING) #STRING,
#define SUM_MACRO(ENUM) + 1
#define ID_MACRO(ENUM) ENUM,


// Enum of Body:
typedef enum {APPLY_BODY(ID_MACRO)} BodyType;


typedef struct
{
	char Name[MAX_NAME_LENGTH + 1];

	const BodyType Type;
	const double Radius;
	const double Mass;
	const double GravityFactor; // For optimization.

	double PosX;
	double PosY;
	double SpeedX;
	double SpeedY;
	double AccelX;
	double AccelY;

	SDL_Texture *TextureName;
} Body;


// Returns the number of supported BodyType:
short getBodyTypeNumber(void);


// Get the name of a BodyType:
const char* getBodyTypeName(BodyType type);


// Get a BodyType from its name:
BodyType getBodyID(char *string);


// Free with a regular free() call.
Body* createBody(char *name, BodyType type, double radius, double mass,
	double initPosX, double initPosY, double initSpeedX, double initSpeedY);


// Necessary to use this over a regular free() call, in case textures have been created.
void freeBody(Body *body);


void printBodyInfo(Body *body);


#endif
