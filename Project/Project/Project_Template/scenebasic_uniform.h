#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"

#include <glad/glad.h>
#include "helper/glslprogram.h"

#include "helper/torus.h"
#include "helper/texture.h"
#include "helper/plane.h"
#include "helper/sphere.h"

#include <glm/glm.hpp>

class SceneBasic_Uniform : public Scene
{
private:

    GLSLProgram prog;

    //GLuint brickTexture, concreteTexture;

    GLuint fsQuad;
    GLuint renderFBO, intermediateFBO;
    GLuint renderTex, intermediateTex;

    Plane plane;
    Sphere sun;
    //Torus torus;

    void setMatrices();

    void compile();

    void setupFBO();
    void pass1();
    void pass2();
    void pass3();
    float gauss(float, float);

public:
    SceneBasic_Uniform();

    void initScene();
    void update( float t );
    void render();
    void resize(int, int);
};

#endif // SCENEBASIC_UNIFORM_H
