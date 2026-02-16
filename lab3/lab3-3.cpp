
#define MAIN
#include "GL_utilities.h"
#include "MicroGlut.h"
#include <math.h>
#include "LittleOBJLoader.h"
#include "LoadTGA.h"
#include "VectorUtils4.h"

GLfloat t;
Model *blade;
Model *balcony;
Model *walls;
Model *roof;
Model *bunny;

Model *ground;

GLuint myTex; 
vec3 startCamera = vec3(0,0,30);

mat4 cameraMovement = mat4(1.0);

#define kGroundSize 100.0f

#define cameraSpeed 0.1

// Projection matrix
#define near 1.0

#define far 100.0

#define right 0.5

#define left -0.5

#define top 0.5

#define bottom -0.5

GLfloat projectionMatrix[] = {    2.0f*near/(right-left), 0.0f, (right+left)/(right-left), 0.0f,

                                            0.0f, 2.0f*near/(top-bottom), (top+bottom)/(top-bottom), 0.0f,

                                            0.0f, 0.0f, -(far + near)/(far - near), -2*far*near/(far - near),

                                            0.0f, 0.0f, -1.0f, 0.0f };
// GROUND
vec3 vertices[] =
	{
	vec3(-kGroundSize,0.0f,-kGroundSize),
	vec3(-kGroundSize,0.0f,kGroundSize),
	vec3(kGroundSize,-0.0f,-kGroundSize),
	vec3(kGroundSize,-0.0f,kGroundSize)
	};

	vec3 vertex_normals[] =
	{
	vec3(0.0f,1.0f,0.0f),
	vec3(0.0f,1.0f,0.0f),
	vec3(0.0f,1.0f,0.0f),
	vec3(0.0f,1.0f,0.0f)
	};

	vec3 ground_colors[] =
	{
	vec3(0.0f,1.0f,0.0f),
	vec3(0.0f,1.0f,0.0f),
	vec3(0.0f,1.0f,0.0f),
	vec3(0.0f,1.0f,0.0f)
	};

	vec2 tex_coords[] =
	{
	vec2(0.0f,0.0f),
	vec2(0.0f,20.0f),
	vec2(20.0f,0.0f), 
	vec2(20.0f,20.0f)
	};
	GLuint indices[] =
	{
	0, 1, 2, 1, 3, 2
	};

// Reference to shader
GLuint shader;

void init(void)
{
	dumpInfo();
	printError("init shader");


	blade = LoadModel("windmill/blade.obj");
	balcony = LoadModel("windmill/windmill-balcony.obj");
	roof = LoadModel("windmill/windmill-roof.obj");
	walls = LoadModel("windmill/windmill-walls2.obj");
	bunny = LoadModel("new models/Godzilla/godzilla.obj");
	// Load and compile shader
	shader = loadShaders("lab3-3.vert", "lab3-3.frag");
	
	// GL inits
	glClearColor(0.8,0.5,0.5,0);
	
	// Buffers
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	//glDisable(GL_DEPTH_TEST); // Disable z-buffer makes it "see-through"
	glDisable(GL_CULL_FACE);
	printError("GL icnits");


	// Upload projection matrix
	glUniformMatrix4fv(glGetUniformLocation(shader, "projectionMatrix"), 1, GL_TRUE, projectionMatrix);

	// Upload lookat matrix
	mat4 lookAtMatrix = lookAtv(startCamera, vec3(0, 7, 0), vec3(0, 1, 0));
	glUniformMatrix4fv(glGetUniformLocation(shader, "lookAtMatrix"), 1, GL_TRUE, lookAtMatrix.m);

	
	ground = LoadDataToModel(vertices, vertex_normals, tex_coords, ground_colors, indices,4,6);	
	
	
	// Load texture

	LoadTGATextureSimple("../lab2/maskros512.tga", &myTex);
	glBindTexture(GL_TEXTURE_2D, myTex);
	glUniform1i(glGetUniformLocation(shader, "texUnit"), 0); // Texture unit 0
	glActiveTexture(GL_TEXTURE0);


	glutRepeatingTimer(50);
	
	printError("init arrays");
}


void display(void)
{
	printError("pre display");

	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	t = (GLfloat)glutGet(GLUT_ELAPSED_TIME);

	float angle = 0.001f * t;


	//cameraSpeed

	if (glutKeyIsDown('a')) {
		cameraMovement = cameraMovement*T(cameraSpeed, 0, 0);
	}
	if (glutKeyIsDown('d')) {
		cameraMovement = cameraMovement*T(-cameraSpeed, 0, 0);
	}
	if (glutKeyIsDown('w')) {
		cameraMovement = cameraMovement*T(0, 0, cameraSpeed);
	}
	if (glutKeyIsDown('s')) {
		cameraMovement = cameraMovement*T(0, 0, -cameraSpeed);
	}
	if (glutKeyIsDown('q')) {
		cameraMovement = cameraMovement*T(0, cameraSpeed, 0);
	}
	if (glutKeyIsDown('e')) {
		cameraMovement = cameraMovement*T(0, -cameraSpeed, 0);
	}
	glUniformMatrix4fv(glGetUniformLocation(shader, "cameraMovement"), 1, GL_TRUE, cameraMovement.m);
	/**/
	GLfloat myMatrix[] = {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,  0.0f,
    0.0f, 0.0f,  1.0f,  0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
	}; // Rotate around y-axis
	
	// Upload matrix
	glUniformMatrix4fv(glGetUniformLocation(shader, "myMatrix"), 1, GL_TRUE, myMatrix);

	DrawModel(ground, shader, "in_Position", "in_Normal", "in_Texture");

	// Draw model
	DrawModel(balcony, shader, "in_Position", "in_Normal", "in_Texture");
	DrawModel(roof, shader, "in_Position", "in_Normal", "in_Texture");
	DrawModel(walls, shader, "in_Position", "in_Normal", "in_Texture");

	mat4 placeDowns = T(5, 0, 0);
	glUniformMatrix4fv(glGetUniformLocation(shader, "myMatrix"), 1, GL_TRUE, placeDowns.m);
	DrawModel(bunny, shader, "in_Position", "in_Normal", "in_Texture");


	mat4 placeDown = T(4.5, 9.2, 0.00421)*Rx(angle);
	glUniformMatrix4fv(glGetUniformLocation(shader, "myMatrix"), 1, GL_TRUE, placeDown.m);
	DrawModel(blade, shader, "in_Position", "in_Normal", "in_Texture");


	mat4 placeUp = T(4.5, 9.2, 0.00421)*Rx(angle)*Rx(3.14);
	glUniformMatrix4fv(glGetUniformLocation(shader, "myMatrix"), 1, GL_TRUE, placeUp.m);
	DrawModel(blade, shader, "in_Position", "in_Normal", "in_Texture");


	mat4 placeRight = T(4.5, 9, 0.00421)*Rx(angle)*Rx(3.14/2);
	glUniformMatrix4fv(glGetUniformLocation(shader, "myMatrix"), 1, GL_TRUE, placeRight.m);
	DrawModel(blade, shader, "in_Position", "in_Normal", "in_Texture");

	mat4 placeLeft = T(4.5, 9, 0.00421)*Rx(angle)*Rx(-3.14/2);
	glUniformMatrix4fv(glGetUniformLocation(shader, "myMatrix"), 1, GL_TRUE, placeLeft.m);
	DrawModel(blade, shader, "in_Position", "in_Normal", "in_Texture");


	

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
