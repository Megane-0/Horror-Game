
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
Model *teapot;
Model *skybox;

Model *ground;

GLuint myTex; 
GLuint skyboxTex;
vec3 startCamera = vec3(0,10,30);

mat4 cameraMovement = mat4(1.0);

#define kGroundSize 100.0f

#define cameraSpeed 0.3

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


vec3 lightSourcesColorsArr[] = { vec3(1.0f, 0.0f, 0.0f), // Red light

                                 vec3(0.0f, 1.0f, 0.0f), // Green light

                                 vec3(0.0f, 0.0f, 1.0f), // Blue light

                                 vec3(1.0f, 1.0f, 1.0f) }; // White light

GLint isDirectional[] = {0,0,1,1};


vec3 lightSourcesDirectionsPositions[] = { vec3(10.0f, 5.0f, 0.0f), // Red light, positional

                                       vec3(0.0f, 5.0f, 10.0f), // Green light, positional

                                       vec3(-1.0f, 0.0f, 0.0f), // Blue light along X

                                       vec3(0.0f, 0.0f, -1.0f) }; // White light along Z

GLfloat specularExponent[] = {100.0, 200.0, 60.0};

// Reference to shader
GLuint shader;

GLuint skyboxShader;

void init(void)
{
	dumpInfo();
	printError("init shader");


	blade = LoadModel("windmill/blade.obj");
	balcony = LoadModel("windmill/windmill-balcony.obj");
	roof = LoadModel("windmill/windmill-roof.obj");
	walls = LoadModel("windmill/windmill-walls2.obj");
	teapot = LoadModel("models/various/teapot.obj");
	skybox = LoadModel("skybox/labskyboxfull.obj");
	ground = LoadDataToModel(vertices, vertex_normals, tex_coords, ground_colors, indices,4,6);	

	// Load and compile shader
	skyboxShader = loadShaders("skybox.vert", "skybox.frag");
	shader = loadShaders("lab3-4a.vert", "lab3-4a.frag");

	
	// GL inits
	glClearColor(0.8,0.5,0.5,0);
	
	// Buffers
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	//glDisable(GL_DEPTH_TEST); // Disable z-buffer makes it "see-through"
	glDisable(GL_CULL_FACE);
	printError("GL icnits");
	
	mat4 lookAtMatrix = lookAtv(startCamera, vec3(1, 10, 0), vec3(0, 1, 0));

	/* SKY BOX */
	glUseProgram(skyboxShader); // Added 2022-03
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader, "projectionMatrix"), 1, GL_TRUE, projectionMatrix);
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader, "lookAtMatrix"), 1, GL_TRUE, lookAtMatrix.m);
	// Load texture for skybox
	LoadTGATextureSimple("skybox/labSkyBoxFull.tga", &skyboxTex);
	glBindTexture(GL_TEXTURE_2D, skyboxTex);
	glUniform1i(glGetUniformLocation(skyboxShader, "texUnit"), 0); // Texture unit 0
	glActiveTexture(GL_TEXTURE0);

	mat4 skyboxInit = T(startCamera.x, startCamera.y, startCamera.z);
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader, "myMatrix"), 1, GL_TRUE, skyboxInit.m);

	/* REST SHADER */
	glUseProgram(shader);
	glUniformMatrix4fv(glGetUniformLocation(shader, "projectionMatrix"), 1, GL_TRUE, projectionMatrix);
	glUniformMatrix4fv(glGetUniformLocation(shader, "lookAtMatrix"), 1, GL_TRUE, lookAtMatrix.m);


	glUniform3fv(glGetUniformLocation(shader, "lightSourcesDirPosArr"), 4, &lightSourcesDirectionsPositions[0].x);

	glUniform3fv(glGetUniformLocation(shader, "lightSourcesColorArr"), 4, &lightSourcesColorsArr[0].x);

	glUniform1iv(glGetUniformLocation(shader, "isDirectional"), 4, isDirectional);


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


	/* CAMERA MOVEMENT */
	if (glutKeyIsDown('a')) {
		cameraMovement = T(cameraSpeed, 0, 0)*cameraMovement;
	}
	if (glutKeyIsDown('d')) {
		cameraMovement = T(-cameraSpeed, 0, 0)*cameraMovement;
	}
	if (glutKeyIsDown('w')) {
		cameraMovement = T(0, 0, cameraSpeed)*cameraMovement;
	}
	if (glutKeyIsDown('s')) {
		cameraMovement = T(0, 0, -cameraSpeed)*cameraMovement;
	}
	if (glutKeyIsDown('q')) {
		cameraMovement = T(0, cameraSpeed, 0)*cameraMovement;
	}
	if (glutKeyIsDown('e')) {
		cameraMovement = T(0, -cameraSpeed, 0)*cameraMovement;
	}
	if (glutKeyIsDown('j')) {
		cameraMovement = Ry(cameraSpeed/6.0)*cameraMovement;
	}
	if (glutKeyIsDown('k')) {
		cameraMovement = Ry(-cameraSpeed/6.0)*cameraMovement;
	}

	/*SKYBOX*/
	glUseProgram(skyboxShader);

	// Upload skybox matrix with no movement
	mat4 camNoTranslate = cameraMovement;
	camNoTranslate.m[3] = 0;
	camNoTranslate.m[7] = 0;
	camNoTranslate.m[11] = 0;
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader, "cameraMovement"), 1, GL_TRUE, camNoTranslate.m);
	
	// Draw skybox
	glDisable(GL_DEPTH_TEST);
	DrawModel(skybox, skyboxShader, "in_Position", "in_Normal", "in_Texture");
	glEnable(GL_DEPTH_TEST);

	/* REST SHADER */
	glUseProgram(shader); 
	// Upload camera matrix
	glUniformMatrix4fv(glGetUniformLocation(shader, "cameraMovement"), 1, GL_TRUE, cameraMovement.m);



	// Create generic matrix
	GLfloat startPos[] = {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,  0.0f,
    0.0f, 0.0f,  1.0f,  0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
	};
	// Upload matrix
	glUniformMatrix4fv(glGetUniformLocation(shader, "myMatrix"), 1, GL_TRUE, startPos);
	

	glUniform1f(glGetUniformLocation(shader, "specularExponent"), specularExponent[0]);
	// Draw ground
	DrawModel(ground, shader, "in_Position", "in_Normal", "in_Texture");
	
	glUniform1f(glGetUniformLocation(shader, "specularExponent"), specularExponent[1]);
	// Draw model
	DrawModel(balcony, shader, "in_Position", "in_Normal", "in_Texture");
	DrawModel(roof, shader, "in_Position", "in_Normal", "in_Texture");
	DrawModel(walls, shader, "in_Position", "in_Normal", "in_Texture");

	mat4 placeDowns = T(20.0, 0, 20.0);
	glUniformMatrix4fv(glGetUniformLocation(shader, "myMatrix"), 1, GL_TRUE, placeDowns.m);
	
	glUniform1f(glGetUniformLocation(shader, "specularExponent"), specularExponent[2]);
	DrawModel(teapot, shader, "in_Position", "in_Normal", "in_Texture");


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
