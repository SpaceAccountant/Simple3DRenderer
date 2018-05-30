#include <math.h>

#include "VecMath.h"

void dVM_AddVec3( const dVEC3 *v0, const dVEC3 *v1, dVEC3 *r ) {
	r->x = v0->x + v1->x;
	r->y = v0->y + v1->y;
	r->z = v0->z + v1->z;
}

void dVM_CrossVec3( const dVEC3 *v0, const dVEC3 *v1, dVEC3 *r ) {
	dVEC3 tmp;

	tmp.x = v0->y * v1->z - v0->z * v1->y;
	tmp.y = v0->z * v1->x - v0->x * v1->z;
	tmp.z = v0->x * v1->y - v0->y * v1->x;

	( *r ) = tmp;
}

void dVM_SubVec3( const dVEC3 *v0, const dVEC3 *v1, dVEC3 *r ) {
	r->x = v0->x - v1->x;
	r->y = v0->y - v1->y;
	r->z = v0->z - v1->z;
}

void dVM_MulVec3( float s, const dVEC3 *v, dVEC3 *r ) {
	r->x = v->x * s;
	r->y = v->y * s;
	r->z = v->z * s;
}

void dVM_DivVec3( float s, const dVEC3 *v, dVEC3 *r ) {
	float inv_s;

	if( s == 0 )
		return;

	inv_s = 1.0f / s;

	r->x = v->x * inv_s;
	r->y = v->y * inv_s;
	r->z = v->z * inv_s;
}

float dVM_DotVec3( const dVEC3 *v0, const dVEC3 *v1 ) {
	return v0->x * v1->x + v0->y * v1->y + v0->z * v1->z;
}

float dVM_LengthVec3( const dVEC3 *v ) {
	return sqrtf( dVM_LengthSquaredVec3( v ) );
}

float dVM_LengthSquaredVec3( const dVEC3 *v ) {
	return dVM_DotVec3( v, v );
}

void dVM_NormalizeVec3( const dVEC3 *v, dVEC3 *r ) {
	float len;

	len = dVM_LengthVec3( v );

	dVM_DivVec3( len == 0.0f ? 1.0f : len, v, r );
}

void dVM_RotateVec3( float angle, float x, float y, float z, const dVEC3 *v, dVEC3 *r ) {
	dVEC3 axis;
	float c, s, oc, sx, sy, sz, xyc, xzc, yzc;
	dMAT3 rotation;

	// get the axis unit vector
	axis.x = x;
	axis.y = y;
	axis.z = z;
	dVM_NormalizeVec3( &axis, &axis );

	// avoid duplicate calculations
	c   = cosf( -angle );
	s   = sinf( -angle );
	oc  = 1.0f - c;
	sx  = s * axis.x;
	sy  = s * axis.y;
	sz  = s * axis.z;
	xyc = axis.x * axis.y * oc;
	xzc = axis.x * axis.z * oc;
	yzc = axis.y * axis.z * oc;

	// create the rotation matrix
	rotation[0][0] = axis.x * axis.x * oc + c;
	rotation[0][1] = xyc - sz;
	rotation[0][2] = xzc + sy;
	rotation[1][0] = xyc + sz;
	rotation[1][1] = axis.y * axis.y * oc + c;
	rotation[1][2] = yzc - sx;
	rotation[2][0] = xyc - sy;
	rotation[2][1] = yzc + sx;
	rotation[2][2] = axis.z * axis.z * oc + c;

	// rotate the vector
	dVM_MulMat3xVec3( &rotation, v, r );
}

void dVM_ReflectVec3( const dVEC3 *v0, const dVEC3 *v1, dVEC3 *r ) {
	dVEC3 tmp;

	dVM_MulVec3( 2.0f * dVM_DotVec3( v0, v1 ) / dVM_LengthSquaredVec3( v1 ), v1, &tmp );
	dVM_SubVec3( &tmp, v0, r );
}

void dVM_AddVec4( const dVEC4 *v0, const dVEC4 *v1, dVEC4 *r ) {
	r->x = v0->x + v1->x;
	r->y = v0->y + v1->y;
	r->z = v0->z + v1->z;
	r->w = v0->w + v1->w;
}

void dVM_SubVec4( const dVEC4 *v0, const dVEC4 *v1, dVEC4 *r ) {
	r->x = v0->x - v1->x;
	r->y = v0->y - v1->y;
	r->z = v0->z - v1->z;
	r->w = v0->w - v1->w;
}

