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

    GLuint hdrFBO;
    GLuint quad;
    GLuint hdrTex, avgTex;

    Plane plane;
    Sphere sun;
    //Torus torus;

    void setMatrices();

    void compile();

    void setupFBO();
    void pass1();
    void pass2();

    void computeLogAveLuminance();
    void drawScene();

    /*
    void pass3();
    float gauss(float, float);
    */

public:
    SceneBasic_Uniform();

    void initScene();
    void update( float t );
    void render();
    void resize(int, int);
};

#endif // SCENEBASIC_UNIFORM_H
