#version 450

/////////////////////////////////////////////////////
// Uniform
/////////////////////////////////////////////////////

layout(set=2, binding=0) uniform samplerCube baseTexture;


/////////////////////////////////////////////////////
// Inpuut
/////////////////////////////////////////////////////

layout(location = 0) in vec3 inUVW;

/////////////////////////////////////////////////////
// Output
/////////////////////////////////////////////////////

layout(location = 0) out vec4 o_Color;

/////////////////////////////////////////////////////
// Shader
/////////////////////////////////////////////////////

void main() {
	
	o_Color = texture(baseTexture, inUVW);
}