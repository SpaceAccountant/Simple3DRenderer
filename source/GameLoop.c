#include <curses.h>
#include <stdio.h>

#include "GameLoop.h"

static GameLoopDrawFunc_t   _gameloopMainDrawFunc   = NULL;
static GameLoopKeyFunc_t    _gameloopMainKeyFunc    = NULL;
static GameLoopUpdateFunc_t _gameloopMainUpdateFunc = NULL;
static unsigned char        _D_game_loop_running    = FALSE;

void ExitGameLoop( void ) {
	_D_game_loop_running = FALSE;
}

void GameLoop( void ) {
	int c;

	_D_game_loop_running = TRUE;

	while( _D_game_loop_running ) {
		// handle keys
		if( _gameloopMainKeyFunc )
			while( ( c = getch() ) != ERR )
				_gameloopMainKeyFunc( c );

		// update
		if( _gameloopMainUpdateFunc )
			_gameloopMainUpdateFunc();

		// draw
		if( _gameloopMainDrawFunc )
			_gameloopMainDrawFunc();
	}
}

void SetGameLoopDrawFunc( GameLoopDrawFunc_t draw_func ) {
	_gameloopMainDrawFunc = draw_func;
}

void SetGameLoopKeyFunc( GameLoopKeyFunc_t key_func ) {
	_gameloopMainKeyFunc = key_func;
}

void SetGameLoopUpdateFunc( GameLoopUpdateFunc_t update_func ) {
	_gameloopMainUpdateFunc = update_func;
}
