#include <curses.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include <graphics/Device.h>

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#define dDV_Sleep( nanosecs ) Sleep( ( int ) ( ( nanosecs ) / 1000.0f ) )

#define NORMAL_COLOR_CHAR			32
#define DARK_COLOR_CHAR				104
#define DARKER_COLOR_CHAR			97
#define EVEN_DARKER_COLOR_CHAR		97
#define EVEN_EVEN_DARKER_COLOR_CHAR	104
#else
#include <unistd.h>

#define dDV_Sleep( nanosecs ) usleep( ( int ) ( nanosecs ) )

#define NORMAL_COLOR_CHAR			32
#define DARK_COLOR_CHAR				97
#define DARKER_COLOR_CHAR			97
#define EVEN_DARKER_COLOR_CHAR		97
#define EVEN_EVEN_DARKER_COLOR_CHAR	97
#endif // WIN32

#define DARK_COLOR_FLAG				0x08
#define DARKER_COLOR_FLAG			0x10
#define EVEN_DARKER_COLOR_FLAG		0x20
#define EVEN_EVEN_DARKER_COLOR_FLAG	0x40

typedef struct {
	WINDOW		   *window;
	dCOLORMAP	   *colors;
	dFRAMEBUFFER	framebuffer;
} dCURSESDEVICE;

typedef struct { unsigned char i, fg, bg; float r, g, b; } dPALETTEENTRY;

