#ifndef DH_ATOMIC_H
#define DH_ATOMIC_H
#define NUM_CHUNKS_ATOMIC 512 
#define NUM_SUB_CHUNKS 8
#define NUM_BLOCKS_SUB_CHUNKS 8
#define Z_CHUNKS_ATOMIC 7
#define X_CHUNKS_ATOMIC 7
#define Y_CHUNKS_ATOMIC 7
#define CHUNK_SIZE_REL 4.0
struct t_id_pos
{
	unsigned int x: 16;
	unsigned int y: 16;
	unsigned int z: 16;
	unsigned int sector: 16;
};
union t_id
{
	unsigned long id;
	struct t_id_pos;
};

struct t_chunk
{
	char exists;
	// 4*4*4 8 atom wide blocks
	unsigned char points[NUM_SUB_CHUNKS][NUM_BLOCKS_SUB_CHUNKS][X_CHUNKS_ATOMIC+1][Y_CHUNKS_ATOMIC+1][Z_CHUNKS_ATOMIC+1];
	// octant order goes from z=0 x=0 y=0
	// x++ first 
	// y++ second
	// z++
	// z=0 y23
	//      01
	//       x
	// z=1  y67
	//       45
	//        x
	// same for sub octants
	unsigned char octants;
	/* unsigned long blocks; */
	// represented by a bit field	
	unsigned char so[8];
	// pos in chunk map  
	union t_id id;
};


// GLOBAL VARS

struct t_chunk *g_chunks[NUM_CHUNKS_ATOMIC]; //xyz

struct t_chunk gen_full_chunk(int x, int y, int z);
double dist_to_ray_local_chunks(struct t_ray r);
double dist_ray_chunk(struct t_ray r, unsigned int c_id);
unsigned int v3i_g_chunk_id(struct t_v3i p);
#endif
