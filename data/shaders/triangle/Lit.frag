#version 450

/////////////////////////////////////////////////////
// Light
/////////////////////////////////////////////////////

#define MaxLights 16

// 会导致pso的创建非常耗
layout (constant_id = 0) const int NUM_DIR_LIGHTS = 1;
layout (constant_id = 1) const int NUM_POINT_LIGHTS = 1;
layout (constant_id = 2) const int NUM_SPOT_LIGHTS = 1;

//
//#define NUM_DIR_LIGHTS 1
//#define NUM_POINT_LIGHTS 0
//#define NUM_SPOT_LIGHTS 0

struct Light
{
	vec3 strength; // light color
	float falloffStart; // point/spot light only
	vec3 direction;// directional/spot lightonly
	float falloffEnd; // point/spot light only
	vec3 position; // point/spot light only
	float spotPower; // spot light only
};

struct Material
{
	vec4 diffuseAlbedo;
	vec3 fresnelR0;
	float roughness;
	mat4 matTransform;
};

float CalcAttenuation(float d, float falloffStart, float falloffEnd)
{
	// Linear falloff.
	return clamp((falloffEnd - d) / (falloffEnd - falloffStart), 0.0f, 1.0f);
}

// Schlick gives an approximation to Fresne reflectance
// (see pg. 233 “Real-Time Rendering 3rd Ed.”).
// R0 = ( (n-1)/(n+1) )^2, where n is the index of refraction.
vec3 SchlickFresnel(vec3 R0, vec3 normal, vec3 lightVec)
{
	float cosIncidentAngle = clamp(dot(normal, lightVec), 0.0f, 1.0f);
	float f0 = 1.0f - cosIncidentAngle;
	vec3 reflectPercent = R0 + (1.0f - R0)* (f0*f0*f0*f0*f0);
	return reflectPercent;
}

vec3 BlinnPhong(vec3 lightStrength, vec3 lightVec, vec3 normal, vec3 toEye, Material mat)
{
	// Derive m from the shininess, which is derived from the roughness.
	const float shininess = 1.0f - mat.roughness;
	const float m = shininess * 256.0f;
	vec3 halfVec = normalize(toEye + lightVec);
	
	float roughnessFactor = (m + 8.0f)*pow(max(dot(halfVec, normal), 0.0f), m) / 8.0f;
	vec3 fresnelFactor = SchlickFresnel(mat.fresnelR0, halfVec, lightVec);
	
	vec3 specAlbedo = fresnelFactor*roughnessFactor;
	
	// Our spec formula goes outside [0,1] range, but we are doing
	// LDR rendering. So scale it down a bit.
	specAlbedo = specAlbedo / (specAlbedo + 1.0f);
	return (mat.diffuseAlbedo.rgb + specAlbedo) * lightStrength;
}

vec3 ComputeDirectionalLight(Light L, Material mat, vec3 normal, vec3 toEye)
{
	// The light vector aims opposite the direction the light rays travel.
	vec3 lightVec = -L.direction;
	
	// Scale light down by Lambert’s cosine law.
	float ndotl = max(dot(lightVec, normal), 0.0f);
	vec3 lightStrength = L.strength * ndotl;
	return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
}

vec3 ComputePointLight(Light L, Material mat, vec3 pos, vec3 normal, vec3 toEye)
{
	// The vector from the surface to the light.
	vec3 lightVec = L.position - pos;
	
	// The distance from surface to light.
	float d = length(lightVec);
	
	// Range test.
	if(d > L.falloffEnd)
	return vec3(0.0f, 0.0f, 0.0f);
	
	// Normalize the light vector.
	lightVec /= d;
	
	// Scale light down by Lambert’s cosine law.
	float ndotl = max(dot(lightVec, normal), 0.0f);
	vec3 lightStrength = L.strength * ndotl;
	
	// Attenuate light by distance.
	float att = CalcAttenuation(d, L.falloffStart, L.falloffEnd);
	lightStrength *= att;
	
	return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
}

