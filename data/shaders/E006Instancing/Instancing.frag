#version 450

layout(set=2, binding=0) uniform sampler2D baseTexture;

layout(location = 0) in vec2 v_Texcoord;

layout(location = 0) out vec4 o_Color;

void main() {
  o_Color = texture(baseTexture, v_Texcoord);
}
