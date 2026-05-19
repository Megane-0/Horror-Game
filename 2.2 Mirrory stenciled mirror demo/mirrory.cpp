// Mirror effect clipped by using the stencil buffer.
// By Ingemar Ragnemalm 2013

// Partially based on code from Fabien Sanglard's shadow mapping
// demos, but very little remains of that. You may recognise a few
// variable names.

// Algorithm:
// 1. Clear the stencil buffer
// 2. Draw the ground ONLY in stencil
// 3. Draw the mirrored teapot (behind the ground), masked by stencil buffer
// 4. Draw the ground with some transparency, to make the mirrored geometry visible
// 5. Draw the teapot in a position matching the mirrored one
// Thus, the reflectivity of the mirror is determined by its transparency.
// Note that back-face culling must be reversed (culling fronts) for the mirrored
// geometry since it has been reversed.

// This method has some limitations and drawbacks:
// - Obviously, the mirroring surface must be flat. (Otherwise, use cube mapping, rendering the scene to the cube map in every frame.)
// - Geometry penetrating the surface must be dealt with, cutting it along the surface.

#define MAIN
#include "MicroGlut.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "LoadTGA.h"
#include "VectorUtils4.h"
#include "GL_utilities.h"
#include "LittleOBJLoader.h"
// uses framework Cocoa
// uses framework OpenGL

// Expressed as float so gluPerspective division returns a float and not 0 (640/480 != 640.0/480.0).
#define RENDER_WIDTH 640.0
#define RENDER_HEIGHT 480.0

//Camera position
vec3 p_camera = vec3(42,5,-30);

//Camera lookAt
vec3 l_camera = vec3(0,0,0);

//vec3 potPos = {0, 4, -16};
vec3 potPos = vec3(0, 3, -10);

// Shader reference
GLuint texturedShaderId;

//-----------------------------matrices------------------------------
mat4	modelViewMatrix, textureMatrix;
mat4 projectionMatrix;
GLuint tex0, tex1; // Texture references

void loadShader()
{
	texturedShaderId = loadShaders("tex.vert", "tex.frag");
}

// This update only change the position of the light.
void updatePositions(void)
{
	potPos.y = 3.0 + 3.0 * sin(glutGet(GLUT_ELAPSED_TIME)/500.0);
}

// Arrays for ground and the Model references

GLfloat groundcolor[] = {0.3f,0.3f,0.3f,1};
vec3 ground[] = {
							vec3(-25, -25,0),
							vec3(-25, 25, 0),
							vec3( 25, 25, 0),
							vec3( 25, -25, 0)
							};
vec2 groundtex[] = {
							vec2(-1,-1),
							vec2(-1, 1),
							vec2( 1, 1),
							vec2( 1,-1)
							};
GLuint groundIndices[] = {0, 1, 2, 0, 2, 3};

Model *groundModel, *teapotModel;

void loadObjects(void)
{
	// Load grund from inline arrays
	groundModel = LoadDataToModel(
			ground,
			NULL,
			groundtex,
			NULL,
			groundIndices,
			4,
			6);

	// Load teapot model from OBJ
	teapotModel = LoadModel("teapot.obj");
}


