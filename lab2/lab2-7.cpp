
#define MAIN
#include "GL_utilities.h"
#include "MicroGlut.h"
#include <math.h>
#include "LittleOBJLoader.h"
#include "LoadTGA.h"
#include "VectorUtils4.h"

GLfloat t;
Model *bunny;
Model *cube;
GLuint myTex; 


// Projection matrix
#define near 1.0

#define far 30.0

#define right 0.5

#define left -0.5

#define top 0.5

#define bottom -0.5

GLfloat projectionMatrix[] = {    2.0f*near/(right-left), 0.0f, (right+left)/(right-left), 0.0f,

                                            0.0f, 2.0f*near/(top-bottom), (top+bottom)/(top-bottom), 0.0f,

                                            0.0f, 0.0f, -(far + near)/(far - near), -2*far*near/(far - near),

                                            0.0f, 0.0f, -1.0f, 0.0f };


// Reference to shader
GLuint shader;

void init(void)
{
	dumpInfo();
	printError("init shader");


	bunny = LoadModel("bunnyplus.obj");
	cube = LoadModel("cubeplus.obj");

	// Load and compile shader
	shader = loadShaders("lab2-7.vert", "lab2-7.frag");
	
	// GL inits
	glClearColor(0.8,0.5,0.5,0);
	
	// Buffers
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	//glDisable(GL_DEPTH_TEST); // Disable z-buffer makes it "see-through"
	glDisable(GL_CULL_FACE);
	printError("GL icnits");

	

	// Load texture
	LoadTGATextureSimple("maskros512.tga", &myTex);
	glBindTexture(GL_TEXTURE_2D, myTex);
	glUniform1i(glGetUniformLocation(shader, "texUnit"), 0); // Texture unit 0
	glActiveTexture(GL_TEXTURE0);


	// Upload projection matrix
	glUniformMatrix4fv(glGetUniformLocation(shader, "projectionMatrix"), 1, GL_TRUE, projectionMatrix);

	//Upload lookat matrix
	mat4 lookAtMatrix = lookAtv(vec3(0, 0, 1), vec3(0, 0, 0), vec3(0, 1, 0));
	glUniformMatrix4fv(glGetUniformLocation(shader, "lookAtMatrix"), 1, GL_TRUE, lookAtMatrix.m);

	glutRepeatingTimer(50);
	
	printError("init arrays");
}


void display(void)
{
	printError("pre display");

	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Get Time
	t = (GLfloat)glutGet(GLUT_ELAPSED_TIME);

	float angle = 0.001f * t;

	GLfloat myMatrix[] = {
    1.0f, 0.0f,        0.0f,       0.0f,
    0.0f, cos(angle), -sin(angle),  0.0f,
    0.0f, sin(angle),  cos(angle),  -3.0f,
    0.0f, 0.0f,        0.0f,       1.0f
	}; // Rotate around x-axis
					

	// Upload matrix and draw bunny
	glUniformMatrix4fv(glGetUniformLocation(shader, "myMatrix"), 1, GL_TRUE, myMatrix);
	DrawModel(bunny, shader, "in_Position", "in_Normal", "in_Texture");
	
	GLfloat myMatrix2[] = {
    1.0f, 0.0f,        0.0f,       1.0f,
    0.0f, cos(angle), -sin(angle),  0.0f,
    0.0f, sin(angle),  cos(angle),  -3.0f,
    0.0f, 0.0f,        0.0f,       1.0f
	}; // Rotate around x-axis a bit to the side.
					

	// Upload matrix and draw cube
	glUniformMatrix4fv(glGetUniformLocation(shader, "myMatrix"), 1, GL_TRUE, myMatrix2);
	
	DrawModel(cube, shader, "in_Position", "in_Normal", "in_Texture");

	printError("display");
	
	
	glutSwapBuffers();
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitContextVersion(3, 2);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH); // Call init displaymode early
	glutInitWindowSize(600, 600);
	glutCreateWindow ("white triangle example");
	glutDisplayFunc(display); 
	init ();
	glutMainLoop();
	return 0;
}
