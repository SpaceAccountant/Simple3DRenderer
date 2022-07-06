#include "Geometry.h"

#include <math.h>

#include <math/VecMath.h>

#define D_MIN( X, Y )		( ( X ) < ( Y ) ? ( X ) : ( Y ) )
#define D_MAX( X, Y )		( ( X ) > ( Y ) ? ( X ) : ( Y ) )
#define D_CLAMP( X, Y, Z )	( D_MIN( D_MAX( ( X ), ( Y ) ), ( Z ) ) )
#define D_ABS( X )			( ( X ) < 0 ? -( X ) : ( X ) )

#include <stdlib.h>

void dRG_TransformationModel( const dVERTEX *input, int count, const dMAT4 *model, dVERTEX *output ) {
	dMAT4 nm;
	int i;
	dVEC4 n;

	// calculate the normal transformation matrix
	dVM_InvertMat4   ( model, &nm );
	dVM_TransposeMat4( &nm,   &nm );

	for( i = 0; i < count; i++ ) {
		// copy all of the vertex information to the output
		output[i] = input[i];

		// transform the vertex using the model-view matrix
		dVM_MulMat4xVec4( model, &( output[i].position ), &( output[i].position ) );

		// get the vertex normal as a 4 dimensional vector
		n.x = output[i].normal.x;
		n.y = output[i].normal.y;
		n.z = output[i].normal.z;
		n.w = 0.0f;

		// transform the normal using the normal matrix
		dVM_MulMat4xVec4( &nm, &n, &n );

		// copy the transformed normal to the output vertex
		output[i].normal.x = n.x;
		output[i].normal.y = n.y;
		output[i].normal.z = n.z;
	}
}

void dRG_Lighting( const dVERTEX *input, int count, const dMAT4 *view, const dLIGHT *lights, int light_count, dVERTEX *output ) {
	int i, j;
	dVEC3 dir, n;
	float len;
	float diff, atten;
	dVEC4 ambient, diffuse;
	dVEC4 total_ambient, total_diffuse;

	for( i = 0; i < count; i++ ) {
		output[i] = input[i];

		if( light_count > 0 ) {
			total_ambient.x = total_ambient.y = total_ambient.z = total_ambient.w = 0.0f;
			total_diffuse.x = total_diffuse.y = total_diffuse.z = total_diffuse.w = 0.0f;

			for( j = 0; j < light_count; j++ ) {
				dir.x = lights[j].position.x - output[i].position.x;
				dir.y = lights[j].position.y - output[i].position.y;
				dir.z = lights[j].position.z - output[i].position.z;

				len = dVM_LengthVec3( &dir );
			
				dVM_NormalizeVec3( &( output[i].normal ), &n );
				dVM_NormalizeVec3( &dir, &dir );
			
				diff  = dVM_DotVec3( &n, &dir );
				diff  = D_MAX( 0.0f, diff );
				atten = 1.0f / ( lights[j].const_atten + lights[j].linear_atten * len + lights[j].quadratic_atten * len * len );
				
				dVM_MulVec4( atten, &( lights[j].ambient_color ), &ambient );
				dVM_AddVec4( &total_ambient, &ambient, &total_ambient );

				dVM_MulVec4( atten * diff, &( lights[j].diffuse_color ), &diffuse );
				dVM_AddVec4( &total_diffuse, &diffuse, &total_diffuse );
			}

			output[i].color.x *= total_diffuse.x + total_ambient.x;
			output[i].color.y *= total_diffuse.y + total_ambient.y;
			output[i].color.z *= total_diffuse.z + total_ambient.z;

			D_CLAMP( output[i].color.x, 0.0f, 1.0f );
			D_CLAMP( output[i].color.y, 0.0f, 1.0f );
			D_CLAMP( output[i].color.y, 0.0f, 1.0f );
		}
	}
}

void dRG_TransformationView( const dVERTEX *input, int count, const dMAT4 *view, dVERTEX *output ) {
	int i;

	for( i = 0; i < count; i++ ) {
		// copy all of the vertex information to the output
		output[i] = input[i];

		// transform the vertex using the model-view matrix
		dVM_MulMat4xVec4( view, &( output[i].position ), &( output[i].position ) );
	}
}

void dRG_Projection( const dVERTEX *input, int count, const dMAT4 *projection, dVERTEX *output ) {
	int i;

	for( i = 0; i < count; i++ ) {
		// copy all of the vertex information to the output
		output[i] = input[i];

		// transform the vertex using the provided projection matrix
		dVM_MulMat4xVec4( projection, &( input[i].position ), &( output[i].position ) );
	}
}

static inline int dRG_TestVertexBounds( const dVEC4 *position ) {
	return position->w > 1;
}

static inline int dRG_FindLineIntersections( const dVEC4 *v0, const dVEC4 *v1, dVEC4 *intersection, float *trace ) {
	float t;
	dVEC4 v;

	t = v1->w - v0->w;

	if( t == 0 )
		return 0;
	
	t = ( 1.0f - v0->w ) / t;

	if( t <= 0.0f || t >= 1.0f )
		return 0;

	dVM_SubVec4( v1, v0, &v );
	dVM_MulVec4( t, &v, &v );
	dVM_AddVec4( v0, &v, intersection );

	( *trace ) = t;

	return 1;
}

