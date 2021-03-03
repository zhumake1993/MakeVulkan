#version 450

out gl_PerVertex
{
  vec4 gl_Position;
};

const vec4 corners[3] = {{-1.0f, -1.0f, 0.0f, 1.0f}, {3.0f, -1.0f, 0.0f, 1.0f}, {-1.0f, 3.0f, 0.0f, 1.0f}};

void main() {
    gl_Position = corners[gl_VertexIndex / 2]; // gl_VertexIndex的值是0，2，4
}
