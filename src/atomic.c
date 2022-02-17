#ifndef DH_ATOMIC_C
#define DH_ATOMIC_C
#include "ray_util.h"
#include "atomic.h"
#include <math.h>

unsigned int v3i_g_chunk_id(struct t_v3i p)
{
	return (p.z * (X_CHUNKS_ATOMIC +1) * (Y_CHUNKS_ATOMIC+1))	
		+ (p.y * (X_CHUNKS_ATOMIC +1)) + p.x;
}

double dist_ray_chunk(struct t_ray r, unsigned int c_id)
{
	// get chunk location in float
	struct t_chunk chunk = g_chunks[c_id][0];
	float c_x = ((float)chunk.id.x) * CHUNK_SIZE_REL;
	float c_y = ((float)chunk.id.y) * CHUNK_SIZE_REL;
	float c_z = ((float)chunk.id.z) * CHUNK_SIZE_REL;
	
	double min_dist = 1000.0;
	unsigned long min_dist_loc =0;
	unsigned char cur_oct =0;
	for(unsigned char mask =1; mask!=0;mask*=2)
	{
		// if there is something in the octant
		if((mask & chunk.octants)!=0)
		{
			unsigned char cur_block =0;
			for(unsigned char b_mask =1; b_mask!=0;b_mask*=2)
			{
				// if there is a block in the sub octant
				if((b_mask & chunk.so[cur_oct]) != 0)
				{
					// get min distance to all points in block
					for(int x =0; x<8; x++)
					{
						for(int y=0; y<8;y++)
						{
							for(int z=0; z<8; z++)
							{
								if(chunk.points[cur_oct][cur_block][x][y][z] == 0)
									continue;
								float p_z = c_z + ((float)(cur_oct > 3)*16 + (cur_block>4)*8 + z)*CHUNK_SIZE_REL/32.0;
								float p_y = c_y + ((float)((cur_oct%4)>1)*16 + ((cur_block%4)>1)*8 + y)*CHUNK_SIZE_REL/32.0;
								float p_x = c_x + ((float)(cur_oct%2)*16 + (cur_block%2)*8 + x)*CHUNK_SIZE_REL/32.0;
								double tmp_dist = sqrt((r.pos.x-p_x)*(r.pos.x-p_x) + (r.pos.y-p_y)*(r.pos.y-p_y)+ (r.pos.z-p_z)*(r.pos.z-p_z))- (0.15625 * CHUNK_SIZE_REL);
								if(tmp_dist < min_dist)
								{
									min_dist = tmp_dist;
									min_dist_loc = 0;
									min_dist_loc += cur_oct;
									min_dist_loc <<=3;
									min_dist_loc += cur_block;
									min_dist_loc <<=3;
									min_dist_loc += x;
									min_dist_loc <<=3;
									min_dist_loc += y;
									min_dist_loc <<=3;
									min_dist_loc += z;
								}
							}
						}
					}
				}
				cur_block++;
			}
		}
		cur_oct++;
	}
	return min_dist;
}

// get local chunks to the ray
double dist_to_ray_local_chunks(struct t_ray r)
{
	printf("hehe");
	// for chunks 1 away
	double min_dist = 1000.0;
	for(int y = -1; y <=1;y++)
	{
		// dont check if invalid
		if( (y+r.chunk_pos.y) > Y_CHUNKS_ATOMIC ||(y+r.chunk_pos.y) <0)
			continue;
		for(int x = -1; x <=1;x++)
		{
			// dont check if invalid
			if( (x+r.chunk_pos.x) > X_CHUNKS_ATOMIC ||(x+r.chunk_pos.x) <0)
				continue;
			for(int z = -1; z <=1;z++)
			{
				// don't check if invalid
				if( (z+r.chunk_pos.z) > Z_CHUNKS_ATOMIC ||(z+r.chunk_pos.z) <0)
					continue;
				
				 unsigned int tmp_c_id = v3i_g_chunk_id((struct t_v3i){x+r.chunk_pos.x, y+r.chunk_pos.y, z+r.chunk_pos.z});
				 if (g_chunks[tmp_c_id] == NULL)
					 continue;
					 //thats an error
				 double tmp_dist = dist_ray_chunk(r, tmp_c_id);
				 if(tmp_dist < min_dist)
				 {
				  min_dist = tmp_dist;
				 }
			}
		}
	}
	return min_dist;
}

struct t_chunk gen_full_chunk(int x, int y, int z)
{
	struct t_chunk t = {0};
	t.exists = 1;
	for(int a=0; a<8; a++)
	{
		for(int b=0; b<8; b++)
		{
			for(int c=0; c<8; c++)
			{
				for(int d=0; d<8; d++)
				{
					for(int e=0; e<8; e++)
					{
						t.points[a][b][c][d][e] = 0xFF;
					}
				}
			}
		}
	}
	// pos on chunk map
	t.octants = 0xFF;
	for(int i =0; i <8;i++)
	{
		t.so[i] = 0xFF;
	}
	t.id.x=x;
	t.id.y=y;
	t.id.z=z;
	
	return t;
}

#endif


