# **COMP3015 - Games Graphics Pipeline C1 Project**

#  Project Settings

- Visual Studio Version 2019
- Operating System: Windows 10 Enterprise

# Project Guide

- To run the project, simply run "Project_Template.exe". The render will appear automatically. 
- To explore the code, click on the "Project_Templace" folder and then open "Project_Template.sln"

# Project Description

This project aims to show a Basic Graphics pipeline primarily using OpenGl and it's extensions:
- glut
- glad
- glsl

Below will be basic descriptions of what the core functions do

## Scenebasic_uniform.h and Scenebasic_uniform.cpp

These files are where the scene is initialised and rendered, calling the various functions of any model loaded. 

- The header file **(.h)** includes any rendered object (in my example, an ObjMesh Ogre and 3 Planes) and methods.

The **.cpp** file starts with a constructor of all the loaded objects. 
- **initScene()** will initialise the scene. Setting the position of the camera and projection, as well as the uniforms of the point light, spotlight and the fog
- **Compile()** loads the vertex shader and Fragment shader
- **update(float t)** isn't used in this project, but can be used to track actions beyond the initial render. Such as input from the user.
- **render()** handles the loaded objects. It sets their position, material properties, retrieves and binds their textures and calls their render function
- **setMatrices()** sets the model view matrix
- **resize(int w, int h)** will resize the projection according to a width and height input

## PhongModel.vert and Phong Model.frag

These are the vertex and fragment shaders. 

The **Vertex Shader** transforms the vertex positions of the objects into a 3d co-ordinate space. It is called once per vertex
- The layout variables are the in-variables, taking in the properties of a vertex
- The out variables are then declared, these will be given to the Fragment Shader
- The unifrms for the model view matrix and light are then given
- **main()** processes the information, projecting the vertex position using the model view martix with "gl_Position"

The **Fragment Shader** defines the RGB values on a pixel by pixel basis. In calculates this value using a BlingPhong shading model and the colour of texture co-ordinates

- in-variables are declared: Light and material structs are taken in, as well as the appropriate texture for calculation
- Output is the colour of the pixel
- **BlinnPhong(vec4 position, vec3 n, int light)** uses the in-variable info to calculate an Ambient, Diffuse and Specular component. Combining that with the texture colour.
- main() combines a fog calculation, BlingPhong() output, and the texture's normal setting to output the frag colour

## Helper and Media File

The Helper file contains the basic functions for any objects and textures loaded into the scene, as well as the functions of any extensions such as glsl.
The Media Folder contains the models and textures used in the project.

# Video Link

