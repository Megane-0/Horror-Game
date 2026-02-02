
#define MAIN
#include "GL_utilities.h"
#include "MicroGlut.h"
#include <math.h>
#include "LittleOBJLoader.h"
#include "LoadTGA.h"
#include "VectorUtils4.h"

// uses framework OpenGL
// uses framework Cocoa
GLfloat t;
Model *m = LoadModelData("bunnyplus.obj");
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


// vertex array objects
unsigned int bunnyVertexArrayObjID;

unsigned int bunnyVertexBufferObjID;

unsigned int bunnyIndexBufferObjID;

unsigned int bunnyNormalBufferObjID;

unsigned int bunnyTexCoordBufferObjID;

// Reference to shader
GLuint program;

void init(void)
{


	dumpInfo();
	

	// GL inits
	glClearColor(0.8,0.5,0.5,0);
	
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	//glDisable(GL_DEPTH_TEST); // Disable z-buffer makes it "see-through"
	glDisable(GL_CULL_FACE);
	printError("GL icnits");

	// Load and compile shader
	program = loadShaders("lab2-5.vert", "lab2-5.frag");
	glUseProgram(program); // activate shader
	printError("init shader");
	
	// Upload geometry to the GPU:
	

	glGenVertexArrays(1, &bunnyVertexArrayObjID);

    glGenBuffers(1, &bunnyVertexBufferObjID);

    glGenBuffers(1, &bunnyIndexBufferObjID);

    glGenBuffers(1, &bunnyNormalBufferObjID);
    

    glBindVertexArray(bunnyVertexArrayObjID);



    // VBO for vertex data

    glBindBuffer(GL_ARRAY_BUFFER, bunnyVertexBufferObjID);

    glBufferData(GL_ARRAY_BUFFER, m->numVertices*3*sizeof(GLfloat), m->vertexArray, GL_STATIC_DRAW);

    glVertexAttribPointer(glGetAttribLocation(program, "in_Position"), 3, GL_FLOAT, GL_FALSE, 0, 0); 

    glEnableVertexAttribArray(glGetAttribLocation(program, "in_Position"));



    // VBO for normal data

    glBindBuffer(GL_ARRAY_BUFFER, bunnyNormalBufferObjID);

    glBufferData(GL_ARRAY_BUFFER, m->numVertices*3*sizeof(GLfloat), m->normalArray, GL_STATIC_DRAW);

    glVertexAttribPointer(glGetAttribLocation(program, "in_Normal"), 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(glGetAttribLocation(program, "in_Normal"));

    

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bunnyIndexBufferObjID);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m->numIndices*sizeof(GLuint), m->indexArray, GL_STATIC_DRAW);
	// End of upload of geometry



	// Upload texture coordinates

	    glGenBuffers(1, &bunnyTexCoordBufferObjID);    


    if (m->texCoordArray != NULL)

    {
        glBindBuffer(GL_ARRAY_BUFFER, bunnyTexCoordBufferObjID);
        glBufferData(GL_ARRAY_BUFFER, m->numVertices*2*sizeof(GLfloat), m->texCoordArray, GL_STATIC_DRAW);
        glVertexAttribPointer(glGetAttribLocation(program, "inTexCoord"), 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(glGetAttribLocation(program, "inTexCoord"));
    }


	// Load texture

	LoadTGATextureSimple("maskros512.tga", &myTex);
	glBindTexture(GL_TEXTURE_2D, myTex);
	glUniform1i(glGetUniformLocation(program, "texUnit"), 0); // Texture unit 0
	glActiveTexture(GL_TEXTURE0);


	// Upload projection matrix
	glUniformMatrix4fv(glGetUniformLocation(program, "projectionMatrix"), 1, GL_TRUE, projectionMatrix);

	//Upload lookat matrix
	mat4 lookAtMatrix = lookAtv(vec3(0, 0, 1), vec3(0, 0, 0), vec3(0, 1, 0));
	glUniformMatrix4fv(glGetUniformLocation(program, "lookAtMatrix"), 1, GL_TRUE, lookAtMatrix.m);

	

	
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
					



	// Upload matrix
	glUniformMatrix4fv(glGetUniformLocation(program, "myMatrix"), 1, GL_TRUE, myMatrix);


	

	glBindVertexArray(bunnyVertexArrayObjID);    // Select VAO

    glDrawElements(GL_TRIANGLES, m->numIndices, GL_UNSIGNED_INT, 0L);
	
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
