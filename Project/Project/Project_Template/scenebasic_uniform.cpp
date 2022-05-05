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

//constructor for Ogre
SceneBasic_Uniform::SceneBasic_Uniform() : plane(10.0f, 20.0f, 1, 1), plane2(20.0f, 20.0f, 1, 1), plane3(20.0f, 20.0f, 1, 1)
{
    ogre = ObjMesh::load("media/bs_ears.obj", false, true);


}

void SceneBasic_Uniform::initScene()
{
    compile();
	glEnable(GL_DEPTH_TEST);
  
    //set camera angle and projection
    view = glm::lookAt(vec3(2.0f, 0.0f, 2.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    projection = mat4(1.0f);
    //set light position coords
    vec4 lightPos = vec4(10.0f, 5.0f, -5.0f, 0.0f);


     //light uniforms
    prog.setUniform("Light.Ld", 0.1f, 0.1f, 0.1f);
    prog.setUniform("Light.La", 0.4f, 0.4f, 0.4f);
    prog.setUniform("Light.Ls", 0.3f, 0.3f, 0.3f);
    prog.setUniform("Light.LightPosition", lightPos);

    //spotlight uniforms
    prog.setUniform("Spot.L", vec3(0.9f));
    prog.setUniform("Spot.La", vec3(1.0f));
    prog.setUniform("Spot.Exponent", 1.0f);
    prog.setUniform("Spot.Cutoff", glm::radians(1.0f));  
    prog.setUniform("Spot.SpotPosition", vec4(1.0f, 7.0f, 1.0f, 0.0f));
    prog.setUniform("Spot.Direction", vec4(0.0f, 0.0f, 0.0f, 0.0f));

    //fog uniforms
    prog.setUniform("Fog.MaxDist", 5.0f);
    prog.setUniform("Fog.MinDist", 1.0f);
    prog.setUniform("Fog.Color", vec3(171.0f, 171.0f, 171.0f));
}

void SceneBasic_Uniform::compile()
{
	try {
		prog.compileShader("shader/PhongModel.vert");
		prog.compileShader("shader/PhongModel.frag");
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //make sure you use the correct name, check your vertex shader
    //material uniforms
    prog.setUniform("Material.Kd", 0.4f, 0.4f, 0.4f);
    prog.setUniform("Material.Ks", 0.95f, 0.95f, 0.95f);
    prog.setUniform("Material.Ka", 0.5f, 0.5f, 0.5f);
    prog.setUniform("Material.Shininess", 180.0f);
   
    //ogre textures
    GLuint ogreDiffuse = Texture::loadTexture("media/texture/ogre_diffuse.png");
    GLuint ogreNormal = Texture::loadTexture("media/texture/ogre_normalmap.png");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ogreDiffuse);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, ogreNormal);

    //render ogre model
    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(45.0f), vec3(0.0f, 1.0f, 0.0f));
    setMatrices();
    ogre->render();

    //concrete floor texture
    GLuint concreteTexture = Texture::loadTexture("media/texture/cement.jpg");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, concreteTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, GL_CLEAR);

    //concrete & brick material uniforms
    prog.setUniform("Material.Kd", 0.6f, 0.6f, 0.6f);
    prog.setUniform("Material.Ks", 0.05f, 0.05f, 0.05f);
    prog.setUniform("Material.Ka", 0.5f, 0.5f, 0.5f);
    prog.setUniform("Material.Shininess", 1.0f);
    
    //render concrete floor
    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, -1.45f, 0.0f));
    setMatrices(); 
    plane.render();

    //brick wall texture
    GLuint brickTexture = Texture::loadTexture("media/texture/brick1.jpg");
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, brickTexture);


    //render brick walls
    model = glm::translate(model, vec3(0.0f, 10.0f, -5.0f));
    model = glm::rotate(model, glm::radians(90.0f), vec3(1.0f, 0.0f, 0.0f));

    setMatrices();
    plane2.render();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, brickTexture);

    model = glm::translate(model, vec3(-5.0f, 10.0f, 0.0f));
    model = glm::rotate(model, glm::radians(90.0f), vec3(0.0f, 0.0f, 1.0f));

    setMatrices();
    plane3.render();

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


