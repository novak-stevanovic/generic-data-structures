#ifndef _GDS_HASH_MAP_H
#define _GDS_HASH_MAP_H

#ifdef GDS_ENABLE_OPAQUE_STRUCTS
struct GDSHashMap;
#else
#define __GDS_HASH_MAP_DEF_ALLOW__
#include "def/gds_hash_map_def.h"
#endif

typedef struct GDSHashMap GDSHashMap;

void gds_hash_map_init(GDSHashMap* hash_map);

GDSHashMap* gds_hash_map_create();

#endif // _GDS_HASH_MAP_H

