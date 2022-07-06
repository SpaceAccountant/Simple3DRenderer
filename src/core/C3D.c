#include <curses.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <core/GameLoop.h>
#include <core/ObjLoader.h>
#include <core/Splash.h>
#include <graphics/Controller.h>
#include <graphics/Device.h>

#define KEY_ESCAPE		27

#define D_PI			3.141592f
#define D_DEG_TO_RAD	( D_PI / 180.0f )

typedef struct {
	dVEC3					AA;
	dVEC3					BB;
} dAABB;

typedef struct {
	dENTITY					entity;
	dAABB					AABB;
} dLOOT;

typedef struct _dLOOTNODE {
	dLOOT					value;
	struct _dLOOTNODE	   *next;
} dLOOTNODE;

typedef struct _dLIGHTNODE {
	dLIGHT					value;
	struct _dLIGHTNODE	   *next;
} dLIGHTNODE;

typedef struct _dPOLYGONNODE {
	dPLANE					plane;
	dVEC3					v0;
	dVEC3					v1;
	dVEC3					v2;
	struct _dPOLYGONNODE   *next;
} dPOLYGONNODE;

static int _D_map_light = 0;
static dMODEL *_D_map_model = NULL, *_D_loot_model = NULL;
static dPOLYGONNODE *_D_polygon_list = NULL;
static dENTITY _D_player = { { 0.0f, 6.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } };
static dLOOTNODE *_D_loot_list = NULL;
static dVEC3 _D_loot_rotation_speed = { 0.0f, D_DEG_TO_RAD, 0.0f };
static int _D_collected_pickups = 0, _D_total_pickups = 0;
static dLIGHTNODE *_D_light_list = NULL;
static dVEC3 _D_exit_position = { 0.0f, 0.0f, 0.0f };
static dAABB _D_exit_span = { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } };
static int _D_win = 0, _D_win_update = 0;

