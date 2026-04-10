#define MAIN
#include "MicroGlut.h"
#include "GL_utilities.h"
#include "VectorUtils4.h"
#include "LittleOBJLoader.h"
#include "LoadTGA.h"
#include "Camera.h"


#define cameraSpeed 0.3

mat4 projectionMatrix;
vec3 objPos = vec3(0.0,0.0,0.0);
vec3 camPos = vec3(0,5,0);
bool firstMouse = true;

// vertex array object
Model *m, *m2, *tm, *sphere;
// Reference to shader program
GLuint program;
GLuint tex1, tex2;
TextureData ttex; // terrain
bool ignoreWarp = false;
vec3 *vertexArray;
Camera camera;
int mouseX = 300, mouseY = 300;


unsigned int max(float a, float b) {

	if (a >= b) {
		return a;
	} else {
		return b;
	}
}

unsigned int min(float a, float b) {

	if (a <= b) {
		return a;
	} else {
		return b;
	}
}
bool sphereCollision(vec3 p1, float r1, vec3 p2, float r2){
	return false; // todo
}
Model* GenerateTerrain(TextureData *tex)
{
	int vertexCount = tex->width * tex->height;
	int triangleCount = (tex->width-1) * (tex->height-1) * 2;
	unsigned int x, z;
	
	vertexArray = (vec3 *)malloc(sizeof(GLfloat) * 3 * vertexCount);
	vec3 *normalArray = (vec3 *)malloc(sizeof(GLfloat) * 3 * vertexCount);
	vec2 *texCoordArray = (vec2 *)malloc(sizeof(GLfloat) * 2 * vertexCount);
	GLuint *indexArray = (GLuint *) malloc(sizeof(GLuint) * triangleCount*3);
	
	printf("bpp %d\n", tex->bpp);
	for (x = 0; x < tex->width; x++)
		for (z = 0; z < tex->height; z++)
		{
			// Vertex array. You need to scale this properly

			vertexArray[(x + z * tex->width)].x = x / 1.0;
			vertexArray[(x + z * tex->width)].y = tex->imageData[(x + z * tex->width) * (tex->bpp/8)] / 10.0; // 10.0 for fft-terrain
			vertexArray[(x + z * tex->width)].z = z / 1.0;

			// Texture coordinates. You may want to scale them.
			texCoordArray[(x + z * tex->width)].x = x; // (float)x / tex->width;
			texCoordArray[(x + z * tex->width)].y = z; // (float)z / tex->height;
		}
			
	for (x = 0; x < tex->width; x++)
			for (z = 0; z < tex->height; z++)
			{
			// Normal vectors. we need to calculate these.
			vec3 vUp = vertexArray[(x + max((z-1), 0)* tex->width)];
			vec3 vDown = vertexArray[(x + min((z+1), tex->height-1)* tex->width)];
			vec3 vLeft = vertexArray[(max((x-1), 0) + z* tex->width)];
			vec3 vRight = vertexArray[(min((x+1), tex->width-1) + z* tex->width)];
			
			vec3 vNorm = cross((vUp-vDown), (vRight-vLeft));

			normalArray[(x + z * tex->width)] = vNorm;
			//printf("vec3(%.2f, %2f, %2f)\n", vNorm.x, vNorm.y,vNorm.z);
	}

	
	for (x = 0; x < tex->width-1; x++)
		for (z = 0; z < tex->height-1; z++)
		{
			// Triangle 1
			indexArray[(x + z * (tex->width-1))*6 + 0] = x + z * tex->width;
			indexArray[(x + z * (tex->width-1))*6 + 1] = x + (z+1) * tex->width;
			indexArray[(x + z * (tex->width-1))*6 + 2] = x+1 + z * tex->width;
			// Triangle 2
			indexArray[(x + z * (tex->width-1))*6 + 3] = x+1 + z * tex->width;
			indexArray[(x + z * (tex->width-1))*6 + 4] = x + (z+1) * tex->width;
			indexArray[(x + z * (tex->width-1))*6 + 5] = x+1 + (z+1) * tex->width;

		}
	// End of terrain generation
	
	// Create Model and upload to GPU:

	Model* model = LoadDataToModel(
			vertexArray,
			normalArray,
			texCoordArray,
			NULL,
			indexArray,
			vertexCount,
			triangleCount*3);

	return model;
}


