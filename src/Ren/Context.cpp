#include "Context.h"

#include <algorithm>

Ren::MeshRef Ren::Context::LoadMesh(const char *name, std::istream *data,
                                    const material_load_callback &on_mat_load,
                                    eMeshLoadStatus *load_status) {
    return LoadMesh(name, data, on_mat_load, default_vertex_buf1_, default_vertex_buf2_,
                    default_indices_buf_, default_skin_vertex_buf_, default_delta_buf_,
                    load_status);
}

Ren::MeshRef Ren::Context::LoadMesh(const char *name, std::istream *data,
                                    const material_load_callback &on_mat_load,
                                    BufferRef &vertex_buf1, BufferRef &vertex_buf2,
                                    BufferRef &index_buf, BufferRef &skin_vertex_buf,
                                    BufferRef &delta_buf, eMeshLoadStatus *load_status) {
    MeshRef ref = meshes_.FindByName(name);
    if (!ref) {
        ref = meshes_.Add(name, data, on_mat_load, vertex_buf1, vertex_buf2, index_buf,
                          skin_vertex_buf, delta_buf, load_status, log_);
    } else {
        if (ref->ready()) {
            if (load_status)
                *load_status = MeshFound;
        } else if (data) {
            ref->Init(data, on_mat_load, vertex_buf1, vertex_buf2, index_buf,
                      skin_vertex_buf, delta_buf, load_status, log_);
        }
    }

    return ref;
}

Ren::MaterialRef Ren::Context::LoadMaterial(const char *name, const char *mat_src,
                                            eMatLoadStatus *status,
                                            const program_load_callback &on_prog_load,
                                            const texture_load_callback &on_tex_load) {
    MaterialRef ref = materials_.FindByName(name);
    if (!ref) {
        ref = materials_.Add(name, mat_src, status, on_prog_load, on_tex_load, log_);
    } else {
        if (ref->ready()) {
            if (status)
                *status = MatFound;
        } else if (!ref->ready() && mat_src) {
            ref->Init(mat_src, status, on_prog_load, on_tex_load, log_);
        }
    }

    return ref;
}

Ren::MaterialRef Ren::Context::GetMaterial(uint32_t index) {
    return {&materials_, index};
}

int Ren::Context::NumMaterialsNotReady() {
    return (int)std::count_if(materials_.begin(), materials_.end(),
                              [](const Material &m) { return !m.ready(); });
}

void Ren::Context::ReleaseMaterials() {
    if (!materials_.size())
        return;
    log_->Error("---------REMAINING MATERIALS--------");
    for (const Material &m : materials_) {
        log_->Error("%s", m.name().c_str());
    }
    log_->Error("-----------------------------------");
    materials_.clear();
}

Ren::ProgramRef Ren::Context::GetProgram(uint32_t index) { return {&programs_, index}; }

int Ren::Context::NumProgramsNotReady() {
    return (int)std::count_if(programs_.begin(), programs_.end(),
                              [](const Program &p) { return !p.ready(); });
}

void Ren::Context::ReleasePrograms() {
    if (!programs_.size())
        return;
    log_->Error("---------REMAINING PROGRAMS--------");
    for (const Program &p : programs_) {
#if defined(USE_GL_RENDER) || defined(USE_SW_RENDER)
        log_->Error("%s %i", p.name().c_str(), (int)p.prog_id());
#endif
    }
    log_->Error("-----------------------------------");
    programs_.clear();
}

Ren::Texture2DRef Ren::Context::LoadTexture2D(const char *name, const void *data,
                                              int size, const Texture2DParams &p,
                                              eTexLoadStatus *load_status) {
    Texture2DRef ref = textures_.FindByName(name);
    if (!ref) {
        ref = textures_.Add(name, data, size, p, load_status, log_);
    } else {
        if (load_status) {
            *load_status = eTexLoadStatus::TexFound;
        }
        if (!ref->ready() && data) {
            ref->Init(data, size, p, load_status, log_);
        }
    }

    return ref;
}

