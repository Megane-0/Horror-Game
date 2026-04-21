#define MAIN
#include "MicroGlut.h"
#include "GL_utilities.h"
#include "VectorUtils4.h"
#include "LittleOBJLoader.h"
#include "LoadTGA.h"
#include "Camera.h"
#include "GhostMovement.h"


#define cameraSpeed 0.3

mat4 projectionMatrix;
vec3 objPos = vec3(0.0,0.0,0.0);
vec3 camPos = vec3(0,5,0);

// vertex array object
Model *m, *m2, *tm, *house, *ghost;
// Reference to shader program
GLuint program;
GLuint tex1, tex2, tex3;
TextureData ttex; // terrain
vec3 *vertexArray;
Camera camera;
int CENTER_X = 1024/2, CENTER_Y = 768/2;

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

float length(vec3 v)
{
    return sqrt(dot(v, v));
}

// Gets the point on a line ab which is closest to p
vec3 closestPointOnSegment(vec3 a, vec3 b, vec3 p)
{
	// get line
    vec3 ab = b - a;
    float t = dot(p - a, ab) / dot(ab, ab);

	// clamp
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;

    return a + ab * t;
}

vec3 closestPointTriangle(vec3 p, vec3 a, vec3 b, vec3 c)
{
	// get edge vectors
    vec3 ab = b - a;
    vec3 ac = c - a;
    vec3 bc = c - b;

    vec3 ap = p - a;
    vec3 bp = p - b;
    vec3 cp = p - c;

	// get normal
    vec3 n = cross(ab, ac);

    // same-side tests
    vec3 n1 = cross(ab, ap);// edge AB
    vec3 n2 = cross(bc, bp);// edge BC
    vec3 ca = a - c;        // edge CA
    vec3 n3 = cross(ca, cp);

    if (dot(n, n1) >= 0.0f &&
        dot(n, n2) >= 0.0f &&
        dot(n, n3) >= 0.0f)
    {
        // inside face region -> project onto the plane
        float denom = dot(n, n);
        if (denom < 1e-8f) return a; // degenerate triangle, treat as vertex A
        float dist = dot(p - a, n) / denom;
        return p - n * dist;
    }

    // otherwise check the three edges
    vec3 p1 = closestPointOnSegment(a, b, p);
    vec3 p2 = closestPointOnSegment(b, c, p);
    vec3 p3 = closestPointOnSegment(c, a, p);

    float d1 = length(p - p1);
    float d2 = length(p - p2);
    float d3 = length(p - p3);

	// compare distances
    if (d1 <= d2 && d1 <= d3) return p1;
    if (d2 <= d3) return p2;
    return p3;
}

// Checks if two spheres p1,p2 with radius r1,r2 are colliding
bool sphereCollision(vec3 p1, float r1, vec3 p2, float r2)
{
   	float dx = p1.x - p2.x;
    float dy = p1.y - p2.y;
    float dz = p1.z - p2.z;

    float distSq = dx*dx + dy*dy + dz*dz;
    float r = r1 + r2;

    return distSq <= r*r;
}

// Checks for collision between camera and house model
bool houseCollision(vec3 cam, float r, mat4 modelmatrix){
	// checks collision with house
	for (int i = 0; i < house->numIndices; i += 3)
    {
        int i0 = house->indexArray[i];
        int i1 = house->indexArray[i+1];
        int i2 = house->indexArray[i+2];

        vec3 a = house->vertexArray[i0];
        vec3 b = house->vertexArray[i1];
        vec3 c = house->vertexArray[i2];
		
		// transform to world space
		vec3 aWorld = MultVec3(modelmatrix, a);
		vec3 bWorld = MultVec3(modelmatrix, b);
		vec3 cWorld = MultVec3(modelmatrix, c);

		vec3 normal = normalize(cross(bWorld -aWorld, cWorld - aWorld));
		
		// early rejection (distance)
		if (length(cam - aWorld) > 10.0f) continue;
		
		// early rejection (face)
		vec3 camToTri = cam - aWorld;
		if (dot(normal, camToTri) < 0) continue;

		// get closest point to triangle
		vec3 q = closestPointTriangle(cam, aWorld, bWorld, cWorld);

		// check distance to point and compare against radius
        vec3 diff = cam - q;
        float distSq = dot(diff, diff);

        if (distSq <= r * r)
        {
            return true;
        }

    }
	
	return false;
}
Model* GenerateTerrain(TextureData *tex)
{
	int vertexCount = tex->width * tex->height;
	int triangleCount = (tex->width-1) * (tex->height-1) * 2;
	int x, z;
	
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
	glutWarpPointer(1024/2, 768/2);
	// Load and compile shader
	program = loadShaders("projekt.vert", "projekt.frag");
	glUseProgram(program);
	printError("init shader");
	
	glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
	


	LoadTGATextureSimple("maskros512.tga", &tex1);
	LoadTGATextureSimple("ghost_texture.tga", &tex3);
	//LoadTGATextureSimple("44-terrain.tga", &tex2);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex1);		// Bind Our Texture tex1
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex2);		// Bind Our Texture tex2

	glUniform1i(glGetUniformLocation(program, "tex1"), 0); // Texture unit 0
	glUniform1i(glGetUniformLocation(program, "tex2"), 1); // Texture unit 1


	// sphere
	house = LoadModel("house_v10.obj"); // testwall
	
	

	// load ghost model
	ghost = LoadModel("ghost.obj");
	
	// Load terrain data
	LoadTGATextureData("fft-terrain.tga", &ttex);
	tm = GenerateTerrain(&ttex);
	printError("init terrain");


	// Set up ghost
	std::vector<vec3> path = {
        {10.0f, 10.0f, 0.0f},
        {10.0f, 10.0f, 10.0f},
        {5.0f, 10.0f, 5.0f}
    };
    
    // Set up the ghost: Start Pos, Speed, Path
    initGhost(vec3(10, 10, 10), 2.0f, path);
    gGhostYawOffset = -M_PI/2.0; // Tweak this if the model faces the wrong way

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
	if (ttex.width < 2 || ttex.height < 2) {
		return 0.0f;
	}

	if (x < 0.0f) x = 0.0f;
	if (z < 0.0f) z = 0.0f;
	if (x > ttex.width - 2) x = ttex.width - 2;
	if (z > ttex.height - 2) z = ttex.height - 2;

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