void dVM_MulVec4( float s, const dVEC4 *v, dVEC4 *r ) {
	r->x = v->x * s;
	r->y = v->y * s;
	r->z = v->z * s;
	r->w = v->w * s;
}

float dVM_DotVec4( const dVEC4 *v0, const dVEC4 *v1 ) {
	return v0->x * v1->x + v0->y * v1->y + v0->z * v1->z + v0->w * v1->w;
}

float dVM_DeterminantMat2( const dMAT2 *m ) {
	return ( *m )[0][0] * ( *m )[1][1] - ( *m )[0][1] * ( *m )[1][0];
}

void dVM_DivMat2( float s, const dMAT2 *m, dMAT2 *r ) {
	float inv_s;

	if( s == 0 )
		return;

	inv_s = 1.0f / s;

	( *r )[0][0] = ( *m )[0][0] * inv_s;
	( *r )[0][1] = ( *m )[0][1] * inv_s;
	( *r )[1][0] = ( *m )[1][0] * inv_s;
	( *r )[1][1] = ( *m )[1][1] * inv_s;
}

float dVM_InvertMat2( const dMAT2 *m, dMAT2 *r ) {
	float det;

	det = dVM_DeterminantMat2( m );

	if( det == 0 )
		return 0;
	
	( *r )[0][0] =  ( *m )[0][0];
	( *r )[0][1] = -( *m )[0][1];
	( *r )[1][0] = -( *m )[1][0];
	( *r )[1][1] =  ( *m )[1][1];

	dVM_DivMat2( det, r, r );

	return det;
}

void dVM_MulMat2xVec2( const dMAT2 *m, const dVEC2 *v, dVEC2 *r ) {
	dVEC2 tmp;

	tmp.x = ( *m )[0][0] * v->x + ( *m )[0][1] * v->y;
	tmp.y = ( *m )[1][0] * v->x + ( *m )[1][1] * v->y;

	( *r ) = tmp;
}

float dVM_InvertMat3( const dMAT3 *m, dMAT3 *r ) {
	dMAT3 cofactors;
	float det, inv_d;

	cofactors[0][0] =  ( *m )[0][0] * ( ( *m )[1][1] * ( *m )[2][2] - ( *m )[1][2] - ( *m )[2][1] );
	cofactors[0][1] = -( *m )[0][1] * ( ( *m )[1][0] * ( *m )[2][2] - ( *m )[1][2] - ( *m )[2][0] );
	cofactors[0][2] =  ( *m )[0][2] * ( ( *m )[1][0] * ( *m )[2][1] - ( *m )[1][1] - ( *m )[2][0] );
	cofactors[1][0] = -( *m )[1][0] * ( ( *m )[0][1] * ( *m )[2][2] - ( *m )[0][2] - ( *m )[2][1] );
	cofactors[1][1] =  ( *m )[1][1] * ( ( *m )[0][0] * ( *m )[2][2] - ( *m )[0][2] - ( *m )[2][0] );
	cofactors[1][2] = -( *m )[1][2] * ( ( *m )[0][0] * ( *m )[2][1] - ( *m )[0][1] - ( *m )[2][0] );
	cofactors[2][0] =  ( *m )[2][0] * ( ( *m )[0][1] * ( *m )[1][2] - ( *m )[0][2] - ( *m )[1][1] );
	cofactors[2][1] = -( *m )[2][1] * ( ( *m )[0][0] * ( *m )[1][2] - ( *m )[0][2] - ( *m )[1][0] );
	cofactors[2][2] =  ( *m )[2][2] * ( ( *m )[0][0] * ( *m )[1][1] - ( *m )[0][1] - ( *m )[1][0] );

	det = cofactors[0][0] + cofactors[0][1] + cofactors[0][2];

	if( det == 0 )
		return 0;

	inv_d = 1.0f / det;

	( *r )[0][0] = ( *m )[0][0] * inv_d;
	( *r )[0][1] = ( *m )[0][1] * inv_d;
	( *r )[0][2] = ( *m )[0][2] * inv_d;
	( *r )[1][0] = ( *m )[1][0] * inv_d;
	( *r )[1][1] = ( *m )[1][1] * inv_d;
	( *r )[1][2] = ( *m )[1][2] * inv_d;
	( *r )[2][0] = ( *m )[2][0] * inv_d;
	( *r )[2][1] = ( *m )[2][1] * inv_d;
	( *r )[2][2] = ( *m )[2][2] * inv_d;

	return det;
}

