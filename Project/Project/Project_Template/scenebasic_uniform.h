#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"

#include <glad/glad.h>
#include "helper/glslprogram.h"

#include "helper/torus.h"
#include "helper/objmesh.h"
#include "helper/texture.h"
#include "helper/plane.h"
#include "helper/sphere.h"

#include <glm/glm.hpp>

class SceneBasic_Uniform : public Scene
{
private:
    GLSLProgram prog;
    
    std::unique_ptr<ObjMesh> ogre;
    Plane plane;
    //Sphere sun, planet1, planet2;
    Torus torus;

    GLuint brickTexture, concreteTexture, ogreDiffuse, ogreNormal;


    void setMatrices();

    void compile();

public:
    SceneBasic_Uniform();

    void initScene();
    void update( float t );
    void render();
    void resize(int, int);
};

#endif // SCENEBASIC_UNIFORM_H