void init(void)
{
	// GL inits
	glClearColor(0.4,0.6,0.9,0);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	printError("GL inits");
	glutHideCursor();
	projectionMatrix = frustum(-0.1, 0.1, -0.1, 0.1, 0.2, 5000.0);
	glutWarpPointer(300, 300);
	// Load and compile shader
	program = loadShaders("projekt.vert", "projekt.frag");
	glUseProgram(program);
	printError("init shader");
	
	glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
	


	LoadTGATextureSimple("maskros512.tga", &tex1);
	LoadTGATextureSimple("44-terrain.tga", &tex2);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex1);		// Bind Our Texture tex1
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex2);		// Bind Our Texture tex2

	glUniform1i(glGetUniformLocation(program, "tex1"), 0); // Texture unit 0
	glUniform1i(glGetUniformLocation(program, "tex2"), 1); // Texture unit 1


	// sphere
	sphere = LoadModel("groundsphere.obj");
	
	// Load terrain data
	LoadTGATextureData("fft-terrain.tga", &ttex);
	tm = GenerateTerrain(&ttex);
	printError("init terrain");
	camera.firstMouse = true;

}

float getLength(vec2 p1, vec2 p2){
	
	vec2 p3 = vec2 (p2.x - p1.x, p2.y - p1.y);
	return sqrt(pow(p3.x, 2) + pow(p3.y,2));
	
}
// https://stackoverflow.com/questions/25385361/point-within-a-triangle-barycentric-co-ordinates
float barycentricHeight(vec3 A, vec3 B, vec3 C, float x, float z)
{
    float det = (B.z - C.z)*(A.x - C.x) + (C.x - B.x)*(A.z - C.z);

    float l1 = ((B.z - C.z)*(x - C.x) + (C.x - B.x)*(z - C.z)) / det;
    float l2 = ((C.z - A.z)*(x - C.x) + (A.x - C.x)*(z - C.z)) / det;
    float l3 = 1.0f - l1 - l2;

    return l1*A.y + l2*B.y + l3*C.y;
}

float getHeight(float x, float z) {
	int gridX = (int)floor(x);
	int gridZ = (int)floor(z);

	int nextX = gridX + 1;
	int nextZ = gridZ + 1;

	vec3 p3 = vertexArray[gridX + gridZ * ttex.width]; // (x,z)
	vec3 p4 = vertexArray[gridX + nextZ * ttex.width]; // (x,z+1)
	vec3 p1 = vertexArray[nextX + gridZ * ttex.width]; // (x+1,z)
	vec3 p2 = vertexArray[nextX + nextZ * ttex.width]; // (x+1,z+1)

	float localX = x - gridX;
	float localZ = z - gridZ;
	
	if (localX + localZ < 1.0f)
	{
		// top left (p3) triangle
		return barycentricHeight(p3, p4, p1, x, z);
	}
	else
	{
		// bottom right (p2) triangle
		return barycentricHeight(p1, p4, p2, x, z);
	}
}

void display(void)
{
	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	
	printError("pre display");
	
	glUseProgram(program);
	static float lastTime = 0;
	float currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	float deltaTime = currentTime - lastTime;
	lastTime = currentTime;
	camera.update(deltaTime, mouseX, mouseY);

	mat4 view = camera.getViewMatrix();
	glUniformMatrix4fv(glGetUniformLocation(program, "viewMatrix"), 1, GL_TRUE, view.m);

	mat4 modelToWorld = IdentityMatrix();
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_TRUE, modelToWorld.m);
	
	objPos.x += 0.01;
	objPos.z += 0.02;
	
	objPos.y = getHeight(objPos.x, objPos.z);

	mat4 placeDowns = T(objPos.x, objPos.y, objPos.z);
	glUniformMatrix4fv(glGetUniformLocation(program, "placeDowns"), 1, GL_TRUE, placeDowns.m);
	DrawModel(sphere, program, "inPosition", "inNormal", "inTexCoord");
	placeDowns = mat4(1.0);
	
	glUniformMatrix4fv(glGetUniformLocation(program, "placeDowns"), 1, GL_TRUE, placeDowns.m);


	DrawModel(tm, program, "inPosition", "inNormal", "inTexCoord");

	printError("display 2");
	
	glutSwapBuffers();
}

void mouse(int x, int y)
{

    mouseX = x;
    mouseY = y;
}
int onMouse;

void mouseClick(int button, int state, int x, int y){
	printf("%d %d\n", x, y);

}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
	glutInitContextVersion(3, 2);
	glutInitWindowSize (600, 600);
	glutCreateWindow ("TSBK07 Lab 4");
	glutDisplayFunc(display);
	init ();
	glutRepeatingTimer(100);
	
	glutPassiveMotionFunc(mouse);
	glutMouseFunc(mouseClick);
	glutMainLoop();
	exit(0);
}