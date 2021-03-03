#version 450

layout (input_attachment_index = 0, set = 2, binding = 0) uniform subpassInput inputColor;
layout (input_attachment_index = 1, set = 2, binding = 1) uniform subpassInput inputDepth;

layout(location = 0) out vec4 o_Color;

void main() {
  o_Color = subpassLoad(inputColor) + vec4(0.2f, 0.2f, 0.2f, 0.0f);
}
