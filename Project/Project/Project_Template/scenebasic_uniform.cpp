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



//constructor for torus
//torus(0.7f, 0.3f, 50, 50) {}
//constructor for teapot
//teapot(13, glm::translate(mat4(1.0f), vec3(0.0f, 1.5f, 0.25f))) {}

SceneBasic_Uniform::SceneBasic_Uniform() : sun(0.7f,30,30), plane(40.0f, 40.0f, 1, 1) {}

void SceneBasic_Uniform::initScene()
{
    compile();

    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);

	glEnable(GL_DEPTH_TEST);

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

    prog.setUniform("Light.L", vec3(0.2f));
    prog.setUniform("Light.La", vec3(0.5f));

    float weights[5], sum, sigma2 = 8.0f;

    // Compute and sum the weights
    weights[0] = gauss(0, sigma2);
    sum = weights[0];

    for (int i = 1; i < 5; i++)
    {
        weights[i] = gauss(float(i), sigma2);
        sum += 2 * weights[i];
    }

    // Normalize the weights and set the uniform
    for (int i = 0; i < 5; i++)
    {
        std::stringstream uniName;
        uniName << "Weight[" << i << "]";
        float val = weights[i] / sum;
        prog.setUniform(uniName.str().c_str(), val);
    }

    //concreteTexture = Texture::loadTexture("media/texture/hardwood2_diffuse.jpg");
    //brick wall texture
    //brickTexture = Texture::loadTexture("media/texture/brick1.jpg");
}

void SceneBasic_Uniform::compile()
{
	try {
		prog.compileShader("shader/BlurShader.vert");
		prog.compileShader("shader/BlurShader.frag");
		prog.link();
		prog.use();
	} catch (GLSLProgramException &e) {
		cerr << e.what() << endl;
		exit(EXIT_FAILURE);
	}
}

void SceneBasic_Uniform::update( float t )
{
	
}

void SceneBasic_Uniform::render()
{
    pass1();
    pass2();
    pass3();
}

void SceneBasic_Uniform::setMatrices()
{
    mat4 mv = view * model; //we create a model view matrix
    
    prog.setUniform("ModelViewMatrix", mv); //set the uniform for the model view matrix
    
    prog.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2]))); //we set the uniform for normal matrix
    
    prog.setUniform("MVP", projection * mv); //we set the model view matrix by multiplying the mv with the projection matrix
}

void SceneBasic_Uniform::resize(int w, int h)
{
    glViewport(0, 0, w, h);
    width = w;
    height = h;
    projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.0f);
}

void SceneBasic_Uniform::setupFBO()
{
    // Generate and bind the framebuffer
    glGenFramebuffers(1, &renderFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, renderFBO);

    // Create the texture object
    glGenTextures(1, &renderTex);
    glBindTexture(GL_TEXTURE_2D, renderTex);

    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

    // Bind the texture to the FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTex, 0);

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

    // Unbind the framebuffer, and revert to default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Generate and bind the framebuffer
    glGenFramebuffers(1, &intermediateFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, intermediateFBO);

    // Create the texture object
    glGenTextures(1, &intermediateTex);
    glActiveTexture(GL_TEXTURE0); // Use texture unit 0
    glBindTexture(GL_TEXTURE_2D, intermediateTex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

    // Bind the texture to the FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, intermediateTex, 0);

    // No depth buffer needed for this FBO
    // Set the targets for the fragment output variables
    glDrawBuffers(1, drawBuffers);

    // Unbind the framebuffer, and revert to default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SceneBasic_Uniform::pass1()
{
    prog.setUniform("Pass", 1);

    glBindFramebuffer(GL_FRAMEBUFFER, renderFBO);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    //set camera angle and projection
    view = glm::lookAt(vec3(5.0f, 5.0f, 5.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    projection = glm::perspective(glm::radians(60.0f), (float)width / height, 0.3f, 100.0f);

    //set light position coords
    vec4 lightPos = vec4(10.0f, 5.0f, 5.0f, 0.0f);
    //light uniforms
    prog.setUniform("Light.LightPosition", lightPos);
   

    prog.setUniform("Material.Kd", 0.6f, 0.6f, 0.6f);
    prog.setUniform("Material.Ks", 0.05f, 0.05f, 0.05f);
    prog.setUniform("Material.Ka", 0.5f, 0.5f, 0.5f);
    prog.setUniform("Material.Shininess", 1.0f);

    //render concrete floor
    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, -1.45f, 0.0f));
    setMatrices();
    plane.render();

    //concretes
    prog.setUniform("Material.Kd", 0.7f, 0.6f, 0.2f);
    prog.setUniform("Material.Ks", 0.05f, 0.03f, 0.1f);
    prog.setUniform("Material.Ka", 0.5f, 0.4f, 0.4f);
    prog.setUniform("Material.Shininess", 1.0f);

    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, 0.0f, 0.0f));
    setMatrices();
    sun.render();
}

void SceneBasic_Uniform::pass2()
{
    prog.setUniform("Pass", 2);

    glBindFramebuffer(GL_FRAMEBUFFER, intermediateFBO);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, renderTex);

    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);

    model = mat4(1.0f);
    view = mat4(1.0f);
    projection = mat4(1.0f);
    setMatrices();

    // Render the full-screen quad
    glBindVertexArray(fsQuad);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void SceneBasic_Uniform::pass3()
{
    prog.setUniform("Pass", 3);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, intermediateTex);
    glClear(GL_COLOR_BUFFER_BIT);

    model = mat4(1.0f);
    view = mat4(1.0f);
    projection = mat4(1.0f);
    setMatrices();

    // Render the full-screen quad
    glBindVertexArray(fsQuad);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

float SceneBasic_Uniform::gauss(float x, float sigma2)
{
    double coeff = 1.0 / (glm::two_pi<double>() * sigma2);
    double expon = -(x * x) / (2.0 * sigma2);
    return (float)(coeff * exp(expon));
}