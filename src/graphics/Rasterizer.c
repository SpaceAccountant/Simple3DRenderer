#include "Rasterizer.h"

#include <assert.h>
#include <stdlib.h>

#define D_MIN( X, Y )		( ( X ) < ( Y ) ? ( X ) : ( Y ) )
#define D_MAX( X, Y )		( ( X ) > ( Y ) ? ( X ) : ( Y ) )
#define D_CLAMP( X, Y, Z )	( D_MIN( D_MAX( ( X ), ( Y ) ), ( Z ) ) )

// Finds the bound box of the primitive specified by input as AA and BB
static inline void dRR_GetAABB( const dPRIMATIVE *input, dVEC2 *AA, dVEC2 *BB ) {
	AA->x = BB->y = input->vertices[0].position.x;
	AA->y = BB->y = input->vertices[0].position.y;

	for( int i = 0; i < input->size; i++ ) {
		if( input->vertices[i].position.x < AA->x ) AA->x = input->vertices[i].position.x;
		if( input->vertices[i].position.y < AA->y ) AA->y = input->vertices[i].position.y;
		if( input->vertices[i].position.x > BB->x ) BB->x = input->vertices[i].position.x;
		if( input->vertices[i].position.y > BB->y ) BB->y = input->vertices[i].position.y;
	}
}

// Rasters count number of primitives specified by input to the screen specified by output using the colors avaliable in color_map.
void dRR_RasterPrimitives( const dPRIMATIVE *input, int count, const dFRAMEBUFFER *output, const dCOLORMAP *color_map ) {
	for( int i = 0; i < count; i++ ) {
		// check if there is a special case function for the primative
		switch( input[i].size ) {
		case 3:		// the primative is a triangle
			dRR_RasterTriangles( input + i, 1, output, color_map );
			break;
		}
	}
}

// Returns a positive number if the pixel is on the bounding side of the edge v0 and v1 and a negitive number otherwise.
static inline float dRR_TriangleEdgeFunc( const dVEC4 *v0, const dVEC4 *v1, const dVEC4 *pixel ) {
	return ( pixel->x - v0->x ) * ( v1->y - v0->y ) - ( v1->x - v0->x ) * ( pixel->y - v0->y );
}

