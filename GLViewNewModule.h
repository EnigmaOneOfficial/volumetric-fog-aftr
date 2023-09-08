#pragma once

#include "GLView.h"
#include <vector>
#include <unordered_map>

namespace Aftr
{
   class Camera;

/**
   \class GLViewNewModule
   \author Scott Nykl 
   \brief A child of an abstract GLView. This class is the top-most manager of the module.

   Read \see GLView for important constructor and init information.

   \see GLView

    \{
*/
   class PerlinNoise {
   public:
       float noise(float x) {
           int x0 = std::floor(x);
           int x1 = x0 + 1;

           float w1 = smoothstep(x - x0);
           float w0 = 1 - w1;

           return w0 * hash(x0) + w1 * hash(x1);
       }
   private:
       float smoothstep(float t) {
           return t * t * (3 - 2 * t);
       }

       float hash(int x) { // Acquired from StackOverflow
           x = (x << 13) ^ x;
           return (1.0 - ((x * (x * x * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
       }
   };
class VolumetricGrid {
public:
    std::vector<WO*> cubes;
    std::unordered_map< WO*, Vector > cubeOrigins;
    float spacing = 0.0f;
    int length;
    int width;
    int depth;
    float scale;
    float gap;
    float alphaMin = .95f;
    float alphaMax = 1.0f;
    float displacement = 0.04f;
    float frequency = 0.15f;

    void initializeCubes();
    void updateCubePosition(WO* cube, Vector cubeOrigin, PerlinNoise& noiseGenerator, float timeCounter, float maxCubeDisplacement);
    void updateCubeAlpha(WO* cube, Vector cubeOrigin, PerlinNoise& noiseGenerator, float timeCounter, Vector cameraPosition);
    void updateGrid(int newLength, int newWidth, int newDepth, float newScale, float newGap, float frequency, float maxDisplacement, float minAlpha, float maxAlpha);
    WO* createCube(Vector pos = Vector(0, 0, 0), Vector size = Vector(0.5f, 0.5f, 0.5f));
    void update();
    VolumetricGrid(int length, int width, int depth, float scale = 1.0f, float gap = 0.0f);
protected:
};

class GLViewNewModule : public GLView
{
public:
    VolumetricGrid* grid = nullptr;
   static GLViewNewModule* New( const std::vector< std::string >& outArgs );
   virtual ~GLViewNewModule();
   virtual void updateWorld();
   virtual void loadMap();
   virtual void onResizeWindow( GLsizei width, GLsizei height );
   virtual void onMouseDown( const SDL_MouseButtonEvent& e );
   virtual void onMouseUp( const SDL_MouseButtonEvent& e );
   virtual void onMouseMove( const SDL_MouseMotionEvent& e );
   virtual void onKeyDown( const SDL_KeyboardEvent& key );
   virtual void onKeyUp( const SDL_KeyboardEvent& key );

protected:
   GLViewNewModule( const std::vector< std::string >& args );
   virtual void onCreate();   
};

/** \} */

} //namespace Aftr
