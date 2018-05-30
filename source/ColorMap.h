#ifndef __DECKARD_RENDERER_H__
#define __DECKARD_RENDERER_H__

typedef struct _dCOLORMAP dCOLORMAP;

dCOLORMAP *		dCM_CreateColorMap( void );
void			dCM_DeleteColorMap( dCOLORMAP *color_map );

char			dCM_AddEntry( unsigned char index, float red, float green, float blue, dCOLORMAP *color_map );
void			dCM_RemoveEntry( unsigned char index, dCOLORMAP *color_map );
char			dCM_FindIndex( float red, float green, float blue, const dCOLORMAP *color_map );

#endif // __DECKARD_RENDERER_H__
