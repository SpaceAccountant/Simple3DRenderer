#ifndef __DECKARD_VECMATH_H__
#define __DECKARD_VECMATH_H__

#include "VecTypes.h"

// Functions were implemented as needed (hence why there are no vec2 functions)

// vector 3 math
void	dVM_AddVec3( const dVEC3 *v0, const dVEC3 *v1, dVEC3 *r );
void	dVM_CrossVec3( const dVEC3 *v0, const dVEC3 *v1, dVEC3 *r );
void	dVM_SubVec3( const dVEC3 *v0, const dVEC3 *v1, dVEC3 *r );
void	dVM_MulVec3( float s, const dVEC3 *v, dVEC3 *r );
void	dVM_DivVec3( float s, const dVEC3 *v, dVEC3 *r );
float	dVM_DotVec3( const dVEC3 *v0, const dVEC3 *v1 );
float	dVM_LengthVec3( const dVEC3 *v );
float	dVM_LengthSquaredVec3( const dVEC3 *v );
void	dVM_NormalizeVec3( const dVEC3 *v, dVEC3 *r );
void	dVM_ReflectVec3( const dVEC3 *v0, const dVEC3 *v1, dVEC3 *r );
void	dVM_RotateVec3( float angle, float x, float y, float z, const dVEC3 *v, dVEC3 *r );

// vector 4 math
void	dVM_AddVec4( const dVEC4 *v0, const dVEC4 *v1, dVEC4 *r );
void	dVM_SubVec4( const dVEC4 *v0, const dVEC4 *v1, dVEC4 *r );
void	dVM_MulVec4( float s, const dVEC4 *v, dVEC4 *r );
float	dVM_DotVec4( const dVEC4 *v0, const dVEC4 *v1 );

// matrix 2 math
float	dVM_DeterminantMat2( const dMAT2 *m );
void	dVM_DivMat2( float s, const dMAT2 *m, dMAT2 *r );
float	dVM_InvertMat2( const dMAT2 *m, dMAT2 *r );
void	dVM_MulMat2xVec2( const dMAT2 *m, const dVEC2 *v, dVEC2 *r );

// matrix 3 math
float	dVM_InvertMat3( const dMAT3 *m, dMAT3 *r );
void	dVM_MulMat3( float s, const dMAT3 *m, dMAT3 *r );
void	dVM_MulMat3xVec3( const dMAT3 *m, const dVEC3 *v, dVEC3 *r );

// matrix 4 math
void	dVM_InvertMat4( const dMAT4 *m, dMAT4 *r );
void	dVM_MulMat4xMat4( const dMAT4 *m0, const dMAT4 *m1, dMAT4 *r );
void	dVM_MulMat4xVec4( const dMAT4 *m, const dVEC4 *v, dVEC4 *r);
void	dVM_TransposeMat4( const dMAT4 *m, dMAT4 *r );

// ray math
float	dVM_FindLineDistance( const dRAY *ray0, const dRAY *ray1, float *trace0, float *trace1 );
int		dVM_FindRayIntersection( const dRAY *ray0, const dRAY *ray1, dVEC3 *intersection, float *trace0, float *trace1 );

// plane math
void	dVM_RotatePlane( float angle, float x, float y, float z, const dPLANE *p, dPLANE *r );
void	dVM_TranslatePlane( float x, float y, float z, const dPLANE *p, dPLANE *r );
int		dVM_FindPlaneIntersection( const dRAY *ray, const dPLANE *plane, dVEC3 *intersection, float *trace );

#endif // __DECKARD_VECMATH_H__
