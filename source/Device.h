#ifndef __DECKARD_DEVICE_H__
#define __DECKARD_DEVICE_H__

#include "ColorMap.h"
#include "RenderTypes.h"

int						dDV_InitDevice( int request_width, int request_height );
void					dDV_ShutdownDevice( void );

void					dDV_DrawText( int x, int y, float red, float green, float blue, const char *text );
const dCOLORMAP *		dDV_GetDeviceColorMap( void );
const dFRAMEBUFFER *	dDV_GetDeviceFrameBuffer( void );
void					dDV_GetDeviceSize( int *width , int *height );
void					dDV_SwapBuffers( int refresh_rate );

#endif // __DECKARD_DEVICE_H__
