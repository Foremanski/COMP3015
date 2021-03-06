#include "scenebasic_uniform.h"

#include <sstream>
#include <iostream>
using std::cerr;
using std::endl;

#include <glm/gtc/matrix_transform.hpp>
using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;
#include <Windows.h>

//constructor for torus
//torus(0.7f, 0.3f, 50, 50) {}
//constructor for teapot
//teapot(13, glm::translate(mat4(1.0f), vec3(0.0f, 1.5f, 0.25f))) {}

SceneBasic_Uniform::SceneBasic_Uniform() : time(0), plane(40.0f, 40.0f, 100, 100) {}

void SceneBasic_Uniform::initScene()
{
    compile();
    prog.use();
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    glEnable(GL_DEPTH_TEST);
    
    vec3 intense = vec3(0.3f);
    prog.setUniform("Lights[0].L", intense);
    //prog.setUniform("Lights[1].L", intense);
    //prog.setUniform("Lights[2].L", intense);

    intense = vec3(0.2f);
    prog.setUniform("Lights[0].La", intense);
    //prog.setUniform("Lights[1].La", intense);
    //prog.setUniform("Lights[2].La", intense);
    
    waveProg.use();
    waveProg.setUniform("Material.Kd", 0.3f, 0.3f, 0.3f);
    waveProg.setUniform("Material.Ks", 0.3f, 0.3f, 0.3f);
    waveProg.setUniform("Material.Ka", 0.9f, 0.3f, 0.3f);
    waveProg.setUniform("Material.Shininess", 10.0f);
    prog.use();

    projection = mat4(4.0f);

    angle = glm::pi<float>() / 2.0f;

    VCounter = 0.5f;
    ECounter = 0.35;
    ACounter = 0.7;

    setupFBO();

    // Array for full-screen quad
    GLfloat verts[] = {
    -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
    -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f
    };
    GLfloat tc[] = {
    0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f
    };


    // Set up the buffers
    unsigned int handle[2];
    glGenBuffers(2, handle);
    glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    glBufferData(GL_ARRAY_BUFFER, 6 * 3 * sizeof(float), verts, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
    glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), tc, GL_STATIC_DRAW);

    // Set up the vertex array object
    glGenVertexArrays(1, &fsQuad);
    glBindVertexArray(fsQuad);

    glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0); // Vertex position

    glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
    glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2); // Texture coordinates

    glBindVertexArray(0);

    prog.setUniform("LumThresh", 1.7f);

    float weights[10], sum, sigma2 = 25.0f;

    // Compute and sum the weights
    weights[0] = gauss(0, sigma2);
    sum = weights[0];

    for (int i = 1; i < 10; i++) 
    {
        weights[i] = gauss(float(i), sigma2);
        sum += 2 * weights[i];
    }

    // Normalize the weights and set the uniform
    for (int i = 0; i < 10; i++) 
    {
        std::stringstream uniName;
        uniName << "Weight[" << i << "]";
        float val = weights[i] / sum;
        prog.setUniform(uniName.str().c_str(), val);
    }

    // Set up two sampler objects for linear and nearest filtering
    GLuint samplers[2];
    glGenSamplers(2, samplers);
    linearSampler = samplers[0];
    nearestSampler = samplers[1];

    GLfloat border[] = { 0.0f,0.0f,0.0f,0.0f };

    // Set up the nearest sampler
    glSamplerParameteri(nearestSampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glSamplerParameteri(nearestSampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glSamplerParameteri(nearestSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glSamplerParameteri(nearestSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glSamplerParameterfv(nearestSampler, GL_TEXTURE_BORDER_COLOR, border);

    // Set up the linear sampler
    glSamplerParameteri(linearSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(linearSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(linearSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glSamplerParameteri(linearSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glSamplerParameterfv(linearSampler, GL_TEXTURE_BORDER_COLOR, border);

    // Nearest Sampling until last pass
    glBindSampler(0, nearestSampler);
    glBindSampler(1, nearestSampler);
    glBindSampler(2, nearestSampler);
}

void SceneBasic_Uniform::setupFBO()
{
    // Create and bind the FBO
    glGenFramebuffers(1, &hdrFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFbo);

    // Create the texture object
    glGenTextures(1, &hdrTex);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, width, height);

    // Bind the texture to the FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hdrTex, 0);

    // Create the depth buffer
    GLuint depthBuf;
    glGenRenderbuffers(1, &depthBuf);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

    // Bind the depth buffer to the FBO
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuf);

    // Set the targets for the fragment output variables
    GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, drawBuffers);

    // Create an FBO for the bright-pass filter and blur
    glGenFramebuffers(1, &blurFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, blurFbo);

    // Create two texture objects to ping-pong for the bright-pass filter
    // and the two-pass blur
    bloomBufWidth = width / 8;
    bloomBufHeight = height / 8;

    glGenTextures(1, &tex1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tex1);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, bloomBufWidth, bloomBufHeight);
    glActiveTexture(GL_TEXTURE2);
    glGenTextures(1, &tex2);
    glBindTexture(GL_TEXTURE_2D, tex2);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, bloomBufWidth, bloomBufHeight);

    // Bind tex1 to the FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex1, 0);
    glDrawBuffers(1, drawBuffers);

    // Unbind the framebuffer, and revert to default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void SceneBasic_Uniform::render()
{
    pass1();
    computeLogAveLuminance();
    pass2();
    pass3();
    pass4();
    pass5();
}

void SceneBasic_Uniform::pass1()
{
    prog.use();
    prog.setUniform("Pass", 1);

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glViewport(0, 0, width, height);

    glBindFramebuffer(GL_FRAMEBUFFER, hdrFbo);   
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    //set camera angle and projection
    view = glm::lookAt(vec3(5.0f, 5.0f, 5.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    projection = glm::perspective(glm::radians(60.0f), (float)width / height, 0.3f, 100.0f);

    drawScene();
}

//Blur FBO to tex1
void SceneBasic_Uniform::pass2()
{
    prog.use();
    prog.setUniform("Pass", 2);
    glBindFramebuffer(GL_FRAMEBUFFER, blurFbo);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex1, 0);
    glViewport(0, 0, bloomBufWidth, bloomBufHeight);
    glDisable(GL_DEPTH_TEST);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    model = mat4(1.0f);
    view = mat4(1.0f);
    projection = mat4(1.0f);
    setMatrices();

    // Render full-screen quad
    glBindVertexArray(fsQuad);
    glDrawArrays(GL_TRIANGLES, 0, 6);

}

//tex 2
void SceneBasic_Uniform::pass3()
{
    prog.use();
    prog.setUniform("Pass", 3);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex2, 0);

    // Render the full-screen quad
    glBindVertexArray(fsQuad);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void SceneBasic_Uniform::pass4()
{
    prog.use();
    prog.setUniform("Pass", 4);

    // Write to Tex1
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex1, 0);

    // Render the full-screen quad
    glBindVertexArray(fsQuad);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

//write to the screen
void SceneBasic_Uniform::pass5()
{
    prog.use();
    prog.setUniform("Pass", 5);

    // Bind to the default framebuffer, this time we're going to
    // actually draw to the screen!
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, width, height);

    // In this pass, we're reading from tex1 (unit 1) and we want
    // linear sampling to get an extra blur
    glBindSampler(1, linearSampler);

    // Render the full-screen quad
    glBindVertexArray(fsQuad);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Revert to nearest sampling
    glBindSampler(1, nearestSampler);

}

void SceneBasic_Uniform::computeLogAveLuminance()
{
    prog.use();
    int size = width * height;
    std::vector<GLfloat> texData(size * 3);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTex);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, texData.data());

    float sum = 0.0f;
    for (int i = 0; i < size; i++) 
    {
        float lum = glm::dot(vec3( texData[i * 3 + 0], texData[i * 3 + 1], texData[i * 3 + 2]), vec3(0.2126f, 0.7152f, 0.0722f));

        sum += logf(lum + 0.00001f);
    }

    prog.setUniform("AveLum", expf(sum / size));
}

float SceneBasic_Uniform::gauss(float x, float sigma2)
{
    double coeff = 1.0 / (glm::two_pi<double>() * sigma2);
    double expon = -(x * x) / (2.0 * sigma2);
    return (float)(coeff * exp(expon));
}

void SceneBasic_Uniform::drawScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    prog.use();

    vec4 lightPos = vec4(0.0f, 10.0f, 0.0f, 0.0f);   
    vec3 intense = vec3(ECounter);

    waveProg.use();
    waveProg.setUniform("Time", time);

    waveProg.setUniform("Light.L", intense);
    waveProg.setUniform("Light.La", intense);
    waveProg.setUniform("Light.LightPosition", view * lightPos);

    //render plane
    model = mat4(1.0f);
    model = glm::translate(model, vec3(-10.0f, -10.45f, -10.0f));
    setMatrices();
    waveProg.use();
    plane.render();

    prog.use();
}

