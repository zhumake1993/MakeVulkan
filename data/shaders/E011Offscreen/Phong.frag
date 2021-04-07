#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

layout (location = 0) out vec4 outColor;

layout(set=1, binding=0) uniform PerView{
	mat4 view;
    mat4 proj;
	vec4 eyePos;
	
	vec4 ambientLight;
	vec4 strength; // light color
	vec4 direction; // light direction
};

void main() {

	// Interpolating normal can unnormalize it, so renormalize it.
	vec3 N = normalize(inNormal);
	
	// The vector from the surface to the light.
	vec3 L = normalize(-direction.xyz);
	
	// Vector from point being lit to eye.
	vec3 V = normalize(eyePos.xyz - inPosition);
	
	vec3 R = reflect(-L, N);
	
	vec3 diffuse = max(dot(N, L), 0.0) * strength.xyz + ambientLight.xyz;
	float specular = pow(max(dot(R, V), 0.0), 16.0);
	
	outColor = vec4(diffuse + specular, 1.0f);
}
