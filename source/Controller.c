#include <math.h>
#include <stdlib.h>

#include "Controller.h"
#include "Geometry.h"
#include "Rasterizer.h"

#define D_CONTROLLER_VIDEO_MEMORY_PART_SIZE	( D_CONTROLLER_VIDEO_MEMORY / 2 )

#define D_SQR_UNIT_LENGTH_LOWER_THRESHOLD	0.9f
#define D_SQR_UNIT_LENGTH_UPPER_THRESHOLD	1.1f

typedef struct {
	void			   *memory;
	dVERTEX			   *vertex_part;
	dPRIMATIVE		   *primitive_part;
} dCONTROLLERMEM;

typedef struct {
	const dFRAMEBUFFER *framebuffer;
	const dCOLORMAP	   *color_map;
} dDRAWINFO;

typedef struct {
	dMAT4				model;
	dMAT4				view;
	dMAT4				projection;
	dVIEWPORT			viewport;
} dTRANSFORMINFO;

typedef struct {
	dLIGHT				light;
	char				active;
} dCONTROLLERLIGHT;

static dCONTROLLERMEM   _DRC_memory = { NULL, NULL, NULL };
static dDRAWINFO        _DRC_draw_info = { NULL, NULL };
static dTRANSFORMINFO   _DRC_transform_info;
static dCONTROLLERLIGHT _DRC_lights[D_MAX_LIGHTS];

int dRC_InitController( const dFRAMEBUFFER *framebuffer, const dCOLORMAP *color_map ) {
	int i;

	if( _DRC_memory.memory )
		return 0;

	// create the controller video memory buffer
	_DRC_memory.memory = malloc( D_CONTROLLER_VIDEO_MEMORY );

	if( !_DRC_memory.memory )
		return 0;

	// set the controller video memory buffer partitions
	_DRC_memory.vertex_part    = ( dVERTEX   * ) _DRC_memory.memory;
	_DRC_memory.primitive_part = ( dPRIMATIVE* ) ( ( char* ) _DRC_memory.memory + D_CONTROLLER_VIDEO_MEMORY_PART_SIZE );

	// store the output information
	_DRC_draw_info.framebuffer = framebuffer;
	_DRC_draw_info.color_map   = color_map;

	// load the default transformation matrices
	dVT_SetMat4Identity( &( _DRC_transform_info.model ) );
	dVT_SetMat4Identity( &( _DRC_transform_info.view ) );
	dVT_SetMat4Identity( &( _DRC_transform_info.projection ) );

	// load the default viewport
	_DRC_transform_info.viewport.x = 0;
	_DRC_transform_info.viewport.y = 0;
	_DRC_transform_info.viewport.width  = framebuffer->width;
	_DRC_transform_info.viewport.height = framebuffer->height;

	// make all light de-activated by default
	for( i = 0; i < D_MAX_LIGHTS; i++ )
		_DRC_lights[i].active = 0;

	return 1;
}

void dRC_ShutdownController( void ) {
	if( !_DRC_memory.memory )
		return;
	
	// release the controller video memory
	free( _DRC_memory.memory );

	_DRC_memory.memory = NULL;
}

void dRC_RotateModel( float angle, const dVEC3 *axis ) {
	float sqr_len, inv_len;
	dVEC3 norm_axis;
	float c, s, oc;
	float sx, sy, sz;
	float xyc, xzc, yzc;
	dMAT4 t;

	// get the square of the axis length
	sqr_len = axis->x * axis->x + axis->y * axis->y + axis->z * axis->z;
	norm_axis = *axis;

	// normalize the axis if it isn't unit length
	if( sqr_len < D_SQR_UNIT_LENGTH_LOWER_THRESHOLD || sqr_len > D_SQR_UNIT_LENGTH_UPPER_THRESHOLD ) {
		// get the inverse length of the axis
		inv_len = 1.0f / sqrtf( sqr_len );

		// normalize the axis
		norm_axis.x *= inv_len;
		norm_axis.y *= inv_len;
		norm_axis.z *= inv_len;
	}

	// avoid duplicate calculations
	c   = cosf( angle );
	s   = sinf( angle );
	oc  = 1.0f - c;
	sx  = s * norm_axis.x;
	sy  = s * norm_axis.y;
	sz  = s * norm_axis.z;
	xyc = norm_axis.x * norm_axis.y * oc;
	xzc = norm_axis.x * norm_axis.z * oc;
	yzc = norm_axis.y * norm_axis.z * oc;

	dVT_SetMat4Identity( &t );

	// create the rotation matrix
	t[0][0] = norm_axis.x * norm_axis.x * oc + c;
	t[0][1] = xyc - sz;
	t[0][2] = xzc + sy;
	t[1][0] = xyc + sz;
	t[1][1] = norm_axis.y * norm_axis.y * oc + c;
	t[1][2] = yzc - sx;
	t[2][0] = xyc - sy;
	t[2][1] = yzc + sx;
	t[2][2] = norm_axis.z * norm_axis.z * oc + c;

	// rotate the model matrix
	dVM_MulMat4xMat4( &t, &( _DRC_transform_info.model ), &( _DRC_transform_info.model ) );
}

