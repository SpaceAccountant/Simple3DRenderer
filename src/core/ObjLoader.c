#include "ObjLoader.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define D_ABS( X )			( ( X ) < 0 ? -( X ) : ( X ) )

typedef struct _dMODELPOSITIONNODE {
	struct _dMODELPOSITIONNODE *next;
	dVEC4						value;
} dPOSITIONNODE;

typedef struct _dMODELNORMALNODE {
	struct _dMODELNORMALNODE   *next;
	dVEC3						value;
} dNORMALNODE;

typedef struct {
	int							position0;
	int							position1;
	int							position2;

	int							tex_coord0;
	int							tex_coord1;
	int							tex_coord2;

	int							normal0;
	int							normal1;
	int							normal2;

	int							material;
} dMODELINDEX;

typedef struct _dMODELINDEXNODE {
	struct _dMODELINDEXNODE	   *next;
	dMODELINDEX					value;
} dINDEXNODE;

typedef struct {
	char					   *name;
	dVEC4						color;
} dMATERIAL;

typedef struct _dMATERIALNODE {
	struct _dMATERIALNODE	   *next;
	dMATERIAL					value;
} dMATERIALNODE;

static int dU_LoadMaterials( const char *path, dMATERIALNODE **materials ) {
	FILE *file;
	char header[512];
	dMATERIALNODE *m;

	if( !( file = fopen( path, "r" ) ) )
		return 0;

	( *materials ) = NULL;
	
	while( fscanf( file, "%s", header ) != EOF ) {
		if( strcmp( header, "newmtl" ) == 0 ) {
			m = ( dMATERIALNODE* ) malloc( sizeof( dMATERIALNODE ) );

			if( !m )
				break;

			fscanf( file, "%s\n", header );

			m->value.name = ( char* ) malloc( strlen( header ) + 1 );

			if( !m->value.name ) {
				free( m );
				break;
			}

			strcpy( m->value.name, header );

			m->next = ( *materials );
			( *materials ) = m;
		} else if( strcmp( header, "Kd" ) == 0 && m ) {
			fscanf( file, "%f %f %f\n", &( m->value.color.x ), &( m->value.color.y ), &( m->value.color.z ) );
			m->value.color.w = 1.0f;
		}
	}

	fclose( file );

	return 1;
}

static int dU_FindMaterialIndex( dMATERIALNODE *materials, const char *name ) {
	dMATERIALNODE *m;
	int i;

	for( m = materials, i = 0; m != NULL; m = m->next, i++ )
		if( strcmp( name, m->value.name ) == 0 )
			return i;

	return -1;
}

static void dU_FreeMaterials( dMATERIALNODE *materials ) {
	dMATERIALNODE *m, *next;

	for( m = materials; m != NULL; m = next ) {
		next = m->next;
		free( m->value.name );
		free( m );
	}
}

static int dU_LoadObject( const char *path, dMATERIALNODE *materials, dPOSITIONNODE **positions, dNORMALNODE **normals, dINDEXNODE **faces, int *face_count ) {
	FILE *file;
	int material;
	char header[512];
	dPOSITIONNODE *p, *pnext;
	dNORMALNODE *n, *nnext;
	dINDEXNODE *i;

	if( !( file = fopen( path, "r" ) ) )
		return 0;

	material = -1;

	while( fscanf( file, "%s\n", header ) != EOF ) {
		if( materials && strcmp( header, "o" ) == 0 ) {
			material = -1;
		} else if( strcmp( header, "v" ) == 0 ) {
			p = ( dPOSITIONNODE* ) malloc( sizeof( dPOSITIONNODE ) );

			if( !p ) {
				( *face_count ) = -1;
				break;
			}

			fscanf( file, "%f %f %f\n", &( p->value.x ), &( p->value.y ), &( p->value.z ) );
			p->value.w = 1.0f;

			p->next = NULL;

			if( !( *positions ) ) {
				( *positions ) = p;
			} else {
				for( pnext = ( *positions ); pnext->next != NULL; pnext = pnext->next );
				pnext->next = p;
			}
		} else if( strcmp( header, "vn" ) == 0 ) {
			n = ( dNORMALNODE* ) malloc( sizeof( dNORMALNODE ) );

			if( !n ) {
				( *face_count ) = -1;
				break;
			}

			fscanf( file, "%f %f %f\n", &( n->value.x ), &( n->value.y ), &( n->value.z ) );

			n->next = NULL;

			if( !( *normals ) ) {
				( *normals ) = n;
			} else {
				for( nnext = ( *normals ); nnext->next != NULL; nnext = nnext->next );
				nnext->next = n;
			}
		} else if( materials && strcmp( header, "usemtl" ) == 0 ) {
			fscanf( file, "%s\n", header );
			material = dU_FindMaterialIndex( materials, header );
		} else if( strcmp( header, "f" ) == 0 ) {
			i = ( dINDEXNODE* ) malloc( sizeof( dINDEXNODE ) );

			if( !i ) {
				( *face_count ) = -1;
				break;
			}

			fscanf( file, "%d//%d %d//%d %d//%d\n", &( i->value.position0 ), &( i->value.normal0 ), &( i->value.position1 ), &( i->value.normal1 ), &( i->value.position2 ), &( i->value.normal2 ) );
			i->value.material = material;

			i->next = ( *faces );
			( *faces ) = i;

			( *face_count )++;
		}
	}

	fclose( file );

	return ( ( *face_count ) == -1 ? 0 : 1 );
}

