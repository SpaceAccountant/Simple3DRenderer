#ifndef __DECKARD_RASTERIZER_H__
#define __DECKARD_RASTERIZER_H__

#include "ColorMap.h"
#include "RenderTypes.h"

void	dRR_RasterPrimitives( const dPRIMATIVE *input, int count, const dFRAMEBUFFER *output, const dCOLORMAP *color_map );
void	dRR_RasterTriangles( const dPRIMATIVE *input, int count, const dFRAMEBUFFER *output, const dCOLORMAP *color_map );

#endif // __DECKARD_RASTERIZER_H__
