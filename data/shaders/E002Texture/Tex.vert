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
};

layout(set=3, binding=0) uniform PerDraw {
    mat4 world;
};

layout(location = 0) in vec3 i_Position;
layout(location = 1) in vec2 i_Texcoord;

out gl_PerVertex
{
  vec4 gl_Position;
};

layout(location = 0) out vec2 v_Texcoord;

void main() {
    gl_Position = proj * view * world * vec4(i_Position, 1.0f);
	v_Texcoord = i_Texcoord;
}
