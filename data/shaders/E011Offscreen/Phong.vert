#version 450

/////////////////////////////////////////////////////
// Uniform
/////////////////////////////////////////////////////

layout(set=1, binding=0) uniform PerView {
    mat4 view;
    mat4 proj;
};

layout(set=2, binding=0) uniform PerMaterial{
	vec2 clip;
};

layout(set=3, binding=0) uniform PerDraw {
    mat4 world;
};

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

out gl_PerVertex
{
	vec4 gl_Position;
	float gl_ClipDistance[];
};

layout (location = 0) out vec3 outWorldPos;
layout (location = 1) out vec3 outNormal;

void main() {
	gl_Position = proj * view * world * vec4(inPosition, 1.0f);
	
	// Vertex position in world space
	outWorldPos = vec3(world * vec4(inPosition, 1.0f));
	
	// Normal in world space
	mat3 mNormal = transpose(inverse(mat3(world)));
	outNormal = mNormal * normalize(inNormal);
	
	// Clip against plane
	vec4 clipPlane = vec4(0.0, -1.0, 0.0, clip.y);	
	gl_ClipDistance[0] = dot(vec4(outWorldPos, 1.0), clipPlane) * clip.x;
}
