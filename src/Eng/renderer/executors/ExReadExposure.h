#pragma once

#include "../Renderer_DrawList.h"
#include "../framegraph/FgNode.h"

namespace Eng {
class ExReadExposure : public FgExecutor {
  public:
    struct Args {
        FgResRef input_tex;
        FgResRef output_buf;
    };

    void Setup(const Args *args) { args_ = args; }
    void Execute(FgBuilder &builder) override;

    float exposure() const { return exposure_; }

  private:
    bool initialized_ = false;
    float exposure_ = 1.0f;

    // temp data (valid only between Setup and Execute calls)
    const Args *args_ = nullptr;
};
} // namespace Eng