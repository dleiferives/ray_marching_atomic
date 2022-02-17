#ifndef DH_RAY_UTIL_H
#define DH_RAY_UTIL_H
#include "atomic.h"
struct t_v3f
{
	float x;
	float y;
	float z;
};
struct t_v3i
{
	int x;
	int y;
	int z;
};
struct t_ray
{
	struct t_v3f pos;
	struct t_v3f dir;
	union t_id chunk_pos;
	double distance;
};
#endif
