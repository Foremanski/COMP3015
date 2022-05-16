#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"

#include <glad/glad.h>
#include "helper/glslprogram.h"

#include "helper/torus.h"
#include "helper/texture.h"
#include "helper/plane.h"
#include "helper/sphere.h"
#include "helper/cube.h"

#include <glm/glm.hpp>



class SceneBasic_Uniform : public Scene
{
private:

    GLSLProgram prog, waveProg;

    GLuint fsQuad;
    GLuint hdrFbo, blurFbo;
    GLuint hdrTex, tex1, tex2;
    GLuint linearSampler, nearestSampler;

    Plane plane;

    //brick wall texture
    GLuint brickTexture;

    float angle;
    float time;

    float VCounter;
    float ECounter;
    float ACounter;

    int bloomBufWidth, bloomBufHeight;

    void setMatrices();
    void compile();

    void setupFBO();
    void pass1();
    void pass2();
    void pass3();
    void pass4(); 
    void pass5();
    float gauss(float, float);
    void computeLogAveLuminance();

    void drawScene(); 

    

public:
    SceneBasic_Uniform();

    void initScene();
    void update( float t );
    void render();
    void resize(int, int);
};

#endif // SCENEBASIC_UNIFORM_H
