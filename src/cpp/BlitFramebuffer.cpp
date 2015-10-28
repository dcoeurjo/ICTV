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


#include "BlitFramebuffer.h"

#include "GL/GLQuery.h"
#include "GL/GLTexture.h"
#include "GL/GLVertexArray.h"
#include "GL/GLBuffer.h"
#include "ProgramManager.h"
#include "Format.h"

void updateFramebufferRegion()
{
	float regionw = 0.5f * Parameters::getInstance()->g_window.width / Parameters::getInstance()->g_framebuffer_region.mag;
        float regionh = 0.5f * Parameters::getInstance()->g_window.height / Parameters::getInstance()->g_framebuffer_region.mag;
        gk::TVec2<float> min(regionw, regionh);
        gk::TVec2<float> max(Parameters::getInstance()->g_window.width - regionw, Parameters::getInstance()->g_window.height - regionh);
        Parameters::getInstance()->g_framebuffer_region.p[0] = std::max(Parameters::getInstance()->g_framebuffer_region.p[0], min[0]);
        Parameters::getInstance()->g_framebuffer_region.p[1] = std::max(Parameters::getInstance()->g_framebuffer_region.p[1], min[1]);
        Parameters::getInstance()->g_framebuffer_region.p[0] = std::min(Parameters::getInstance()->g_framebuffer_region.p[0], max[0]);
        Parameters::getInstance()->g_framebuffer_region.p[1] = std::min(Parameters::getInstance()->g_framebuffer_region.p[1], max[1]);
        glProgramUniform3f (Parameters::getInstance()->g_programs[PROGRAM_FRAMEBUFFER_BLIT],
                            Parameters::getInstance()->g_uniform_locations[LOCATION_FRAMEBUFFER_BLIT_VIEWPORT],
                            Parameters::getInstance()->g_framebuffer_region.p[0] - regionw,
                            Parameters::getInstance()->g_framebuffer_region.p[1] - regionh,
                            Parameters::getInstance()->g_framebuffer_region.mag);
}

void BlitFramebuffer::loadFramebuffersTextures()
{
	fprintf (stderr, "Loading framebuffer textures... "); fflush (stderr);
        if (glIsTexture (Parameters::getInstance()->g_textures[TEXTURE_Z]))
                glDeleteTextures (1, &Parameters::getInstance()->g_textures[TEXTURE_Z]);
        if (glIsTexture (Parameters::getInstance()->g_textures[TEXTURE_RGBA]))
                glDeleteTextures (1, &Parameters::getInstance()->g_textures[TEXTURE_RGBA]);
        glGenTextures (1, &Parameters::getInstance()->g_textures[TEXTURE_Z]);
        glGenTextures (1, &Parameters::getInstance()->g_textures[TEXTURE_RGBA]);

        // TODO use ARB_texture_storage_multisample asap
        if (Parameters::getInstance()->g_window.msaa_factor > 0) {
                glBindTexture (GL_TEXTURE_2D_MULTISAMPLE, Parameters::getInstance()->g_textures[TEXTURE_Z]);
                glTexImage2DMultisample (GL_TEXTURE_2D_MULTISAMPLE,
                                         Parameters::getInstance()->g_window.msaa_factor,
                                         GL_DEPTH24_STENCIL8,
                                         Parameters::getInstance()->g_window.width, Parameters::getInstance()->g_window.height,
                                         Parameters::getInstance()->g_window.msaa_fixedsamplelocations);

                glBindTexture (GL_TEXTURE_2D_MULTISAMPLE, Parameters::getInstance()->g_textures[TEXTURE_RGBA]);
                glTexImage2DMultisample (GL_TEXTURE_2D_MULTISAMPLE,
                                         Parameters::getInstance()->g_window.msaa_factor,
                                         GL_RGBA8,
                                         Parameters::getInstance()->g_window.width, Parameters::getInstance()->g_window.height,
                                         Parameters::getInstance()->g_window.msaa_fixedsamplelocations);
        } else {
                glBindTexture (GL_TEXTURE_2D, Parameters::getInstance()->g_textures[TEXTURE_Z]);
                glTexStorage2D (GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8,
                                Parameters::getInstance()->g_window.width, Parameters::getInstance()->g_window.height);

                glBindTexture (GL_TEXTURE_2D, Parameters::getInstance()->g_textures[TEXTURE_RGBA]);
                glTexStorage2D (GL_TEXTURE_2D, 1, GL_RGBA8,
                                Parameters::getInstance()->g_window.width, Parameters::getInstance()->g_window.height);
        }
        fprintf (stderr, "Success\n");
}


