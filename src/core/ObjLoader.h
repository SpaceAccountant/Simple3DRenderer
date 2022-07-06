#ifndef __DECKARD_OBJLOADER_H__
#define __DECKARD_OBJLOADER_H__

#include <graphics/RenderTypes.h>

int		dU_LoadModel( const char *obj_path, const char *mtl_path, dMODEL *model );
void	dU_DeleteModel( dMODEL *model );

#endif // __DECKARD_OBJLOADER_H__