void dVM_MulMat3( float s, const dMAT3 *m, dMAT3 *r ) {
	( *r )[0][0] = ( *m )[0][0] * s;
	( *r )[0][1] = ( *m )[0][1] * s;
	( *r )[0][2] = ( *m )[0][2] * s;
	( *r )[1][0] = ( *m )[1][0] * s;
	( *r )[1][1] = ( *m )[1][1] * s;
	( *r )[1][2] = ( *m )[1][2] * s;
	( *r )[2][0] = ( *m )[2][0] * s;
	( *r )[2][1] = ( *m )[2][1] * s;
	( *r )[2][2] = ( *m )[2][2] * s;
}

void dVM_MulMat3xVec3( const dMAT3 *m, const dVEC3 *v, dVEC3 *r ) {
	dVEC3 tmp;

	tmp.x = ( *m )[0][0] * v->x + ( *m )[0][1] * v->y + ( *m )[0][2] * v->z;
	tmp.y = ( *m )[1][0] * v->x + ( *m )[1][1] * v->y + ( *m )[1][2] * v->z;
	tmp.z = ( *m )[2][0] * v->x + ( *m )[2][1] * v->y + ( *m )[2][2] * v->z;
	
	( *r ) = tmp;
}

void dVM_InvertMat4( const dMAT4 *m, dMAT4 *r ) {
	// =========================================================
	// DOES NOT INVERT CURRENTLY, ONLY TRANSPOSES
	// WORKS FINE FOR ANYTHING BUT NON-UNIFORMLY SCALED MATRICES
	// =========================================================
	dVM_TransposeMat4( m, r );
}

void dVM_MulMat4xMat4( const dMAT4 *m0, const dMAT4 *m1, dMAT4 *r ) {
	dMAT4 tmp;

	tmp[0][0] = ( *m0 )[0][0] * ( *m1 )[0][0] + ( *m0 )[0][1] * ( *m1 )[1][0] + ( *m0 )[0][2] * ( *m1 )[2][0] + ( *m0 )[0][3] * ( *m1 )[3][0];
	tmp[0][1] = ( *m0 )[0][0] * ( *m1 )[0][1] + ( *m0 )[0][1] * ( *m1 )[1][1] + ( *m0 )[0][2] * ( *m1 )[2][1] + ( *m0 )[0][3] * ( *m1 )[3][1];
	tmp[0][2] = ( *m0 )[0][0] * ( *m1 )[0][2] + ( *m0 )[0][1] * ( *m1 )[1][2] + ( *m0 )[0][2] * ( *m1 )[2][2] + ( *m0 )[0][3] * ( *m1 )[3][2];
	tmp[0][3] = ( *m0 )[0][0] * ( *m1 )[0][3] + ( *m0 )[0][1] * ( *m1 )[1][3] + ( *m0 )[0][2] * ( *m1 )[2][3] + ( *m0 )[0][3] * ( *m1 )[3][3];
	tmp[1][0] = ( *m0 )[1][0] * ( *m1 )[0][0] + ( *m0 )[1][1] * ( *m1 )[1][0] + ( *m0 )[1][2] * ( *m1 )[2][0] + ( *m0 )[1][3] * ( *m1 )[3][0];
	tmp[1][1] = ( *m0 )[1][0] * ( *m1 )[0][1] + ( *m0 )[1][1] * ( *m1 )[1][1] + ( *m0 )[1][2] * ( *m1 )[2][1] + ( *m0 )[1][3] * ( *m1 )[3][1];
	tmp[1][2] = ( *m0 )[1][0] * ( *m1 )[0][2] + ( *m0 )[1][1] * ( *m1 )[1][2] + ( *m0 )[1][2] * ( *m1 )[2][2] + ( *m0 )[1][3] * ( *m1 )[3][2];
	tmp[1][3] = ( *m0 )[1][0] * ( *m1 )[0][3] + ( *m0 )[1][1] * ( *m1 )[1][3] + ( *m0 )[1][2] * ( *m1 )[2][3] + ( *m0 )[1][3] * ( *m1 )[3][3];
	tmp[2][0] = ( *m0 )[2][0] * ( *m1 )[0][0] + ( *m0 )[2][1] * ( *m1 )[1][0] + ( *m0 )[2][2] * ( *m1 )[2][0] + ( *m0 )[2][3] * ( *m1 )[3][0];
	tmp[2][1] = ( *m0 )[2][0] * ( *m1 )[0][1] + ( *m0 )[2][1] * ( *m1 )[1][1] + ( *m0 )[2][2] * ( *m1 )[2][1] + ( *m0 )[2][3] * ( *m1 )[3][1];
	tmp[2][2] = ( *m0 )[2][0] * ( *m1 )[0][2] + ( *m0 )[2][1] * ( *m1 )[1][2] + ( *m0 )[2][2] * ( *m1 )[2][2] + ( *m0 )[2][3] * ( *m1 )[3][2];
	tmp[2][3] = ( *m0 )[2][0] * ( *m1 )[0][3] + ( *m0 )[2][1] * ( *m1 )[1][3] + ( *m0 )[2][2] * ( *m1 )[2][3] + ( *m0 )[2][3] * ( *m1 )[3][3];
	tmp[3][0] = ( *m0 )[3][0] * ( *m1 )[0][0] + ( *m0 )[3][1] * ( *m1 )[1][0] + ( *m0 )[3][2] * ( *m1 )[2][0] + ( *m0 )[3][3] * ( *m1 )[3][0];
	tmp[3][1] = ( *m0 )[3][0] * ( *m1 )[0][1] + ( *m0 )[3][1] * ( *m1 )[1][1] + ( *m0 )[3][2] * ( *m1 )[2][1] + ( *m0 )[3][3] * ( *m1 )[3][1];
	tmp[3][2] = ( *m0 )[3][0] * ( *m1 )[0][2] + ( *m0 )[3][1] * ( *m1 )[1][2] + ( *m0 )[3][2] * ( *m1 )[2][2] + ( *m0 )[3][3] * ( *m1 )[3][2];
	tmp[3][3] = ( *m0 )[3][0] * ( *m1 )[0][3] + ( *m0 )[3][1] * ( *m1 )[1][3] + ( *m0 )[3][2] * ( *m1 )[2][3] + ( *m0 )[3][3] * ( *m1 )[3][3];
	
	( *r )[0][0] = tmp[0][0];
	( *r )[0][1] = tmp[0][1];
	( *r )[0][2] = tmp[0][2];
	( *r )[0][3] = tmp[0][3];
	( *r )[1][0] = tmp[1][0];
	( *r )[1][1] = tmp[1][1];
	( *r )[1][2] = tmp[1][2];
	( *r )[1][3] = tmp[1][3];
	( *r )[2][0] = tmp[2][0];
	( *r )[2][1] = tmp[2][1];
	( *r )[2][2] = tmp[2][2];
	( *r )[2][3] = tmp[2][3];
	( *r )[3][0] = tmp[3][0];
	( *r )[3][1] = tmp[3][1];
	( *r )[3][2] = tmp[3][2];
	( *r )[3][3] = tmp[3][3];
}