static dPALETTEENTRY _DDV_palette[] = {
	// normal filled colors
	{ 0,                               COLOR_BLACK,   COLOR_BLACK,   0.0f, 0.0f, 0.0f },
	{ 1,                               COLOR_BLUE,    COLOR_BLUE,    0.0f, 0.0f, 0.9f },
	{ 2,                               COLOR_GREEN,   COLOR_GREEN,   0.0f, 0.9f, 0.0f },
	{ 3,                               COLOR_CYAN,    COLOR_CYAN,    0.0f, 0.9f, 0.9f },
	{ 4,                               COLOR_RED,     COLOR_RED,     0.9f, 0.0f, 0.0f },
	{ 5,                               COLOR_MAGENTA, COLOR_MAGENTA, 0.9f, 0.0f, 0.9f },
	{ 6,                               COLOR_YELLOW,  COLOR_YELLOW,  0.9f, 0.9f, 0.0f },
	{ 7,                               COLOR_WHITE,   COLOR_WHITE,   0.9f, 0.9f, 0.9f },

	// dark special colors
	{ 0 | DARK_COLOR_FLAG,             COLOR_BLACK,   COLOR_BLUE,    0.0f, 0.0f, 0.7f },
	{ 1 | DARK_COLOR_FLAG,             COLOR_BLACK,   COLOR_GREEN,   0.0f, 0.7f, 0.0f },
	{ 2 | DARK_COLOR_FLAG,             COLOR_BLACK,   COLOR_CYAN,    0.0f, 0.7f, 0.7f },
	{ 3 | DARK_COLOR_FLAG,             COLOR_BLACK,   COLOR_RED,     0.7f, 0.0f, 0.0f },
	{ 4 | DARK_COLOR_FLAG,             COLOR_BLACK,   COLOR_MAGENTA, 0.7f, 0.0f, 0.7f },
	{ 5 | DARK_COLOR_FLAG,             COLOR_BLACK,   COLOR_YELLOW,  0.7f, 0.7f, 0.0f },
	{ 6 | DARK_COLOR_FLAG,             COLOR_BLACK,   COLOR_WHITE,   0.7f, 0.7f, 0.7f },

	// darker special colors
	{ 0 | DARKER_COLOR_FLAG,           COLOR_BLACK,   COLOR_BLUE,    0.0f, 0.0f, 0.5f },
	{ 1 | DARKER_COLOR_FLAG,           COLOR_BLACK,   COLOR_GREEN,   0.0f, 0.5f, 0.0f },
	{ 2 | DARKER_COLOR_FLAG,           COLOR_BLACK,   COLOR_CYAN,    0.0f, 0.5f, 0.5f },
	{ 3 | DARKER_COLOR_FLAG,           COLOR_BLACK,   COLOR_RED,     0.5f, 0.0f, 0.0f },
	{ 4 | DARKER_COLOR_FLAG,           COLOR_BLACK,   COLOR_MAGENTA, 0.5f, 0.0f, 0.5f },
	{ 5 | DARKER_COLOR_FLAG,           COLOR_BLACK,   COLOR_YELLOW,  0.5f, 0.5f, 0.0f },
	{ 6 | DARKER_COLOR_FLAG,           COLOR_BLACK,   COLOR_WHITE,   0.5f, 0.5f, 0.5f },

	// even darker special colors
	{ 0 | EVEN_DARKER_COLOR_FLAG,      COLOR_BLUE,    COLOR_BLACK,   0.0f, 0.0f, 0.3f },
	{ 1 | EVEN_DARKER_COLOR_FLAG,      COLOR_GREEN,   COLOR_BLACK,   0.0f, 0.3f, 0.0f },
	{ 2 | EVEN_DARKER_COLOR_FLAG,      COLOR_CYAN,    COLOR_BLACK,   0.0f, 0.3f, 0.3f },
	{ 3 | EVEN_DARKER_COLOR_FLAG,      COLOR_RED,     COLOR_BLACK,   0.3f, 0.0f, 0.0f },
	{ 4 | EVEN_DARKER_COLOR_FLAG,      COLOR_MAGENTA, COLOR_BLACK,   0.3f, 0.0f, 0.3f },
	{ 5 | EVEN_DARKER_COLOR_FLAG,      COLOR_YELLOW,  COLOR_BLACK,   0.3f, 0.3f, 0.0f },
	{ 6 | EVEN_DARKER_COLOR_FLAG,      COLOR_WHITE,   COLOR_BLACK,   0.3f, 0.3f, 0.3f },

	// EVEN even darker special colors
	{ 0 | EVEN_EVEN_DARKER_COLOR_FLAG, COLOR_BLUE,    COLOR_BLACK,   0.0f, 0.0f, 0.1f },
	{ 1 | EVEN_EVEN_DARKER_COLOR_FLAG, COLOR_GREEN,   COLOR_BLACK,   0.0f, 0.1f, 0.0f },
	{ 2 | EVEN_EVEN_DARKER_COLOR_FLAG, COLOR_CYAN,    COLOR_BLACK,   0.0f, 0.1f, 0.1f },
	{ 3 | EVEN_EVEN_DARKER_COLOR_FLAG, COLOR_RED,     COLOR_BLACK,   0.1f, 0.0f, 0.0f },
	{ 4 | EVEN_EVEN_DARKER_COLOR_FLAG, COLOR_MAGENTA, COLOR_BLACK,   0.1f, 0.0f, 0.1f },
	{ 5 | EVEN_EVEN_DARKER_COLOR_FLAG, COLOR_YELLOW,  COLOR_BLACK,   0.1f, 0.1f, 0.0f },
	{ 6 | EVEN_EVEN_DARKER_COLOR_FLAG, COLOR_WHITE,   COLOR_BLACK,   0.1f, 0.1f, 0.1f }
};
static int _DDV_colorCount = sizeof( _DDV_palette ) / sizeof( dPALETTEENTRY );

static dCURSESDEVICE _DDV_device = { NULL, NULL };

int dDV_InitDevice( int request_width, int request_height ) {
	int buffer_size;
	int i;

	// start curses
	_DDV_device.window = initscr();
	cbreak();
	noecho();
	keypad( stdscr, TRUE );
	nodelay( stdscr, TRUE );
	curs_set( 0 );

	// create the color map
	_DDV_device.colors = dCM_CreateColorMap();

	if( has_colors() ) {
		start_color();

		for( i = 0; i < _DDV_colorCount; i++ ) {
			init_pair   ( _DDV_palette[i].i, _DDV_palette[i].fg, _DDV_palette[i].bg );
			dCM_AddEntry( _DDV_palette[i].i, _DDV_palette[i].r,  _DDV_palette[i].g, _DDV_palette[i].b, _DDV_device.colors );
		}
	}
	
	// get the actual frame buffer sizes
	getmaxyx( _DDV_device.window, _DDV_device.framebuffer.height, _DDV_device.framebuffer.width );
	buffer_size = _DDV_device.framebuffer.width * _DDV_device.framebuffer.height;

	// create the color buffer
	_DDV_device.framebuffer.color = ( unsigned char* ) malloc( buffer_size );

	if( !_DDV_device.framebuffer.color ) {
		endwin();
		_DDV_device.window = NULL;
		return 0;
	}

	memset( _DDV_device.framebuffer.color, dCM_FindIndex( 0.0f, 0.0f, 0.0f, _DDV_device.colors ), buffer_size );

	// create the depth buffer
	_DDV_device.framebuffer.depth = ( float* ) malloc( buffer_size * sizeof( float ) );

	if( !_DDV_device.framebuffer.depth ) {
		free( _DDV_device.framebuffer.color );
		endwin();
		_DDV_device.window = NULL;
		return 0;
	}

	memset( _DDV_device.framebuffer.depth, 0, buffer_size * sizeof( float ) );

	return 1;
}