int dG_LoadLevel( const char *path ) {
	FILE *file;
	char header[512], model_path[512], material_path[512];
	dLOOTNODE *g;
	dLIGHTNODE *l;
	dAABB aabb = { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } };
	int i;
	dPOLYGONNODE *pn;
	dVEC3 v0, v1, v2;
	dVEC3 a, b;

	if( !( file = fopen( path, "r" ) ) )
		return 0;

	while( fscanf( file, "%s", header ) != EOF ) {
		if( strcmp( header, "m" ) == 0 ) {
			_D_map_model = ( dMODEL* ) malloc( sizeof( dMODEL ) );

			if( !_D_map_model )
				break;

			fscanf( file, "%s %s\n", model_path, material_path );
			dU_LoadModel( model_path, material_path, _D_map_model );
			continue;
		} else if( strcmp( header, "gm" ) == 0 ) {
			_D_loot_model = ( dMODEL* ) malloc( sizeof( dMODEL ) );

			if( !_D_loot_model )
				break;

			fscanf( file, "%s %s\n", model_path, material_path );
			dU_LoadModel( model_path, material_path, _D_loot_model );
		} else if( strcmp( header, "g" ) == 0 ) {
			g = ( dLOOTNODE* ) malloc( sizeof( dLOOTNODE ) );

			if( !g )
				break;

			g->value.AABB = aabb;
			g->value.entity.rotation.x = g->value.entity.rotation.y = g->value.entity.rotation.z = 0;
			fscanf( file, "%f %f %f\n", &( g->value.entity.position.x ), &( g->value.entity.position.y ), &( g->value.entity.position.z ) );

			g->next = _D_loot_list;
			_D_loot_list = g;

			_D_total_pickups++;
		} else if( strcmp( header, "gb" ) == 0 ) {
			fscanf( file, "%f %f %f ", &( aabb.AA.x ), &( aabb.AA.y ), &( aabb.AA.z ) );
			fscanf( file, "%f %f %f\n", &( aabb.BB.x ), &( aabb.BB.y ), &( aabb.BB.z ) );
		} else if( strcmp( header, "ps" ) == 0 ) {
			fscanf( file, "%f %f %f ", &( _D_player.position.x ), &( _D_player.position.y ), &( _D_player.position.z ) );
			fscanf( file, "%f %f %f\n", &( _D_player.rotation.x ), &( _D_player.rotation.y ), &( _D_player.rotation.z ) );
		} else if( strcmp( header, "ex" ) == 0 ) {
			fscanf( file, "%f %f %f ", &( _D_exit_position.x ), &( _D_exit_position.y ), &( _D_exit_position.z ) );
			fscanf( file, "%f %f %f ", &( _D_exit_span.AA.x ), &( _D_exit_span.AA.y ), &( _D_exit_span.AA.z ) );
			fscanf( file, "%f %f %f\n", &( _D_exit_span.BB.x ), &( _D_exit_span.BB.y ), &( _D_exit_span.BB.z ) );
		} else if( strcmp( header, "l" ) == 0 ) {
			l = ( dLIGHTNODE* ) malloc( sizeof( dLIGHTNODE ) );

			if( !l )
				break;

			fscanf( file, "%f %f %f ", &( l->value.position.x ), &( l->value.position.y ), &( l->value.position.z ) );
			fscanf( file, "%f %f %f %f ", &( l->value.ambient_color.x ), &( l->value.ambient_color.y ), &( l->value.ambient_color.z ), &( l->value.ambient_color.w ) );
			fscanf( file, "%f %f %f %f ", &( l->value.diffuse_color.x ), &( l->value.diffuse_color.y ), &( l->value.diffuse_color.z ), &( l->value.diffuse_color.w ) );
			fscanf( file, "%f %f %f\n", &( l->value.const_atten ), &( l->value.linear_atten ), &( l->value.quadratic_atten ) );

			l->next = _D_light_list;
			_D_light_list = l;
		} else if( strcmp( header, "map_light" ) == 0 ) {
			_D_map_light = 1;
		} else if( strcmp( header, "gr" ) == 0 ) {
			fscanf( file, "%f %f %f ", &( _D_loot_rotation_speed.x ), &( _D_loot_rotation_speed.y ), &( _D_loot_rotation_speed.z ) );
		}
	}

	fclose( file );
	
	// generate the map polygon list
	for( i = 0; i < _D_map_model->vertex_count; i += 3 ) {
		v0.x = _D_map_model->vertices[i    ].position.x;
		v0.y = _D_map_model->vertices[i    ].position.y;
		v0.z = _D_map_model->vertices[i    ].position.z;
		v1.x = _D_map_model->vertices[i + 1].position.x;
		v1.y = _D_map_model->vertices[i + 1].position.y;
		v1.z = _D_map_model->vertices[i + 1].position.z;
		v2.x = _D_map_model->vertices[i + 2].position.x;
		v2.y = _D_map_model->vertices[i + 2].position.y;
		v2.z = _D_map_model->vertices[i + 2].position.z;

		dVM_SubVec3( &v1, &v0, &a );
		dVM_SubVec3( &v2, &v0, &b );

		pn = ( dPOLYGONNODE* ) malloc( sizeof( dPOLYGONNODE ) );

		if( !pn )
			break;

		pn->next = _D_polygon_list;
		_D_polygon_list = pn;

		pn->plane.position = v0;

		dVM_CrossVec3( &b, &a, &( pn->plane.normal ) );
		dVM_NormalizeVec3( &( pn->plane.normal ), &( pn->plane.normal ) );

		pn->v0 = v0;
		pn->v1 = v1;
		pn->v2 = v2;
	}

	return 1;
}

int dG_Init( const char *level ) {
	static float fovy = 70.0f * D_DEG_TO_RAD;
	const dFRAMEBUFFER *fb;
	const dCOLORMAP *cm;
	dVIEWPORT vp;
	float aspect;
	dFRUSTUM fr;

	// let the user decide their screen size
	printf( "This game is best experienced when the terminal window is maximized.\nPlease resize the terminal to your desired size and press ENTER to continue...\n" );
	getchar();

	// load the game models
	if( !dG_LoadLevel( level ) )
		return 0;

	// create the device
	dDV_InitDevice( 256, 128 );

	// get the device rendering information
	fb = dDV_GetDeviceFrameBuffer();
	cm = dDV_GetDeviceColorMap();

	// get the device viewport
	vp.x = 0;
	vp.y = 0;
	dDV_GetDeviceSize( &( vp.width ), &( vp.height ) );

	// create the render controller
	dRC_InitController( fb, cm );

	aspect = ( float ) vp.width / ( float ) vp.height;

	// set the projection frustum
	fr.near_val = 1.0f;
	fr.far_val  = 1000.0f;
	fr.top      = fr.near_val * tanf( fovy / 2.0f );
	fr.bottom   = -fr.top;
	fr.right    = fr.top * aspect / 2.0f;
	fr.left     = -fr.right;
	dRC_SetFrustum( &fr );

	return 1;
}