void dVM_MulMat4xVec4( const dMAT4 *m, const dVEC4 *v, dVEC4 *r ) {
	dVEC4 tmp;

	tmp.x = ( *m )[0][0] * v->x + ( *m )[0][1] * v->y + ( *m )[0][2] * v->z + ( *m )[0][3] * v->w;
	tmp.y = ( *m )[1][0] * v->x + ( *m )[1][1] * v->y + ( *m )[1][2] * v->z + ( *m )[1][3] * v->w;
	tmp.z = ( *m )[2][0] * v->x + ( *m )[2][1] * v->y + ( *m )[2][2] * v->z + ( *m )[2][3] * v->w;
	tmp.w = ( *m )[3][0] * v->x + ( *m )[3][1] * v->y + ( *m )[3][2] * v->z + ( *m )[3][3] * v->w;
	
	( *r ) = tmp;
}

void dVM_TransposeMat4( const dMAT4 *m, dMAT4 *r ) {
	dMAT4 tmp;

	tmp[0][0] = ( *m )[0][0];
	tmp[0][1] = ( *m )[1][0];
	tmp[0][2] = ( *m )[2][0];
	tmp[0][3] = ( *m )[3][0];
	tmp[1][0] = ( *m )[0][1];
	tmp[1][1] = ( *m )[1][1];
	tmp[1][2] = ( *m )[2][1];
	tmp[1][3] = ( *m )[3][1];
	tmp[2][0] = ( *m )[0][2];
	tmp[2][1] = ( *m )[1][2];
	tmp[2][2] = ( *m )[2][2];
	tmp[2][3] = ( *m )[3][2];
	tmp[3][0] = ( *m )[0][3];
	tmp[3][1] = ( *m )[1][3];
	tmp[3][2] = ( *m )[2][3];
	tmp[3][3] = ( *m )[3][3];

	( *r )[0][0] = tmp[0][0];
	( *r )[0][1] = tmp[0][1];
	( *r )[0][2] = tmp[0][2];
	( *r )[0][3] = tmp[0][3];
	( *r )[1][0] = tmp[1][0];
	( *r )[1][1] = tmp[1][1];
	( *r )[1][2] = tmp[1][2];
	( *r )[1][3] = tmp[1][3];
	( *r )[2][0] = tmp[2][0];
	( *r )[2][1] = tmp[2][1];
	( *r )[2][2] = tmp[2][2];
	( *r )[2][3] = tmp[2][3];
	( *r )[3][0] = tmp[3][0];
	( *r )[3][1] = tmp[3][1];
	( *r )[3][2] = tmp[3][2];
	( *r )[3][3] = tmp[3][3];
}

