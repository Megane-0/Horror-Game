#define MAIN
#include "MicroGlut.h"
#include "GL_utilities.h"
#include "VectorUtils4.h"
#include "LittleOBJLoader.h"
#include "LoadTGA.h"
#include "Camera.h"
#include "GhostMovement.h"


#define cameraSpeed 0.3


struct Door {
    Model* model;
    vec3 pivot;         
    float angle;        
    float targetAngle; 
};

mat4 projectionMatrix;
vec3 objPos = vec3(0.0,0.0,0.0);
vec3 camPos = vec3(0,5,0);

// vertex array object
Model *m, *m2, *tm, *sphere, *ghost;
Model *house, *modelWall, *modelFloor, *modelCeiling, *modelFrames, *modelDoor, *modelWindow, *modelFurnitues;
// Reference to shader program
GLuint program;
GLuint tex1, tex2, tex3;
GLuint texWall, texFloor, texDoor, texCeiling, texWindow; // House textures
TextureData ttex; // terrain
vec3 *vertexArray;
Camera camera;
int CENTER_X = 1024/2, CENTER_Y = 768/2;

// Scene light (lamp inside the house)
vec3 lightPos = vec3(8.1f, 2.0f, 11.0f);
vec3 lightColor = vec3(1.0f, 0.95f, 0.8f);

// Door animation variables
float doorSpeed = 2.5f;          
std::vector<Door> doors;

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
float vecLength(vec3 value)
{
	return sqrt(dot(value, value));
}

vec3 closestPointOnSegment(vec3 a, vec3 b, vec3 p)
{
	vec3 ab = b - a;
	float denominator = dot(ab, ab);
	if (denominator < 1e-8f)
	{
		return a;
	}

	float t = dot(p - a, ab) / denominator;
	if (t < 0.0f) t = 0.0f;
	if (t > 1.0f) t = 1.0f;

	return a + ab * t;
}

vec3 closestPointTriangle(vec3 p, vec3 a, vec3 b, vec3 c)
{
	vec3 ab = b - a;
	vec3 ac = c - a;
	vec3 bc = c - b;

	vec3 ap = p - a;
	vec3 bp = p - b;
	vec3 cp = p - c;

	vec3 n = cross(ab, ac);

	vec3 n1 = cross(ab, ap);
	vec3 n2 = cross(bc, bp);
	vec3 ca = a - c;
	vec3 n3 = cross(ca, cp);

	if (dot(n, n1) >= 0.0f &&
		dot(n, n2) >= 0.0f &&
		dot(n, n3) >= 0.0f)
	{
		float denom = dot(n, n);
		if (denom < 1e-8f) return a;
		float dist = dot(p - a, n) / denom;
		return p - n * dist;
	}

	vec3 p1 = closestPointOnSegment(a, b, p);
	vec3 p2 = closestPointOnSegment(b, c, p);
	vec3 p3 = closestPointOnSegment(c, a, p);

	float d1 = vecLength(p - p1);
	float d2 = vecLength(p - p2);
	float d3 = vecLength(p - p3);

	if (d1 <= d2 && d1 <= d3) return p1;
	if (d2 <= d3) return p2;
	return p3;
}

bool sphereCollision(vec3 p1, float r1, vec3 p2, float r2)
{
	float dx = p1.x - p2.x;
	float dy = p1.y - p2.y;
	float dz = p1.z - p2.z;

	float distSq = dx*dx + dy*dy + dz*dz;
	float r = r1 + r2;

	return distSq <= r*r;
}

