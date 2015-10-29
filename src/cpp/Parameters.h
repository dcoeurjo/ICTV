/*
 * Copyright 2015 
 * Hélène Perrier <helene.perrier@liris.cnrs.fr>
 * Jérémy Levallois <jeremy.levallois@liris.cnrs.fr>
 * David Coeurjolly <david.coeurjolly@liris.cnrs.fr>
 * Jacques-Olivier Lachaud <jacques-olivier.lachaud@univ-savoie.fr>
 * Jean-Philippe Farrugia <jean-philippe.farrugia@liris.cnrs.fr>
 * Jean-Claude Iehl <jean-claude.iehl@liris.cnrs.fr>
 * 
 * This file is part of ICTV.
 * 
 * ICTV is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * ICTV is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with ICTV.  If not, see <http://www.gnu.org/licenses/>
 */


#ifndef PARAMETERS_H_
#define PARAMETERS_H_

#include "Vec.h"
#include "Transform.h"

#include "GLResource.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "Definitions.h"
#include "Utils.h"

#define SHADER_PATH(x) "../glsl/" x
#define CAPTURE_PATH(x) "./captures/" x
#define DATA_PATH(x) "../../data/" x
#define VIEWPOINT_FILE "vp.txt" 
#define QUATPOINT_FILE "qp.txt" 

class Parameters
{
private:
    static Parameters* instance;
    Parameters();
    
public:
    static Parameters* getInstance();

    window_t g_window;
    framebuffer_region_t g_framebuffer_region;
    capture_t  g_capture;
    geometry_t g_geometry;
    shading_t g_shading;
    camera_t g_camera;
    transforms_t g_transforms;

    float g_frustum[24];
    int g_mouse;

    GLuint g_programs[PROGRAM_COUNT],
           g_framebuffers[FRAMEBUFFER_COUNT],
           g_textures[TEXTURE_COUNT],
           g_vertex_arrays[VERTEX_ARRAY_COUNT],
           g_feedbacks[FEEDBACK_COUNT],
           g_buffers[BUFFER_COUNT],
           g_query[QUERY_COUNT];

    GLint  g_uniform_locations[LOCATION_COUNT];

    // flags
    bool g_solid_wireframe; //display the wireframe over the triangles
    bool g_draw_cells; //draw octree cells
    bool g_draw_triangles; //draw extracted triangles
    bool g_culling; //cull or not the octree nodes
    bool g_controls; //set the controls to first-person style
    //bool g_fromtexture; //set if the data is to be computed or read from a texture
    //bool g_textured_data; //color with the textures
    //bool g_radial_length; //use the LoD criteria with radial length
    
    bool g_radius_show; //show the radius ball
	bool g_triangle_normals; //are we using the triangle normals or the normal vector field
	
	bool g_adaptive_geom; //are we displaying the objet with a full res regular grid of with adaptive geometry
	bool g_auto_refine; //When using regular curvature computation, are we atomatically refining the computations or are we sticking to a chosen lvl
	
	bool g_compute_min_max; //to automatically compute the min max curvature
	bool g_export; //to export the data
    bool g_gui; //hides/show the gui
    
    float g_scale; //sets the size of the different levels
    float g_tessel; //tessellates the cell in N^3 smaller cells [UNUSED BECAUSE TOO SLOW]
    //float g_isosurface; //sets the isovalue for potential or density fields [UNUSED]
    float g_curvradius; //sets the radius of the curvature ball
    
    float g_curvmin; //the min curv value
    float g_curvmax; //the max curvature value
	
	float g_curv_dir; //shows 0.nothing 1.the first principal direction 2.the second principal direction 3.both first and second principal directions 4.the normal vector field
	float g_curv_val; //show 0.nothing 1.the mean curvature value 2.The gaussian curvature value 3.the first principal curvature 4.the second principal curvature
	
	float g_lvl; //for automatic refinement, to keep track of which is the current refinement lvl.
    int g_sizetex; //the resolution of the input dataset

    float g_ground_truth; //computed using a regular or hierachical grid
    
    float g_time_elapsed; //in seconds
    
    bool g_lightmode; //run in light mode or not
};

#endif