float dVM_FindLineDistance( const dRAY *ray0, const dRAY *ray1, float *trace0, float *trace1 ) {
	dMAT2 m;
	dVEC2 v;
	dVEC3 q, p, d;

	m[0][0] =  dVM_LengthSquaredVec3( &( ray0->direction ) );
	m[0][1] = -dVM_DotVec3( &( ray0->direction ), &( ray1->direction ) );
	m[1][0] =  dVM_DotVec3( &( ray0->direction ), &( ray1->direction ) );
	m[1][1] = -dVM_LengthSquaredVec3( &( ray1->direction ) );

	if( dVM_InvertMat2( &m, &m ) == 0 )
		return -1.0f;

	v.x = dVM_DotVec3( &( ray0->direction ), &( ray1->position ) ) - dVM_DotVec3( &( ray0->direction ), &( ray0->position ) );
	v.y = dVM_DotVec3( &( ray1->direction ), &( ray1->position ) ) - dVM_DotVec3( &( ray1->direction ), &( ray0->position ) );

	dVM_MulMat2xVec2( &m, &v, &v );

	( *trace0 ) = v.x;
	( *trace1 ) = v.y;

	dVM_MulVec3( v.x, &( ray0->direction ), &q );
	dVM_AddVec3( &( ray0->position ), &q, &q );

	dVM_MulVec3( v.y, &( ray1->direction ), &p );
	dVM_AddVec3( &( ray1->position ), &p, &p );

	dVM_SubVec3( &p, &q, &d );

	return dVM_LengthVec3( &d );
}

int dVM_FindRayIntersection( const dRAY *ray0, const dRAY *ray1, dVEC3 *intersection, float *trace0, float *trace1 ) {
	dMAT2 m;
	dVEC2 v;
	dVEC3 q;

	v.x = ray1->position.x - ray0->position.x;
	v.y = ray1->position.y - ray0->position.y;

	m[0][0] =  ray0->direction.x;
	m[0][1] = -ray1->direction.x;
	m[1][0] =  ray0->direction.y;
	m[1][1] = -ray1->direction.y;

	if( dVM_InvertMat2( &m, &m ) == 0 )
		return 0;

	dVM_MulMat2xVec2( &m, &v, &v );

	( *trace0 ) = v.x;
	( *trace1 ) = v.y;

	dVM_MulVec3( v.x, &( ray0->direction ), &q );
	dVM_AddVec3( &( ray0->position ), &q, intersection );

	return 1;
}

void dVM_RotatePlane( float angle, float x, float y, float z, const dPLANE *p, dPLANE *r ) {
	dVM_RotateVec3( angle, x, y, z, &( p->normal ), &( r->normal ) );
}

void dVM_TranslatePlane( float x, float y, float z, const dPLANE *p, dPLANE *r ) {
	r->position.x = p->position.x + x;
	r->position.y = p->position.y + y;
	r->position.z = p->position.z + z;
}

int dVM_FindPlaneIntersection( const dRAY *ray, const dPLANE *plane, dVEC3 *intersection, float *trace ) {
	float t;
	dVEC3 tmp;

	t = dVM_DotVec3( &( plane->normal ), &( ray->direction ) );

	if( t == 0 )
		return 0;

	t = ( dVM_DotVec3( &( plane->normal ), &( plane->position ) ) - dVM_DotVec3( &( plane->normal ), &( ray->position ) ) ) / t;

	dVM_MulVec3( t, &( ray->direction ), &tmp );
	dVM_AddVec3( &( ray->position ), &tmp, intersection );

	( *trace ) = t;

	return 1;
}
