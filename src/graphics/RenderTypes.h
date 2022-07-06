#ifndef __DECKARD_RENDERTYPES_H__
#define __DECKARD_RENDERTYPES_H__

#include <math/VecTypes.h>

typedef struct {
	dVEC4			position;
	dVEC4			color;
	dVEC2			tex_coord;
	dVEC3			normal;
} dVERTEX;

typedef struct {
	dVERTEX		   *vertices;
	unsigned char	size;
} dPRIMATIVE;

typedef struct {
	dVEC3			position;
	dVEC4			ambient_color;
	dVEC4			diffuse_color;
	float			const_atten;
	float			linear_atten;
	float			quadratic_atten;
} dLIGHT;

typedef struct {
	float			left;
	float			right;
	float			bottom;
	float			top;
	float			near_val;
	float			far_val;
} dFRUSTUM;

typedef struct {
	int				x;
	int				y;
	int				width;
	int				height;
} dVIEWPORT;

typedef struct {
	dVEC3			position;
	dVEC4			color;
} dFRAGMENT;

typedef struct {
	unsigned char  *color;
	float		   *depth;
	int				width;
	int				height;
} dFRAMEBUFFER;

typedef struct {
	dVERTEX		   *vertices;
	int				vertex_count;
} dMODEL;

typedef struct {
	dVEC3			position;
	dVEC3			rotation;
} dENTITY;

#endif // __DECKARD_RENDERTYPES_H__
