#version 450

/////////////////////////////////////////////////////
// Uniform
/////////////////////////////////////////////////////

layout(set=1, binding=0) uniform PerView {
    mat4 view;
    mat4 proj;
	mat4 invView;
};

layout(set=2, binding=0) uniform PerMaterial {
    int textureIndex;
	float lodBias;
};

layout(set=2, binding=1) uniform samplerCubeArray baseTexture;


/////////////////////////////////////////////////////
// Inpuut
/////////////////////////////////////////////////////

layout(location = 0) in vec3 v_Normal;
layout(location = 1) in vec4 v_wPos;

/////////////////////////////////////////////////////
// Output
/////////////////////////////////////////////////////

layout(location = 0) out vec4 o_Color;

/////////////////////////////////////////////////////
// Shader
/////////////////////////////////////////////////////

void main() {

	vec3 cI = normalize (v_wPos.xyz);
	vec3 cR = reflect (cI, normalize(v_Normal));

	cR = vec3(invView * vec4(cR, 0.0));
	// Convert cubemap coordinates into Vulkan coordinate space
	// 这地方有点奇怪
	cR.y *= -1.0;

	o_Color = texture(baseTexture, vec4(cR, textureIndex), lodBias);
}