void dRC_RotateView( float angle, const dVEC3 *axis ) {
	float sqr_len, inv_len;
	dVEC3 norm_axis;
	float c, s, oc;
	float sx, sy, sz;
	float xyc, xzc, yzc;
	dMAT4 t;

	// get the square of the axis length
	sqr_len = axis->x * axis->x + axis->y * axis->y + axis->z * axis->z;
	norm_axis = *axis;

	// normalize the axis if it isn't unit length
	if( sqr_len < D_SQR_UNIT_LENGTH_LOWER_THRESHOLD || sqr_len > D_SQR_UNIT_LENGTH_UPPER_THRESHOLD ) {
		// get the inverse length of the axis
		inv_len = 1.0f / sqrtf( sqr_len );

		// normalize the axis
		norm_axis.x *= inv_len;
		norm_axis.y *= inv_len;
		norm_axis.z *= inv_len;
	}

	// avoid duplicate calculations
	c   = cosf( -angle );
	s   = sinf( -angle );
	oc  = 1.0f - c;
	sx  = s * norm_axis.x;
	sy  = s * norm_axis.y;
	sz  = s * norm_axis.z;
	xyc = norm_axis.x * norm_axis.y * oc;
	xzc = norm_axis.x * norm_axis.z * oc;
	yzc = norm_axis.y * norm_axis.z * oc;

	dVT_SetMat4Identity( &t );

	// create the rotation matrix
	t[0][0] = norm_axis.x * norm_axis.x * oc + c;
	t[0][1] = xyc - sz;
	t[0][2] = xzc + sy;
	t[1][0] = xyc + sz;
	t[1][1] = norm_axis.y * norm_axis.y * oc + c;
	t[1][2] = yzc - sx;
	t[2][0] = xyc - sy;
	t[2][1] = yzc + sx;
	t[2][2] = norm_axis.z * norm_axis.z * oc + c;

	// rotate the model matrix
	dVM_MulMat4xMat4( &t, &( _DRC_transform_info.view ), &( _DRC_transform_info.view ) );
}

void dRC_ScaleModel( const dVEC3 *scale ) {
	dMAT4 t;
	
	dVT_SetMat4Identity( &t );

	// create the scale matrix
	t[0][0] = scale->x;
	t[1][1] = scale->y;
	t[2][2] = scale->z;

	// scale the model matrix
	dVM_MulMat4xMat4( &t, &( _DRC_transform_info.model ), &( _DRC_transform_info.model ) );
}

void dRC_SetFrustum( const dFRUSTUM *frustum ) {
	float inv_dx, inv_dy, inv_dz;
	float n2;

	dVT_SetMat4Zero( &( _DRC_transform_info.projection ) );

	// avoid duplicate calculations
	inv_dx = 1.0f / ( frustum->right - frustum->left );
	inv_dy = 1.0f / ( frustum->top - frustum->bottom );
	inv_dz = 1.0f / ( frustum->near_val - frustum->far_val );
	n2     = 2.0f * frustum->near_val;

	// calculate the projection matrix
	_DRC_transform_info.projection[0][0] = n2 * inv_dx;
	_DRC_transform_info.projection[0][2] = -( frustum->right + frustum->left ) * inv_dx;
	_DRC_transform_info.projection[1][1] = n2 * inv_dy;
	_DRC_transform_info.projection[1][2] = -( frustum->top + frustum->bottom ) * inv_dy;
	_DRC_transform_info.projection[2][2] = -( frustum->near_val + frustum->far_val ) * inv_dz;
	_DRC_transform_info.projection[2][3] = n2 * frustum->far_val * inv_dz;
	_DRC_transform_info.projection[3][2] = 1.0f;
}

