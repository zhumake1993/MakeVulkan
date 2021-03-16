#version 450

#define MAX_NUM_LIGHTS 128

layout (constant_id = 0) const int NUM_LIGHTS = 0;

layout (input_attachment_index = 0, set = 2, binding = 0) uniform subpassInput inputPosition;
layout (input_attachment_index = 1, set = 2, binding = 1) uniform subpassInput inputNormal;
layout (input_attachment_index = 2, set = 2, binding = 2) uniform subpassInput inputColor;

struct Light {
	vec4 position;
	vec3 color;
	float radius;
};

layout(set=1, binding=0) uniform PerView{
	mat4 view;
    mat4 proj;
	vec4 eyePos;
	
	vec4 ambient;
	Light lights[MAX_NUM_LIGHTS];
};

layout(location = 0) out vec4 outColor;

void main() {

	// Read G-Buffer values from previous sub pass
	vec3 fragPos = subpassLoad(inputPosition).rgb;
	vec3 normal = subpassLoad(inputNormal).rgb;
	vec4 albedo = subpassLoad(inputColor);
	
	// Ambient part
	vec3 fragcolor  = albedo.rgb * ambient.xyz;
	
	for(int i = 0; i < NUM_LIGHTS; ++i)
	{
		// Vector to light
		vec3 L = lights[i].position.xyz - fragPos;
		// Distance from light to fragment position
		float dist = length(L);

		// Viewer to fragment
		vec3 V = eyePos.xyz - fragPos;
		V = normalize(V);
		
		// Light to fragment
		L = normalize(L);

		// Attenuation
		float atten = lights[i].radius / (pow(dist, 2.0) + 1.0);

		// Diffuse part
		vec3 N = normalize(normal);
		float NdotL = max(0.0, dot(N, L));
		vec3 diff = lights[i].color * albedo.rgb * NdotL * atten;

		// Specular part
		// Specular map values are stored in alpha of albedo mrt
		vec3 R = reflect(-L, N);
		float NdotR = max(0.0, dot(R, V));
		//vec3 spec = lights[i].color * albedo.a * pow(NdotR, 32.0) * atten;

		fragcolor += diff;// + spec;	
	}    	
   
	outColor = vec4(fragcolor, 1.0);
}
