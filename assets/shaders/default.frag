#version 450

layout(location = 0) out vec4 outFragColor;

layout(binding = 0) uniform UniformBufferObject {
    vec3 resolution;
    float time;
} ubo;

void main() {
    vec2 uv = (gl_FragCoord.xy * 2.0 - ubo.resolution.xy) / ubo.resolution.y;
    outFragColor = vec4(uv, 1.0, 1.0);
}