#ifndef __CHAMBERED_GAMELOOP_H__
#define __CHAMBERED_GAMELOOP_H__

#define GAME_SCREEN_WIDTH	154
#define GAME_SCREEN_HEIGHT	48

typedef void ( *GameLoopDrawFunc_t   )( void );
typedef void ( *GameLoopKeyFunc_t    )( int key );
typedef void ( *GameLoopUpdateFunc_t )( void );

void	ExitGameLoop( void );
void	GameLoop( void );
void	SetGameLoopDrawFunc( GameLoopDrawFunc_t draw_func );
void	SetGameLoopKeyFunc( GameLoopKeyFunc_t key_func );
void	SetGameLoopUpdateFunc( GameLoopUpdateFunc_t update_func );

#endif //__CHAMBERED_GAMELOOP_H__