void dDV_ShutdownDevice( void ) {
	if( !_DDV_device.window )
		return;

	// release the screen buffers
	free( _DDV_device.framebuffer.color );
	free( _DDV_device.framebuffer.depth );
	
	// release the color map
	dCM_DeleteColorMap( _DDV_device.colors );

	// end curses
	endwin();

	_DDV_device.window = NULL;
}

void dDV_DrawText( int x, int y, float red, float green, float blue, const char *text ) {
	unsigned char color;

	if( !_DDV_device.window )
		return;

	// cheating so I dont have to do special text colors
	color = dCM_FindIndex( red * 0.1f, green * 0.1f, blue * 0.1f, _DDV_device.colors ) | EVEN_EVEN_DARKER_COLOR_FLAG;

	attron  ( COLOR_PAIR( color ) );
	mvprintw( y, x, text );
	attroff ( COLOR_PAIR( color ) );
}

const dCOLORMAP * dDV_GetDeviceColorMap( void ) {
	if( !_DDV_device.window )
		return NULL;

	return _DDV_device.colors;
}

const dFRAMEBUFFER * dDV_GetDeviceFrameBuffer( void ) {
	if( !_DDV_device.window )
		return NULL;

	return &_DDV_device.framebuffer;
}

void dDV_GetDeviceSize( int *width , int *height ) {
	if( !_DDV_device.window )
		return;

	getmaxyx( _DDV_device.window, ( *height ), ( *width ) );
}

void dDV_SwapBuffers( int refresh_rate ) {
	int y, x;
	char color;

	if( !_DDV_device.window )
		return;

	// draw the information in the color buffer to the curses screen
	for( y = 0; y < _DDV_device.framebuffer.height; y++ ) {
		for( x = 0; x < _DDV_device.framebuffer.width; x++ ) {
			color = _DDV_device.framebuffer.color[x + y * _DDV_device.framebuffer.width];

			if( color == -1 )
				continue;

			// activate the curses color pair
			attron( COLOR_PAIR( color ) );

			if( color & EVEN_EVEN_DARKER_COLOR_FLAG ) {		// EVEN even darker colors
				attron ( A_ALTCHARSET );
				mvaddch( y, x, EVEN_EVEN_DARKER_COLOR_CHAR );
				attroff( A_ALTCHARSET );
			} else  if( color & EVEN_DARKER_COLOR_FLAG ) {	// even darker colors
				attron ( A_ALTCHARSET );
				mvaddch( y, x, EVEN_DARKER_COLOR_CHAR );
				attroff( A_ALTCHARSET );
			} else if( color & DARKER_COLOR_FLAG ) {		// darker colors
				attron ( A_ALTCHARSET );
				mvaddch( y, x, DARKER_COLOR_CHAR );
				attroff( A_ALTCHARSET );
			} else if( color & DARK_COLOR_FLAG ) {			// dark colors
				attron ( A_ALTCHARSET );
				mvaddch( y, x, DARK_COLOR_CHAR );
				attroff( A_ALTCHARSET );
			} else											// normal colors
				mvaddch( y, x, NORMAL_COLOR_CHAR );

			// de-activate the curses color pair
			attroff( COLOR_PAIR( color ) );
		}
	}

	// refresh the screen
	refresh();
	dDV_Sleep( 1000000.0f / refresh_rate );
}
