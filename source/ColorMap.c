#include <stdlib.h>
#include <string.h>

#include "ColorMap.h"

#define D_MIN( X, Y )		( ( X ) < ( Y ) ? ( X ) : ( Y ) )
#define D_MAX( X, Y )		( ( X ) > ( Y ) ? ( X ) : ( Y ) )
#define D_CLAMP( X, Y, Z )	( D_MIN( D_MAX( ( X ), ( Y ) ), ( Z ) ) )
#define D_ABS( X )			( ( X ) < 0 ? -( X ) : ( X ) )

struct _dCOLORMAPELEMENT {
	// the clamped RGB color
	float						red;
	float						green;
	float						blue;

	// the palette index
	unsigned char				index;
};

struct _dCOLORMAP {
	struct _dCOLORMAPELEMENT   *elements;
	int							capacity;
	int							count;
};

dCOLORMAP * dCM_CreateColorMap( void ) {
	dCOLORMAP *map;

	// create the color map
	map = ( dCOLORMAP* ) malloc( sizeof( dCOLORMAP ) );

	if( !map )
		return NULL;

	map->capacity = 5;
	map->elements = ( struct _dCOLORMAPELEMENT* ) malloc( map->capacity * sizeof( struct _dCOLORMAPELEMENT ) );
	map->count    = 0;

	return map;
}

void dCM_DeleteColorMap( dCOLORMAP *color_map ) {
	if( !color_map )
		return;

	// release the color map
	free( color_map->elements );
	free( color_map );
}

char dCM_AddEntry( unsigned char index, float red, float green, float blue, dCOLORMAP *color_map ) {
	int i;
	struct _dCOLORMAPELEMENT *tmp;

	if( !color_map )
		return -1;
	
	// clamp the color
	red   = D_CLAMP( red,   0.0f, 1.0f );
	green = D_CLAMP( green, 0.0f, 1.0f );
	blue  = D_CLAMP( blue,  0.0f, 1.0f );

	i = color_map->count++;

	// resize the color map as needed
	if( color_map->count > color_map->capacity ) {
		color_map->capacity = color_map->capacity + color_map->capacity / 2;

		tmp = ( struct _dCOLORMAPELEMENT* ) malloc( color_map->capacity * sizeof( struct _dCOLORMAPELEMENT ) );

		if( !tmp ) {
			free( color_map->elements );
			free( color_map );

			return -1;
		}

		memcpy( tmp, color_map->elements, color_map->count * sizeof( struct _dCOLORMAPELEMENT ) );
		free( color_map->elements );

		color_map->elements = tmp;
	}

	// store the color in the map
	color_map->elements[i].red   = red;
	color_map->elements[i].green = green;
	color_map->elements[i].blue  = blue;

	return ( color_map->elements[i].index = index );
}

void dCM_RemoveEntry( unsigned char index, dCOLORMAP *color_map ) {
	int i, j;

	if( !color_map )
		return;
	
	for( i = 0; i < color_map->count; i++ )
		if( color_map->elements[i].index == index )
			for( j = i; j < color_map->count; j++ )
				color_map->elements[j] = color_map->elements[j + 1];
}

char dCM_FindIndex( float red, float green, float blue, const dCOLORMAP *color_map ) {
	int i;
	int cur;
	float best;
	float dr, dg, db, sq_dis;

	if( !color_map )
		return -1;

	// clamp the color
	red   = D_CLAMP( red,   0.0f, 1.0f );
	green = D_CLAMP( green, 0.0f, 1.0f );
	blue  = D_CLAMP( blue,  0.0f, 1.0f );

	cur  = -1;
	best =  3.0f;

	for( i = 0; i < color_map->count; i++ ) {
		// get the color component difference
		dr = red   - color_map->elements[i].red;
		dg = green - color_map->elements[i].green;
		db = blue  - color_map->elements[i].blue;

		// calculate the squared length of the distance between the colors in the color space
		sq_dis = dr * dr + dg * dg + db * db;

		if( sq_dis <= best ) {
			best = sq_dis;
			cur  = color_map->elements[i].index;

			// you can't get any beter than exact (trust me, I've tried)
			if( sq_dis == 0 )
				break;
		}
	}

	return cur;
}
