#define MAIN
#include "MicroGlut.h"
#include "GL_utilities.h"
#include "VectorUtils4.h"
#include "LittleOBJLoader.h"
#include "LoadTGA.h"
#include "Camera.h"
#include "GhostMovement.h"


#define cameraSpeed 0.3


const float flashlightCos = 0.97;

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
GLuint crossProgram = 0;
GLuint crossVAO = 0;
GLuint crossVBO = 0;
GLuint promptVAO = 0;
GLuint promptVBO = 0;
GLuint tex1, tex2, tex3, tex4; // tex4 for mirror
GLuint texWall, texFloor, texDoor, texCeiling, texWindow; // House textures

bool gPlayerDead = false;
bool gPlayerWon = false;

std::vector<vec3> gGhostPath;

GLuint deathTex = 0;
GLuint winTex = 0;
GLuint deathVAO = 0;
GLuint deathVBO = 0;

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

bool ghostCollisionTriggered = false;

Model *mirrorModel;
vec3 mirror[] = {
	vec3(-1, -1,0),
	vec3(-1, 1, 0),
	vec3( 1, 1, 0),
	vec3( 1, -1, 0)
};
vec2 mirrortex[] = {
		vec2(-1,-1),
		vec2(-1, 1),
		vec2( 1, 1),
		vec2( 1,-1)
		};
GLuint mirrorIndices[] = {0, 1, 2, 0, 2, 3};


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


void loadShader(){
	program = loadShaders("projekt.vert", "projekt.frag");
}
void loadTextures(){
	LoadTGATextureSimple("maskros512.tga", &tex1);
	LoadTGATextureSimple("ghost_texture.tga", &tex3);
	LoadTGATextureSimple("maskros512.tga", &tex4);
	//LoadTGATextureSimple("44-terrain.tga", &tex2);
	LoadTGATextureSimple("house-objects/house-textures/wall_diffuse.tga", &texWall);
	LoadTGATextureSimple("house-objects/house-textures/floor_tiles.tga", &texFloor);
	LoadTGATextureSimple("house-objects/house-textures/floor_tiles.tga", &texDoor); // TODO: Separate door texture
	LoadTGATextureSimple("house-objects/house-textures/shatteredglass.tga", &texWindow);
	
	LoadTGATextureSimple("house-objects/house-textures/ceiling.tga", &texCeiling);
	LoadTGATextureData("fft-terrain.tga", &ttex);
	LoadTGATextureSimple("house-objects/house-textures/death.tga", &deathTex);
	LoadTGATextureSimple("house-objects/house-textures/win_screen.tga", &winTex);


}
void loadObjects(){
	
	mirrorModel = LoadDataToModel(
			mirror,
			NULL,
			mirrortex,
			NULL,
			mirrorIndices,
			4,
			6);

	tm = GenerateTerrain(&ttex);
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


}

