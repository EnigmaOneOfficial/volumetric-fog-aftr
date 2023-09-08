#include "GLViewNewModule.h"
#include "WorldList.h"
#include "ManagerOpenGLState.h"
#include "Axes.h"
#include "PhysicsEngineODE.h"
#include "WO.h"
#include "WOStatic.h"
#include "WOLight.h"
#include "WOSkyBox.h"
#include "Camera.h"
#include "Model.h"
#include "AftrGLRendererBase.h"
#include "WOImGui.h"
#include "imgui/imgui.h"

using namespace Aftr;

GLViewNewModule* GLViewNewModule::New( const std::vector< std::string >& args ) {
   GLViewNewModule* glv = new GLViewNewModule( args );
   glv->init( Aftr::GRAVITY, Vector( 0, 0, -1.0f ), "aftr.conf", PHYSICS_ENGINE_TYPE::petODE );
   glv->onCreate();
   return glv;
}


GLViewNewModule::GLViewNewModule( const std::vector< std::string >& args ) : GLView( args ) {}


void GLViewNewModule::onCreate() {
   if( this->pe != NULL )
   {
      this->pe->setGravityNormalizedVector( Vector( 0,0,-1.0f ) );
      this->pe->setGravityScalar( Aftr::GRAVITY );
   }
   this->setActorChaseType( STANDARDEZNAV );
}


GLViewNewModule::~GLViewNewModule() {}


void GLViewNewModule::updateWorld() {
   GLView::updateWorld();
   if (grid != nullptr) {
      grid->update();
   }
}


void GLViewNewModule::onResizeWindow( GLsizei width, GLsizei height ) {
   GLView::onResizeWindow( width, height );
}


void GLViewNewModule::onMouseDown( const SDL_MouseButtonEvent& e ) {
   GLView::onMouseDown( e );
}


void GLViewNewModule::onMouseUp( const SDL_MouseButtonEvent& e ) {
   GLView::onMouseUp( e );
}


void GLViewNewModule::onMouseMove( const SDL_MouseMotionEvent& e ) {
   GLView::onMouseMove( e );
}


void GLViewNewModule::onKeyDown( const SDL_KeyboardEvent& key ) {
   GLView::onKeyDown( key );
   if (key.keysym.sym == SDLK_0) {
       this->setNumPhysicsStepsPerRender(1);
   } else if (key.keysym.sym == SDLK_r) {
       this->cam->setPosition(15, 15, 10);
   }
}


void GLViewNewModule::onKeyUp( const SDL_KeyboardEvent& key ) {
   GLView::onKeyUp( key );
}


