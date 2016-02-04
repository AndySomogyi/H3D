#version 150 compatibility

layout(triangles) in;
layout(triangle_strip, max_vertices = 96) out;

void main() {
	for(int i = 0; i < 32; i++) {
		gl_Position= gl_in[0].gl_Position;
		gl_Layer = i;
		EmitVertex();

		gl_Position= gl_in[1].gl_Position;
		gl_Layer = i;
		EmitVertex();

		gl_Position= gl_in[2].gl_Position;
		gl_Layer = i;
		EmitVertex();
	}
}
