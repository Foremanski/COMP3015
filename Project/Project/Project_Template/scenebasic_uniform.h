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

    GLuint fsQuad;
    GLuint hdrFbo, blurFbo;
    GLuint hdrTex, tex1, tex2;
    GLuint linearSampler, nearestSampler;

<<<<<<< HEAD

=======
    GLuint spotTex;
    //glm::vec4 lightPos;
>>>>>>> parent of eb51001... Fixes to shadow volume
    //GLuint brickTexture, concreteTexture;

    Plane plane;
    Sphere sun;
    //Torus torus;

    float angle;
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
<<<<<<< HEAD
=======
    
    //void updateLight();
>>>>>>> parent of eb51001... Fixes to shadow volume

    void drawScene(); 

public:
    SceneBasic_Uniform();

    void initScene();
    void update( float t );
    void render();
    void resize(int, int);
};

#endif // SCENEBASIC_UNIFORM_H