bool modelCollision(Model* model, vec3 cam, float r, mat4 modelMatrix)
{
	if (model == NULL) return false;

	for (int i = 0; i < model->numIndices; i += 3)
	{
		int i0 = model->indexArray[i];
		int i1 = model->indexArray[i + 1];
		int i2 = model->indexArray[i + 2];

		vec3 a = model->vertexArray[i0];
		vec3 b = model->vertexArray[i1];
		vec3 c = model->vertexArray[i2];

		vec3 aWorld = MultVec3(modelMatrix, a);
		vec3 bWorld = MultVec3(modelMatrix, b);
		vec3 cWorld = MultVec3(modelMatrix, c);

		vec3 normal = normalize(cross(bWorld - aWorld, cWorld - aWorld));

		if (vecLength(cam - aWorld) > 10.0f) continue;

		vec3 camToTri = cam - aWorld;
		if (dot(normal, camToTri) < 0) continue;

		vec3 q = closestPointTriangle(cam, aWorld, bWorld, cWorld);
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

			vertexArray[(x + z * tex->width)].x = x / 5.0;
			//vertexArray[(x + z * tex->width)].y = tex->imageData[(x + z * tex->width) * (tex->bpp/8)] / 10.0; // 10.0 for fft-terrain
			vertexArray[(x + z * tex->width)].y = 0.0f; // Flaten terrain
			vertexArray[(x + z * tex->width)].z = z / 5.0;

			// Texture coordinates. You may want to scale them.
			texCoordArray[(x + z * tex->width)].x = x; // (float)x / tex->width;
			texCoordArray[(x + z * tex->width)].y = z; // (float)z / tex->height;
			
			normalArray[(x + z * tex->width)] = vec3(0.0f, 1.0f, 0.0f); // uppåt-normals

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

	// Upload light parameters
	glUniform3fv(glGetUniformLocation(program, "lightPos"), 1, &lightPos.x);
	glUniform3fv(glGetUniformLocation(program, "lightColor"), 1, &lightColor.x);
	


	LoadTGATextureSimple("maskros512.tga", &tex1);
	LoadTGATextureSimple("ghost_texture.tga", &tex3);
	//LoadTGATextureSimple("44-terrain.tga", &tex2);
	LoadTGATextureSimple("house-objects/house-textures/wall_diffuse.tga", &texWall);
	LoadTGATextureSimple("house-objects/house-textures/floor_tiles.tga", &texFloor);
	LoadTGATextureSimple("house-objects/house-textures/floor_tiles.tga", &texDoor); // TODO: Separate door texture
	LoadTGATextureSimple("house-objects/house-textures/shatteredglass.tga", &texWindow);
	
	LoadTGATextureSimple("house-objects/house-textures/ceiling.tga", &texCeiling);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex1);		// Bind Our Texture tex1
	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, tex2);		// Bind Our Texture tex2

	glUniform1i(glGetUniformLocation(program, "tex1"), 0); // Texture unit 0
	



	modelWall= LoadModel("house-objects/walls.obj");
	modelFloor = LoadModel("house-objects/floor.obj");
	modelCeiling = LoadModel("house-objects/ceiling.obj");
	modelFrames = LoadModel("house-objects/frames.obj");

	modelWindow = LoadModel("house-objects/windowFixed.obj");

	modelFurnitues = LoadModel("house-objects/furniture.obj");
	

	// Door 1: Main entrance
    Door d1;
    d1.model = LoadModel("house-objects/door_main_entrance.obj");
    d1.pivot = vec3(-2.81285f, 1.0f, 5.54151f); 
	//d1.pivot = vec3(-3.1285f, 1.0f, 5.56151f); 
    d1.angle = 0.0f;
    d1.targetAngle = 0.0f;
    doors.push_back(d1);


    Door d2;
    d2.model = LoadModel("house-objects/door_2.obj");
    d2.pivot = vec3(-2.7f, 1.0f, 2.66f);
    d2.angle = 0.0f;
    d2.targetAngle = 0.0f;
    doors.push_back(d2);

	Door d3;
	d3.model = LoadModel("house-objects/door_3.obj");
	d3.pivot = vec3(-4.5f, 1.0f, 2.76f);
	d3.angle = 0.0f;
	d3.targetAngle = 0.0f;
	doors.push_back(d3);

	Door d4;
	d4.model = LoadModel("house-objects/door_4.obj");
	d4.pivot = vec3(-6.46f, 1.0f, 2.76f);
	d4.angle = 0.0f;
	d4.targetAngle = 0.0f;
	doors.push_back(d4);

	Door d5;
	d5.model = LoadModel("house-objects/door_5.obj");
	d5.pivot = vec3(0.278f, 1.0f, -0.482103f);
	d5.angle = 0.0f;
	d5.targetAngle = 0.0f;
	doors.push_back(d5);


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

void handleCollision(mat4 houseMatrix, vec3 lastCameraPosition)
{
	bool colliding = false;
	vec3 cam = camera.getPosition();
	vec3 enemy = gGhostPos;

	if (sphereCollision(cam, camera.cameraRadius, enemy, 1.5f))
	{
		printf("Collision!\n");
		colliding = true;
	}
	else if (modelCollision(modelWall, cam, camera.cameraRadius, houseMatrix))
	{
		//printf("House Collision\n");
		colliding = true;
	}

	if (colliding)
	{
		camera.setPosition(lastCameraPosition);
	}
}

