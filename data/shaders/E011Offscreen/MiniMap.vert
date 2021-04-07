#version 450

out gl_PerVertex
{
  vec4 gl_Position;
};

vec2 positions[6] = vec2[](
    vec2(0.0, 1.0),
    vec2(0.0, 0.0),
    vec2(1.0, 0.0),
	vec2(0.0, 1.0),
    vec2(1.0, 0.0),
    vec2(1.0, 1.0)
);

layout (location = 0) out vec2 outUV;

void main() {
	outUV = positions[gl_VertexIndex];
    gl_Position = vec4(outUV, 0.0, 1.0);
}