void dRC_SetModelIdentity( void ) {
	dVT_SetMat4Identity( &( _DRC_transform_info.model ) );
}

void dRC_SetViewIdentity( void ) {
	dVT_SetMat4Identity( &( _DRC_transform_info.view ) );
}

void dRC_TranslateModel( const dVEC3 *translation ) {
	dMAT4 t;
	
	dVT_SetMat4Identity( &t );

	// create the translation matrix
	t[0][3] = translation->x;
	t[1][3] = translation->y;
	t[2][3] = translation->z;

	// translate the model matrix
	dVM_MulMat4xMat4( &t, &( _DRC_transform_info.model ), &( _DRC_transform_info.model ) );
}

void dRC_TranslateView( const dVEC3 *translation ) {
	dMAT4 t;
	
	dVT_SetMat4Identity( &t );

	// create the translation matrix
	t[0][3] = -translation->x;
	t[1][3] = -translation->y;
	t[2][3] = -translation->z;

	// translate the view matrix
	dVM_MulMat4xMat4( &t, &( _DRC_transform_info.view ), &( _DRC_transform_info.view ) );
}

void dRC_ActivateLight( int index ) {
	if( index < 0 || index >= D_MAX_LIGHTS )
		return;

	_DRC_lights[index].active = 1;
}

void dRC_DeactivateLight( int index ) {
	if( index < 0 || index >= D_MAX_LIGHTS )
		return;

	_DRC_lights[index].active = 0;
}

void dRC_SetLight( int index, const dLIGHT *light ) {
	if( index < 0 || index >= D_MAX_LIGHTS )
		return;

	_DRC_lights[index].light = *light;
}

void dRC_ClearColor( float red, float green, float blue, float alpha ) {
	int size;
	unsigned char color;
	int i;

	size  = _DRC_draw_info.framebuffer->width * _DRC_draw_info.framebuffer->height;
	color = dCM_FindIndex( red * alpha, green * alpha, blue * alpha, _DRC_draw_info.color_map );

	for( i = 0; i < size; i++ ) _DRC_draw_info.framebuffer->color[i] = color;
}

void dRC_ClearDepth( float depth ) {
	int size;
	int i;

	size = _DRC_draw_info.framebuffer->width * _DRC_draw_info.framebuffer->height;

	for( i = 0; i < size; i++ ) _DRC_draw_info.framebuffer->depth[i] = depth;
}

void dRC_DrawTriangles( const dVERTEX *input, int count ) {
	static dLIGHT lights[D_MAX_LIGHTS];
	int i;
	int lcount, vcount, pcount;

	if( !_DRC_memory.memory )
		return;

	lcount = 0;

	// get the scene's active lights
	for( i = 0; i < D_MAX_LIGHTS; i++ )
		if( _DRC_lights[i].active )
			lights[lcount++] = _DRC_lights[i].light;

	// don't draw any vertices not part of a full triangle
	vcount = count - ( count % 3 );

	// run the pipeline
	dRG_TransformationModel( input, vcount, &( _DRC_transform_info.model ), _DRC_memory.vertex_part );
	dRG_Lighting( _DRC_memory.vertex_part, vcount, &( _DRC_transform_info.view ), lights, lcount, _DRC_memory.vertex_part );
	dRG_TransformationModel( _DRC_memory.vertex_part, vcount, &( _DRC_transform_info.view ), _DRC_memory.vertex_part );
	dRG_Projection( _DRC_memory.vertex_part, vcount, &( _DRC_transform_info.projection ), _DRC_memory.vertex_part );
	dRG_Cliping( _DRC_memory.vertex_part, vcount, _DRC_memory.primitive_part, &pcount );
	dRG_PerspectiveDivide( _DRC_memory.primitive_part, pcount, _DRC_memory.primitive_part );
	dRG_ViewportTransformation( _DRC_memory.primitive_part, pcount, &( _DRC_transform_info.viewport ), _DRC_memory.primitive_part );
	dRR_RasterPrimitives( _DRC_memory.primitive_part, pcount, _DRC_draw_info.framebuffer, _DRC_draw_info.color_map );
}
