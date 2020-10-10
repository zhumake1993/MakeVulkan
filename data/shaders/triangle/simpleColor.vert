#version 450

layout(set=0, binding=0) uniform u_PassUniformBuffer {
    mat4 view;
    mat4 proj;
	vec4 lightPos;
};

layout(set=0, binding=1) uniform u_ObjectUniformBuffer {
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