void Aftr::GLViewNewModule::loadMap()
{
   this->worldLst = new WorldList();
   this->actorLst = new WorldList();
   this->netLst = new WorldList();
   ManagerOpenGLState::GL_CLIPPING_PLANE = 1000.0;
   ManagerOpenGLState::GL_NEAR_PLANE = 0.1f;
   ManagerOpenGLState::enableFrustumCulling = false;
   Axes::isVisible = true;
   this->glRenderer->isUsingShadowMapping( true );

   this->cam->setPosition( 15,15,10 );

   std::string grass( ManagerEnvironmentConfiguration::getSMM() + "/models/grassFloor400x400_pp.wrl" );
   
   //SkyBox Textures readily available
   std::vector< std::string > skyBoxImageNames;
   skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/space_Hubble_Nebula+6.jpg" );

   {
       //Create a light
       float ga = 0.1f; //Global Ambient Light level for this module
       ManagerLight::setGlobalAmbientLight(aftrColor4f(ga, ga, ga, 1.0f));
       WOLight* light = WOLight::New();
       light->isDirectionalLight(true);
       light->setPosition(Vector(0, 0, 100));
       light->getModel()->setDisplayMatrix(Mat4::rotateIdentityMat({ 0, 1, 0 }, 90.0f * Aftr::DEGtoRAD));
       light->setLabel("Light");
       worldLst->push_back(light);
   }

    {
        //Create the SkyBox
        WO* wo = WOSkyBox::New(skyBoxImageNames.at(0), this->getCameraPtrPtr());
        wo->setPosition(Vector(0, 0, 0));
        wo->setLabel("Sky Box");
        wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
        worldLst->push_back(wo);
    }

   {
       // Create the fog grid
       grid = new VolumetricGrid(5, 5, 5, 4.0f, 0.0f);
       WOImGui* gui = WOImGui::New(nullptr);
       gui->setLabel("My Gui");
       gui->subscribe_drawImGuiWidget(
           [this, gui]() //this is a lambda, the capture clause is in [], the input argument list is in (), and the body is in {}
       {
           ImGui::Begin("Volumetric Grid Control Panel");

           static int length = grid->length;
           static int width = grid->width;
           static int depth = grid->depth;
           static float scale = grid->scale;
           static float gap = grid->gap;
           static float freq = grid->frequency;
           static float displacement = grid->displacement;
           static float alphaMin = grid->alphaMin;
           static float alphaMax = grid->alphaMax;

           if (ImGui::InputInt("Grid Width", &width)) {
               grid->updateGrid(length, width, depth, scale, gap, freq, displacement, alphaMin, alphaMax);
           }
           if (ImGui::InputInt("Grid Length", &length)) {
               grid->updateGrid(length, width, depth, scale, gap, freq, displacement, alphaMin, alphaMax);
           }
           if (ImGui::InputInt("Grid Depth", &depth)) {
               grid->updateGrid(length, width, depth, scale, gap, freq, displacement, alphaMin, alphaMax);
           }
           if (ImGui::InputFloat("Cube Scale", &scale)) {
               grid->updateGrid(length, width, depth, scale, gap, freq, displacement, alphaMin, alphaMax);
           }
           if (ImGui::InputFloat("Cube Gap", &gap)) {
               grid->updateGrid(length, width, depth, scale, gap, freq, displacement, alphaMin, alphaMax);
           }
           if (ImGui::InputFloat("Frequency", &freq)) {
               grid->updateGrid(length, width, depth, scale, gap, freq, displacement, alphaMin, alphaMax);
           }
           if (ImGui::InputFloat("Displacement", &displacement)) {
               grid->updateGrid(length, width, depth, scale, gap, freq, displacement, alphaMin, alphaMax);
           }
           if (ImGui::SliderFloat("Alpha Min", &alphaMin, 0.0f, 1.0f)) {
               grid->updateGrid(length, width, depth, scale, gap, freq, displacement, alphaMin, alphaMax);
           }
           if (ImGui::SliderFloat("Alpha Max", &alphaMax, 0.0f, 1.0f)) {
               grid->updateGrid(length, width, depth, scale, gap, freq, displacement, alphaMin, alphaMax);
           }
           
           ImGui::End();
       });
       this->worldLst->push_back(gui);
   }
}

float lerp(const float& start, const float& end, float factor) {
    return start + factor * (end - start);
}

void VolumetricGrid::updateGrid(
    int newLength, int newWidth, int newDepth,
    float newScale, float newGap, float newFrequency,
    float newDisplacement, float newAlphaMin, float newAlphaMax
) {
    bool isResizeNeeded = (newLength != length || newWidth != width || newDepth != depth || newScale != scale || newGap != gap);

    // Update class members
    length = newLength;
    width = newWidth;
    depth = newDepth;
    scale = newScale;
    gap = newGap;
    frequency = newFrequency;
    displacement = newDisplacement;
    alphaMin = newAlphaMin;
    alphaMax = newAlphaMax;

    float baseCubeSize = 4.0f;
    float scaledCubeSize = baseCubeSize * newScale;
    spacing = newGap + scaledCubeSize;

    if (isResizeNeeded) {
        for (WO* cube : cubes) {
            cube->isVisible = false;
            ManagerGLView::getGLView()->getWorldContainer()->eraseViaWOptr(cube);
        }
        cubes.clear();
        initializeCubes();
    }
}

VolumetricGrid::VolumetricGrid(int initialLength, int initialWidth, int initialDepth, float initialScale, float initialGap)
    : length(initialLength), width(initialWidth), depth(initialDepth), scale(initialScale), gap(initialGap) {

    float baseCubeSize = 4.0f;
    float scaledCubeSize = baseCubeSize * initialScale;
    spacing = initialGap + scaledCubeSize;

    initializeCubes();
}

