#version 310 es
#extension GL_ARB_texture_multisample : enable

#ifdef GL_ES
    precision mediump float;
#endif

#include "_fs_common.glsl"
    
layout(binding = REN_BASE0_TEX_SLOT) uniform mediump sampler2DMS s_texture;
layout(location = 4) uniform float multiplier;

in vec2 aVertexUVs_;

out vec4 outColor;

void main() {
    outColor = vec4(multiplier, multiplier, multiplier, 1.0) * texelFetch(s_texture, ivec2(aVertexUVs_), 0);
}
