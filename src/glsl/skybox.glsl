#version 410

// ---------------------------------------------------------
// Vertex shader

#ifdef VERTEX_SHADER

out vec2 tex_coord;

void main (void) {
	vec2 p = vec2 (gl_VertexID & 1, gl_VertexID >> 1 & 1);
        gl_Position = vec4 (p * 2.0 - 1.0, 0.0, 1.0);
        tex_coord = p;
}
#endif

// ---------------------------------------------------------
// Fragment shader

#ifdef FRAGMENT_SHADER
layout(location = 0) out vec4 o_colour;
in vec2 tex_coord;

uniform sampler2D u_texture;

void main (void) {
	o_colour = vec4(texture(u_texture, tex_coord).rgb, 1.0);
}
#endif // FRAGMENT_SHADER

