
// Lab 1-1.
// This is the same as the first simple example in the course book,
// but with a few error checks.
// Remember to copy your file to a new on appropriate places during the lab so you keep old results.
// Note that the files "lab1-1.frag", "lab1-1.vert" are required.

#include "GL_utilities.h"
#include "MicroGlut.h"
#include <math.h>

// uses framework OpenGL
// uses framework Cocoa
GLfloat t;

// Globals
// Data would normally be read from files
GLfloat vertices[] = {
	0.0f,0.5f,0.0f,
	-0.5f,0.0f,0.5f,
	0.5f,0.0f,0.5f,

	0.0f,0.5f,0.0f,
	-0.5f,0.0f,-0.5f,
	0.5f,0.0f,-0.5f,

	0.0f,0.5f,0.0f,
	0.5f,0.0f,0.5f,
	0.5f,0.0f,-0.5f,

	0.0f,0.5f,0.0f,
	-0.5f,0.0f,0.5f,
	-0.5f,0.0f,-0.5f,

	
	-0.5f, 0.0f,  0.5f, 
	0.5f, 0.0f,  0.5f,
	0.5f, 0.0f, -0.5f,


	-0.5f, 0.0f,  0.5f,
	0.5f, 0.0f, -0.5f,
	-0.5f, 0.0f, -0.5f
};

GLfloat color[] = {
1.0f,0.0f,0.0f,
1.0f,0.0f,0.0f,
1.0f,0.0f,0.0f,

0.0f,1.0f,0.0f,
0.0f,1.0f,0.0f,
0.0f,1.0f,0.0f,

1.0f,0.0f,1.0f,
1.0f,0.0f,1.0f,
1.0f,0.0f,1.0f,

1.0f,1.0f,0.0f,
1.0f,1.0f,0.0f,
1.0f,1.0f,0.0f,

0.5f,0.0f,0.0f,
0.5f,0.0f,0.0f,
0.5f,0.0f,0.0f,

1.0f,0.5f,0.0f,
1.0f,0.5f,0.0f,
1.0f,0.5f,0.0f,

1.0f,0.0f,0.5f,
1.0f,0.0f,0.5f,
1.0f,0.0f,0.5f
};


// vertex array object
unsigned int vertexArrayObjID;

// Reference to shader
GLuint shader;

void init(void)
{
	// vertex buffer object, used for uploading the geometry
	unsigned int vertexBufferObjID;
	unsigned int colorBufferObjID;


	dumpInfo();
	

	// GL inits
	glClearColor(0.8,0.5,0.5,0);
	
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	//glDisable(GL_DEPTH_TEST); // Disable z-buffer makes it "see-through"
	//glDisable(GL_CULL_FACE);
	printError("GL icnits");

	// Load and compile shader
	shader = loadShaders("lab1-4.vert", "lab1-4.frag");
	glUseProgram(shader); // activate shader
	printError("init shader");
	
	// Upload geometry to the GPU:
	
	// Allocate and activate Vertex Array Object
	glGenVertexArrays(1, &vertexArrayObjID);
	glBindVertexArray(vertexArrayObjID);
	// Allocate Vertex Buffer Objects
	glGenBuffers(1, &vertexBufferObjID);
	
	// VBO for vertex data
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjID);
	glBufferData(GL_ARRAY_BUFFER, 54*sizeof(GLfloat), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(glGetAttribLocation(shader, "in_Position"), 3, GL_FLOAT, GL_FALSE, 0, 0); 
	glEnableVertexAttribArray(glGetAttribLocation(shader, "in_Position"));


	glGenBuffers(1, &colorBufferObjID);
	glBindBuffer(GL_ARRAY_BUFFER, colorBufferObjID);

	glBufferData(GL_ARRAY_BUFFER, 54*sizeof(GLfloat), color, GL_STATIC_DRAW);
	glVertexAttribPointer(glGetAttribLocation(shader, "in_Color"), 3, GL_FLOAT, GL_FALSE, 0, 0); 
	glEnableVertexAttribArray(glGetAttribLocation(shader, "in_Color"));
	
	
	// End of upload of geometry

	// Upload matrix to shader
	
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
    0.0f, sin(angle),  cos(angle),  0.0f,
    0.0f, 0.0f,        0.0f,       1.0f
	}; // Rotate around x-axis
					



	// Upload matrix
	glUniformMatrix4fv(glGetUniformLocation(shader, "myMatrix"), 1, GL_TRUE, myMatrix);

	


	glBindVertexArray(vertexArrayObjID);	// Select VAO
	glDrawArrays(GL_TRIANGLES, 0, 18);	// draw object
	
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
