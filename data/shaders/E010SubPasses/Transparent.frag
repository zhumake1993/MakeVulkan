#version 450

layout (input_attachment_index = 0, set = 2, binding = 0) uniform subpassInput inputPosition;

layout(set=2, binding=1) uniform sampler2D baseTexture;

layout (location = 0) in vec3 inColor;
layout (location = 1) in vec2 inUV;

layout (location = 0) out vec4 outColor;

#define NEAR_PLANE 0.1f
#define FAR_PLANE 256.0f

float linearDepth(float depth)
{
	float z = depth * 2.0f - 1.0f; 
	return (2.0f * NEAR_PLANE * FAR_PLANE) / (FAR_PLANE + NEAR_PLANE - z * (FAR_PLANE - NEAR_PLANE));	
}

void main() {

	// Sample depth from deferred depth buffer and discard if obscured
	float depth = subpassLoad(inputPosition).a;

	// Save the sampled texture color before discarding.
	// This is to avoid implicit derivatives in non-uniform control flow.
	vec4 sampledColor = texture(baseTexture, inUV);
	if ((depth != 0.0) && (linearDepth(gl_FragCoord.z) > depth))
	{
		discard;
	};

	outColor = sampledColor;
}
