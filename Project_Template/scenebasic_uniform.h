#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"

#include <glad/glad.h>
#include "helper/glslprogram.h"

#include "helper/torus.h"
#include "helper/teapot.h"
#include "helper/cube.h"
#include "helper/objmesh.h"
#include "helper/texture.h"
#include "helper/plane.h"

#include <glm/glm.hpp>

class SceneBasic_Uniform : public Scene
{
private:
    GLSLProgram prog;
    
    std::unique_ptr<ObjMesh> ogre;
    Plane plane;

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
