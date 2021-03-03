#version 450

/////////////////////////////////////////////////////
// Uniform
/////////////////////////////////////////////////////

layout(set=1, binding=0) uniform PerView {
    mat4 view;
    mat4 proj;
};

layout(set=3, binding=0) uniform PerDraw {
    mat4 world;
};

layout(location = 0) in vec3 i_Position;
layout(location = 1) in vec3 i_Color;

out gl_PerVertex
{
  vec4 gl_Position;
};

layout(location = 0) out vec3 v_Color;

void main() {
    gl_Position = proj * view * world * vec4(i_Position, 1.0f);
	v_Color = i_Color;
}
