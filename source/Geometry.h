#ifndef __DECKARD_GEOMETRY_H__
#define __DECKARD_GEOMETRY_H__

#include "RenderTypes.h"
#include "VecTypes.h"

void	dRG_TransformationModel( const dVERTEX *input, int count, const dMAT4 *model, dVERTEX *output );
void	dRG_Lighting( const dVERTEX *input, int count, const dMAT4 *view, const dLIGHT *lights, int light_count, dVERTEX *output );
void	dRG_TransformationView( const dVERTEX *input, int count, const dMAT4 *view, dVERTEX *output );
void	dRG_Projection( const dVERTEX *input, int count, const dMAT4 *projection, dVERTEX *output );
void	dRG_Cliping( dVERTEX *input, int count, dPRIMATIVE *output, int *output_count );
void	dRG_PerspectiveDivide( const dPRIMATIVE *input, int count, dPRIMATIVE *output );
void	dRG_ViewportTransformation( const dPRIMATIVE *input, int count, const dVIEWPORT *viewport, dPRIMATIVE *output );

#endif // __DECKARD_GEOMETRY_H__