static inline void dRG_InterpolateValueVec3( const dVEC3 *v0, const dVEC3 *v1, float t, dVEC3 *vi ) {
	dVEC3 tmp;

	dVM_MulVec3( 1.0f - t, v0, vi );
	dVM_MulVec3( t, v1, &tmp );
	dVM_AddVec3( v0, &tmp, vi );
}

static inline void dRG_InterpolateValueVec4( const dVEC4 *v0, const dVEC4 *v1, float t, dVEC4 *vi ) {
	dVEC4 tmp;

	dVM_MulVec4( 1.0f - t, v0, vi );
	dVM_MulVec4( t, v1, &tmp );
	dVM_AddVec4( v0, &tmp, vi );
}

void dRG_Cliping( dVERTEX *input, int count, dPRIMATIVE *output, int *output_count ) {
	int i, j, k;
	int i0, i1, i2;
	float t0, t1, t2;
	dVEC4 v0, v1, v2;
	
	for( i = 0, j = 0, k = 0; i < count; i += 3 ) {
		i0 = dRG_TestVertexBounds( &( input[i    ].position ) );
		i1 = dRG_TestVertexBounds( &( input[i + 1].position ) );
		i2 = dRG_TestVertexBounds( &( input[i + 2].position ) );

		if( i0 && i1 && i2 ) {
			// everything is normal
			output[j].size     = 3;
			output[j].vertices = input + i;
			
			j++;
		} else if( i0 || i1 || i2 ) {
			// Everything beneth this comment is clipping.
			//
			// How clipping works in here is that a triangle is only clipped against
			// the w = 1 plane and new tirangles are formed.

			i0 = dRG_FindLineIntersections( &( input[i    ].position ), &( input[i + 1].position ), &v0, &t0 );
			i1 = dRG_FindLineIntersections( &( input[i + 1].position ), &( input[i + 2].position ), &v1, &t1 );
			i2 = dRG_FindLineIntersections( &( input[i + 2].position ), &( input[i    ].position ), &v2, &t2 );

			if( i0 && i1 ) {
				if( input[i + 1].position.w > 1 ) {
					input[i    ].position = v0;
					input[i + 2].position = v1;
					
					dRG_InterpolateValueVec4( &( input[i    ].color  ), &( input[i + 1].color  ), t0, &( input[i    ].color  ) );
					dRG_InterpolateValueVec3( &( input[i    ].normal ), &( input[i + 1].normal ), t0, &( input[i    ].normal ) );

					dRG_InterpolateValueVec4( &( input[i + 1].color  ), &( input[i + 2].color  ), t1, &( input[i + 2].color  ) );
					dRG_InterpolateValueVec3( &( input[i + 1].normal ), &( input[i + 2].normal ), t1, &( input[i + 2].normal ) );

					output[j].size     = 3;
					output[j].vertices = input + i;

					j++;
				} else {
					input[count + k    ] = input[i + 2];

					input[i + 1].position = v0;

					input[count + k + 1].position = v0;
					input[count + k + 2].position = v1;

					dRG_InterpolateValueVec4( &( input[i    ].color  ), &( input[i + 1].color  ), t0, &( input[count + k + 1].color  ) );
					dRG_InterpolateValueVec3( &( input[i    ].normal ), &( input[i + 1].normal ), t0, &( input[count + k + 1].normal ) );

					dRG_InterpolateValueVec4( &( input[i + 1].color  ), &( input[i + 2].color  ), t1, &( input[count + k + 2].color  ) );
					dRG_InterpolateValueVec3( &( input[i + 1].normal ), &( input[i + 2].normal ), t1, &( input[count + k + 2].normal ) );

					dRG_InterpolateValueVec4( &( input[i    ].color  ), &( input[i + 1].color  ), t0, &( input[i + 1].color  ) );
					dRG_InterpolateValueVec3( &( input[i    ].normal ), &( input[i + 1].normal ), t0, &( input[i + 1].normal ) );

					output[j    ].size     = 3;
					output[j    ].vertices = input + i;
					output[j + 1].size     = 3;
					output[j + 1].vertices = input + count + k;

					j += 2;
					k += 3;
				}
			} else if( i1 && i2 ) {
				if( input[i + 2].position.w > 1 ) {
					input[i    ].position = v2;
					input[i + 1].position = v1;

					dRG_InterpolateValueVec4( &( input[i + 1].color  ), &( input[i + 2].color  ), t1, &( input[i + 1].color  ) );
					dRG_InterpolateValueVec3( &( input[i + 1].normal ), &( input[i + 2].normal ), t1, &( input[i + 1].normal ) );

					dRG_InterpolateValueVec4( &( input[i + 2].color  ), &( input[i    ].color  ), t2, &( input[i    ].color  ) );
					dRG_InterpolateValueVec3( &( input[i + 2].normal ), &( input[i    ].normal ), t2, &( input[i    ].normal ) );

					output[j].size     = 3;
					output[j].vertices = input + i;

					j++;
				} else {
					input[count + k    ] = input[i    ];

					input[i + 2].position = v1;

					input[count + k + 1].position = v1;
					input[count + k + 2].position = v2;

					dRG_InterpolateValueVec4( &( input[i + 1].color  ), &( input[i + 2].color  ), t1, &( input[count + k + 1].color  ) );
					dRG_InterpolateValueVec3( &( input[i + 1].normal ), &( input[i + 2].normal ), t1, &( input[count + k + 1].normal ) );

					dRG_InterpolateValueVec4( &( input[i + 2].color  ), &( input[i    ].color  ), t2, &( input[count + k + 2].color  ) );
					dRG_InterpolateValueVec3( &( input[i + 2].normal ), &( input[i    ].normal ), t2, &( input[count + k + 2].normal ) );

					dRG_InterpolateValueVec4( &( input[i + 1].color  ), &( input[i + 2].color  ), t1, &( input[i + 2].color  ) );
					dRG_InterpolateValueVec3( &( input[i + 1].normal ), &( input[i + 2].normal ), t1, &( input[i + 2].normal ) );

					output[j    ].size     = 3;
					output[j    ].vertices = input + i;
					output[j + 1].size     = 3;
					output[j + 1].vertices = input + count + k;

					j += 2;
					k += 3;
				}
			} else if( i0 && i2 ) {
				if( input[i    ].position.w > 1 ) {
					input[i + 1].position = v0;
					input[i + 2].position = v2;

					dRG_InterpolateValueVec4( &( input[i + 2].color  ), &( input[i    ].color  ), t2, &( input[i + 2].color  ) );
					dRG_InterpolateValueVec3( &( input[i + 2].normal ), &( input[i    ].normal ), t2, &( input[i + 2].normal ) );

					dRG_InterpolateValueVec4( &( input[i    ].color  ), &( input[i + 1].color  ), t0, &( input[i + 1].color  ) );
					dRG_InterpolateValueVec3( &( input[i    ].normal ), &( input[i + 1].normal ), t0, &( input[i + 1].normal ) );

					output[j].size     = 3;
					output[j].vertices = input + i;

					j++;
				} else {
					input[count + k    ] = input[i + 2];
					
					input[i    ].position = v0;

					input[count + k + 1].position = v2;
					input[count + k + 2].position = v0;

					dRG_InterpolateValueVec4( &( input[i + 2].color  ), &( input[i    ].color  ), t2, &( input[count + k + 1].color  ) );
					dRG_InterpolateValueVec3( &( input[i + 2].normal ), &( input[i    ].normal ), t2, &( input[count + k + 1].normal ) );

					dRG_InterpolateValueVec4( &( input[i    ].color  ), &( input[i + 1].color  ), t0, &( input[count + k + 2].color  ) );
					dRG_InterpolateValueVec3( &( input[i    ].normal ), &( input[i + 1].normal ), t0, &( input[count + k + 2].normal ) );

					dRG_InterpolateValueVec4( &( input[i + 2].color  ), &( input[i    ].color  ), t2, &( input[i    ].color  ) );
					dRG_InterpolateValueVec3( &( input[i + 2].normal ), &( input[i    ].normal ), t2, &( input[i    ].normal ) );

					output[j    ].size     = 3;
					output[j    ].vertices = input + i;
					output[j + 1].size     = 3;
					output[j + 1].vertices = input + count + k;

					j += 2;
					k += 3;
				}
			}
		}
	}

	( *output_count ) = j;
}

