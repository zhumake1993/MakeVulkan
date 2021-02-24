#version 450

/////////////////////////////////////////////////////
// Uniform
/////////////////////////////////////////////////////

layout(set=1, binding=0) uniform PerView {
    mat4 view;
    mat4 proj;
	vec4 eyePos;
	vec4 ambientLight;
	vec3 strength; // light color
	float pad;
	vec3 direction; // light direction
};

layout(set=2, binding=0) uniform sampler2D baseTexture;


/////////////////////////////////////////////////////
// Inpuut
/////////////////////////////////////////////////////

layout(location = 0) in vec3 v_Normal;
layout(location = 1) in vec2 v_Texcoord;
layout(location = 2) in vec4 v_wPos;

/////////////////////////////////////////////////////
// Output
/////////////////////////////////////////////////////

layout(location = 0) out vec4 o_Color;

/////////////////////////////////////////////////////
// Shader
/////////////////////////////////////////////////////

void main() {

	// Interpolating normal can unnormalize it, so renormalize it.
	vec3 N = normalize(v_Normal);
	
	// The vector from the surface to the light.
	vec3 L = normalize(-direction);
	
	// Vector from point being lit to eye.
	vec3 V = normalize(eyePos - v_wPos).xyz;
	
	vec3 R = reflect(-L, N);
	
	// Material
	vec4 color = texture(baseTexture, v_Texcoord);
	
	vec3 diffuse = max(dot(N, L), 0.0) * strength + ambientLight.xyz;
	float specular = pow(max(dot(R, V), 0.0), 16.0) * color.a;
	
	o_Color = vec4(diffuse * color.rgb + specular, color.a);
}