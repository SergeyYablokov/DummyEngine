#pragma once

#include <condition_variable>
#include <mutex>
#include <thread>

#include <Eng/ViewerBase.h>
#include <Eng/ViewerState.h>
#include <Eng/renderer/Renderer.h>
#include <Eng/scene/SceneData.h>
#include <Ray/Ray.h>
#include <Ren/Camera.h>
#include <Ren/MVec.h>
#include <Ren/Mesh.h>
#include <Ren/Program.h>
#include <Ren/SW/SW.h>
#include <Ren/Texture.h>
#include <Sys/SmallVector.h>

class FontStorage;
class Viewer;
namespace Eng {
class Cmdline;
class ViewerStateManager;
class ILog;
class PhysicsManager;
class SceneManager;
class ShaderLoader;
class DebugInfoUI;
} // namespace Eng

namespace Gui {
class BaseElement;
class BitmapFont;
class Image9Patch;
class Renderer;
} // namespace Gui

namespace Ray {
class RegionContext;
class RendererBase;
class SceneBase;
} // namespace Ray

namespace Snd {
class Context;
}

namespace Sys {
struct TaskList;
}

class GSBaseState : public Eng::ViewerState {
  protected:
    Viewer *viewer_;
    Eng::Cmdline *cmdline_ = nullptr;
    Ren::Context *ren_ctx_ = nullptr;
    Snd::Context *snd_ctx_ = nullptr;
    Eng::ILog *log_ = nullptr;
    Eng::Renderer *renderer_ = nullptr;
    Eng::SceneManager *scene_manager_ = nullptr;
    Eng::PhysicsManager *physics_manager_ = nullptr;
    Eng::Random *random_ = nullptr;
    Eng::ShaderLoader *shader_loader_ = nullptr;
    std::unique_ptr<Ray::RendererBase> ray_renderer_;
    std::unique_ptr<Ray::SceneBase> ray_scene_;
    std::vector<Sys::SmallVector<Ray::RegionContext, 128>> ray_reg_ctx_;
    Ray::unet_filter_properties_t unet_props_ = {};
    Sys::ThreadPool *threads_ = nullptr;
    std::unique_ptr<Sys::TaskList> render_tasks_, render_and_denoise_tasks_;
    std::unique_ptr<Sys::TaskList> update_cache_tasks_;

    Gui::Renderer *ui_renderer_ = nullptr;
    Gui::BaseElement *ui_root_ = nullptr;
    const Gui::BitmapFont *font_ = {};
    Eng::DebugInfoUI *debug_ui_ = nullptr;
    std::unique_ptr<Gui::Image9Patch> cmdline_back_;

    std::mutex mtx_;
    std::thread background_thread_;
    std::condition_variable thr_notify_, thr_done_;
    bool shutdown_ = false, notified_ = false;

    Eng::FrameInfo fr_info_;

    Ren::Camera temp_probe_cam_;
    // FrameBuf temp_probe_buf_;
    Eng::DrawList temp_probe_lists_[6];
    Eng::LightProbe *probe_to_render_ = nullptr, *probe_to_update_sh_ = nullptr;
    bool probes_dirty_ = true;
    int probe_sh_update_iteration_ = 0;
    std::vector<int> probes_to_update_;

    std::atomic_bool update_all_probes_{false};

    Ren::Vec3f sun_dir_ = Ren::Vec3f{0.0f, -1.0f, 0.0f}, prev_sun_dir_;
    Ray::LightHandle pt_sun_light_ = Ray::InvalidLightHandle;

    bool use_pt_ = false, use_lm_ = false;
    bool invalidate_view_ = true;

    Eng::DrawList main_view_lists_[2];
    int front_list_ = 0;

    Eng::FrontendInfo prev_front_info_;
    Eng::BackendInfo prev_back_info_;
    Eng::TimeInterval prev_swap_interval_;

    uint64_t last_frame_time_ = 0;
    double cur_fps_ = 0.0;

    std::vector<Eng::InputManager::Event> cmdline_input_;
    std::vector<std::string> cmdline_history_;
    int cmdline_history_index_ = -1;
    uint64_t cmdline_cursor_blink_us_ = 0;
    bool cmdline_enabled_ = false;
    bool ui_enabled_ = true;
    bool shift_down_ = false;

    bool streaming_finished_ = false;
    enum class eCaptureState { None, UpdateGICache, Warmup, Started } capture_state_ = eCaptureState::None;
    Ren::Tex2DRef capture_result_;

    bool LoadScene(std::string_view name);

    virtual void OnPreloadScene(JsObjectP &js_scene);
    virtual void OnPostloadScene(JsObjectP &js_scene);

    virtual void SaveScene(JsObjectP &js_scene);

    void BackgroundProc();
    void UpdateFrame(int list_index);

    virtual void DrawUI(Gui::Renderer *r, Gui::BaseElement *root);

    void InitRenderer_PT();
    void InitScene_PT();
    void SetupView_PT(const Ren::Vec3f &origin, const Ren::Vec3f &fwd, const Ren::Vec3f &up, float fov);
    void Clear_PT();
    void Draw_PT(const Ren::Tex2DRef &target);

    int WriteAndValidateCaptureResult();

  public:
    explicit GSBaseState(Viewer *viewer);
    ~GSBaseState() override;

    void Enter() override;
    void Exit() override;

    void Draw() override;

    void UpdateFixed(uint64_t dt_us) override;
    void UpdateAnim(uint64_t dt_us) override;

    bool HandleInput(const Eng::InputManager::Event &evt) override;
};