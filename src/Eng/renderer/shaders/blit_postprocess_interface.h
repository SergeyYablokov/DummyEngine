#ifndef BLIT_COMBINE_INTERFACE_H
#define BLIT_COMBINE_INTERFACE_H

#include "_interface_common.h"

INTERFACE_START(BlitCombine)

struct Params {
    vec4 transform;
    vec2 tex_size;
    float tonemap_mode;
    float inv_gamma;
    float aberration;
    float fade;
};

const int HDR_TEX_SLOT = 0;
const int BLURED_TEX_SLOT = 1;
const int EXPOSURE_TEX_SLOT = 2;
const int LUT_TEX_SLOT = 3;

INTERFACE_END

#endif // BLIT_COMBINE_INTERFACE_H