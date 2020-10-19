#version 450

layout(set=0, binding=0) uniform u_PassUniformBuffer {
    mat4 view;
    mat4 proj;
	vec4 eyePos;
	vec4 lightPos;
};

layout(set=0, binding=1) uniform u_ObjectUniformBuffer {
    mat4 world;
};

layout(location = 0) in vec4 i_Position;
layout(location = 1) in vec4 i_Normal;
layout(location = 2) in vec4 i_Color;
layout(location = 3) in vec2 i_Texcoord;

out gl_PerVertex
{
  vec4 gl_Position;
};

layout(location = 0) out vec4 v_Normal;
layout(location = 1) out vec2 v_Texcoord;
layout(location = 2) out vec4 v_wPos;

void main() {
    gl_Position = proj * view * world * i_Position;

	v_Normal = i_Normal;
    v_Texcoord = i_Texcoord;
	v_wPos = world * i_Position;
}
