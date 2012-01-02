#include "stdafx.h"
#include "types.h"
#include "Game/AtomicApplication.h"
#include "Game/AtomicGame.h"
#include "Game/World.h"
#include "Game/World.h"
#include "GPGPU/SPH.cuh"
#include "AtomicRenderingSystem.h"
#include "Renderer.h"
#include "Util.h"

namespace atomic {


AtomicRenderer* AtomicRenderer::s_inst = NULL;

void AtomicRenderer::initializeInstance()
{
    if(!s_inst) {
        s_inst = istNew(AtomicRenderer) ();
    }
    else {
        istAssert("already initialized");
    }
}

void AtomicRenderer::finalizeInstance()
{
    istSafeDelete(s_inst);
}

AtomicRenderer::AtomicRenderer()
{
    m_va_screenquad = atomicGetVertexArray(VA_SCREEN_QUAD);
    m_sh_out        = atomicGetShader(SH_OUTPUT);

    m_rt_gbuffer    = atomicGetRenderTarget(RT_GBUFFER);
    m_rt_deferred   = atomicGetRenderTarget(RT_DEFERRED);

    // 追加の際はデストラクタでの消去処理も忘れずに
    m_renderer_sph          = istNew(PassGBuffer_SPH)();
    m_renderer_dir_lights   = istNew(PassDeferredShading_DirectionalLights)();
    m_renderer_point_lights = istNew(PassDeferredShading_PointLights)();
    m_renderer_fxaa         = istNew(PassPostprocess_FXAA)();
    m_renderer_bloom        = istNew(PassPostprocess_Bloom)();
    m_renderer_fade         = istNew(PassPostprocess_Fade)();

    m_renderers[PASS_GBUFFER].push_back(m_renderer_sph);
    m_renderers[PASS_DEFERRED].push_back(m_renderer_dir_lights);
    m_renderers[PASS_DEFERRED].push_back(m_renderer_point_lights);
    m_renderers[PASS_POSTPROCESS].push_back(m_renderer_fxaa);
    m_renderers[PASS_POSTPROCESS].push_back(m_renderer_bloom);
    m_renderers[PASS_POSTPROCESS].push_back(m_renderer_fade);

    m_stext = istNew(SystemTextRenderer)();

    m_default_viewport = Viewport(ivec2(0), atomicGetWindowSize());
}

AtomicRenderer::~AtomicRenderer()
{
    istSafeDelete(m_stext);
    istSafeDelete(m_renderer_fade);
    istSafeDelete(m_renderer_bloom);
    istSafeDelete(m_renderer_fxaa);
    istSafeDelete(m_renderer_point_lights);
    istSafeDelete(m_renderer_dir_lights);
    istSafeDelete(m_renderer_sph);
}

void AtomicRenderer::beforeDraw()
{
    for(uint32 i=0; i<_countof(m_renderers); ++i) {
        uint32 size = m_renderers[i].size();
        for(uint32 j=0; j<size; ++j) {
            m_renderers[i][j]->beforeDraw();
        }
    }
    m_stext->beforeDraw();
}


void AtomicRenderer::draw()
{
    PerformanceCounter timer;
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);

    glLoadIdentity();
    {
        PerspectiveCamera *camera      = atomicGetCamera();
        UniformBuffer *ubo_renderstates= atomicGetUniformBufferObject(UBO_RENDER_STATES);
        const uvec2 &wsize = atomicGetWindowSize();
        camera->updateMatrix();
        m_render_states.ModelViewProjectionMatrix = camera->getModelViewProjectionMatrix();
        m_render_states.CameraPosition  = camera->getPosition();
        m_render_states.ScreenSize      = vec2(atomicGetWindowSize());
        m_render_states.RcpScreenSize   = vec2(1.0f, 1.0f) / m_render_states.ScreenSize;
        m_render_states.AspectRatio     = (float32)wsize.x / (float32)wsize.y;
        m_render_states.RcpAspectRatio  = 1.0f / m_render_states.AspectRatio;
        m_render_states.ScreenTexcoord = vec2(
            m_render_states.ScreenSize/vec2(m_rt_deferred->getColorBuffer(0)->getSize()) );
        MapAndWrite(*ubo_renderstates, &m_render_states, sizeof(m_render_states));
    }

    passShadow();
    passGBuffer();
    passDeferredShading();
    passForwardShading();
    passPostprocess();
    passHUD();
    passOutput();

    //glFinish();

