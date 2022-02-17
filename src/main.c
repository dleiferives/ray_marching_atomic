#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#define GS_IMPL
#include <gs/gs.h>

#define GS_IMMEDIATE_DRAW_IMPL
#include <gs/util/gs_idraw.h>
#include "atomic.h"
#include "ray_util.h"

struct t_ray g_ray = {{1,1,1},{1,0,0}};




// Command buffer
// as far as I can tell this is something by which the data is sent to the screen
// I think it may be instructions to the gpu or something. 
// Eitherway everything uses it
gs_command_buffer_t                  gcb   = {0};
gs_immediate_draw_t                  gsi   = {0};

// used for frame buffer... which I do not yet understand gs_handle(gs_graphics_render_pass_t) rp   = {0};
gs_handle(gs_graphics_texture_t)     rt = {0}; // render target


// Initilizaton function 
void app_init()
{
	gcb = gs_command_buffer_new();
	gsi = gs_immediate_draw_new(gs_platform_main_window());

	rt = gs_graphics_texture_create(
        &(gs_graphics_texture_desc_t) {
            .width = gs_platform_framebuffer_width(gs_platform_main_window()),   // Width of texture in pixels
            .height = gs_platform_framebuffer_height(gs_platform_main_window()), // Height of texture in pixels
            .format = GS_GRAPHICS_TEXTURE_FORMAT_RGBA8,                          // Format of texture data (rgba32, rgba8, rgba32f, r8, depth32f, etc...)
            .wrap_s = GS_GRAPHICS_TEXTURE_WRAP_REPEAT,                           // Wrapping type for s axis of texture
            .wrap_t = GS_GRAPHICS_TEXTURE_WRAP_REPEAT,                           // Wrapping type for t axis of texture
            .min_filter = GS_GRAPHICS_TEXTURE_FILTER_LINEAR,                     // Minification filter for texture
            .mag_filter = GS_GRAPHICS_TEXTURE_FILTER_LINEAR,                     // Magnification filter for texture
            .render_target = true
						}
  );

	/* generate_objects(80); */
	struct t_chunk t = gen_full_chunk(1,0,0);
	g_chunks[v3i_g_chunk_id((struct t_v3i){1,0,0})] = &t;

	/* g_chunks = */ 
	gs_platform_lock_mouse(gs_platform_main_window(), true);
}

void draw_quad_screen(gs_immediate_draw_t * gsi)
{
	gsi_begin(gsi, GS_GRAPHICS_PRIMITIVE_TRIANGLES);
		gsi_tc2f(gsi, 0.f, 0.f); // sets the uv to 0
		gsi_c4ub(gsi, 100, 100, 100, 100); // r g b a
		gsi_v3f(gsi, 2, 2, 0);
		gsi_v3f(gsi, -2, 2, 0);
		gsi_v3f(gsi, 2, -2, 0); // points for quad corners
	gsi_end(gsi);

	gsi_begin(gsi, GS_GRAPHICS_PRIMITIVE_TRIANGLES);
		gsi_tc2f(gsi, 0.f, 0.f); // sets the uv to 0
		gsi_c4ub(gsi, 100, 100, 100, 100); // r g b a
		gsi_v3f(gsi, -2, 2, 0); // points for quad corners
		gsi_v3f(gsi, -2, -2, 0);
		gsi_v3f(gsi, 2, -2, 0);
	gsi_end(gsi);

}

/* void draw_square(gs_immediate_draw_t * gsi, float x_off, float y_off, float divisor,color_t color) */
/* { */
/* 	float xt = ((x_off)*4.0)/divisor; */
/* 	float yt = ((y_off)*4.0)/divisor; */
/* 	gsi_begin(gsi, GS_GRAPHICS_PRIMITIVE_TRIANGLES); */
/* 		gsi_tc2f(gsi, 0.f, 0.f); // sets the uv to 0 */
/* 		gsi_c4ub(gsi, color.r, color.g, color.b, color.a); // r g b a */
/* 		gsi_v3f(gsi, -2.0+xt, 2.0-yt, 0); */
/* 		gsi_v3f(gsi, (-2.0+xt)+(4.0/divisor), 2.0-yt, 0); */
/* 		gsi_v3f(gsi, (-2.0+xt)+(4.0/divisor), (2.0-yt)-(4.0/divisor), 0); // points for quad corners */
/* 	gsi_end(gsi); */

