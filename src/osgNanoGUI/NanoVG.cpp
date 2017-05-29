
#include <GL/glew.h>

#include <osgNanoGUI/NanoVG>

//#include "glad/glad.h"

NanoVGDrawCallback::~NanoVGDrawCallback()
{
    //if (vg) { delete vg; vg = 0; }

#if defined NANOVG_GL2
    nvgDeleteGL2(vg);
#elif defined NANOVG_GL3
    nvgDeleteGL3(vg);
#elif defined NANOVG_GLES2
    nvgDeleteGLES2(vg);
#elif defined NANOVG_GLES3
    nvgDeleteGLES3(vg);
#endif

    vg = 0;
}

void NanoVGDrawCallback::drawImplementation(osg::RenderInfo& renderInfo, const osg::Drawable* drawable) const
{
    NanoVGDrawCallback* np = const_cast<NanoVGDrawCallback*>(this);

    //init
    if (vg == 0)
    {
        int flags = 0;

#ifdef DEMO_MSAA
        flags = (NVG_STENCIL_STROKES | NVG_DEBUG);
#else
        flags = (NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
#endif

#ifdef NANOVG_GL2_IMPLEMENTATION
        np->vg = nvgCreateGL2(flags);
#endif
#ifdef NANOVG_GL3_IMPLEMENTATION
        np->vg = nvgCreateGL3(flags);
#endif
#ifdef OSG_GLES2_AVAILABLE
        np->vg = nvgCreateGL2(flags);
#endif
#ifdef OSG_GLES3_AVAILABLE
        np->vg = nvgCreateGL2(flags);
#endif

    }
    if (!np->vg) return;

    //init 
    if (!np->inited)
    {
        np->inited = np->init(np->vg);
    }

    nvgGlobalCompositeBlendFunc(vg, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //draw
    float devicePixelRatio = 1.0;
    nvgBeginFrame(vg, windowWidth, windowHeight, devicePixelRatio);
    {
        np->draw(np->vg, windowWidth, windowHeight);
    }
    nvgEndFrame(vg);

}


