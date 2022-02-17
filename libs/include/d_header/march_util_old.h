#ifndef MARCH_UTIL_HEADER
#define MARCH_UTIL_HEADER

#define MAX_RAY_DIST 10000.0
#define MIN_RAY_HIT_DIST 0.1
#define MAX_RAY_MARCHES 1000 
#define NUM_OBJECTS 10
typedef struct color_t
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
}color_t;

typedef struct v_3f
{
	float x;
	float y;
	float z;
}v_3f;

typedef struct ray_t
{
	struct v_3f pos;
	struct v_3f dir;// direction
	struct color_t color;	
}ray_t;

typedef struct ob_t
{
	uint8_t type;
	double radius;
	struct v_3f pos;
	struct v_3f rot;// direction
	struct color_t color;// for now just a pure color, eventually want it to be mapped from the texture... dunno how to do that yet tho
}ob_t;

/* Camera Gloabl Variables */
// dependent on march_util.h
float focal_length =2.2;
struct v_3f c_pos ={1,1,500};
struct v_3f c_dir ={0,0,0};
uint32_t cam_width = 10;
uint32_t cam_height = 10;

/* Object Gloabl Variables */
struct v_3f cube_normals[6] ={{1,0,0},{-1,0,0},{0,1,0},{0,-1,0},{0,0,1},{0,0,-1}};
uint32_t g_objects_id[100] = {1,2,3,4,4,1,1};

ob_t g_objects[100] = {{0,1,{0,0,0},{0,0,0},{0,0,0,255}},{2,1,{50,50,-5000},{0,0,0},{255,0,0,100}}};

s32 random_val(s32 lower, s32 upper)
{
	if (upper < lower) {
		s32 tmp = lower;
		lower = upper;
		upper = tmp;
	}
	return (rand() % (upper - lower + 1) + lower);
}
s32 randm(s32 lower, s32 upper)
{
	if (upper < lower) {
		s32 tmp = lower;
		lower = upper;
		upper = tmp;
	}
	return (rand() % (upper - lower + 1) + lower);
}

void generate_objects(int number)
{
	if (number > NUM_OBJECTS-1)
		number = NUM_OBJECTS -1;

	for(int i=0; i<number; i++)
	{
		g_objects[i+2] = (ob_t){0,
		                 	randm(0,100),
											{randm(-1000,1000),randm(-1000,1000),randm(-500,500)},
											{0,0,0},
											{randm(0,255),randm(0,255),randm(0,255),randm(100,255)}};
		g_objects_id[i+1] = i+1;
	}
	g_objects[2] = (ob_t){1,0,{0,0,0},{0,0,0},{255,0,0,255}};
}

// s = sphere pos
// r = ray pos
struct v_3f noramalize_v_3f(struct v_3f v)
{
	double length = sqrt((v.x*v.x)+(v.y*v.y)+(v.z*v.z));
	return (v_3f){v.x/length, v.y/length, v.z/length};
}
double len_v_3f ( struct v_3f v)
{
	return sqrt((v.x*v.x)+(v.y*v.y)+(v.z*v.z));
}

struct v_3f add_v_3f(struct v_3f v1, struct v_3f v2)
{
	return (v_3f){v1.x+v2.x,v1.y+v2.y,v1.z+v2.z};
}
struct v_3f sub_v_3f(struct v_3f v1, struct v_3f v2)
{
	return (v_3f){v1.x-v2.x,v1.y-v2.y,v1.z-v2.z};
}

struct v_3f mult_float_to_v_3f(struct v_3f v, float i)
{
	return (v_3f){v.x*i,v.y*i,v.z*i};
}
struct v_3f mult_int32_to_v_3f(struct v_3f v, int32_t i)
{
	return (v_3f){v.x*i,v.y*i,v.z*i};
}

struct v_3f flip_v_3f_sign(struct v_3f v)
{
	return (v_3f){-v.x,-v.y,-v.z};
}

