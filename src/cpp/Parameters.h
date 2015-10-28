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
    bool g_fromtexture; //set if the data is to be computed or read from a texture
    bool g_textured_data; //color with the textures
    bool g_radial_length; //use the LoD criteria with radial length
    bool g_regular;
    bool g_radius_show;
	bool g_auto_refine;
	bool g_k1k2_normals;
	
	bool g_compute_min_max;
	bool g_export;

    bool g_gui; //hides/show the gui
    
    float g_scale; //sets the size of the different levels
    float g_tessel; //tessellates the cell in N^3 smaller cells
    float g_isosurface; //sets the isovalue for potential or density fields
    float g_curvradius;
    float g_curvmin;
    float g_curvmax;
	float g_curv_dir;
	float g_curv_val;
	float g_lvl;
    int g_sizetex;

    float g_ground_truth;
    
    float g_time_elapsed; //in seconds
    
    bool g_light;
};

#endif
