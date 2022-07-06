#ifndef __DECKARD_CONTROLLER_H__
#define __DECKARD_CONTROLLER_H__

#include <graphics/ColorMap.h>
#include <graphics/RenderTypes.h>

#define D_CONTROLLER_VIDEO_MEMORY	( 5 * 1024 * 1024 )	// 1 MiB of memory for the controller

#define D_MAX_LIGHTS	8

int		dRC_InitController( const dFRAMEBUFFER *framebuffer, const dCOLORMAP *color_map );
void	dRC_ShutdownController( void );

void	dRC_RotateModel( float angle, const dVEC3 *axis );
void	dRC_RotateView( float angle, const dVEC3 *axis );
void	dRC_ScaleModel( const dVEC3 *scale );
void	dRC_SetFrustum( const dFRUSTUM *frustum );
void	dRC_SetModelIdentity( void );
void	dRC_SetViewIdentity( void );
void	dRC_TranslateModel( const dVEC3 *translation );
void	dRC_TranslateView( const dVEC3 *translation );

void	dRC_ActivateLight( int index );
void	dRC_DeactivateLight( int index );
void	dRC_SetLight( int index, const dLIGHT *light );

void	dRC_ClearColor( float red, float green, float blue, float alpha );
void	dRC_ClearDepth( float depth );
void	dRC_DrawTriangles( const dVERTEX *input, int count );

#endif // __DECKARD_CONTROLLER_H__
