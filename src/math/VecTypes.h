#ifndef __DECKARD_VECTYPES_H__
#define __DECKARD_VECTYPES_H__

// vector types
typedef struct { float x, y;       } dVEC2;
typedef struct { float x, y, z;    } dVEC3;
typedef struct { float x, y, z, w; } dVEC4;

// matrix types
typedef float dMAT2[2][2];
typedef float dMAT3[3][3];
typedef float dMAT4[4][4];

// ray type
typedef struct { dVEC3 position, direction; } dRAY;

// plane type
typedef struct { dVEC3 normal, position; } dPLANE;

void	dVT_SetMat4Identity( dMAT4 *m );
void	dVT_SetMat4Zero( dMAT4 *m );

#endif // __DECKARD_VECTYPES_H__
