
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

//#include <osgCEF/Browser>
#include <osgNanoGUI/NanoVG>

#include "demo.h"

#if 0
int isBlack(NVGcolor col)
{
    if (col.r == 0.0f && col.g == 0.0f && col.b == 0.0f && col.a == 0.0f)
    {
        return 1;
    }
    return 0;
}

static char* cpToUTF8(int cp, char* str)
{
    int n = 0;
    if (cp < 0x80) n = 1;
    else if (cp < 0x800) n = 2;
    else if (cp < 0x10000) n = 3;
    else if (cp < 0x200000) n = 4;
    else if (cp < 0x4000000) n = 5;
    else if (cp <= 0x7fffffff) n = 6;
    str[n] = '\0';
    switch (n) {
    case 6: str[5] = 0x80 | (cp & 0x3f); cp = cp >> 6; cp |= 0x4000000;
    case 5: str[4] = 0x80 | (cp & 0x3f); cp = cp >> 6; cp |= 0x200000;
    case 4: str[3] = 0x80 | (cp & 0x3f); cp = cp >> 6; cp |= 0x10000;
    case 3: str[2] = 0x80 | (cp & 0x3f); cp = cp >> 6; cp |= 0x800;
    case 2: str[1] = 0x80 | (cp & 0x3f); cp = cp >> 6; cp |= 0xc0;
    case 1: str[0] = cp;
    }
    return str;
}

void drawButton(NVGcontext* vg, int preicon, const char* text, float x, float y, float w, float h, NVGcolor col)
{
    NVGpaint bg;
    char icon[8];
    float cornerRadius = 4.0f;
    float tw = 0, iw = 0;

    bg = nvgLinearGradient(vg, x, y, x, y + h, nvgRGBA(255, 255, 255, isBlack(col) ? 16 : 32), nvgRGBA(0, 0, 0, isBlack(col) ? 16 : 32));
    nvgBeginPath(vg);
    nvgRoundedRect(vg, x + 1, y + 1, w - 2, h - 2, cornerRadius - 1);
    if (!isBlack(col)) {
        nvgFillColor(vg, col);
        nvgFill(vg);
    }
    nvgFillPaint(vg, bg);
    nvgFill(vg);

    nvgBeginPath(vg);
    nvgRoundedRect(vg, x + 0.5f, y + 0.5f, w - 1, h - 1, cornerRadius - 0.5f);
    nvgStrokeColor(vg, nvgRGBA(0, 0, 0, 48));
    nvgStroke(vg);

    nvgFontSize(vg, 20.0f);
    nvgFontFace(vg, "sans-bold");
    tw = nvgTextBounds(vg, 0, 0, text, NULL, NULL);
    if (preicon != 0) {
        nvgFontSize(vg, h*1.3f);
        nvgFontFace(vg, "icons");
        iw = nvgTextBounds(vg, 0, 0, cpToUTF8(preicon, icon), NULL, NULL);
        iw += h*0.15f;
    }

    if (preicon != 0) {
        nvgFontSize(vg, h*1.3f);
        nvgFontFace(vg, "icons");
        nvgFillColor(vg, nvgRGBA(255, 255, 255, 96));
        nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
        nvgText(vg, x + w*0.5f - tw*0.5f - iw*0.75f, y + h*0.5f, cpToUTF8(preicon, icon), NULL);
    }

    nvgFontSize(vg, 20.0f);
    nvgFontFace(vg, "sans-bold");
    nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
    nvgFillColor(vg, nvgRGBA(0, 0, 0, 160));
    nvgText(vg, x + w*0.5f - tw*0.5f + iw*0.25f, y + h*0.5f - 1, text, NULL);
    nvgFillColor(vg, nvgRGBA(255, 255, 255, 160));
    nvgText(vg, x + w*0.5f - tw*0.5f + iw*0.25f, y + h*0.5f, text, NULL);
}
#endif




class myNanoVGDrawCallback
    :public NanoVGDrawCallback
{
public:

    DemoData data;
    float i = 0;

    bool init(NVGcontext* vg) override
    {
        if (loadDemoData(vg, &data) == -1)
        {
            return false;
        }

        return true;
    }

    void draw(NVGcontext* vg, int width, int height) override
    {
        //NVGcolor c = { 1,1,1,1 };
        //drawButton(vg, 1, "aaaa", 0, 0, 20, 20, c);

        //nvgGlobalCompositeBlendFunc(vg, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        renderDemo(vg, 0, 0, width, height, i+=0.01 , 0, &data);

    }


};


int main( int argc, char **argv )
{
    //viewer
    osgViewer::Viewer viewer;
    osg::Group* root = new osg::Group();
    viewer.setSceneData(root);
    viewer.setUpViewInWindow(100, 100, 1280, 720);


    //setup GLEW
    viewer.setThreadingModel(osgViewer::Viewer::SingleThreaded);
    viewer.realize();
    viewer.getCamera()->getGraphicsContext()->makeCurrent();
    glewInit();


    //
    osg::Drawable* draw = new osg::Drawable();
    draw->setUseDisplayList(false);
    myNanoVGDrawCallback* mydc = new myNanoVGDrawCallback;
    mydc->resize(1280, 720);
    draw->setDrawCallback(mydc);

    osg::Geode* geode = new osg::Geode();
    geode->addDrawable(draw);

    root->addChild(geode);


    //
    osg::ref_ptr<osg::BlendFunc> blend0 = new osg::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    geode->getOrCreateStateSet()->setAttributeAndModes(blend0, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);



#if 0
    //
    osg::Geometry* geom = osg::createTexturedQuadGeometry(osg::Vec3(-1, -1, 0), osg::Vec3(2, 0, 0), osg::Vec3(0, 2, 0));
    osg::Geode* geode = new osg::Geode();
    geode->addDrawable(geom);
    b->addChild(geode);

    //no lighting
    geode->getOrCreateStateSet()->setMode(GL_LIGHTING, false);
    geode->getOrCreateStateSet()->setTextureAttributeAndModes(0, new osg::TexMat(osg::Matrix::scale(1, -1, 1)));


    //hud
    if (true)
    {
        osg::Camera* camera = new osg::Camera();

        camera->setReferenceFrame(osg::Camera::ABSOLUTE_RF);
        camera->setComputeNearFarMode(osg::Camera::DO_NOT_COMPUTE_NEAR_FAR);
        camera->setProjectionMatrixAsOrtho(-1, 1, -1, 1, 0, 1);
        camera->setViewMatrixAsLookAt(osg::Vec3(0, 0, 0.999999), osg::Vec3(0, 0, 0), osg::Vec3(0, 1, 0));
        //camera->getOrCreateStateSet()->setMode(GL_LIGHTING, false);

        camera->addChild(b);
        root->addChild(camera);
    }
    else
    {
        root->addChild(b);
    }
#endif

    //force resize event
    viewer.frame();
    osgViewer::GraphicsWindow* gw = dynamic_cast<osgViewer::GraphicsWindow*>(viewer.getCamera()->getGraphicsContext());
    if (gw)
    {
        // Send window size event for initializing
        int x, y, w, h; 
        gw->getWindowRectangle(x, y, w, h);
        viewer.getEventQueue()->windowResize(x, y, w, h);
    }

    //run
    //viewer.run();
    while (!viewer.done())
    {

        viewer.frame();
    }


    return 0;
}