void dRG_PerspectiveDivide( const dPRIMATIVE *input, int count, dPRIMATIVE *output ) {
	int i, j;

	for( i = 0; i < count; i++ ) {
		// copy all of the primitive information to the output
		output[i] = input[i];

		for( j = 0; j < output[i].size; j++ ) {
			// divide by the homogenous coordinate
			output[i].vertices[j].position.x /= output[i].vertices[j].position.w;
			output[i].vertices[j].position.y /= output[i].vertices[j].position.w;
			output[i].vertices[j].position.z /= output[i].vertices[j].position.w;
		}
	}
}

void dRG_ViewportTransformation( const dPRIMATIVE *input, int count, const dVIEWPORT *viewport, dPRIMATIVE *output ) {
	int i, j;
	float wh, hh;

	// do this now just so it isn't calculated multiple times in the loop
	wh = viewport->width  * 0.5f;
	hh = viewport->height * 0.5f;

	for( i = 0; i < count; i++ ) {
		// copy all of the primitive information to the output
		output[i] = input[i];

		for( j = 0; j < output[i].size; j++ ) {
			// transform into the viewport region
			output[i].vertices[j].position.x = viewport->x + roundf( ( 1.0f + output[i].vertices[j].position.x ) * wh );
			output[i].vertices[j].position.y = viewport->y + roundf( ( 1.0f - output[i].vertices[j].position.y ) * hh );
		}
	}
}
