#version 430 core
//#extension GL_EXT_control_flow_attributes : enable
#if !defined(VULKAN)
#extension GL_ARB_bindless_texture : enable
#endif

#include "../internal/_fs_common.glsl"
#include "../internal/texturing_common.glsl"

#define LIGHT_ATTEN_CUTOFF 0.004

#if !defined(BINDLESS_TEXTURES)
layout(binding = BIND_MAT_TEX0) uniform sampler2D g_mat0_tex;
#endif // BINDLESS_TEXTURES

layout (binding = BIND_UB_SHARED_DATA_BUF, std140) uniform SharedDataBlock {
    SharedData g_shrd_data;
};

layout(location = 4) in vec2 g_vtx_uvs0;
#if defined(BINDLESS_TEXTURES)
layout(location = 8) in flat TEX_HANDLE g_mat0_tex;
#endif // BINDLESS_TEXTURES

layout(location = LOC_OUT_COLOR) out vec4 g_out_color;
layout(location = LOC_OUT_NORM) out vec4 g_out_normal;

void main() {
    vec4 col = texture(SAMPLER2D(g_mat0_tex), g_vtx_uvs0);

    float scale = (col.b * (255.0 / 8.0)) + 1.0;
    float Y = col.a;
    float Co = (col.r - (0.5 * 256.0 / 255.0)) / scale;
    float Cg = (col.g - (0.5 * 256.0 / 255.0)) / scale;

    vec3 col_rgb;
    col_rgb.r = Y + Co - Cg;
    col_rgb.g = Y + Cg;
    col_rgb.b = Y - Co - Cg;

    g_out_color = vec4(SRGBToLinear(col_rgb), 1.0);
    g_out_normal = vec4(0.0);
}