void dG_Shutdown( void ) {
	dPOLYGONNODE *p, *pnext;
	dLIGHTNODE *l, *lnext;
	dLOOTNODE *g, *gnext;

	// free the map polygon list
	for( p = _D_polygon_list; p != NULL; p = pnext ) {
		pnext = p->next;
		free( p );
	}

	// free the light list
	for( l = _D_light_list; l != NULL; l = lnext ) {
		lnext = l->next;
		free( l );
	}

	// free the loot list
	for( g = _D_loot_list; g != NULL; g = gnext ) {
		gnext = g->next;
		free( g );
	}

	// release the map models
	dU_DeleteModel( _D_loot_model );
	dU_DeleteModel( _D_map_model );

	// shutdown the render controller
	dRC_ShutdownController();

	// shutdown the device
	dDV_ShutdownDevice();
}

static int dG_TriangleEdgeFunc( const dVEC3 *v0, const dVEC3 *v1, const dVEC3 *v2, const dVEC3 *n, const dVEC3 *p ) {
	dVEC3 edge;
	dVEC3 extrude;
	dVEC3 cross;
	
	dVM_SubVec3( v1, v0, &edge );
	dVM_SubVec3( p, v0, &extrude );
	dVM_CrossVec3( &extrude, &edge, &cross );

	if( dVM_DotVec3( &cross, n ) < 0 )
		return 0;

	dVM_SubVec3( v2, v1, &edge );
	dVM_SubVec3( p, v1, &extrude );
	dVM_CrossVec3( &extrude, &edge, &cross );

	if( dVM_DotVec3( &cross, n ) < 0 )
		return 0;

	dVM_SubVec3( v0, v2, &edge );
	dVM_SubVec3( p, v2, &extrude );
	dVM_CrossVec3( &extrude, &edge, &cross );

	if( dVM_DotVec3( &cross, n ) < 0 )
		return 0;

	return 1;
}

int dG_TestRay( const dRAY *ray, float epsilon, dVEC3 *intersection ) {
	dPOLYGONNODE *p;
	float trace;

	for( p = _D_polygon_list; p != NULL; p = p->next )
		if( dVM_FindPlaneIntersection( ray, &( p->plane ), intersection, &trace ) && trace >= 0.0f && trace <= epsilon && dG_TriangleEdgeFunc( &( p->v0 ), &( p->v1 ), &( p->v2 ), &( p->plane.normal ), intersection ) )
			return 0;

	return 1;
}

void dG_DrawEntities( void ) {
	static char *win_text[] = {
		"You reach the exit and realize something...",
		"You still don't have your memory and still don't know where you are.",
		"...",
		"At least you feel acomplished for finding the exit.",
		"",
		"",
		"Press Escape to exit."
	};
	static int win_count = sizeof( win_text ) / sizeof( const char* );
	static int win_edit = 5;
	static dVEC3 axisX = { 1.0f, 0.0f, 0.0f };
	static dVEC3 axisY = { 0.0f, 1.0f, 0.0f };
	static dVEC3 axisZ = { 0.0f, 0.0f, 1.0f };
	dLOOTNODE *g;
	dLIGHTNODE *l;
	int w, h;
	int i;

	if( _D_win ) {
		if( _D_win_update ) {
			win_text[win_edit] = malloc( 100 );

			if( win_text[win_edit] )
				sprintf( win_text[win_edit], "%d/%d pickups collected", _D_collected_pickups, _D_total_pickups );

			dRC_ClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
			dRC_ClearDepth( 1.0f );

			dDV_SwapBuffers( 120 );

			dDV_GetDeviceSize( &w, &h );
		
			for( i = 0; i < win_count; i++ )
				dDV_DrawText( ( w / 2 ) - ( strlen( win_text[i] ) / 2 ), ( h / 2 ) - ( win_count / 2 ) + i, 1.0f, 1.0f, 1.0f, win_text[i] );

			free( win_text[win_edit] );

			_D_win_update = 0;
		}
	} else {
		dRC_ClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
		dRC_ClearDepth( 1.0f );

		if( _D_map_light ) {
			// activate the lights
			for( l = _D_light_list, i = 0; l != NULL && i < D_MAX_LIGHTS; l = l->next, i++ ) {
				dRC_SetLight( i, &( l->value ) );
				dRC_ActivateLight( i );
			}
		}

		// setup the player's view
		dRC_SetViewIdentity();
		dRC_TranslateView( &( _D_player.position ) );
		dRC_RotateView( _D_player.rotation.y, &axisY );
		dRC_RotateView( _D_player.rotation.x, &axisX );
		dRC_RotateView( _D_player.rotation.z, &axisZ );

		// draw the map
		dRC_SetModelIdentity();
		dRC_DrawTriangles( _D_map_model->vertices, _D_map_model->vertex_count );

		if( !_D_map_light ) {
			// activate the lights
			for( l = _D_light_list, i = 0; l != NULL && i < D_MAX_LIGHTS; l = l->next, i++ ) {
				dRC_SetLight( i, &( l->value ) );
				dRC_ActivateLight( i );
			}
		}
		
		// draw the loot list
		for( g = _D_loot_list; g != NULL; g = g->next ) {
			dRC_SetModelIdentity();
			dRC_RotateModel( g->value.entity.rotation.x, &axisX );
			dRC_RotateModel( g->value.entity.rotation.y, &axisY );
			dRC_RotateModel( g->value.entity.rotation.z, &axisZ );
			dRC_TranslateModel( &( g->value.entity.position ) );
			dRC_DrawTriangles( _D_loot_model->vertices, _D_loot_model->vertex_count );
		}

		// deactivate the lights
		for( l = _D_light_list, i = 0; l != NULL && i < D_MAX_LIGHTS; l = l->next, i++ )
			dRC_DeactivateLight( i );

		// update the screen
		dDV_SwapBuffers( 120 );
	}
}

