// Lab 1-1.
// This is the same as the first simple example in the course book,
// but with a few error checks.
// Remember to copy your file to a new on appropriate places during the lab so you keep old results.
// Note that the files "lab1-1.frag", "lab1-1.vert" are required.

#include "GL_utilities.h"
#include "MicroGlut.h"
// uses framework OpenGL
// uses framework Cocoa

// Globals
// Data would normally be read from files
GLfloat vertices[] =
{
	0.1f,0.1f,0.0f,
	0.0f,1.0f,0.0f,
	0.5f,-0.5f,0.0f
};

GLfloat myMatrix[] = {  1.0f, 0.0f, 0.0f, 0.5f,

                        0.0f, 1.0f, 0.0f, 0.0f,

                        0.0f, 0.0f, 1.0f, 0.0f,

                        0.0f, 0.0f, 0.0f, 1.0f };

// vertex array object
unsigned int vertexArrayObjID;

void init(void)
{
	// vertex buffer object, used for uploading the geometry
	unsigned int vertexBufferObjID;
	// Reference to shader
	GLuint shader;

	dumpInfo();

	// GL inits
	glClearColor(0.8,0.5,0.5,0);
	glDisable(GL_DEPTH_TEST);
	printError("GL inits");

	// Load and compile shader
	shader = loadShaders("lab1-2.vert", "lab1-2.frag");
	printError("init shader");
	
	// Upload geometry to the GPU:
	
	// Allocate and activate Vertex Array Object
	glGenVertexArrays(1, &vertexArrayObjID);
	glBindVertexArray(vertexArrayObjID);
	// Allocate Vertex Buffer Objects
	glGenBuffers(1, &vertexBufferObjID);
	
	// VBO for vertex data
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjID);
	glBufferData(GL_ARRAY_BUFFER, 9*sizeof(GLfloat), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(glGetAttribLocation(shader, "in_Position"), 3, GL_FLOAT, GL_FALSE, 0, 0); 
	glEnableVertexAttribArray(glGetAttribLocation(shader, "in_Position"));
	
	// End of upload of geometry

	// Upload matrix to shader
	glUniformMatrix4fv(glGetUniformLocation(shader, "myMatrix"), 1, GL_TRUE, myMatrix);
	
	printError("init arrays");
}


void display(void)
{
	printError("pre display");

	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT);

	glBindVertexArray(vertexArrayObjID);	// Select VAO
	glDrawArrays(GL_TRIANGLES, 0, 3);	// draw object
	
	printError("display");
	
	glutSwapBuffers();
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitContextVersion(3, 2);
	glutInitWindowSize(600, 600);
	glutCreateWindow ("white triangle example");
	glutDisplayFunc(display); 
	init ();
	glutMainLoop();
	return 0;
}
