#include "ExDebugRT.h"

#include <Ren/Context.h>
#include <Ren/Texture.h>

#include "../../utils/ShaderLoader.h"
#include "../Renderer_Structs.h"
#include "../shaders/rt_debug_interface.h"

void Eng::ExDebugRT::Execute(FgBuilder &builder) {
    LazyInit(builder.ctx(), builder.sh());

#if !defined(USE_GL_RENDER)
    if (builder.ctx().capabilities.hwrt) {
        Execute_HWRT(builder);
    } else
#endif
    {
        Execute_SWRT(builder);
    }
}

void Eng::ExDebugRT::LazyInit(Ren::Context &ctx, Eng::ShaderLoader &sh) {
    if (!initialized) {
#if defined(USE_VK_RENDER)
        if (ctx.capabilities.hwrt) {
            Ren::ProgramRef debug_hwrt_prog =
                sh.LoadProgram2(ctx, "internal/rt_debug.rgen.glsl", "internal/rt_debug.rchit.glsl@GI_CACHE",
                                "internal/rt_debug.rahit.glsl", "internal/rt_debug.rmiss.glsl", {});
            assert(debug_hwrt_prog->ready());

            if (!pi_debug_hwrt_.Init(ctx.api_ctx(), debug_hwrt_prog, ctx.log())) {
                ctx.log()->Error("ExDebugRT: Failed to initialize pipeline!");
            }
        }
#endif
        Ren::ProgramRef debug_swrt_prog = sh.LoadProgram(ctx, "internal/rt_debug_swrt.comp.glsl@GI_CACHE");
        assert(debug_swrt_prog->ready());

        if (!pi_debug_swrt_.Init(ctx.api_ctx(), debug_swrt_prog, ctx.log())) {
            ctx.log()->Error("ExDebugRT: Failed to initialize pipeline!");
        }

        initialized = true;
    }
}