double dot_v_3f(struct v_3f v1, struct v_3f v2)
{
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

struct v_3f abs_v_3f(struct v_3f v)
{
	return (v_3f){abs(v.x),abs(v.y),abs(v.z)};
}

double max_v_3f(struct v_3f v)
{
	double tmp1 = (v.x > v.y) ? v.x : v.y;
	double tmp2 = (tmp2 > v.z) ? tmp2 : v.z;
	return tmp2;
}
struct v_3f max_v_3f_each(struct v_3f v, double c)
{
	double cx = (v.x > c) ? v.x : c;
	double cy = (v.y > c) ? v.y : c;
	double cz = (v.z > c) ? v.z : c;
	return (v_3f){cx,cy,cz};
}

// s is position 
// r is ray position
// radius is self explanitory
double dist_to_sphere(struct v_3f s, float radius, struct v_3f r)
{
	double dist =sqrt(((s.x-r.x)*(s.x-r.x))+((s.y-r.y)*(s.y-r.y))+((s.z-r.z)*(s.z-r.z))) -radius;
	return dist;
}

double dist_to_plane(struct v_3f s, float radius, struct v_3f r)
{
	double dist = abs(r.z - s.z);
	return dist;
}

double dist_to_cube(struct v_3f s, float radius, struct v_3f r)
{
	// v dot n = distance to the plane of the face 
	// v = [p0 - [cube_orign + n[?]*radius]
	double min_dist = MAX_RAY_DIST;
	struct v_3f t_pos = (mult_float_to_v_3f(sub_v_3f(r,s),1/radius));
	struct v_3f corner = {1,1,1}; 
	struct v_3f ray = abs_v_3f(t_pos); // fold ray into positive octant
	struct v_3f cornerToRay = sub_v_3f(ray,corner);
	float cornerToRayMaxComponent = max_v_3f(cornerToRay); 
	float distToInsideRay = (cornerToRayMaxComponent < 0) ? cornerToRayMaxComponent : 0.0;
	struct v_3f closestToOutsideRay = max_v_3f_each(cornerToRay, 0.0);
	return (len_v_3f(closestToOutsideRay) + distToInsideRay) * radius;
}

double get_dist_ray_to_object(struct ray_t r,uint32_t g_id)
{
	ob_t t_ob =g_objects[g_id];
	if(t_ob.type == 0)
		return dist_to_sphere(t_ob.pos,t_ob.radius,r.pos);
	// Distance to a defined plane :0... it is infinatly streching and glorious
	if(t_ob.type == 1)
		return dist_to_plane(t_ob.pos, t_ob.radius, r.pos);
	if(t_ob.type == 2)
		return dist_to_cube(t_ob.pos, t_ob.radius, r.pos);
	if(t_ob.type == 3)
		return -1;
}
struct color_t ray_march(ray_t r,uint32_t * obs,uint32_t n_obs)
{
	/* printf("r dir11 (%f,%f,%f)\n",r.dir.x,r.dir.y,r.dir.z); */
	double min_dist = MAX_RAY_DIST-1;
	uint32_t min_dist_id =0;
	for(uint32_t steps=0;(min_dist > MIN_RAY_HIT_DIST) && (min_dist < MAX_RAY_DIST) ;steps++)
	{
		min_dist = MAX_RAY_DIST;
		for(uint32_t i=0; i<n_obs; i++)
		{
			double temp_dist =get_dist_ray_to_object(r,obs[i]);
			/* printf("temp dist %f\n",temp_dist); */
			/* printf(" if statment check? %i\n",(temp_dist < min_dist)); */
			if(temp_dist < min_dist)
			{
				min_dist = temp_dist;
				min_dist_id = obs[i];
			/* printf("min dist p_temp %f %i\n",min_dist,obs[i]); */
			}
		}
		if(min_dist <= MIN_RAY_HIT_DIST)
			return g_objects[min_dist_id].color;
		/* printf("r dir (%f,%f,%f)\n",r.dir.x,r.dir.y,r.dir.z); */
		r.dir = noramalize_v_3f(r.dir);
		/* printf("r dir (%f,%f,%f)\n",r.dir.x,r.dir.y,r.dir.z); */
		/* printf("min dist %f\n",min_dist); */
		r.pos.x += r.dir.x*min_dist;	
		r.pos.y += r.dir.y*min_dist;	
		r.pos.z += r.dir.z*min_dist;	
		/* printf("r pos (%f,%f,%f)\n",r.pos.x,r.pos.y,r.pos.z); */
		if(steps > MAX_RAY_MARCHES)
		{
			return g_objects[0].color;
		}

	}
	/* printf("id (%i)\n",min_dist_id); */
	return g_objects[0].color;
}

struct color_t get_ray_pixel_color(float x, float y)
{
	x += -5; 
	y += -5; 
	/* printf("%f x %f y\n",x,y); */
	struct ray_t tmp_ray;
	tmp_ray.pos = c_pos;
	struct v_3f nc_dir = noramalize_v_3f(c_dir);
	/* printf("nc_dir (%f,%f,%f)\n",nc_dir.x,nc_dir.y,nc_dir.z); */
	struct v_3f dx_v = noramalize_v_3f((v_3f){-nc_dir.y,nc_dir.x,0});
	struct v_3f dy_v = noramalize_v_3f((v_3f){-nc_dir.x*nc_dir.z,-nc_dir.z*nc_dir.y,(nc_dir.x*nc_dir.x)+(nc_dir.y*nc_dir.y)});
	nc_dir = mult_float_to_v_3f(nc_dir,focal_length);
	/* printf("normalized c_dir (%f,%f,%f)\n",nc_dir.x,nc_dir.y,nc_dir.z); */
	/* printf("dx_v   (%f,%f,%f)\n",dx_v.x,dx_v.y,dx_v.z); */
	/* printf("dy_v   (%f,%f,%f)\n",dy_v.x,dy_v.y,dy_v.z); */

	tmp_ray.dir = add_v_3f(nc_dir,
										 add_v_3f(mult_float_to_v_3f(dx_v,x),
											        mult_float_to_v_3f(dy_v,y)));

	/* printf("(%f,%f,%f)\n",nc_dir.x,nc_dir.y,nc_dir.z); */
	/* printf("(%f,%f,%f)\n",dx_v.x,dx_v.y,dx_v.z); */
	/* printf("(%f,%f,%f)\n",dy_v.x,dy_v.y,dy_v.z); */
	/* printf("tmp_ray_dir (%f,%f,%f)\n",tmp_ray.dir.x,tmp_ray.dir.y,tmp_ray.dir.z); */
	color_t tmp_color =	ray_march(tmp_ray, g_objects_id, 70);
	/* printf("(%i)\n",tmp_color.r); */
	return tmp_color;
}

#endif