static void dU_FreeObject( dPOSITIONNODE *positions, dNORMALNODE *normals, dINDEXNODE *faces ) {
	dPOSITIONNODE *p, *pnext;
	dNORMALNODE *n, *nnext;
	dINDEXNODE *f, *fnext;

	for( p = positions; p != NULL; p = pnext ) {
		pnext = p->next;
		free( p );
	}
	
	for( n = normals; n != NULL; n = nnext ) {
		nnext = n->next;
		free( n );
	}

	for( f = faces; f != NULL; f = fnext ) {
		fnext = f->next;
		free( f );
	}
}

static int dU_LoadVertexList( dMATERIALNODE *materials, dPOSITIONNODE *positions, dNORMALNODE *normals, dINDEXNODE *faces, int face_count, dVERTEX **vertices ) {
	dPOSITIONNODE *p;
	dNORMALNODE *n;
	dINDEXNODE *f;
	dMATERIALNODE *m;
	int i, j;

	if( face_count > 0 ) {
		( *vertices ) = ( dVERTEX* ) malloc( 3 * face_count * sizeof( dVERTEX ) );

		if( vertices ) {
			for( f = faces, i = 0; f != NULL; f = f->next, i++ ) {
				for( p = positions, j = 1; p != NULL; p = p->next, j++ ) {
					if( f->value.position0 == j ) ( *vertices )[i * 3 + 2].position = p->value;
					if( f->value.position1 == j ) ( *vertices )[i * 3 + 1].position = p->value;
					if( f->value.position2 == j ) ( *vertices )[i * 3    ].position = p->value;
				}
				
				for( n = normals, j = 1; n != NULL; n = n->next, j++ ) {
					if( f->value.normal0 == j ) ( *vertices )[i * 3 + 2].normal = n->value;
					if( f->value.normal1 == j ) ( *vertices )[i * 3 + 1].normal = n->value;
					if( f->value.normal2 == j ) ( *vertices )[i * 3    ].normal = n->value;
				}

				if( f->value.material == -1 ) {
					( *vertices )[i * 3    ].color.x = ( *vertices )[i * 3    ].color.y = ( *vertices )[i * 3    ].color.z = ( *vertices )[i * 3    ].color.w = 1.0f;
					( *vertices )[i * 3 + 1].color.x = ( *vertices )[i * 3 + 1].color.y = ( *vertices )[i * 3 + 1].color.z = ( *vertices )[i * 3 + 1].color.w = 1.0f;
					( *vertices )[i * 3 + 2].color.x = ( *vertices )[i * 3 + 2].color.y = ( *vertices )[i * 3 + 2].color.z = ( *vertices )[i * 3 + 2].color.w = 1.0f;
				} else {
					for( m = materials, j = 0; m != NULL; m = m->next, j++ ) {
						if( f->value.material == j ) {
							( *vertices )[i * 3].color = ( *vertices )[i * 3 + 1].color = ( *vertices )[i * 3 + 2].color = m->value.color;
							break;
						}
					}
				}
			}
		}
	}

	return 1;
}

int dU_LoadModel( const char *obj_path, const char *mtl_path, dMODEL *model ) {
	dMATERIALNODE *materials;
	dPOSITIONNODE *positions;
	dNORMALNODE *normals;
	dINDEXNODE *faces;
	int face_count;

	materials = NULL;

	if( mtl_path )
		dU_LoadMaterials( mtl_path, &materials );

	positions       = NULL;
	normals         = NULL;
	faces           = NULL;
	face_count      = 0;
	model->vertices = NULL;

	dU_LoadObject( obj_path, materials, &positions, &normals, &faces, &face_count );
	dU_LoadVertexList( materials, positions, normals, faces, face_count, &( model->vertices ) );
	dU_FreeObject( positions, normals, faces );
	
	if( mtl_path )
		dU_FreeMaterials( materials );

	model->vertex_count = 3 * face_count;

	return 1;
}

void dU_DeleteModel( dMODEL *model ) {
	if( !model )
		return;

	free( model->vertices );
}
