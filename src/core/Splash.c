#include "Splash.h"

#include <curses.h>
#include <stdlib.h>
#include <string.h>

#include <core/ObjLoader.h>
#include <graphics/Controller.h>
#include <graphics/Device.h>
#include <graphics/RenderTypes.h>

#define KEY_ENTER		10
#define KEY_ESCAPE		27

#define D_PI			3.141592f
#define D_DEG_TO_RAD	( D_PI / 180.0f )

typedef struct {
	const char **	options;
	int				choices;
	int				current;
} MenuDesc_t;

static const char *_DS_splash_menu_options[] = { "New Game", "Story", "Controls", "Quit" };
static MenuDesc_t _DS_main_menu = { _DS_splash_menu_options, sizeof( _DS_splash_menu_options ) / sizeof( const char* ), 0 };
static int _DS_story_active = FALSE;
static int _DS_controls_active = FALSE;
static int _DS_update = FALSE;
static dMODEL _DS_logo_model;
static SplashExitFunc_t _DS_exit_func = NULL;

void dS_DrawMenu( int x, int y, const MenuDesc_t *menu ) {
	int i;

	for( i = 0; i < menu->choices; i++ )
		if( menu->current == i ) {
			dDV_DrawText( x - 2, y + i, 1.0f, 1.0f, 1.0f, "[" );
			dDV_DrawText( x, y + i, 1.0f, 1.0f, 1.0f, menu->options[i] );
			dDV_DrawText( x + strlen( menu->options[i] ) + 1, y + i, 1.0f, 1.0f, 1.0f, "]" );
		} else
			dDV_DrawText( x, y + i, 1.0f, 1.0f, 1.0f, menu->options[i] );
}

void dS_DrawEntities( void ) {
	static const char *story_text[] = {
		"You awake in a strange place with no memory. Perhaps you should find the exit (and pick up some gold along the way).",
		"Press Enter to go back to the menu..."
	};
	static int story_count = sizeof( story_text ) / sizeof( const char* );
	static const char *controls_text[] = {
		"Walk forward: W, Up Arrow Key",
		"Walk backward: S, Down Arrow Key",
		"Strafe left: A, <",
		"Strafe right: D, >",
		"Turn left: Q, Left Arrow Key",
		"Turn right: E, Right Arrow Key",
		"Look up: Z, Home Key",
		"Look down: C, End Key",
		"Exit: Escape",
		"Press Enter to go back to the menu..."
	};
	static int controls_count = sizeof( controls_text ) / sizeof( const char* );
	static dVEC3 logo_position = { 0.0f, 0.0f, 3.0f };
	static dLIGHT light = { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, 1.0f, 0.0f, 0.0f };
	int w, h;
	int i;

	if( _DS_update ) {
		if( _DS_story_active ) {
			dRC_ClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
			dRC_ClearDepth( 1.0f );

			dDV_SwapBuffers( 120 );

			dDV_GetDeviceSize( &w, &h );

			for( i = 0; i < story_count; i++ )
				dDV_DrawText( ( w / 2 ) - ( strlen( story_text[i] ) / 2 ), ( h / 2 ) - ( story_count / 2 ) + i, 1.0f, 1.0f, 1.0f, story_text[i] );
		} else if( _DS_controls_active ) {
			dRC_ClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
			dRC_ClearDepth( 1.0f );

			dDV_SwapBuffers( 120 );

			dDV_GetDeviceSize( &w, &h );

			for( i = 0; i < controls_count; i++ )
				dDV_DrawText( ( w / 2 ) - ( strlen( controls_text[i] ) / 2 ), ( h / 2 ) - ( controls_count / 2 ) + i, 1.0f, 1.0f, 1.0f, controls_text[i] );
		} else {
			dRC_ClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
			dRC_ClearDepth( 1.0f );

			dRC_SetLight( 0, &light );
			dRC_ActivateLight( 0 );

			dRC_SetViewIdentity();
			dRC_SetModelIdentity();
			dRC_TranslateModel( &logo_position );
			dRC_DrawTriangles( _DS_logo_model.vertices, _DS_logo_model.vertex_count );
	
			dRC_DeactivateLight( 0 );

			dDV_SwapBuffers( 120 );
		
			dDV_DrawText( 3, 2, 1.0f, 1.0f, 1.0f, "Keyboard Cowboys present..." );
			dS_DrawMenu( 3, 3, &_DS_main_menu );
		}

		_DS_update = FALSE;
	}
}

void dS_Init( void ) {
	dU_LoadModel( "res/models/logo.obj", "res/models/logo.mtl", &_DS_logo_model );

	_DS_update = TRUE;
}

void dS_HandleKey( int keycode ) {
	switch( keycode ) {
	// menu up
	case KEY_UP: {
		_DS_update = TRUE;

		if( _DS_main_menu.current == 0 )
			_DS_main_menu.current = _DS_main_menu.choices - 1;
		else
			_DS_main_menu.current--;
		} break;
	// menu down
	case KEY_DOWN: {
		_DS_update = TRUE;

		if( _DS_main_menu.current == _DS_main_menu.choices - 1 )
			_DS_main_menu.current = 0;
		else
			_DS_main_menu.current++;
		} break;
	// menu select
	case KEY_ENTER:
		if( !_DS_story_active && !_DS_controls_active ) {
			if( _DS_main_menu.current == 0 )
				_DS_exit_func( D_PLAY_GAME );
			else if( _DS_main_menu.current == 1 ) {
				_DS_update = TRUE;
				_DS_story_active = TRUE;
			} else if( _DS_main_menu.current == 2 ) {
				_DS_update = TRUE;
				_DS_controls_active = TRUE;
			} else if( _DS_main_menu.current == 3 )
				_DS_exit_func( D_EXIT_GAME );
		} else {
			_DS_update          = TRUE;
			_DS_story_active    = FALSE;
			_DS_controls_active = FALSE;
		}
		break;
	// exit
	case KEY_ESCAPE:
		_DS_exit_func( D_EXIT_GAME );
		break;
	}
}

void dS_SetSplashExitFunc( SplashExitFunc_t func ) {
	_DS_exit_func = func;
}

void dS_Shutdown( void ) {
	dU_DeleteModel( &_DS_logo_model );
}

void dS_Update( void ) {
}
