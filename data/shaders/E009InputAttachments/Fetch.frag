#version 450

layout (input_attachment_index = 0, set = 2, binding = 0) uniform subpassInput inputColor;
layout (input_attachment_index = 1, set = 2, binding = 1) uniform subpassInput inputDepth;

layout(set=2, binding=2) uniform PerMaterial{
	float brightness;
	float contrast;
	float depthMin;
	float depthMax;
	int depth;
};

layout(location = 0) out vec4 o_Color;

vec3 brightnessContrast(vec3 color, float brightness, float contrast) {
	return (color - 0.5) * contrast + 0.5 + brightness;
}

void main() {
	if(depth == 0){
		vec3 color = subpassLoad(inputColor).rgb;
		o_Color.rgb = brightnessContrast(color, brightness, contrast);
	}else{
		float depth = subpassLoad(inputDepth).r;
		o_Color.rgb = vec3((depth - depthMin) * 1.0 / (depthMax - depthMin));
	}
}