// Rasters count number of triangles specified by input to the screen specified by output using the colors avaliable in color_map.
void dRR_RasterTriangles( const dPRIMATIVE *input, int count, const dFRAMEBUFFER *output, const dCOLORMAP *color_map ) {
	// NOTE:
	// When reading this keep in mind I was trying to remove all floating point multiplication from the raster loop and
	// also eliminate creating too many variables. Thats why there are so many precalculations and some variables are used for
	// multiple things. Anyway, enjoy the code.
	//
	// Cheers,
	//     Jaiden Clark-Muth

	int max_index;	// the maximum buffer index
	int i;			// the current primitive
	dVEC2 AA, BB;	// bounds of the triangle on the screen
	dVEC4 p;		// fragment position
	dVEC3 a;		// used for the change in barycentric interpolation values per column and initial edge function values per row
	float c;		// used for area, inverse area, and initial z coordinate per row
	dVEC3 d;		// the initial color per row
	dVEC3 dex, dey;	// the change in the edge function values
	float dzx, dzy;	// the change in the z coordinate
	dVEC3 dcx, dcy;	// the change in the color
	dVEC3 e;		// used for the change in barycentric interpolation values per row and edge function values
	int index;		// the buffer index of the fragment
	dVEC3 color;	// the color of the fragment

	max_index = output->width * output->height;

	for( i = 0; i < count; i++ ) {
		// get the area of the triangle
		c = dRR_TriangleEdgeFunc( &( input[i].vertices[0].position ), &( input[i].vertices[1].position ), &( input[i].vertices[2].position ) );

		// the triangle isn't visible if it has no area or negative area
		if( c < 0 )
			continue;

		// get the bounding box of the triangle
		dRR_GetAABB( input + i, &AA, &BB );

		AA.x = D_CLAMP( AA.x, 0, output->width  );
		AA.y = D_CLAMP( AA.y, 0, output->height );
		BB.x = D_CLAMP( BB.x, 0, output->width  );
		BB.y = D_CLAMP( BB.y, 0, output->height );

		// invalid bounds
		if( AA.x >= output->width || AA.y >= output->height || BB.x <= 0 || BB.y <= 0 )
			continue;

		// calculate the change in edge function values per column
		dex.x = input[i].vertices[1].position.y - input[i].vertices[0].position.y;
		dex.y = input[i].vertices[2].position.y - input[i].vertices[1].position.y;
		dex.z = input[i].vertices[0].position.y - input[i].vertices[2].position.y;

		// calculate the change in edge function values per row
		dey.x = input[i].vertices[0].position.x - input[i].vertices[1].position.x;
		dey.y = input[i].vertices[1].position.x - input[i].vertices[2].position.x;
		dey.z = input[i].vertices[2].position.x - input[i].vertices[0].position.x;

		c = 1.0f / c;

		// calculate the change in barycentric interpolation values per column
		a.x = c * dex.y;
		a.y = c * dex.z;
		a.z = c * dex.x;

		// calculate the change in barycentric interpolation values per row
		e.x = c * dey.y;
		e.y = c * dey.z;
		e.z = c * dey.x;

		// calculate the change in depth per column
		dzx = a.x * input[i].vertices[0].position.z + a.y * input[i].vertices[1].position.z + a.z * input[i].vertices[2].position.z;

		// calculate the change in depth per row
		dzy = e.x * input[i].vertices[0].position.z + e.y * input[i].vertices[1].position.z + e.z * input[i].vertices[2].position.z;

		// calculate the change in color per column
		dcx.x = a.x * input[i].vertices[0].color.x + a.y * input[i].vertices[1].color.x + a.z * input[i].vertices[2].color.x;
		dcx.y = a.x * input[i].vertices[0].color.y + a.y * input[i].vertices[1].color.y + a.z * input[i].vertices[2].color.y;
		dcx.z = a.x * input[i].vertices[0].color.z + a.y * input[i].vertices[1].color.z + a.z * input[i].vertices[2].color.z;

		// calculate the change in color per row
		dcy.x = e.x * input[i].vertices[0].color.x + e.y * input[i].vertices[1].color.x + e.z * input[i].vertices[2].color.x;
		dcy.y = e.x * input[i].vertices[0].color.y + e.y * input[i].vertices[1].color.y + e.z * input[i].vertices[2].color.y;
		dcy.z = e.x * input[i].vertices[0].color.z + e.y * input[i].vertices[1].color.z + e.z * input[i].vertices[2].color.z;

		p.x = AA.x;
		p.y = AA.y;

		// calculate the initial edge function values for the primitive
		a.x = dRR_TriangleEdgeFunc( &( input[i].vertices[0].position ), &( input[i].vertices[1].position ), &p );
		a.y = dRR_TriangleEdgeFunc( &( input[i].vertices[1].position ), &( input[i].vertices[2].position ), &p );
		a.z = dRR_TriangleEdgeFunc( &( input[i].vertices[2].position ), &( input[i].vertices[0].position ), &p );

		// calculate the initial interpolation values for each vertex
		e.x = a.y * c;
		e.y = a.z * c;
		e.z = a.x * c;

		// calculate the initial interpolated z coordinate
		c = e.x * input[i].vertices[0].position.z + e.y * input[i].vertices[1].position.z + e.z * input[i].vertices[2].position.z;

		// calculate the initial interpolated color
		d.x = e.x * input[i].vertices[0].color.x + e.y * input[i].vertices[1].color.x + e.z * input[i].vertices[2].color.x;
		d.y = e.x * input[i].vertices[0].color.y + e.y * input[i].vertices[1].color.y + e.z * input[i].vertices[2].color.y;
		d.z = e.x * input[i].vertices[0].color.z + e.y * input[i].vertices[1].color.z + e.z * input[i].vertices[2].color.z;

		// calculate the inital index of the fragment
		index = ( int ) AA.x + ( int ) AA.y * output->width;

		// draw the primative line by line
		for( p.y = AA.y; p.y <= BB.y; p.y++ ) {
			// incremental rasterization
			e     = a;
			p.z   = c;
			color = d;

			// only draw lines that can be seen (should be resolved in clipping instead)
			if( p.y >= 0 && p.y < output->height ) {
				for( p.x = AA.x; p.x <= BB.x; p.x++ ) {

					// only draw rows that can be seen (should be resolved in clipping instead)
					if( p.x >= 0 && p.x < output->width ) {
						if( e.x >= 0 && e.y >= 0 && e.z >= 0 ) {
							if( index >= 0 && index < max_index && p.z <= output->depth[index] ) {
								// store the new closest depth
								output->depth[index] = p.z;

								// draw the fragment to the frame buffer's color part							// ======================================================
								output->color[index] = dCM_FindIndex( color.x, color.y, color.z, color_map );	// FIND INDEX IS A BOTTLENECK ON DEVICES WITH MANY COLORS
																												// ======================================================
							}
						}
					}

					// incremental rasterization
					e.x     += dex.x;
					e.y     += dex.y;
					e.z     += dex.z;
					p.z     += dzx;
					color.x += dcx.x;
					color.y += dcx.y;
					color.z += dcx.z;
					index++;
				}

				index -= ( int ) ( BB.x - AA.x ) + 1;
			}

			// incremental rasterization
			a.x   += dey.x;
			a.y   += dey.y;
			a.z   += dey.z;
			c     += dzy;
			d.x   += dcy.x;
			d.y   += dcy.y;
			d.z   += dcy.z;
			index += output->width;
		}
	}
}
