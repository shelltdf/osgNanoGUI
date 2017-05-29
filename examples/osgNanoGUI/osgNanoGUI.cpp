
#include <iostream>
#include <thread>

#include <GL/glew.h>

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgDB/FileNameUtils>
#include <osgDB/fstream>
#include <osgViewer/Viewer>
#include <osg/TexMat>
#include <osg/BlendFunc>
#include <osgViewer/ViewerEventHandlers>


#include <osgNanoGUI/NanoGUI>

#include "formhelper.h"


class myNanoVGDrawCallback
    :public NanoGUIDrawCallback
{
public:

    bool init(NVGcontext* vg, nanogui::Widget* widget) override
    {
        //set theme
        widget->setTheme(new nanogui::Theme(vg));


        //form example3.c

        enum test_enum
        {
            Item1 = 0,
            Item2,
            Item3
        };

        bool bvar = true;
        int ivar = 12345678;
        double dvar = 3.1415926;
        float fvar = (float)dvar;
        std::string strval = "A string";
        test_enum enumval = Item2;
        nanogui::Color colval(0.5f, 0.5f, 0.7f, 1.f);



        // Create nanogui gui
        bool enabled = true;
        nanogui::FormHelper *gui = new nanogui::FormHelper(widget);
        nanogui::ref<nanogui::Window> nanoguiWindow = gui->addWindow(Eigen::Vector2i(10, 10), "Form helper example");

        gui->addGroup("Basic types");
        gui->addVariable("bool", bvar)->setTooltip("Test tooltip.");
        gui->addVariable("string", strval);

        gui->addGroup("Validating fields");
        gui->addVariable("int", ivar)->setSpinnable(true);
        gui->addVariable("float", fvar)->setTooltip("Test.");
        gui->addVariable("double", dvar)->setSpinnable(true);

        gui->addGroup("Complex types");
        gui->addVariable("Enumeration", enumval, enabled)->setItems({ "Item 1", "Item 2", "Item 3" });
        gui->addVariable("Color", colval);

        gui->addGroup("Other widgets");
        gui->addButton("A button", []() { std::cout << "Button pressed." << std::endl; })->setTooltip("Testing a much longer tooltip, that will wrap around to new lines multiple times.");;

        widget->setVisible(true);
        widget->performLayout(vg);
        nanoguiWindow->center();


        //widget->focusEvent(true);

        return true;
    }

    //void draw(NVGcontext* vg, nanogui::Widget* widget, int width, int height) override
    //{

    //    //    //NVGcolor c = { 1,1,1,1 };
    //    //    //drawButton(vg, 1, "aaaa", 0, 0, 20, 20, c);

    //    //    //nvgGlobalCompositeBlendFunc(vg, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //    //    renderDemo(vg, 0, 0, width, height, i += 0.01, 0, &data);

    //}

};



class NanoEventHandler
    : public osgGA::GUIEventHandler
{
public:

    NanoEventHandler(myNanoVGDrawCallback* dc) :_dc(dc) {}
    virtual ~NanoEventHandler() {}

    nanogui::Vector2i mMousePos;
    int mMouseState = 0;
    int mModifiers = 0;

    bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
    {
        //if(_dc)_dc->handle(ea,aa);

        bool ret = false;

        if (ea.getEventType() == osgGA::GUIEventAdapter::MOVE)
        {
            nanogui::Vector2i p(ea.getX(), (ea.getYmax() - ea.getYmin()) - ea.getY());

            ret = _dc->widget->mouseMotionEvent(p, p - mMousePos, mMouseState, mModifiers);

            mMousePos = p;
        }

        if (ea.getEventType() == osgGA::GUIEventAdapter::DRAG)
        {
            nanogui::Vector2i p(ea.getX(), (ea.getYmax() - ea.getYmin()) - ea.getY());

            //_dc->widget->mouseDragEvent(p, p - mMousePos, mMouseState, mModifiers);

            for (int i = 0; i < _dc->widget->childCount(); i++)
            {
                nanogui::Window* win = dynamic_cast<nanogui::Window*>(_dc->widget->childAt(i));
                if (win)
                {
                    bool b = win->mouseDragEvent(p, p - mMousePos, mMouseState, mModifiers);
                    if (b) ret = true;
                }
            }


            mMousePos = p;
        }


        if (ea.getEventType() == osgGA::GUIEventAdapter::PUSH || ea.getEventType() == osgGA::GUIEventAdapter::RELEASE)
        {
            int button = 0;

            if (ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON) button = 0;
            if (ea.getButton() == osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON) button = 1;
            if (ea.getButton() == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON) button = 2;

            bool action = false;
            if (ea.getEventType() == osgGA::GUIEventAdapter::PUSH)
            {
                mMouseState |= 1 << button;
                action = true;
            }

            if (ea.getEventType() == osgGA::GUIEventAdapter::RELEASE)
            {
                mMouseState &= ~(1 << button);
            }

            nanogui::Widget* w = _dc->widget->findWidget(mMousePos);
            if (w)
            {
                for (int i = 0; i < _dc->widget->childCount(); i++)
                {
                    _dc->widget->childAt(i)->focusEvent(false);
                }

                w->focusEvent(true);
            }

            ret = _dc->widget->mouseButtonEvent(mMousePos, button, action, mModifiers);

        }

        if (ea.getEventType() == osgGA::GUIEventAdapter::RESIZE)
        {
            _dc->resize(ea.getWindowWidth(), ea.getWindowHeight());
        }


        //
        if (_dc->widget)
        {
            nanogui::Widget* w = _dc->widget->findWidget(mMousePos);
            if (w && (w != _dc->widget))
            {
                return true;
            }
        }

        return ret;
    }

protected:
    osg::ref_ptr<myNanoVGDrawCallback> _dc;
};




int main(int argc, char **argv)
{
    //viewer
    osgViewer::Viewer viewer;
    osg::Group* root = new osg::Group();
    viewer.setSceneData(root);
    viewer.setUpViewInWindow(100, 100, 1280, 720);
    viewer.getCamera()->setClearColor(osg::Vec4(0.9, 0.9, 0.9, 1.0));

    //setup GLEW
    viewer.setThreadingModel(osgViewer::Viewer::SingleThreaded);
    viewer.realize();
    viewer.getCamera()->getGraphicsContext()->makeCurrent();
    glewInit();


    //cow
    osg::Node* cow = osgDB::readNodeFile("../data/cow.osg");
    root->addChild(cow);


    //
    osg::Drawable* draw = new osg::Drawable();
    draw->setDataVariance(osg::Object::DYNAMIC);
    draw->setUseDisplayList(false);
    myNanoVGDrawCallback* mydc = new myNanoVGDrawCallback;
    mydc->resize(1280, 720);
    draw->setDrawCallback(mydc);

    osg::Geode* geode = new osg::Geode();
    geode->setDataVariance(osg::Object::DYNAMIC);
    geode->addDrawable(draw);

    root->addChild(geode);


    //
    osg::ref_ptr<osg::BlendFunc> blend0 = new osg::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    geode->getOrCreateStateSet()->setAttributeAndModes(blend0, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);

    //
    viewer.addEventHandler(new NanoEventHandler(mydc));

    //run
    viewer.run();

    return 0;
}
