#ifndef __DECKARD_SPLASH_H__
#define __DECKARD_SPLASH_H__

#define D_PLAY_GAME	0
#define D_EXIT_GAME	1

typedef void ( *SplashExitFunc_t )( int code );

void	dS_DrawEntities( void );
void	dS_HandleKey( int key );
void	dS_Init( void );
void	dS_SetSplashExitFunc( SplashExitFunc_t func );
void	dS_Shutdown( void );
void	dS_Update( void );

#endif // __DECKARD_SPLASH_H__
