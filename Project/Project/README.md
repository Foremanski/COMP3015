# **COMP3015 - Games Graphics Pipeline C2 Project: Glowing Wave Tool**

#  Project Settings

- Visual Studio Version 2019
- Operating System: Windows 10 Enterprise

# Project Guide

- To run the project, simply run the ".exe" file. The render will automatically play. 
- To explore the code, click on the "Project_Template" folder and then open "Project_Template.sln"

# Controls

- Q & E - Adjust Wave Speed
- A & D - Adjust Wave Height
- Z & C - Adjust Bloom Intensity

- R,G,B - Set wave colour to red, green or blue respectively

# Project Description

This project aims to be a tool that can be used to set up and control glowing waves. This could be implemented in other
projects to simulate toxic liquid or other such exotic materials.

Below will be basic descriptions of what the core functions do:

## Scenebasic_uniform.h and Scenebasic_uniform.cpp

# Scenebasic_uniform.h
Header files for all of Scenebasic.cpp's functions. 

- Private and public methods are declared
- Private variables generally handle Bloom processing

- Declares Variables that control the wave's input
- Declares Variables that are used in Bloom processing

# Scenebasic_uniform.cpp
File that handles the generation of render.

#  initScene()
Handles inital setup of the scene, called only once

- Compiles the 2 shaders
- Sets initial variables for Lights, Wave Controls and Wave Materials
- Computes the weights of Gauss Blur
- Sets up the frame buffer object (FBO)
- Provides an array for Full Sceen Quad 
- Creates Sampler objects for Liniear and Nearest filtering

# setupFBO()
- Sets up Frame Buffer Object
- Creates and binds Depth Buffer
- Creates and binds a blur FBO for bright pass an blur


# render()

# Pass1 - Pass5()
Handles the buffer side of bloom effect

- Pass1() - Draws an inital scene prior to bloom with just blingphong lighting
- Pass5() - Changes to linear sampling to get extra blur

# computeLogAveLuminance
Computes the average luminance
- 

# gauss()
- returns gaussian blur calculation

# drawScene()
- sets lighting position and gets lighting intensity from user input
- sets object position
- renders the plane object 

# Update() 
- Handles User Input and setting Time

## BloomShader.vert and BloomShader.frag
This shader handles the bloom effect.

# The Fragment Shader 

- This handles the 5 passes needed to produce the bloom effect 
- main() calls each pass using a uniform set within scenebasic_uniform.cpp
- BlinnPhong Implements a simple BlingPhong Lighting calculation

- Pass1() - gets the pixel color after BlingPhong
- Pass2() - applies the brightness to the pixel, multiplying the rgb value to increase the luminance
- Pass3() & Pass4() - Applies the a guassian blur function on a Y axis and X axis
- Pass5() - handles  tone mapping, converting the pixel's RGB values to XYZ and ensuring the pixel's brightness doesn't exceed 1

# The Vertex Shader 

- Gets the VertexPosition, Normal Value and Texture coordinate
- Translates that into out vectors for the fragment shader
- Gives that to the fragment shader

## WaveShader.vert and WaveShader.frag

This shader manipulates the object's vertices to create a wave effect

# The Fragment Shader 

- Implements a simple bling phong function.

# The Vertex Shader 

- The vertex shader implements the main function. 
- Takes the Y position of plane's current vertex and applies a sin wave to it. Making the vertex go up and down. 

## Helper File

The Helper file contains the basic functions for any objects and textures loaded into the scene, as well as the functions of any extensions such as glsl.


# Video Link