/* 	gsi_begin(gsi, GS_GRAPHICS_PRIMITIVE_TRIANGLES); */
/* 		gsi_tc2f(gsi, 0.f, 0.f); // sets the uv to 0 */
/* 		gsi_c4ub(gsi, color.r, color.g, color.b, color.a); // r g b a */
/* 		gsi_v3f(gsi, -2.0+xt, 2.0-yt, 0); // points for quad corners */
/* 		gsi_v3f(gsi, -2.0+xt, (2.0-yt)-(4.0/divisor), 0); */
/* 		gsi_v3f(gsi, (4.0/divisor)+xt+-2.0, (2.0-yt)-(4.0/divisor), 0); */
/* 	gsi_end(gsi); */

/* } */

// The update loop
uint32_t update_iterator = 0;
void app_update()
{
	update_iterator++;

	/* move_camera(); */

	if (gs_platform_mouse_pressed(GS_MOUSE_LBUTTON) && !gs_platform_mouse_locked()) {
        gs_platform_lock_mouse(gs_platform_main_window(), true);
    }


	if (gs_platform_key_pressed(GS_KEYCODE_ESC)) 
		gs_engine_quit();

	const gs_vec2 ws = gs_platform_window_sizev(gs_platform_main_window());

	gs_vec2 mouse_pos = gs_platform_mouse_positionv();

	gs_graphics_clear_desc_t fb_clear = {.actions = &(gs_graphics_clear_action_t){.color = 0.0f, 0.0f, 0.0f, 1.f}};
  gs_graphics_clear_desc_t bb_clear = {.actions = &(gs_graphics_clear_action_t){.color = 0.1f, 0.1f, 0.1f, 1.f}};


	    // Immediate rendering for offscreen buffer
		/* focal_length = (float)mouse_pos.y/(float)20; */
		
		/* gs_snprintfc(cam_pos_buff, 256, "Coordinates %.8f,%.8f,%.8f Fps: %f", c_pos.x, c_pos.y, c_pos.z,1000.f/gs_engine_subsystem(platform)->time.frame); */
    gsi_camera3D(&gsi);
    gsi_transf(&gsi, 0.f, 0.f, -2.459);

    /* gsi_rotatefv(&gsi, mouse_pos.x * 0.1f, GS_XAXIS); */
    /* gsi_rotatefv(&gsi, mouse_pos.y * 0.1f, GS_YAXIS); */
    /* gsi_rotatefv(&gsi, gs_platform_elapsed_time() * 0.0005f, GS_ZAXIS); */
		gs_graphics_begin_render_pass(&gcb, GS_GRAPHICS_RENDER_PASS_DEFAULT);
        gs_graphics_set_viewport(&gcb, 0, 0, (int32_t)ws.x, (int32_t)ws.y);
        gs_graphics_clear(&gcb, &bb_clear);
				draw_quad_screen(&gsi);
				for(float x=0; x<100; x+=1)
					for(float y=0; y<100; y+=1)
						/* draw_square(&gsi,x,y,100,get_ray_pixel_color(x/10,y/10)); */
				gsi_defaults(&gsi);
				gsi_camera2D(&gsi);
				gsi_text(&gsi, 20.f, 10.f, "- Move: W, A, S, D, Space, Shift, Control", NULL, false, 200, 200, 200, 255);
				gsi_text(&gsi, 20.f, 30.f, "- Mouse to look", NULL, false, 200, 200, 200, 255);
				gsi_text(&gsi, 20.f, 50.f, "- Escape to exit", NULL, false, 200, 200, 200, 255);
				/* gsi_text(&gsi, 20.f, 70.f, cam_pos_buff, NULL, false, 200, 200, 200, 255); */
        gsi_draw(&gsi, &gcb);
    gs_graphics_end_render_pass(&gcb);


		gs_graphics_submit_command_buffer(&gcb);
}

// Cleanup
void app_shutdown()
{

}

// The main function for this app, returns gunslinger pap description whatever that is
gs_app_desc_t gs_main(int32_t argc, char** argv)
{

   return (gs_app_desc_t){
		.window_title = "Toki Pona Game?",
		.init         = app_init,
		.update       = app_update,
		.shutdown     = app_shutdown,
		.window_width = 400,
		.window_height= 400
	 };
}