void VolumetricGrid::update() {
    static float timeCounter = 0;
    timeCounter += 0.07;

    float maxCubeDisplacement = spacing * displacement;

    PerlinNoise noiseGenerator;

    GLView* graphicsView = ManagerGLView::getGLView();
    Vector cameraPosition = graphicsView->getCamera()->getPosition();

    for (WO* cube : cubes) {
        Vector cubeOrigin = cubeOrigins[cube];

        if (cube->isVisible) {
            updateCubePosition(cube, cubeOrigin, noiseGenerator, timeCounter, maxCubeDisplacement);
            updateCubeAlpha(cube, cubeOrigin, noiseGenerator, timeCounter, cameraPosition);
        }
    }
}

void VolumetricGrid::initializeCubes() {
    for (int xIndex = 0; xIndex < length; ++xIndex) {
        for (int yIndex = 0; yIndex < width; ++yIndex) {
            for (int zIndex = 0; zIndex < depth; ++zIndex) {
                float x = xIndex * spacing;
                float y = yIndex * spacing;
                float z = zIndex * spacing;

                WO* cube = createCube(Vector(x, y, z), Vector(scale, scale, scale));
                cubeOrigins[cube] = Vector(x, y, z);
            }
        }
    }
}

void VolumetricGrid::updateCubePosition(WO* cube, Vector cubeOrigin, PerlinNoise& noiseGenerator, float timeCounter, float maxCubeDisplacement) {
    float easedTime = sin(timeCounter);
    float dxNoise = maxCubeDisplacement * noiseGenerator.noise(frequency * (easedTime + cubeOrigin.x));
    float dyNoise = maxCubeDisplacement * noiseGenerator.noise(frequency * (easedTime + cubeOrigin.y));
    float dzNoise = maxCubeDisplacement * noiseGenerator.noise(frequency * (easedTime + cubeOrigin.z));

    Vector newPosition = cubeOrigin + Vector(dxNoise, dyNoise, dzNoise);
    cube->setPosition(newPosition);
}

void VolumetricGrid::updateCubeAlpha(WO* cube, Vector cubeOrigin, PerlinNoise& noiseGenerator, float timeCounter, Vector cameraPosition) {
    float distanceToCamera = (cube->getPosition() - cameraPosition).magnitude();
    float depthCueingFactor = 1.0f / (1.0f + distanceToCamera * 0.1f);
    float alphaNoise = noiseGenerator.noise(frequency * (timeCounter + cubeOrigin.x + cubeOrigin.y + cubeOrigin.z));
    float alpha = lerp(alphaMin, alphaMax, alphaNoise) * std::min(1.0f, depthCueingFactor);

    aftrColor4f fogColor(0.5f, 0.5f, 0.5f, alpha);
    cube->getModel()->getModelDataShared()->getModelMeshes().at(0)->getSkins().at(0).setAmbient(fogColor);
}

WO* VolumetricGrid::createCube(Vector pos, Vector size) {
    WO* cube = WO::New(ManagerEnvironmentConfiguration::getSMM() + "/models/cube4x4x4redShinyPlastic_pp.wrl", size, MESH_SHADING_TYPE::mstAUTO);
    cube->setPosition(pos);
    cube->renderOrderType = RENDER_ORDER_TYPE::roTRANSPARENT;
    cube->setLabel("Cube");

    GLView* glv = ManagerGLView::getGLView();
    glv->getWorldContainer()->push_back(cube);

    cube->upon_async_model_loaded([this, cube]() {
        ModelMeshSkin& cubeSkin = cube->getModel()->getModelDataShared()->getModelMeshes().at(0)->getSkins().at(0);
        cubeSkin.getMultiTextureSet().at(0).setTexRepeats(1.0f);
        cubeSkin.setSpecular(aftrColor4f(1.0f, 1.0f, 1.0f, 1.0f));
        cubeSkin.setSpecularCoefficient(100);
        cubes.push_back(cube);
    });

    return cube;
}