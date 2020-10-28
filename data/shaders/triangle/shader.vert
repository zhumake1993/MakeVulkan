#version 450

/////////////////////////////////////////////////////
// Light
/////////////////////////////////////////////////////

#define MaxLights 16
#define NUM_DIR_LIGHTS 1
#define NUM_POINT_LIGHTS 1
#define NUM_SPOT_LIGHTS 0

struct Light
{
	vec3 strength; // light color
	float falloffStart; // point/spot light only
	vec3 direction;// directional/spot lightonly
	float falloffEnd; // point/spot light only
	vec3 position; // point/spot light only
	float spotPower; // spot light only
};

/////////////////////////////////////////////////////
// Uniform
/////////////////////////////////////////////////////

layout(set=0, binding=0) uniform u_PassUniformBuffer {
    mat4 view;
    mat4 proj;
	vec4 eyePos;
	
	vec4 ambientLight;
	Light lights[MaxLights];
};

layout(set=1, binding=0) uniform u_ObjectUniformBuffer {
    mat4 world;
};

layout(location = 0) in mediump vec3 i_Position;
layout(location = 1) in vec3 i_Normal;
layout(location = 2) in vec2 i_Texcoord;

out gl_PerVertex
{
  vec4 gl_Position;
};

layout(location = 0) out vec3 v_Normal;
layout(location = 1) out vec2 v_Texcoord;
layout(location = 2) out vec4 v_wPos;

/////////////////////////////////////////////////////
// Shader
/////////////////////////////////////////////////////

void main() {

	// Transform to world space.
	v_wPos = world * vec4(i_Position, 1.0f);
	
	// Assumes uniform scaling; otherwise, need to use inverse-transpose of world matrix.
	v_Normal = (world * vec4(i_Normal, 0.0f)).xyz;
	
	v_Texcoord = i_Texcoord;

	// Transform to homogeneous clip space.
    gl_Position = proj * view * v_wPos;
}