void display(void)
{
	// clear the screengit 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	
	printError("pre display");
	
	glUseProgram(program);
	static float lastTime = 0;
	float currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	float deltaTime = currentTime - lastTime;
	lastTime = currentTime;
	vec3 lastCameraPosition = camera.getPosition();
	camera.update(deltaTime);

	mat4 view = camera.getViewMatrix();
	glUniformMatrix4fv(glGetUniformLocation(program, "viewMatrix"), 1, GL_TRUE, view.m);

	mat4 modelToWorld = IdentityMatrix();
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_TRUE, modelToWorld.m);

	//--- House part ---//


	 // House
	vec3 housePos = vec3(10.0f, 0.0f, 10.0f);
	housePos.y = getHeight(housePos.x, housePos.z);
	mat4 houseMatrix = T(housePos.x, housePos.y + 0.5f, housePos.z);
	glUniformMatrix4fv(glGetUniformLocation(program, "placeDowns"), 1, GL_TRUE, houseMatrix.m);


	// walls
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texWall);
	DrawModel(modelWall, program, "inPosition", "inNormal", "inTexCoord");

	// frames
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex3);
	DrawModel(modelFrames, program, "inPosition", "inNormal", "inTexCoord");

	// floor
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texFloor);
	DrawModel(modelFloor, program, "inPosition", "inNormal", "inTexCoord");

	// ceiling
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texCeiling);
	DrawModel(modelCeiling, program, "inPosition", "inNormal", "inTexCoord");

	// windows	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texWindow);
	DrawModel(modelWindow, program, "inPosition", "inNormal", "inTexCoord");

	//furniture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texCeiling);
	DrawModel(modelFurnitues, program, "inPosition", "inNormal", "inTexCoord");


	// door
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texDoor);


    for (int i = 0; i < doors.size(); i++) {
        // (Animation)
        if (doors[i].angle < doors[i].targetAngle) doors[i].angle += doorSpeed * deltaTime;
        if (doors[i].angle > doors[i].targetAngle) doors[i].angle -= doorSpeed * deltaTime;
	    if (fabs(doors[i].angle - doors[i].targetAngle) < 0.01f) doors[i].angle = doors[i].targetAngle;

        // Calculate door matrix
        mat4 doorMatrix = Mult(houseMatrix,
            Mult(T(doors[i].pivot.x, doors[i].pivot.y, doors[i].pivot.z),
            Mult(Ry(doors[i].angle),
                T(-doors[i].pivot.x, -doors[i].pivot.y, -doors[i].pivot.z))));

        // draw
        glUniformMatrix4fv(glGetUniformLocation(program, "placeDowns"), 1, GL_TRUE, doorMatrix.m);
        DrawModel(doors[i].model, program, "inPosition", "inNormal", "inTexCoord");
    }

	handleCollision(houseMatrix, lastCameraPosition);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex3); // Temporarily set the ghost 

	updateGhost(deltaTime); // Update the math

    mat4 ghostMatrix = getGhostMoveMatrix();
    
    glUniformMatrix4fv(glGetUniformLocation(program, "placeDowns"), 1, GL_TRUE, ghostMatrix.m);
    DrawModel(ghost, program, "inPosition", "inNormal", "inTexCoord");

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex1); // And now turn off ghost bind
	

	mat4 placeDowns = mat4(1.0);
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

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        // Find closest door to camera
        vec3 camPos = camera.getPosition();
		
		// House position (same as in display())
		vec3 housePos = vec3(10.0f, 0.0f, 10.0f);
		housePos.y = getHeight(housePos.x, housePos.z);
		mat4 houseMatrix = T(housePos.x, housePos.y + 0.5f, housePos.z);
		
        float closestDist = 1000.0f;
        int closestDoorIdx = -1;

        for (size_t i = 0; i < doors.size(); ++i)
        {
            // Transform door pivot to world coordinates
            vec3 doorLocalPos = doors[i].pivot;
            vec3 doorWorldPos = MultVec3(houseMatrix, doorLocalPos);
            
            vec3 diff = camPos - doorWorldPos;
            float dist = sqrt(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);

            // Only consider doors within reasonable range
            if (dist < 15.0f && dist < closestDist)
            {
                closestDist = dist;
                closestDoorIdx = (int)i;
                printf("Door %zu at world pos (%.2f, %.2f, %.2f), distance: %.2f\n", 
                       i, doorWorldPos.x, doorWorldPos.y, doorWorldPos.z, dist);
            }
        }

        // Toggle only the closest door
        if (closestDoorIdx >= 0)
        {
            printf("Toggling door %d\n", closestDoorIdx);
            if (fabs(doors[closestDoorIdx].angle - doors[closestDoorIdx].targetAngle) < 0.01f)
            {
                if (doors[closestDoorIdx].targetAngle < 0.1f)
                    doors[closestDoorIdx].targetAngle = 1.5f;
                else
                    doors[closestDoorIdx].targetAngle = 0.0f;
            }
        }
    }
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