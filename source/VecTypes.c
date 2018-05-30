#include <string.h>

#include "VecMath.h"
#include "VecTypes.h"

void dVT_SetMat4Identity( dMAT4 *m ) {
	dVT_SetMat4Zero( m );
	( *m )[0][0] = 1.0f;
	( *m )[1][1] = 1.0f;
	( *m )[2][2] = 1.0f;
	( *m )[3][3] = 1.0f;
}

void dVT_SetMat4Zero( dMAT4 *m ) {
	memset( *m, 0, sizeof( dMAT4 ) );
}