void dG_HandleKey( int keycode ) {
	static float walk_speed = 0.5f, turn_speed = 2.5f;
	static float collision_epsilon = 2.5f;
	float walk_multiplier = 1.0f;
	dRAY ray;
	dVEC3 tmp;

	if( _D_win ) {
		switch( keycode ) {
		// exit
		case KEY_ESCAPE:
			ExitGameLoop();
			break;
		}
	} else {
		ray.position = _D_player.position;
		ray.position.y /= 3.0f;

		switch( keycode ) {
		// walk forward
		case 'W':
			walk_multiplier = 2.0f;
		case KEY_UP:
		case 'w': {
			ray.direction.x =  0.0f;
			ray.direction.y =  0.0f;
			ray.direction.z =  1.0f;

			dVM_RotateVec3( -_D_player.rotation.y, 0.0f, 1.0f, 0.0f, &( ray.direction ), &( ray.direction ) );

			if( !dG_TestRay( &ray, collision_epsilon, &tmp ) )
				break;

			ray.direction.x = -0.707107f;
			ray.direction.y =  0.0f;
			ray.direction.z =  0.707107f;

			dVM_RotateVec3( -_D_player.rotation.y, 0.0f, 1.0f, 0.0f, &( ray.direction ), &( ray.direction ) );

			if( !dG_TestRay( &ray, collision_epsilon, &tmp ) )
				break;
			
			ray.direction.x =  0.707107f;
			ray.direction.y =  0.0f;
			ray.direction.z =  0.707107f;

			dVM_RotateVec3( -_D_player.rotation.y, 0.0f, 1.0f, 0.0f, &( ray.direction ), &( ray.direction ) );

			if( !dG_TestRay( &ray, collision_epsilon, &tmp ) )
				break;

			_D_player.position.x += walk_multiplier * walk_speed * sinf( _D_player.rotation.y );
			_D_player.position.z += walk_multiplier * walk_speed * cosf( _D_player.rotation.y );
			} break;
		// backpedal
		case 'S':
			walk_multiplier = 2.0f;
		case KEY_DOWN:
		case 's': {
			ray.direction.x =  0.0f;
			ray.direction.y =  0.0f;
			ray.direction.z = -1.0f;

			dVM_RotateVec3( -_D_player.rotation.y, 0.0f, 1.0f, 0.0f, &( ray.direction ), &( ray.direction ) );

			if( !dG_TestRay( &ray, collision_epsilon, &tmp ) )
				break;

			_D_player.position.x -= walk_multiplier * walk_speed * sinf( _D_player.rotation.y );
			_D_player.position.z -= walk_multiplier * walk_speed * cosf( _D_player.rotation.y );
			} break;
		// strafe left
		case '<':
		case 'A':
			walk_multiplier = 2.0f;
		case ',':
		case 'a': {
			ray.direction.x = -1.0f;
			ray.direction.y =  0.0f;
			ray.direction.z =  0.0f;

			dVM_RotateVec3( -_D_player.rotation.y, 0.0f, 1.0f, 0.0f, &( ray.direction ), &( ray.direction ) );

			if( !dG_TestRay( &ray, collision_epsilon, &tmp ) )
				break;

			_D_player.position.x += walk_multiplier * walk_speed * sinf( _D_player.rotation.y - D_PI / 2.0f );
			_D_player.position.z += walk_multiplier * walk_speed * cosf( _D_player.rotation.y - D_PI / 2.0f );
			} break;
		// strafe right
		case '>':
		case 'D':
			walk_multiplier = 2.0f;
		case '.':
		case 'd': {
			ray.direction.x =  1.0f;
			ray.direction.y =  0.0f;
			ray.direction.z =  0.0f;

			dVM_RotateVec3( -_D_player.rotation.y, 0.0f, 1.0f, 0.0f, &( ray.direction ), &( ray.direction ) );

			if( !dG_TestRay( &ray, collision_epsilon, &tmp ) )
				break;

			_D_player.position.x -= walk_multiplier * walk_speed * sinf( _D_player.rotation.y - D_PI / 2.0f );
			_D_player.position.z -= walk_multiplier * walk_speed * cosf( _D_player.rotation.y - D_PI / 2.0f );
			} break;
		// turn left
		case KEY_LEFT:
		case 'q':
		case 'Q':
			_D_player.rotation.y -= turn_speed * D_DEG_TO_RAD;
			break;
		// turn right
		case KEY_RIGHT:
		case 'e':
		case 'E':
			_D_player.rotation.y += turn_speed * D_DEG_TO_RAD;
			break;
		// pitch up
		case KEY_HOME:
		case 'z':
		case 'Z': {
			if( _D_player.rotation.x + turn_speed * D_DEG_TO_RAD >= -70.0f * D_DEG_TO_RAD )
				_D_player.rotation.x -= turn_speed * D_DEG_TO_RAD;
			} break;
		// pitch down
		case KEY_END:
		case 'c':
		case 'C': {
			if( _D_player.rotation.x + turn_speed * D_DEG_TO_RAD <= 70.0f * D_DEG_TO_RAD )
				_D_player.rotation.x += turn_speed * D_DEG_TO_RAD;
			} break;
		// exit
		case KEY_ESCAPE:
			ExitGameLoop();
			break;
		}
	}
}

