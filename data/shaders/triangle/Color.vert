#version 450

/////////////////////////////////////////////////////
// Uniform
/////////////////////////////////////////////////////

layout(set=0, binding=0) uniform Global {
    vec4 time;
};

layout(set=1, binding=0) uniform PerView {
    mat4 view;
    mat4 proj;
	vec4 eyePos;
};

layout(set=3, binding=0) uniform PerDraw {
    mat4 world;
};

layout(location = 0) in vec4 i_Position;
layout(location = 1) in vec4 i_Color;

out gl_PerVertex
{
  vec4 gl_Position;
};

layout(location = 0) out vec4 v_Color;

void main() {
    gl_Position = proj * view * world * i_Position;
	v_Color = i_Color;
}
