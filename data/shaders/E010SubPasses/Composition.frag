#version 450

layout (input_attachment_index = 0, set = 2, binding = 0) uniform subpassInput inputPosition;
layout (input_attachment_index = 1, set = 2, binding = 1) uniform subpassInput inputColor;
layout (input_attachment_index = 2, set = 2, binding = 2) uniform subpassInput inputNormal;

layout(set=2, binding=3) uniform PerMaterial{
	int mode;
};

layout(location = 0) out vec4 o_Color;

void main() {
	if(mode == 0){
		vec3 color = subpassLoad(inputPosition).rgb;
		o_Color.rgb = color;
	}else if(mode == 1){
		vec3 color = subpassLoad(inputColor).rgb;
		o_Color.rgb = color;
	}else if(mode == 2){
		vec3 color = subpassLoad(inputNormal).rgb;
		o_Color.rgb = color;
	}
}
