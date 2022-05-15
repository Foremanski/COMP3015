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
    GLSLProgram volumeProg, renderProg, compProg;
    GLuint colorDepthFBO;
    GLuint fsQuad;

    GLSLProgram prog;
   
    GLuint hdrFbo, blurFbo;
    GLuint hdrTex, tex1, tex2;
    GLuint linearSampler, nearestSampler;

    GLuint spotTex;
    //glm::vec4 lightPos;
    //GLuint brickTexture, concreteTexture;

    Plane plane;
    Sphere sun, planet1, planet2;
    //Torus torus;

    float angle;
    int bloomBufWidth, bloomBufHeight;

    void setMatrices(GLSLProgram &);
    void compile();

    void setupFBO();
    void pass1();
    void pass2();
    void pass3();
    void pass4(); 
    void pass5();
    /*
    void pass6();
    void pass7();
    void pass8();
    */
    float gauss(float, float);
    void computeLogAveLuminance();
    
    //void updateLight();

    void drawScene(GLSLProgram&, bool); 
    

public:
    SceneBasic_Uniform();

    void initScene();
    void update( float t );
    void render();
    void resize(int, int);
};

#endif // SCENEBASIC_UNIFORM_H