void SceneBasic_Uniform::compile()
{
    try {
        prog.compileShader("shader/BloomShader.vert");
        prog.compileShader("shader/BloomShader.frag");
        prog.link();

        waveProg.compileShader("shader/WaveShader.vert");
        waveProg.compileShader("shader/WaveShader.frag");
        waveProg.link();

        prog.use();
    }
    catch (GLSLProgramException& e) {
        cerr << e.what() << endl;
        exit(EXIT_FAILURE);
    }
}

void SceneBasic_Uniform::update(float t)
{
    time = t;  

    //set Wave speeds
    if (GetKeyState('E') & 0x8000)
    {
        VCounter = VCounter + 0.2;

        waveProg.use();
        waveProg.setUniform("Velocity", VCounter);
    }

    if (GetKeyState('Q') & 0x8000)
    {
        VCounter = VCounter - 0.2;

        waveProg.use();
        waveProg.setUniform("Velocity", VCounter);
    }

    if (GetKeyState('D') & 0x8000)
    {
        ACounter = ACounter + 0.2;

        waveProg.use();
        waveProg.setUniform("Amp", ACounter);
    }

    if (GetKeyState('A') & 0x8000)
    {
        ACounter = ACounter - 0.2;

        waveProg.use();
        waveProg.setUniform("Amp", ACounter);
    }

    //set bloom intensity
    if (GetKeyState('C') & 0x8000)
    {
        ECounter = ECounter + 0.05;
    }

    if (GetKeyState('Z') & 0x8000)
    {
        ECounter = ECounter - 0.05;
    }
    
    //Set RGB
    if (GetKeyState('R') & 0x8000)
    {
        waveProg.use();

        waveProg.setUniform("Material.Kd", 0.3f, 0.3f, 0.3f);
        waveProg.setUniform("Material.Ks", 0.3f, 0.3f, 0.3f);
        waveProg.setUniform("Material.Ka", 0.9f, 0.3f, 0.3f);
    }

    if (GetKeyState('G') & 0x8000)
    {
        waveProg.use();

        waveProg.setUniform("Material.Kd", 0.3f, 0.3f, 0.3f);
        waveProg.setUniform("Material.Ks", 0.3f, 0.3f, 0.3f);
        waveProg.setUniform("Material.Ka", 0.3f, 0.9f, 0.3f);
    }
    if (GetKeyState('B') & 0x8000)
    {
        waveProg.use();

        waveProg.setUniform("Material.Kd", 0.3f, 0.3f, 0.3f);
        waveProg.setUniform("Material.Ks", 0.3f, 0.3f, 0.3f);
        waveProg.setUniform("Material.Ka", 0.3f, 0.3f, 0.9f);
    }

}


void SceneBasic_Uniform::setMatrices()
{
    prog.use();

    mat4 mv = view * model; //we create a model view matrix

    prog.setUniform("ModelViewMatrix", mv); //set the uniform for the model view matrix

    prog.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2]))); //we set the uniform for normal matrix

    prog.setUniform("MVP", projection * mv); //we set the model view matrix by multiplying the mv with the projection matrix

    waveProg.use();

    waveProg.setUniform("ModelViewMatrix", mv); //set the uniform for the model view matrix

    waveProg.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2]))); //we set the uniform for normal matrix

    waveProg.setUniform("MVP", projection * mv); //we set the model view matrix by multiplying the mv with the projection matrix

    prog.use();
}

void SceneBasic_Uniform::resize(int w, int h)
{
    glViewport(0, 0, w, h);
    width = w;
    height = h;
    projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.0f);
}