// helper for mirrors
vec3 reflectVector(vec3 v, vec3 n)
{
    return v - 2.0f * dot(v, n) * n;
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

void init(void)
{
	projectionMatrix = frustum(-0.1, 0.1, -0.1, 0.1, 0.2, 5000.0);
	glutWarpPointer(1024/2, 768/2);
	// Load and compile shader
	glUseProgram(program);
	printError("init shader");
	
	glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);

	// Upload light parameters
	glUniform3fv(glGetUniformLocation(program, "lightPos"), 1, &lightPos.x);
	glUniform3fv(glGetUniformLocation(program, "lightColor"), 1, &lightColor.x);

	// For flashlight
	glUniform1f(glGetUniformLocation(program, "flashlightCos"), flashlightCos);


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex1);		// Bind Our Texture tex1
	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, tex2);		// Bind Our Texture tex2

	// UI shader + attribute locations
    crossProgram = loadShaders("ui.vert", "ui.frag");
    GLint posLoc = glGetAttribLocation(crossProgram, "inPos");
    GLint uvLoc = glGetAttribLocation(crossProgram, "inUV");

    // Crosshair geometry
    float crossVerts[] = {
        -0.02f,  0.0f,  0.02f,  0.0f,
        0.0f, -0.02f,  0.0f,  0.02f
    };

    glGenVertexArrays(1, &crossVAO);
    glGenBuffers(1, &crossVBO);

    glBindVertexArray(crossVAO);
    glBindBuffer(GL_ARRAY_BUFFER, crossVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(crossVerts), crossVerts, GL_STATIC_DRAW);

    glEnableVertexAttribArray(posLoc);
    glVertexAttribPointer(posLoc, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    glBindVertexArray(0);

    // Door prompt geometry
    float promptVerts[] = {
        -0.12f, -0.10f,
        0.12f, -0.10f,
        0.12f, -0.06f,
        -0.12f, -0.06f
    };

    glGenVertexArrays(1, &promptVAO);
    glGenBuffers(1, &promptVBO);

    glBindVertexArray(promptVAO);
    glBindBuffer(GL_ARRAY_BUFFER, promptVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(promptVerts), promptVerts, GL_STATIC_DRAW);

    glEnableVertexAttribArray(posLoc);
    glVertexAttribPointer(posLoc, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    glBindVertexArray(0);

    // Death screen texture + fullscreen quad (pos + uv)
    
    float deathVerts[] = {
        //  x,   y,   u,  v
        -1.f, -1.f,  0.f, 0.f,
         1.f, -1.f,  1.f, 0.f,
         1.f,  1.f,  1.f, 1.f,
        -1.f,  1.f,  0.f, 1.f
    };

    glGenVertexArrays(1, &deathVAO);
    glGenBuffers(1, &deathVBO);

    glBindVertexArray(deathVAO);
    glBindBuffer(GL_ARRAY_BUFFER, deathVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(deathVerts), deathVerts, GL_STATIC_DRAW);

    glEnableVertexAttribArray(posLoc);
    glVertexAttribPointer(posLoc, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(uvLoc);
    glVertexAttribPointer(uvLoc, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindVertexArray(0);


	glUniform1i(glGetUniformLocation(program, "tex1"), 0); // Texture unit 0
	
	// Set up ghost
	gGhostPath = {
		//corner room - start
		{4.0f, 1.0f, 7.0f}, 
    	{3.0f, 1.0f, 7.0f},
      	{4.6f, 1.0f, 10.0f},
		{5.04f, 1.80f, 11.25f},
		//bed room
		{5.09f, 1.83f, 14.65f}, 
		{5.04f, 1.80f, 11.25f}, 
		//Living room
		{6.76f, 1.80f, 12.05f}, 
		{6.59f, 1.80f, 6.10f},
		{8.38f, 1.80f, 6.22f}, 
		{8.38f, 1.80f, 12.00f},
		
		
	    
		//corner room - start
		{4.0f, 1.0f, 7.0f}, 
    	{3.0f, 1.0f, 7.0f},
      	{4.6f, 1.0f, 10.0f},
		{5.04f, 1.80f, 11.25f},
		//bed room
		{5.09f, 1.83f, 14.65f}, 
		{5.04f, 1.80f, 11.25f}, 
		//Living room
		{6.76f, 1.80f, 12.05f}, 
		{6.59f, 1.80f, 6.10f},
		{8.38f, 1.80f, 6.22f}, 
		{8.38f, 1.80f, 12.00f},
		
		
	    
	    };
    
	// Set up the ghost: Start Pos, Speed, Path
	initGhost(vec3(4, 1, 7), 1.0f, gGhostPath);
    gGhostYawOffset = -M_PI/2.0; // Tweak this if the model faces the wrong way

}

void resetGame()
{
	gPlayerDead = false;
	gPlayerWon = false;
	ghostCollisionTriggered = false;

	camera.setPosition(vec3(12.18f, 1.80f, 9.05f));
	initGhost(vec3(4, 1, 7), 1.0f, gGhostPath);

	for (size_t i = 0; i < doors.size(); ++i)
	{
		doors[i].angle = 0.0f;
		doors[i].targetAngle = 0.0f;
	}
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
void onGhostCollision() {
	camera.setPosition(vec3(7.0f, 1.0f, 18.0f)); 
	
	gPlayerDead = true;
}
void handleCollision(mat4 houseMatrix, vec3 lastCameraPosition)
{
	bool colliding = false;
	vec3 cam = camera.getPosition();
	vec3 enemy = gGhostPos;
	vec3 outsidePos = vec3(6.74, 1.80, 16.34);

	if (sphereCollision(cam, camera.cameraRadius, enemy, 1.0f))
	{
		if (!ghostCollisionTriggered)
        {
            onGhostCollision();
            ghostCollisionTriggered = true;
        }
	}
	if (sphereCollision(cam, camera.cameraRadius, outsidePos, 1.0f))
	{
		gPlayerWon = true;
	}

	else
    {
        ghostCollisionTriggered = false;
    }

	if (modelCollision(modelWall, cam, camera.cameraRadius, houseMatrix))
	{
		printf("House Collision\n");
		colliding = true;
	}

    for (int i = 0; i < doors.size(); i++) {
        
        mat4 doorMatrix = Mult(houseMatrix,
            Mult(T(doors[i].pivot.x, doors[i].pivot.y, doors[i].pivot.z),
            Mult(Ry(doors[i].angle),
                T(-doors[i].pivot.x, -doors[i].pivot.y, -doors[i].pivot.z))));

        if (modelCollision(doors[i].model, cam, camera.cameraRadius, doorMatrix))
        {
            colliding = true;
        }
    }

	if (colliding)
	{
		camera.setPosition(lastCameraPosition);
	}
}


void renderScene(mat4 viewMatrix, float deltaTime)
{
	
    glUniformMatrix4fv(glGetUniformLocation(program, "viewMatrix"), 1, GL_TRUE, viewMatrix.m);

    mat4 modelToWorld = IdentityMatrix();
    glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_TRUE, modelToWorld.m);
	
    // housee 
    vec3 housePos = vec3(10.0f, 0.0f, 10.0f);
    housePos.y = getHeight(housePos.x, housePos.z);
    mat4 houseMatrix = T(housePos.x, housePos.y + 0.5f, housePos.z);
    glUniformMatrix4fv(glGetUniformLocation(program, "placeDowns"), 1, GL_TRUE, houseMatrix.m);

    // walls
    glBindTexture(GL_TEXTURE_2D, texWall);
    DrawModel(modelWall, program, "inPosition", "inNormal", "inTexCoord");

    // frames
    glBindTexture(GL_TEXTURE_2D, tex3);
    DrawModel(modelFrames, program, "inPosition", "inNormal", "inTexCoord");

    // floor
    glBindTexture(GL_TEXTURE_2D, texFloor);
    DrawModel(modelFloor, program, "inPosition", "inNormal", "inTexCoord");

    // ceiling
    glBindTexture(GL_TEXTURE_2D, texCeiling);
    DrawModel(modelCeiling, program, "inPosition", "inNormal", "inTexCoord");

    // windows
    glBindTexture(GL_TEXTURE_2D, texWindow);
    DrawModel(modelWindow, program, "inPosition", "inNormal", "inTexCoord");

    // furniture
    glBindTexture(GL_TEXTURE_2D, texCeiling);
    DrawModel(modelFurnitues, program, "inPosition", "inNormal", "inTexCoord");

    // doors
    glBindTexture(GL_TEXTURE_2D, texDoor);


    for (int i = 0; i < doors.size(); i++) {

		
        // (Animation)
		for (int i = 0; i < doors.size(); i++)
		{
			// animation 
			if (doors[i].angle < doors[i].targetAngle) doors[i].angle += doorSpeed * deltaTime;
			if (doors[i].angle > doors[i].targetAngle) doors[i].angle -= doorSpeed * deltaTime;
			if (fabs(doors[i].angle - doors[i].targetAngle) < 0.01f) doors[i].angle = doors[i].targetAngle;

			mat4 doorMatrix = Mult(houseMatrix,
				Mult(T(doors[i].pivot.x, doors[i].pivot.y, doors[i].pivot.z),
				Mult(Ry(doors[i].angle),
					T(-doors[i].pivot.x, -doors[i].pivot.y, -doors[i].pivot.z))));

			glUniformMatrix4fv(glGetUniformLocation(program, "placeDowns"), 1, GL_TRUE, doorMatrix.m);
			DrawModel(doors[i].model, program, "inPosition", "inNormal", "inTexCoord");
		}
	}
	// remove this comment to see ghost
    // ghost
	/*
    glBindTexture(GL_TEXTURE_2D, tex3);
    mat4 ghostMatrix = getGhostMoveMatrix();
    glUniformMatrix4fv(glGetUniformLocation(program, "placeDowns"), 1, GL_TRUE, ghostMatrix.m);
    DrawModel(ghost, program, "inPosition", "inNormal", "inTexCoord");
	*/
	
    // terrain
    glBindTexture(GL_TEXTURE_2D, tex1);
    mat4 placeDowns = mat4(1.0);
    glUniformMatrix4fv(glGetUniformLocation(program, "placeDowns"), 1, GL_TRUE, placeDowns.m);
    DrawModel(tm, program, "inPosition", "inNormal", "inTexCoord");
	

}
void display(void)
{
    static float lastTime = 0;
    float currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    float deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    vec3 lastCameraPosition = camera.getPosition();

    // update positions
    camera.update(deltaTime);
    updateGhost(deltaTime);

	
	float ghostDist = vecLength(camera.getPosition() - gGhostPos);
    float ghostIntensity = 1.0f - (ghostDist / 8.0f);
    if (ghostIntensity < 0.0f) ghostIntensity = 0.0f;
    if (ghostIntensity > 1.0f) ghostIntensity = 1.0f;

    // collision
    vec3 housePos = vec3(10.0f, 0.0f, 10.0f);
    housePos.y = getHeight(housePos.x, housePos.z);
    mat4 houseMatrix = T(housePos.x, housePos.y + 0.5f, housePos.z);

    handleCollision(houseMatrix, lastCameraPosition);
    mat4 view = camera.getViewMatrix();

	mat4 modelToWorld = IdentityMatrix();
	mat4 mirrorMatrix = Mult(T(7.5f, 1.5f, 6.0f), Ry(M_PI)); // place mirror

	// RENDER SCENE
	glUseProgram(program);

	// Flashlight
	vec3 cp = camera.getPosition();
	glUniform3f(glGetUniformLocation(program, "cameraPos"), cp.x, cp.y, cp.z);
	vec3 cd = camera.getForward();
	glUniform3f(glGetUniformLocation(program, "cameraDir"), cd.x, cd.y, cd.z);

	//Upload modelmatrix not used for ghost and terrain
    glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_TRUE, modelToWorld.m);
	glUniform1f(glGetUniformLocation(program, "ghostIntensity"), ghostIntensity);

	glUniform1f(glGetUniformLocation(program, "blend"), 1.0); // Blend value
	glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
	
	// Algorithm:
	// 1. Clear the stencil buffer
	// 2. Draw the ground ONLY in stencil
	// 3. Draw the mirrored teapot (behind the ground), masked by stencil buffer
	// 4. Draw the ground with some transparency, to make the mirrored geometry visible
	// 5. Draw the teapot in a position matching the mirrored one
	
	glClearColor(0,0,0,0); // Not black - we want to see if the mirrored image is drawn outside the plane!
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // Clear Screen, Depth Buffer and Stencil Buffer
	glClearStencil(2); // No, stencil must be set to 2 outside the plane or it will draw double shadows there!
	
	// ** Mirror in stencil
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 1, 0xFFFFFFFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE); // Write 1 if pass! Otherwise keep! Don't draw in invisible areas.

	// Turn off Color Buffer and Depth Buffer
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_FALSE);
	
	
    glBindTexture(GL_TEXTURE_2D, texWall);
    mat4 placeDowns = mat4(1.0);
    glUniformMatrix4fv(glGetUniformLocation(program, "placeDowns"), 1, GL_TRUE, mirrorMatrix.m);
    glUniformMatrix4fv(glGetUniformLocation(program, "viewMatrix"), 1, GL_TRUE, view.m);
	DrawModel(mirrorModel, program, "inPosition", "inNormal", "inTexCoord");


	// ** Ghost mirrored - with stencil!

	// Turn on Color Buffer and Depth Buffer
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthMask(GL_TRUE);
	
	glEnable(GL_STENCIL_TEST); // Try disabling this!
	glStencilFunc(GL_EQUAL, 1, 0xFFFFFFFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_INCR); // Skriv 1 om pass! Annars keep! Vi vill inte rita i osynlig yta!
	glCullFace(GL_FRONT); // Since the teapot is mirrored, culling must be reversed

	mat4 reflectMatrix =
    T(7.5f, 1.5f, 6.0f) *Ry(M_PI) *S(1.0f, 1.0f, -1.0f) *Ry(-M_PI) *T(-7.5f, -1.5f, -6.0f);

	mat4 ghostMatrix = reflectMatrix * getGhostMoveMatrix();
	glBindTexture(GL_TEXTURE_2D, tex3);
	
    glUniformMatrix4fv(glGetUniformLocation(program, "placeDowns"), 1, GL_TRUE, ghostMatrix.m);
	glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
	glUniformMatrix4fv(glGetUniformLocation(program, "viewMatrix"), 1, GL_TRUE, view.m);
	glUniform1f(glGetUniformLocation(program, "blend"), 1.0); // Blend value
	DrawModel(ghost, program, "inPosition", "inNormal", "inTexCoord");

	//test with adding more stuff into the mirror
	mat4 reflectedHouse = reflectMatrix*houseMatrix;
    glUniformMatrix4fv(glGetUniformLocation(program, "placeDowns"), 1, GL_TRUE, reflectedHouse.m);

    // furtniture
    glBindTexture(GL_TEXTURE_2D, texCeiling);
    DrawModel(modelFurnitues, program, "inPosition", "inNormal", "inTexCoord");

	//test end

	// ** Mirror in image
	glDisable(GL_STENCIL_TEST);
	glCullFace(GL_BACK); // Standard culling
	
	glBindTexture(GL_TEXTURE_2D, texWall);
	glUniform1f(glGetUniformLocation(program, "blend"), 0.25); // Blend value
	placeDowns = mat4(1.0);
    glUniformMatrix4fv(glGetUniformLocation(program, "placeDowns"), 1, GL_TRUE, mirrorMatrix.m);
	glUniformMatrix4fv(glGetUniformLocation(program, "viewMatrix"), 1, GL_TRUE, view.m);
    DrawModel(mirrorModel, program, "inPosition", "inNormal", "inTexCoord");

	
	glDisable(GL_STENCIL_TEST);
	// Rest of the scene drawn normally
	glUniform1f(glGetUniformLocation(program, "blend"), 1.0); // Blend value
	renderScene(view, deltaTime);
    bool showDoorPrompt = false;
    float bestDist = 9999.0f;

    for (size_t i = 0; i < doors.size(); ++i)
    {
        vec3 doorWorldPos = MultVec3(houseMatrix, doors[i].pivot);
        vec3 diff = camPos - doorWorldPos;
        float dist = sqrt(diff.x*diff.x + diff.y*diff.y + diff.z*diff.z);

        if (dist < bestDist) bestDist = dist;
    }

    showDoorPrompt = (bestDist < 2.5f); // justera radie
//
    glDisable(GL_DEPTH_TEST);
    if (showDoorPrompt)
    {
        glUseProgram(crossProgram);
        glUniform3f(glGetUniformLocation(crossProgram, "crossColor"), 1.0f, 0.8f, 0.2f);

        glBindVertexArray(promptVAO);
        glDrawArrays(GL_LINE_LOOP, 0, 4);
        glBindVertexArray(0);
    }

    glUseProgram(crossProgram);
    glUniform3f(glGetUniformLocation(crossProgram, "crossColor"), 1.0f, 1.0f, 1.0f);

    glBindVertexArray(crossVAO);
    glDrawArrays(GL_LINES, 0, 4);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);

    if (gPlayerDead){
        glDisable(GL_DEPTH_TEST);
        glUseProgram(crossProgram);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, deathTex);
        glUniform1i(glGetUniformLocation(crossProgram, "uiTex"), 0);
        glUniform1i(glGetUniformLocation(crossProgram, "useTexture"), 1);

        glBindVertexArray(deathVAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glBindVertexArray(0);

        glUniform1i(glGetUniformLocation(crossProgram, "useTexture"), 0);
        glEnable(GL_DEPTH_TEST);
    }
	else if (gPlayerWon){
        glDisable(GL_DEPTH_TEST);
        glUseProgram(crossProgram);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, winTex);
        glUniform1i(glGetUniformLocation(crossProgram, "uiTex"), 0);
        glUniform1i(glGetUniformLocation(crossProgram, "useTexture"), 1);

        glBindVertexArray(deathVAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glBindVertexArray(0);

        glUniform1i(glGetUniformLocation(crossProgram, "useTexture"), 0);
        glEnable(GL_DEPTH_TEST);
    }

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
		//printf("Camera position: (%.2f, %.2f, %.2f)\n", camPos.x, camPos.y, camPos.z);
		
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
			if (dist < 5.0f && dist < closestDist)
            {
                closestDist = dist;
                closestDoorIdx = (int)i;

            }
        }

        // Toggle only the closest door
		// i mouseClick:
		if (closestDoorIdx >= 0)
        {
            if (doors[closestDoorIdx].angle < 0.1f)
                doors[closestDoorIdx].targetAngle = 1.5f;
            else
                doors[closestDoorIdx].targetAngle = 0.0f;
            
        }
		vec3 pos = camera.getPosition();
		//printf("vec3(%2f, %2f, %2f)\n", pos.x, pos.y,pos.z);

    }
	
}

void processNormalKeys(unsigned char key, int x, int y)
{
	if (key == '\r' || key == '\n')
	{
		if (gPlayerDead || gPlayerWon)
		{
			resetGame();
			return;
		}
	}

	if (key == 27) 
		exit(0);
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL);
	glutInitContextVersion(3, 2);
	glutInitWindowSize (1024, 768);

	glutCreateWindow ("TSBK07 PROJEKT");
	
	// Load shaders, textures and objects
	loadShader();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	loadTextures();
	loadObjects();

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.4,0.6,0.9,0);
	glEnable(GL_CULL_FACE);

	//glutFullScreen();
	init ();
	glutDisplayFunc(display);
	
	glutHideCursor();
	glutRepeatingTimer(20);
	
	glutPassiveMotionFunc(mouse);
	glutMouseFunc(mouseClick);
	glutKeyboardFunc(processNormalKeys);

	glutMainLoop();
	exit(0);
}