void BlitFramebuffer::loadFramebuffers()
{
	fprintf (stderr, "Loading framebuffers... "); fflush (stderr);
        if (glIsFramebuffer (Parameters::getInstance()->g_framebuffers[FRAMEBUFFER_DEFAULT]))
                glDeleteFramebuffers(1, &Parameters::getInstance()->g_framebuffers[FRAMEBUFFER_DEFAULT]);

        glGenFramebuffers (1, &Parameters::getInstance()->g_framebuffers[FRAMEBUFFER_DEFAULT]);
        glBindFramebuffer (GL_FRAMEBUFFER, Parameters::getInstance()->g_framebuffers[FRAMEBUFFER_DEFAULT]);
        glFramebufferTexture2D (GL_FRAMEBUFFER,
                                GL_COLOR_ATTACHMENT0,
                                Parameters::getInstance()->g_window.msaa_factor > 0 ? GL_TEXTURE_2D_MULTISAMPLE
                                                         : GL_TEXTURE_2D,
                                Parameters::getInstance()->g_textures[TEXTURE_RGBA],
                                0);
        glFramebufferTexture2D (GL_FRAMEBUFFER,
                                GL_DEPTH_STENCIL_ATTACHMENT,
                                Parameters::getInstance()->g_window.msaa_factor > 0 ? GL_TEXTURE_2D_MULTISAMPLE
                                                         : GL_TEXTURE_2D,
                                Parameters::getInstance()->g_textures[TEXTURE_Z],
                                0);
        glDrawBuffer (GL_COLOR_ATTACHMENT0);
	
        if (GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus (GL_FRAMEBUFFER)) {
                fprintf (stderr, "Failure\n");
		exit(0);
        }
        
        glBindFramebuffer (GL_FRAMEBUFFER, 0);
        fprintf (stderr, "Success\n");
}


void BlitFramebuffer::loadProgram()
{
	fprintf (stderr, "Loading framebuffer blit program... "); fflush (stderr);
	
        gk::GLCompiler& c = gk::loadProgram(SHADER_PATH("framebuffer_blit.glsl"));
        c.defineVertex("MSAA_FACTOR", Format("%i", Parameters::getInstance()->g_window.msaa_factor).text);
        c.defineFragment("MSAA_FACTOR", Format("%i", Parameters::getInstance()->g_window.msaa_factor).text);

        GLProgram* tmp = c.make();
        if (tmp->errors)
            exit(-1);

        Parameters::getInstance()->g_programs[PROGRAM_FRAMEBUFFER_BLIT] = tmp->name;

        Parameters::getInstance()->g_uniform_locations[LOCATION_FRAMEBUFFER_BLIT_VIEWPORT] =
                glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_FRAMEBUFFER_BLIT], "u_viewport");
        Parameters::getInstance()->g_uniform_locations[LOCATION_FRAMEBUFFER_BLIT_SAMPLER] =
                glGetUniformLocation (Parameters::getInstance()->g_programs[PROGRAM_FRAMEBUFFER_BLIT], "u_framebuffer_sampler");

        glProgramUniform1i (Parameters::getInstance()->g_programs[PROGRAM_FRAMEBUFFER_BLIT],
                            Parameters::getInstance()->g_uniform_locations[LOCATION_FRAMEBUFFER_BLIT_SAMPLER],
                            TEXTURE_RGBA);
        
	updateFramebufferRegion();
	
        fprintf (stderr, "Success\n");
}


void BlitFramebuffer::blit()
{
	/** Blit the framebuffer on screen (done manually to allow MSAA) **/
	glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_DEPTH_TEST);
	    
	glBindFramebuffer (GL_READ_FRAMEBUFFER, Parameters::getInstance()->g_framebuffers[FRAMEBUFFER_DEFAULT]);
	glBindFramebuffer (GL_DRAW_FRAMEBUFFER, 0);
	glViewport        (0, 0, Parameters::getInstance()->g_window.width, Parameters::getInstance()->g_window.height);
	glUseProgram      (Parameters::getInstance()->g_programs[PROGRAM_FRAMEBUFFER_BLIT]);
	glBindVertexArray (Parameters::getInstance()->g_vertex_arrays[VERTEX_ARRAY_EMPTY]);
	glDrawArrays      (GL_TRIANGLE_STRIP, 0, 4);
	glBindFramebuffer (GL_READ_FRAMEBUFFER, 0);
	glBindVertexArray (0);
}