Ren::Texture2DRef Ren::Context::LoadTextureCube(const char *name, const void *data[6],
                                                const int size[6],
                                                const Texture2DParams &p,
                                                eTexLoadStatus *load_status) {
    Texture2DRef ref = textures_.FindByName(name);
    if (!ref) {
        ref = textures_.Add(name, data, size, p, load_status, log_);
    } else {
        if (ref->ready()) {
            if (load_status)
                *load_status = eTexLoadStatus::TexFound;
        } else if (data) {
            ref->Init(data, size, p, load_status, log_);
        }
    }

    return ref;
}

void Ren::Context::VisitTextures(uint32_t mask,
                                 const std::function<void(Texture2D &tex)> &callback) {
    for (Texture2D &tex : textures_) {
        if (tex.params().flags & mask) {
            callback(tex);
        }
    }
}

int Ren::Context::NumTexturesNotReady() {
    return (int)std::count_if(textures_.begin(), textures_.end(),
                              [](const Texture2D &t) { return !t.ready(); });
}

void Ren::Context::ReleaseTextures() {
    if (!textures_.size())
        return;
    log_->Error("---------REMAINING TEXTURES--------");
    for (const Texture2D &t : textures_) {
        log_->Error("%s", t.name().c_str());
    }
    log_->Error("-----------------------------------");
    textures_.clear();
}

Ren::TextureRegionRef Ren::Context::LoadTextureRegion(const char *name, const void *data,
                                                      int size, const Texture2DParams &p,
                                                      eTexLoadStatus *load_status) {
    TextureRegionRef ref = texture_regions_.FindByName(name);
    if (!ref) {
        ref = texture_regions_.Add(name, data, size, p, &texture_atlas_, load_status);
    } else {
        if (ref->ready()) {
            if (load_status)
                *load_status = eTexLoadStatus::TexFound;
        } else {
            ref->Init(data, size, p, &texture_atlas_, load_status);
        }
    }
    return ref;
}

void Ren::Context::ReleaseTextureRegions() {
    if (!texture_regions_.size())
        return;
    log_->Error("-------REMAINING TEX REGIONS-------");
    for (const TextureRegion &t : texture_regions_) {
        log_->Error("%s", t.name().c_str());
    }
    log_->Error("-----------------------------------");
    texture_regions_.clear();
}

Ren::AnimSeqRef Ren::Context::LoadAnimSequence(const char *name, std::istream &data) {
    AnimSeqRef ref = anims_.FindByName(name);
    if (!ref) {
        ref = anims_.Add(name, data);
    } else {
        if (ref->ready()) {
        } else if (!ref->ready() && data) {
            ref->Init(data);
        }
    }

    return ref;
}

int Ren::Context::NumAnimsNotReady() {
    return (int)std::count_if(anims_.begin(), anims_.end(),
                              [](const AnimSequence &a) { return !a.ready(); });
}

void Ren::Context::ReleaseAnims() {
    if (!anims_.size()) {
        return;
    }
    log_->Error("---------REMAINING ANIMS--------");
    for (const AnimSequence &a : anims_) {
        log_->Error("%s", a.name().c_str());
    }
    log_->Error("-----------------------------------");
    anims_.clear();
}

Ren::BufferRef Ren::Context::CreateBuffer(const char *name, uint32_t initial_size) {
    return buffers_.Add(name, initial_size);
}

void Ren::Context::ReleaseBuffers() {
    if (!buffers_.size()) {
        return;
    }
    log_->Error("---------REMAINING BUFFERS--------");
    for (const Buffer &b : buffers_) {
        log_->Error("%u", b.size());
    }
    log_->Error("-----------------------------------");
    buffers_.clear();
}

void Ren::Context::ReleaseAll() {
    meshes_.clear();
    default_vertex_buf1_ = {};
    default_vertex_buf2_ = {};
    default_skin_vertex_buf_ = {};
    default_delta_buf_ = {};
    default_indices_buf_ = {};

    ReleaseAnims();
    ReleaseMaterials();
    ReleaseTextures();
    ReleaseTextureRegions();
    ReleaseBuffers();

    texture_atlas_ = {};
}
