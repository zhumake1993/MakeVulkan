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

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;
layout(location = 3) in vec2 inUV;

out gl_PerVertex
{
  vec4 gl_Position;
};

layout (location = 0) out vec3 outColor;
layout (location = 1) out vec2 outUV;

void main() {
	gl_Position = proj * view * world * vec4(inPosition, 1.0f);
	
	outColor = inColor;
	outUV = inUV;
}