void renderScene()
{
	mat4 mv2, tx2, trans;

	// Change light positions
	updatePositions();
	
	// Setup projection matrix
	projectionMatrix = perspective(45, RENDER_WIDTH/RENDER_HEIGHT, 1, 4000);
	// Setup the modelview from the light source
	modelViewMatrix = lookAt(p_camera.x, p_camera.y, p_camera.z,
											l_camera.x, l_camera.y, l_camera.z, 0,1,0);

	glViewport(0,0,RENDER_WIDTH,RENDER_HEIGHT);
	
	glUseProgram(texturedShaderId);
	glUniform1f(glGetUniformLocation(texturedShaderId, "blend"), 1.0); // Blend value
	glUniformMatrix4fv(glGetUniformLocation(texturedShaderId, "projectionMatrix"), 1, GL_TRUE, projectionMatrix.m);
	
	// Algorithm:
	// 1. Clear the stencil buffer
	// 2. Draw the ground ONLY in stencil
	// 3. Draw the mirrored teapot (behind the ground), masked by stencil buffer
	// 4. Draw the ground with some transparency, to make the mirrored geometry visible
	// 5. Draw the teapot in a position matching the mirrored one
	
	// Start of interesting stuff: Clear stencil, then set up to draw in it.
	glClearColor(0,0.1,0.3,1.0f); // Not black - we want to see if the mirrored image is drawn outside the plane!
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // Clear Screen, Depth Buffer and Stencil Buffer
	glClearStencil(2); // No, stencil must be set to 2 outside the plane or it will draw double shadows there!
	
	// ** Ground in stencil
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 1, 0xFFFFFFFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE); // Write 1 if pass! Otherwise keep! Don't draw in invisible areas.

	// Turn off Color Buffer and Depth Buffer
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_FALSE);
	
	glBindTexture(GL_TEXTURE_2D, tex0);
	glUniformMatrix4fv(glGetUniformLocation(texturedShaderId, "modelViewMatrix"), 1, GL_TRUE, modelViewMatrix.m);
	DrawModel(groundModel, texturedShaderId, "in_Position", NULL, "in_Texcoord");
	
	// ** Teapot mirrored - with stencil!

	// Turn on Color Buffer and Depth Buffer
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthMask(GL_TRUE);
	
	glEnable(GL_STENCIL_TEST); // Try disabling this!
	glStencilFunc(GL_EQUAL, 1, 0xFFFFFFFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_INCR); // Skriv 1 om pass! Annars keep! Vi vill inte rita i osynlig yta!
	glCullFace(GL_FRONT); // Since the teapot is mirrored, culling must be reversed

	mat4 sm = S(1, 1, -1);
	sm = sm * T(potPos.x, potPos.y, potPos.z); // Place teapot before projecting (that is to the right)
	mv2 = modelViewMatrix * sm;
	glBindTexture(GL_TEXTURE_2D, tex1);
	glUniformMatrix4fv(glGetUniformLocation(texturedShaderId, "projectionMatrix"), 1, GL_TRUE, projectionMatrix.m);
	glUniformMatrix4fv(glGetUniformLocation(texturedShaderId, "modelViewMatrix"), 1, GL_TRUE, mv2.m);
	glUniform1f(glGetUniformLocation(texturedShaderId, "blend"), 1.0); // Blend value
	DrawModel(teapotModel, texturedShaderId, "in_Position", NULL, "in_Texcoord");

	// ** Ground in image
	glDisable(GL_STENCIL_TEST);
	glCullFace(GL_BACK); // Standard culling
	
	glBindTexture(GL_TEXTURE_2D, tex0);
	glUniform1f(glGetUniformLocation(texturedShaderId, "blend"), 0.7); // Blend value
	glUniformMatrix4fv(glGetUniformLocation(texturedShaderId, "modelViewMatrix"), 1, GL_TRUE, modelViewMatrix.m);
	DrawModel(groundModel, texturedShaderId, "in_Position", NULL, "in_Texcoord");
	
	// ** Teapot drawn normally - after the mirror!
	glDisable(GL_STENCIL_TEST);
	glBindTexture(GL_TEXTURE_2D, tex1);
	glUniform1f(glGetUniformLocation(texturedShaderId, "blend"), 1.0); // Blend value
	trans = T(potPos.x, potPos.y, potPos.z);
	mv2 = modelViewMatrix * trans;
	glUniformMatrix4fv(glGetUniformLocation(texturedShaderId, "modelViewMatrix"), 1, GL_TRUE, mv2.m);
	DrawModel(teapotModel, texturedShaderId, "in_Position", NULL, "in_Texcoord");

	glutSwapBuffers();
}

void processNormalKeys(unsigned char key, int x, int y)
{
	if (key == 27) 
		exit(0);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(RENDER_WIDTH,RENDER_HEIGHT);
	glutInitContextVersion(3, 2);
	glutCreateWindow("Stenciled mirror demo");
	
	loadShader();
	loadObjects();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	LoadTGATextureSimple("floor.tga", &tex0);
	LoadTGATextureSimple("rust.tga", &tex1);
	
	glEnable(GL_DEPTH_TEST);
	glClearColor(0,0,0,1.0f);
	glEnable(GL_CULL_FACE);
	
	glutDisplayFunc(renderScene);
	glutRepeatingTimer(20);
	glutKeyboardFunc(processNormalKeys);
	
	glutMainLoop();
	exit(0);
}