    timer.count();
}

void AtomicRenderer::passShadow()
{
    glClear(GL_DEPTH_BUFFER_BIT);
    glFrontFace(GL_CW);
    glEnable(GL_DEPTH_TEST);

    uint32 num_renderers = m_renderers[PASS_SHADOW_DEPTH].size();
    for(uint32 i=0; i<num_renderers; ++i) {
        m_renderers[PASS_SHADOW_DEPTH][i]->draw();
    }

    glDisable(GL_DEPTH_TEST);
    glFrontFace(GL_CCW);
}

void AtomicRenderer::passGBuffer()
{
    const PerspectiveCamera *camera = atomicGetCamera();

    m_rt_gbuffer->bind();
    glClearColor(0.0f,0.0f,0.0f,1.0f);
    glClearStencil(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 1, ~0);
    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

    uint32 num_renderers = m_renderers[PASS_GBUFFER].size();
    for(uint32 i=0; i<num_renderers; ++i) {
        m_renderers[PASS_GBUFFER][i]->draw();
    }

    glDisable(GL_STENCIL_TEST);
    glDisable(GL_DEPTH_TEST);
    m_rt_gbuffer->unbind();
}

void AtomicRenderer::passDeferredShading()
{
    m_rt_deferred->bind();
    m_rt_gbuffer->getColorBuffer(GBUFFER_COLOR)->bind(GLSL_COLOR_BUFFER);
    m_rt_gbuffer->getColorBuffer(GBUFFER_NORMAL)->bind(GLSL_NORMAL_BUFFER);
    m_rt_gbuffer->getColorBuffer(GBUFFER_POSITION)->bind(GLSL_POSITION_BUFFER);
    m_rt_gbuffer->getColorBuffer(GBUFFER_GLOW)->bind(GLSL_GLOW_BUFFER);

    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glDepthMask(GL_FALSE);
    glStencilFunc(GL_EQUAL, 1, ~0);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

    uint32 num_renderers = m_renderers[PASS_DEFERRED].size();
    for(uint32 i=0; i<num_renderers; ++i) {
        m_renderers[PASS_DEFERRED][i]->draw();
    }

    glDepthMask(GL_TRUE);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    m_rt_deferred->unbind();
}

void AtomicRenderer::passForwardShading()
{
    uint32 num_renderers = m_renderers[PASS_FORWARD].size();
    for(uint32 i=0; i<num_renderers; ++i) {
        m_renderers[PASS_FORWARD][i]->draw();
    }
}

void AtomicRenderer::passPostprocess()
{
    uint32 num_renderers = m_renderers[PASS_POSTPROCESS].size();
    for(uint32 i=0; i<num_renderers; ++i) {
        m_renderers[PASS_POSTPROCESS][i]->draw();
    }
}

void AtomicRenderer::passHUD()
{
    uint32 num_renderers = m_renderers[PASS_HUD].size();
    for(uint32 i=0; i<num_renderers; ++i) {
        m_renderers[PASS_HUD][i]->draw();
    }
}

void AtomicRenderer::passOutput()
{
    m_rt_deferred->getColorBuffer(0)->bind(GLSL_COLOR_BUFFER);
    m_sh_out->bind();
    m_va_screenquad->bind();
    glDrawArrays(GL_QUADS, 0, 4);
    m_sh_out->unbind();

    char buf[64];
    sprintf(buf, "FPS: %.0f", atomicGetRenderingSystem()->getAverageFPS());
    m_stext->addText(ivec2(5, 0), buf);
    sprintf(buf, "Particles: %d", SPHGetStates().fluid_num_particles);
    m_stext->addText(ivec2(5, 20), buf);
    m_stext->draw();
}






SystemTextRenderer::SystemTextRenderer()
{
    m_texts.reserve(128);
}

void SystemTextRenderer::beforeDraw()
{
    m_texts.clear();
}

void SystemTextRenderer::draw()
{
    if(!atomicGetConfig()->show_text) { return; }

    for(uint32 i=0; i<m_texts.size(); ++i) {
        const Text &t = m_texts[i];
        atomicGetFont()->draw(t.pos.x, t.pos.y, t.text);
    }
}

void SystemTextRenderer::addText(const ivec2 &pos, const char *text)
{
    Text tmp;
    strncpy(tmp.text, text, _countof(tmp.text));
    tmp.pos = pos;
    m_texts.push_back(tmp);
}

} // namespace atomic