void dG_Update( void ) {
	dLOOTNODE *prev, *l, *next;

	if( _D_win )
		return;

	for( prev = NULL, l = _D_loot_list; l != NULL; l = next ) {
		dVM_AddVec3( &( l->value.entity.rotation ), &( _D_loot_rotation_speed ), &( l->value.entity.rotation ) );

		next = l->next;

		if( _D_player.position.x >= l->value.entity.position.x + l->value.AABB.AA.x && _D_player.position.x <= l->value.entity.position.x + l->value.AABB.BB.x &&
			_D_player.position.z >= l->value.entity.position.z + l->value.AABB.AA.z && _D_player.position.z <= l->value.entity.position.z + l->value.AABB.BB.z ) {

			if( !prev )
				_D_loot_list = next;
			else
				prev->next = next;

			free( l );

			_D_collected_pickups++;
		} else
			prev = l;
	}

	if( _D_player.position.x >= _D_exit_position.x + _D_exit_span.AA.x && _D_player.position.x <= _D_exit_position.x + _D_exit_span.BB.x &&
		_D_player.position.y >= _D_exit_position.y + _D_exit_span.AA.y && _D_player.position.y <= _D_exit_position.y + _D_exit_span.BB.y &&
		_D_player.position.z >= _D_exit_position.z + _D_exit_span.AA.z && _D_player.position.z <= _D_exit_position.z + _D_exit_span.BB.z ) {
		_D_win        = 1;
		_D_win_update = 1;
	}
}

void dG_SplashExit( int code ) {
	switch( code ) {
	case D_PLAY_GAME: {
		dS_Shutdown();
		SetGameLoopDrawFunc( dG_DrawEntities );
		SetGameLoopKeyFunc( dG_HandleKey );
		SetGameLoopUpdateFunc( dG_Update );
		} break;
	case D_EXIT_GAME:
		dS_Shutdown();
		ExitGameLoop();
		break;
	}
}

int main( int argc, char **argv ) {
	char *level = "res/maps/level1.map";

	if( argc > 1 )
		level = argv[1];

	if( !dG_Init( level ) ) {
		printf( "Couldn't initialize game.\n" );
		
		if( argc > 1 )
			printf( "Please check the requested map file and try again.\n" );
		else
			printf( "Please try again.\n" );

		return -1;
	}

	dS_Init();

	SetGameLoopDrawFunc( dS_DrawEntities );
	SetGameLoopKeyFunc( dS_HandleKey );
	SetGameLoopUpdateFunc( dS_Update );

	dS_SetSplashExitFunc( dG_SplashExit );

	GameLoop();

	dG_Shutdown();

	return 0;
}
