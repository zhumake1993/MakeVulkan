#version 450

layout(set=0, binding=0) uniform u_PassUniformBuffer {
    mat4 view;
    mat4 proj;
	vec4 eyePos;
	vec4 lightPos;
};

layout(set=0, binding=2) uniform sampler2D u_Texture;

layout(location = 0) in vec4 v_Normal;
layout(location = 1) in vec2 v_Texcoord;
layout(location = 2) in vec4 v_wPos;

layout(location = 0) out vec4 o_Color;

void main() {
	vec4 ambient = texture( u_Texture, v_Texcoord );
	vec4 N = normalize(v_Normal);
	vec4 L = normalize(lightPos - v_wPos);
	vec4 V = normalize(eyePos - v_wPos);
	vec4 R = reflect(-L, N);
	vec4 diffuse = max(dot(N, L), 0.0) * ambient;
	vec4 specular = pow(max(dot(R, V), 0.0), 32.0) * vec4(0.35);
	o_Color = ambient + diffuse * 1.75 + specular;	
}