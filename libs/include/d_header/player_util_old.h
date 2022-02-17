#ifndef INCLUDE_DH_PLAYER_UTIL
#define INCLUDE_DH_PLAYER_UTIL
#define SENSITIVITY 10
#define DH_PI 3.14159
struct v_3f camera_vel = {0,0,0};
double cam_lon=0;
double cam_lat=0;

double drag_or_sense = 0.0004;

void move_camera(/* eventually make a camera type*/)
{
	// The way that the camera works is by casting a point onto a sphere.
	// For Geogebra / psudocode
	// a=0.196
	// b=0.708
	// x^(2)+y^(2)+z^(2)=1
	// c=cos(sin^(-1)(a))
	// X = (c cos(t),c sin(t),a) # the circle at height a
	// Point((c cos(b π),c sin(b π),a),Vector((0,0,0))) # the point on that circle at rotation b*pi
	//
	gs_platform_t* platform = gs_engine_subsystem(platform);

	// Camera rotation 
	gs_vec2 mouse_pos = gs_platform_mouse_positionv();
	gs_vec2 dp = gs_vec2_scale(gs_platform_mouse_deltav(), SENSITIVITY);

	if((dp.x > 0) && (cam_lat == 1))
		cam_lat = -1;

	if((dp.x < 0) && (cam_lat == -1))
		cam_lat = 1;

	cam_lon = dp.y * drag_or_sense + cam_lon;
	cam_lat = dp.x * drag_or_sense + cam_lat;

	/* printf("Cam lat %f, lon %f\n",cam_lat, cam_lon); */

	cam_lat = (cam_lat >=1) ? 1.f : cam_lat;
	cam_lat = (cam_lat <=-1) ? -1.f : cam_lat;

	cam_lon = (cam_lon >=0.998) ? 0.998 : cam_lon;
	cam_lon = (cam_lon <=-0.998) ? -0.998 : cam_lon;

	double radius_sphere = cos(asin(cam_lon));
	double lat_mod = cam_lat * DH_PI;




	c_dir.x = radius_sphere * cos(lat_mod);
	c_dir.y = radius_sphere * sin(lat_mod);
  c_dir.z = cam_lon;


	// Finding up and down vectors !
	struct v_3f nc_dir = noramalize_v_3f(c_dir);
	struct v_3f dx_v = noramalize_v_3f((v_3f){-nc_dir.y,nc_dir.x,0});
	struct v_3f dy_v = noramalize_v_3f((v_3f){-nc_dir.x*nc_dir.z,-nc_dir.z*nc_dir.y,(nc_dir.x*nc_dir.x)+(nc_dir.y*nc_dir.y)});

	// Camera movment
	camera_vel = mult_float_to_v_3f(camera_vel,0.8); 
	if (gs_platform_key_down(GS_KEYCODE_W)) camera_vel = add_v_3f(camera_vel, c_dir);
	if (gs_platform_key_down(GS_KEYCODE_S)) camera_vel = add_v_3f(camera_vel, flip_v_3f_sign(c_dir));
	if (gs_platform_key_down(GS_KEYCODE_D)) camera_vel = add_v_3f(camera_vel, dx_v);
	if (gs_platform_key_down(GS_KEYCODE_A)) camera_vel = add_v_3f(camera_vel, flip_v_3f_sign(dx_v));
	if (gs_platform_key_down(GS_KEYCODE_SPACE)) camera_vel = add_v_3f(camera_vel, flip_v_3f_sign(dy_v));
	if (gs_platform_key_down(GS_KEYCODE_LEFT_SHIFT)) camera_vel = add_v_3f(camera_vel, dy_v);
	if (gs_platform_key_down(GS_KEYCODE_LEFT_CONTROL)) camera_vel = mult_float_to_v_3f(camera_vel, 1.5);
	c_pos = add_v_3f(camera_vel,c_pos);

	return;
}
#endif
