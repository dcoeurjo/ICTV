#include "Parameters.h"

#include <stdlib.h>

Parameters* Parameters::instance = NULL;

Parameters::Parameters()
{
    g_window.width = 1500;
    g_window.height = 1000;
    g_window.major = 3;
    g_window.minor = 2;
    g_window.msaa_factor = 1; // multisampling level
    g_window.msaa_fixedsamplelocations = true; // fixed sample locations for msaa

    g_framebuffer_region.p = gk::TVec2<float>(0, 0);
    g_framebuffer_region.mag = 1;

    g_capture.enabled = false;
    g_capture.count = 0;
    g_capture.frame = 0;

    g_geometry.affine.identity();
    g_geometry.pingpong = 1;
    g_geometry.freeze = false;
    g_geometry.scale[0] = 512;
    g_geometry.scale[1] = 512;
    g_geometry.scale[2] = 512;

    g_camera.fovy = 45.f;
    g_camera.znear = 0.01f;
    g_camera.zfar = 10000.f;
    g_camera.exposure = 3.f;

    g_mouse = MOUSE_GEOMETRY;
    
    g_draw_cells = true;
    g_draw_triangles = true;
    g_culling = true;
    g_fromtexture = true;
    g_solid_wireframe = true;
    g_controls = false;
    g_gui = true;
    g_radial_length = true;
    g_textured_data = false;
	g_curv_dir = 0;
	g_curv_val = 0;

    g_ground_truth = 3;
    
    g_regular = false;
    g_radius_show = true;

	g_curvradius = 5.0;
    g_curvmin = -0.5;
    g_curvmax = 0.5;

    g_scale = 1;
    g_tessel = 2.0;
    g_isosurface = 0;

    g_sizetex = 0;
    
    g_time_elapsed = 0;
}

Parameters* Parameters::getInstance()
{
    if (instance == NULL)
        instance = new Parameters();
    return instance;
}
