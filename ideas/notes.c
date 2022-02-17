



everything is representeted "atomically" so this means that a ray just gets the distance to the center of a coordinate, and therefore at everycoordinate, there is a sphere with radius 1/2

everything lies within chunk system

256 by 256 by 256, only holds if it is on [7] and then something for color or id or something ig [7:0]


struct t_chunk
{
	// 4*4*4 8 atom wide blocks
	unsigned char points[32768];
	unsigned char octants;
	unsigned long blocks;
		o0: 8;
		o1: 8;
		o2: 8;
		o3: 8;
		o4: 8;
		o5: 8;
		o6: 8;
		o7: 8;
	int x;
	int y;
	int z;
	char id;
};

ray only goes through its local group of chunks

2d
000
0#0
000
# is chunk with ray 
0's are surrounding 
suppose id  = chunk location => access chunks by their id o(1)?

struct t_ray
	chunk_pos
	global_pos
	dir

min_dis =0
for(y=-1; y<=1;y++)
	for(x=-1; x<=1;x++)
		if min_dis > dist(r.chunk_pos+[x,y])
			min_dis = dist(r.chunk_pos+[x,y])

3d the same
but with z

struct t_chunk *chunk_map[256]

dist_chunk(r,x,y,z)
	chunk_map[x,y,z][0] = a chunk at xyz
	run distance algorithm for everything inside the chunk, and return the point with distance
	now that is a pretty fucking large search no hezzi
	so we will seach it by another value 
	
	for mask in octants
		if chunk.octant and mask
			(search the octant)
			for mask_b in block
				if( chunk.o# and maskb)
					get distance for all points in block
	
	return point id_in_chunk, distance (float) 







