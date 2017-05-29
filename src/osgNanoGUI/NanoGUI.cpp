
#include <GL/glew.h>

#include <osgNanoGUI/NanoGUI>



NanoGUIDrawCallback::~NanoGUIDrawCallback()
{
    if (vg) { delete vg; vg = 0; }

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

void NanoGUIDrawCallback::drawImplementation(osg::RenderInfo& renderInfo, const osg::Drawable* drawable) const
{
    NanoGUIDrawCallback* np = const_cast<NanoGUIDrawCallback*>(this);

    //init vg
    if (vg == 0)
    {
        int flags = 0;

#ifdef DEMO_MSAA
        flags = (NVG_STENCIL_STROKES | NVG_DEBUG);
#else
        flags = (NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
#endif

#ifdef NANOVG_GL2_IMPLEMENTATION
        vg = nvgCreateGL2(flags);
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
    if (!vg) return;


    //init widget
    if (!widget.get())
    {
        widget = new nanogui::Widget(0);
        widget->setWidth(windowWidth);
        widget->setHeight(windowHeight);
    }
    if (!widget) return;


    //init 
    if (!inited)
    {
        np->inited = np->init(np->vg,np->widget);
    }

    nvgGlobalCompositeBlendFunc(vg, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //draw
    float devicePixelRatio = 1.0;
    nvgBeginFrame(vg, windowWidth, windowHeight, devicePixelRatio);
    {
        //np->doEvent();
        //doEvent();
        widget->draw(vg);
    }
    nvgEndFrame(vg);

}


void NanoGUIDrawCallback::doEvent() const
{
    //printf("event\n");

    for (size_t i = 0; i < ea_list.size(); i++)
    {
        osgGA::GUIEventAdapter* ea = ea_list[i];


        if (ea->getEventType() == osgGA::GUIEventAdapter::MOVE || ea->getEventType() == osgGA::GUIEventAdapter::DRAG )
        {
            nanogui::Vector2i p(ea->getX(), (ea->getYmax()- ea->getYmin()) - ea->getY());

            widget->mouseMotionEvent(p, p - mMousePos, mMouseState, mModifiers);

            mMousePos = p;
        }

        if (ea->getEventType() == osgGA::GUIEventAdapter::PUSH || ea->getEventType() == osgGA::GUIEventAdapter::RELEASE)
        {
            int button = 0;

            if (ea->getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON) button = 0;
            if (ea->getButton() == osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON) button = 1;
            if (ea->getButton() == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON) button = 2;

            bool action = false;
            if (ea->getEventType() == osgGA::GUIEventAdapter::PUSH)
            {
                mMouseState |= 1 << button;
                action = true;
            }

            if (ea->getEventType() == osgGA::GUIEventAdapter::RELEASE)
            {
                mMouseState &= ~(1 << button);
            }

            nanogui::Widget* w = widget->findWidget(mMousePos);
            if (w)
            {
                for (int i = 0; i < widget->childCount(); i++)
                {
                    widget->childAt(i)->focusEvent(false);
                }

                w->focusEvent(true);
            }

            widget->mouseButtonEvent(mMousePos, button, action, mModifiers);

        }

    }

    ea_list.clear();
    //aa_list.clear();
}