void handleCollision(mat4 modelmatrix, vec3 lastCameraPosition){
	bool colliding = false;
	// Detect Collision with ghost
	vec3 cam = camera.getPosition();
	vec3 enemy = gGhostPos;
	//cam.y -= 1.0f;
	if (sphereCollision(cam, camera.cameraRadius, enemy, 1.5f))
	{
		printf("Collision!\n");
		colliding = true;
	}

	else if (houseCollision(cam, camera.cameraRadius, modelmatrix)){
		printf("House Collision\n");
		colliding = true;
	}

	if (colliding){
		camera.setPosition(lastCameraPosition);
	}



	
}
void display(void)
{
	if (glutKeyIsDown('p')) // exit program
		exit(0);

	
	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	
	printError("pre display");
	
	glUseProgram(program);
	static float lastTime = 0;
	float currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	float deltaTime = currentTime - lastTime;
	lastTime = currentTime;

	vec3 lastCameraPosition = camera.getPosition();
	camera.update(deltaTime);
	vec3 currentCameraPosition = camera.getPosition();
	

	mat4 view = camera.getViewMatrix();
	glUniformMatrix4fv(glGetUniformLocation(program, "viewMatrix"), 1, GL_TRUE, view.m);

	mat4 modelToWorld = IdentityMatrix();
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_TRUE, modelToWorld.m);
	
	//objPos.x += 0.01;
	//objPos.z += 0.02;
	
	objPos.y = 5;// getHeight(objPos.x, objPos.z);

	mat4 placeDowns = T(objPos.x, objPos.y, objPos.z);
	glUniformMatrix4fv(glGetUniformLocation(program, "placeDowns"), 1, GL_TRUE, placeDowns.m);
	DrawModel(house, program, "inPosition", "inNormal", "inTexCoord");

	handleCollision(placeDowns, lastCameraPosition);



	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex3); // Temporarily set the ghost 

	updateGhost(deltaTime); // Update the math

    mat4 ghostMatrix = getGhostMoveMatrix();
    
    glUniformMatrix4fv(glGetUniformLocation(program, "placeDowns"), 1, GL_TRUE, ghostMatrix.m);
    DrawModel(ghost, program, "inPosition", "inNormal", "inTexCoord");

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex1); // And now turn off ghost bind
	

	placeDowns = mat4(1.0);
	glUniformMatrix4fv(glGetUniformLocation(program, "placeDowns"), 1, GL_TRUE, placeDowns.m);
	DrawModel(tm, program, "inPosition", "inNormal", "inTexCoord");

	printError("display 2");
	
	glutSwapBuffers();
}

void mouse(int x, int y)
{
	// calculate distance from center
    float dx = x - CENTER_X;
    float dy = CENTER_Y - y;
	// update camera based on dx, dy
	camera.processMouse(dx, dy);
	// warp back to center position
    glutWarpPointer(CENTER_X, CENTER_Y);
}

void mouseClick(int button, int state, int x, int y){
	printf("%d %d\n", x, y);
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
	glutInitContextVersion(3, 2);
	glutInitWindowSize (1024, 768);

	glutCreateWindow ("TSBK07 Lab 4");
	//glutFullScreen();
	glutDisplayFunc(display);
	init ();
	glutRepeatingTimer(100);
	
	glutPassiveMotionFunc(mouse);
	glutMouseFunc(mouseClick);

	glutMainLoop();
	exit(0);
}