vec3 ComputeSpotLight(Light L, Material mat, vec3 pos, vec3 normal, vec3 toEye)
{
	// The vector from the surface to the light.
	vec3 lightVec = L.position - pos;
	
	// The distance from surface to light.
	float d = length(lightVec);
	
	// Range test.
	if(d > L.falloffEnd)
	return vec3(0.0f, 0.0f, 0.0f);
	
	// Normalize the light vector.
	lightVec /= d;
	
	// Scale light down by Lambert’s cosine law.
	float ndotl = max(dot(lightVec, normal), 0.0f);
	vec3 lightStrength = L.strength * ndotl;
	
	// Attenuate light by distance.
	float att = CalcAttenuation(d, L.falloffStart, L.falloffEnd);
	lightStrength *= att;
	
	// Scale by spotlight
	float spotFactor = pow(max(dot(-lightVec, L.direction), 0.0f), L.spotPower);
	lightStrength *= spotFactor;
	
	return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
}

vec4 ComputeLighting(Light gLights[MaxLights], Material mat, vec3 pos, vec3 normal, vec3 toEye, vec3 shadowFactor)
{
	vec3 result = vec3(0.0f, 0.0f, 0.0f);
	int i = 0;


	for(i = 0; i < NUM_DIR_LIGHTS; ++i)
	{
		result += shadowFactor[i] * ComputeDirectionalLight(gLights[i], mat, normal, toEye);
	}

	for(i = NUM_DIR_LIGHTS; i < NUM_DIR_LIGHTS+NUM_POINT_LIGHTS; ++i)
	{
		result += ComputePointLight(gLights[i], mat, pos, normal, toEye);
	}

	for(i = NUM_DIR_LIGHTS + NUM_POINT_LIGHTS; i < NUM_DIR_LIGHTS + NUM_POINT_LIGHTS + NUM_SPOT_LIGHTS; ++i)
	{
		result += ComputeSpotLight(gLights[i], mat, pos, normal, toEye);
	}

	return vec4(result, 0.0f);
}

/////////////////////////////////////////////////////
// Uniform
/////////////////////////////////////////////////////

layout(set=1, binding=0) uniform PerView {
    mat4 view;
    mat4 proj;
	vec4 eyePos;
	
	vec4 ambientLight;
	// Indices [0, NUM_DIR_LIGHTS) are directional lights;
	// indices [NUM_DIR_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHTS) are
	// point lights;
	// indices [NUM_DIR_LIGHTS+NUM_POINT_LIGHTS,
	// NUM_DIR_LIGHTS+NUM_POINT_LIGHT+NUM_SPOT_LIGHTS)
	// are spot lights for a maximum of MaxLights per object.
	Light lights[MaxLights];
};

layout(set=2, binding=0) uniform PerMaterial{
	Material gMaterial;
};

layout(set=2, binding=1) uniform sampler2D baseTexture;

layout(location = 0) in vec3 v_Normal;
layout(location = 1) in vec2 v_Texcoord;
layout(location = 2) in vec4 v_wPos;

layout(location = 0) out vec4 o_Color;

/////////////////////////////////////////////////////
// Shader
/////////////////////////////////////////////////////

void main() {

	// Interpolating normal can unnormalize it, so renormalize it.
	vec3 normal = normalize(v_Normal);
	
	// Vector from point being lit to eye.
	vec4 toEyeW = normalize(eyePos - v_wPos);
	
	// Material
	Material mat = gMaterial;
	mat.diffuseAlbedo = mat.diffuseAlbedo * texture(baseTexture, v_Texcoord);
	
	// Indirect lighting.
	vec4 ambient = ambientLight * mat.diffuseAlbedo;
	
	// Direct lighting.
	vec3 shadowFactor = vec3(1.0f, 1.0f, 1.0f);
	vec4 directLight = ComputeLighting(lights, mat, v_wPos.xyz, normal, toEyeW.xyz, shadowFactor);
	vec4 litColor = ambient + directLight;
	
	// Common convention to take alpha from diffuse material.
	litColor.a = mat.diffuseAlbedo.a;

	o_Color = litColor;
}