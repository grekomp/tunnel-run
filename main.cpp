#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <IL\il.h>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <ctime>

#include "CorridorSegment.h"
#include "Common.h"
#include "shaders.h"
#include "Model.h"
#include "Game.h"

// Config
const int WIDTH = 1280;
const int HEIGHT = 720;
const float ROT_STEP = 10.0f;
const float SCALE_STEP = 0.02f;

void onShutdown();
void printStatus();
void initGL();
void changeSize(GLsizei w, GLsizei h);
void updateProjectionMatrix();
void renderScene();
void keyboard(unsigned char key, int x, int y);
void specialKeys(int key, int x, int y);
void setupShaders();
void setupBuffers();
void setupTextures();
void setupTexture(GLenum target, GLuint texture, const wchar_t *fileName);
void loadTexture(GLuint texture, const wchar_t *fileName);

//******************************************************************************************
GLuint shaderPrograms[2];

// Matrices
glm::mat4 projMatrix;

// Textures
std::vector<const wchar_t *> textureFiles = {
	L"wall-diff2.tif",
	L"wall-disp2.tif",
	L"wall-norm.tif",
	L"cobblestone-diff.tif",
	L"cobblestone-disp.tif",
	L"cobblestone-norm.tif",
	L"obstacle-diff.tif",
	L"obstacle-spec.tif",
	L"obstacle-norm.tif",
	L"marble-diff.jpg",
	L"marble-spec.jpg",
	L"marble-norm.jpg",
	L"concrete-diff.tif",
	L"concrete-disp.tif",
	L"concrete-norm.tif"
};
const int nTextures = 15;
GLuint textures[nTextures];
GLuint tex_cube;

Texture wallTexture;
Texture floorTexture;
Texture ceilingTexture;
Texture obstacleTexture;
Texture ballTexture;

// Light parameters
glm::vec4 lightPosition = glm::vec4(20.0f, 20.0f, 15.0f, 1.0f);
glm::vec3 lightAmbient = glm::vec3(0.2f, 0.2f, 0.2f);
glm::vec3 lightDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3 lightSpecular = glm::vec3(1.0, 1.0, 1.0);

// Options
float fovy = 45.0f;
float aspectRatio = (float)WIDTH / HEIGHT;

// ---------------
Model models[5];

// Game
Game game;
CorridorSegment baseCorridor;
Obstacle baseObstacle;
Pickup basePickup;
std::vector<Segment> baseSegments;

//******************************************************************************************
int main(int argc, char *argv[])
{
	srand((int)time(NULL));

	atexit( onShutdown );

    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );

	glutInitContextVersion( 3, 3 );
	glutInitContextFlags( GLUT_DEBUG );
	glutInitContextProfile( GLUT_CORE_PROFILE );

	glutSetOption( GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS );

	glutInitWindowPosition(0, 0);
	//glutInitWindowPosition((1920 - WIDTH) / 2, (1080 - HEIGHT) / 2);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("Tunnel Roll");

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if( err != GLEW_OK )	
	{
		std::cerr << "Blad: " << glewGetErrorString( err ) << std::endl;
		exit( 1 );
	}

	if( !GLEW_VERSION_3_3 )
	{
		std::cerr << "Brak obslugi OpenGL 3.3\n";
		exit( 2 );
	}
    
    glutReshapeFunc(changeSize);
    glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);

	glutKeyboardFunc( keyboard );
	glutSpecialFunc( specialKeys );

	initGL();

    glutMainLoop();
    
    return 0;
}

void onShutdown()
{
	glDeleteProgram(shaderPrograms[0]);
	glDeleteProgram(shaderPrograms[1]);
}

void printStatus()
{
	/*std::cout << "GLEW = "		  << glewGetString( GLEW_VERSION ) << std::endl;
	std::cout << "GL_VENDOR = "	  << glGetString( GL_VENDOR ) << std::endl;
	std::cout << "GL_RENDERER = " << glGetString( GL_RENDERER ) << std::endl;
	std::cout << "GL_VERSION = "  << glGetString( GL_VERSION ) << std::endl;
	std::cout << "GLSL = "		  << glGetString( GL_SHADING_LANGUAGE_VERSION ) << std::endl << std::endl;
	*/
	system("cls");

	std::cout << "================================" << std::endl;
	std::cout << "= TUNNEL ROLL                  =" << std::endl;
	std::cout << "================================" << std::endl << std::endl;

	std::cout << "---- Controls ----" << std::endl;
	std::cout << "* a/left arrow - move left" << std::endl;
	std::cout << "* d/right arrow - move right" << std::endl;
	std::cout << "* r - restart game" << std::endl;
	std::cout << "* esc - exit game" << std::endl << std::endl;

	std::cout << "-- Instructions --" << std::endl;
	std::cout << "* Avoid obstacles " << std::endl;
	std::cout << "* Collect diamonds" << std::endl;
	std::cout << "* Survive as long as you can" << std::endl;
	std::cout << "* Have fun! :D" << std::endl << std::endl << std::endl;
}

void initGL()
{
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_PRIMITIVE_RESTART);

	setupShaders();
	setupTextures();
	setupBuffers();

	printStatus();
}

void changeSize(GLsizei w, GLsizei h)
{
	if( h == 0 )
		h = 1;
    glViewport( 0, 0, w, h );

	aspectRatio = (float)w / h;

	updateProjectionMatrix();
}

void updateProjectionMatrix()
{
	projMatrix = glm::perspective( glm::radians( fovy ), aspectRatio, 1.0f, 100.0f );
}

void renderScene()
{
	// Clearing the screen
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// Picking and using a shader program
	glUseProgram( shaderPrograms[0] );

	// Sending the projection matrix
	glUniformMatrix4fv(PROJECTION_MATRIX_LOCATION, 1, GL_FALSE, glm::value_ptr(projMatrix));

	// Sending light options
	glUniform3fv(LIGHT_AMBIENT_LOCATION, 1, glm::value_ptr(lightAmbient));
	glUniform3fv(LIGHT_DIFFUSE_LOCATION, 1, glm::value_ptr(lightDiffuse));
	glUniform3fv(LIGHT_SPECULAR_LOCATION, 1, glm::value_ptr(lightSpecular));
	
	// Rendering game objects
	game.NextFrame();
	game.Render();

	// Pickups shader program
	glUseProgram(shaderPrograms[1]);

	// Sending the projection matrix
	glUniformMatrix4fv(PROJECTION_MATRIX_LOCATION, 1, GL_FALSE, glm::value_ptr(projMatrix));

	// Sending light options
	glUniform3fv(LIGHT_AMBIENT_LOCATION, 1, glm::value_ptr(lightAmbient));
	glUniform3fv(LIGHT_DIFFUSE_LOCATION, 1, glm::value_ptr(lightDiffuse));
	glUniform3fv(LIGHT_SPECULAR_LOCATION, 1, glm::value_ptr(lightSpecular));

	// Rendering pickups
	game.RenderPickups();

    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y)
{
	switch( key )
	{
		case 27: // ESC
			exit( 0 );
			break;
		case 'a':
			game.status.lane--;
			if (game.status.lane < 1) game.status.lane = 1;
			break;
		case 'd':
			game.status.lane++;
			if (game.status.lane > 3) game.status.lane = 3;
			break;
		case 'r':
			printStatus();
			game.Setup();
			game.PostSetup();
			break;
	}

	glutPostRedisplay();
}

void specialKeys(int key, int x, int y)
{
	switch( key )
	{
		case GLUT_KEY_F4:
			if( glutGetModifiers() == GLUT_ACTIVE_ALT )
				exit( 0 );
			break;
		case GLUT_KEY_LEFT:
			game.status.lane--;
			if (game.status.lane < 1) game.status.lane = 1;
			break;
		case GLUT_KEY_RIGHT:
			game.status.lane++;
			if (game.status.lane > 3) game.status.lane = 3;
			break;
	}

	glutPostRedisplay();
}

void setupShaders()
{
	// Setting up shaders with textures
	if (!setupShaders("shaders/vertex-texture.vert", "shaders/fragment-texture.frag", shaderPrograms[0])) {
		std::cout << "Failed to setup shaders - texture" << std::endl;
	}
	if (!setupShaders("shaders/pickup.vert", "shaders/pickup.frag", shaderPrograms[1])) {
		std::cout << "Failed to setup shaders - pickup" << std::endl;
	}
}

void setupBuffers()
{
	// ----------------
	// Setup Corridor

	// Setup Wall
	Model wall;
	wall.LoadModelFromFile("models/wall.obj");
	wall.position = glm::vec4(0.0, 0.0, 0.0, 1.0);

	// Wall material
	wall.material.texture = wallTexture;
	wall.material.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
	wall.material.diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	wall.material.specular = glm::vec3(0.8f, 0.8f, 0.8f);
	wall.material.shininess = 80.0f;
	wall.material.parallaxStrength = 0.2f;

	// Buffering wall
	wall.Buffer();

	// Adding wall to corridor
	baseCorridor.models.push_back(wall);

	// Setup Floor
	Model floor;
	floor.LoadModelFromFile("models/floor.obj");
	floor.position = glm::vec4(0.0, 0.0, 0.0, 1.0);
	
	// Floor Material
	floor.material.texture = floorTexture;
	floor.material.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
	floor.material.diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	floor.material.specular = glm::vec3(0.8f, 0.8f, 0.8f);
	floor.material.shininess = 80.0f;

	// Buffering floor
	floor.Buffer();

	// Adding floor to corridor
	baseCorridor.models.push_back(floor);

	// Setup ceiling
	Model ceiling;
	ceiling.LoadModelFromFile("models/ceiling.obj");
	ceiling.position = glm::vec4(0.0, 0.0, 0.0, 1.0);

	// Ceiling Material
	ceiling.material.texture = ceilingTexture;
	ceiling.material.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
	ceiling.material.diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	ceiling.material.specular = glm::vec3(0.8f, 0.8f, 0.8f);
	ceiling.material.shininess = 80.0f;
	
	// Buffering ceiling
	ceiling.Buffer();
	
	// Adding ceiling to corridor
	baseCorridor.models.push_back(ceiling);

	// Setup columns
	Model columns;
	columns.LoadModelFromFile("models/wallColumn.obj");
	columns.position = glm::vec4(0.0, 0.0, 0.0, 1.0);

	// Column Material
	columns.material.texture = wallTexture;
	columns.material.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
	columns.material.diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	columns.material.specular = glm::vec3(0.8f, 0.8f, 0.8f);
	columns.material.parallaxStrength = 0.05f;
	columns.material.shininess = 80.0f;

	// Buffering columns
	columns.Buffer();

	// Adding columns to corridor
	baseCorridor.models.push_back(columns);

	// Setting corridor options
	baseCorridor.position = glm::vec4(0, 0, 10, 1);
	baseCorridor.length = 4.0f;

	// Assigning corridor
	game.corridorBase = baseCorridor;

	// ----------------
	// Setup obstacle
	Model obstacle;
	obstacle.LoadModelFromFile("models/obstacle.obj");
	obstacle.position = glm::vec4(0.0, 0.0, 0.0, 1.0);

	//Obstacle Material
	obstacle.material.texture = obstacleTexture;
	obstacle.material.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
	obstacle.material.diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	obstacle.material.specular = glm::vec3(0.8f, 0.8f, 0.8f);
	obstacle.material.parallaxStrength = 0.1f;
	obstacle.material.shininess = 200.0f;

	// Buffering obstacle
	obstacle.Buffer();

	// Adding obstacle 
	baseObstacle.model = obstacle;
	baseObstacle.lane = 1;

	game.obstacleBase = baseObstacle;

	Obstacle baseObstacle2 = baseObstacle;
	baseObstacle2.lane = 2;

	Obstacle baseObstacle3 = baseObstacle;
	baseObstacle3.lane = 3;

	//Setup pickup
	Model pickup;
	pickup.LoadModelFromFile("models/diamond.obj");
	pickup.position = glm::vec4(0.0, 0.0, 0.0, 1.0);

	//Pickup Material
	pickup.material.ambient = glm::vec3(0.3f, 0.3f, 1.0f);
	pickup.material.diffuse = glm::vec3(1.0f, 0.829f, 0.829f);
	pickup.material.specular = glm::vec3(0.992157f, 0.941176f, 0.807843f);
	pickup.material.parallaxStrength = 0.05f;
	pickup.material.shininess = 20.0f;

	// Buffering pickup
	pickup.Buffer();

	// Adding pickup
	basePickup.model = pickup;
	basePickup.cubemapTexture = tex_cube;
	basePickup.lane = 3;
	basePickup.score = 100;
	basePickup.isActive = true;
	
	game.pickupBase = basePickup;

	// Segment
	Segment baseSegment;
	baseSegment.position = glm::vec4(0.0, 0.0, 0.0, 1.0);
	baseSegment.length = 8.0f;
	
	// Segment: ob - no - ob
	baseSegment.obstacles.push_back(baseObstacle);
	baseSegment.obstacles.push_back(baseObstacle3);
	baseSegments.push_back(baseSegment);

	// Segment: ob - ob - pi
	baseSegment.obstacles.clear();
	baseSegment.obstacles.push_back(baseObstacle);
	baseSegment.obstacles.push_back(baseObstacle2);
	baseSegment.pickups.push_back(basePickup);
	baseSegments.push_back(baseSegment);

	// Segment: ob - ob - no
	baseSegment.pickups.clear();
	baseSegments.push_back(baseSegment);

	// Segment: ob - pi - ob
	baseSegment.obstacles.clear();
	baseSegment.pickups.clear();
	baseSegment.obstacles.push_back(baseObstacle);
	baseSegment.obstacles.push_back(baseObstacle3);
	basePickup.lane = 2;
	baseSegment.pickups.push_back(basePickup);
	baseSegments.push_back(baseSegment);

	// Segment: pi - ob - ob
	baseSegment.obstacles.clear();
	baseSegment.pickups.clear();
	baseSegment.obstacles.push_back(baseObstacle2);
	baseSegment.obstacles.push_back(baseObstacle3);
	basePickup.lane = 1;
	baseSegment.pickups.push_back(basePickup);
	baseSegments.push_back(baseSegment);

	// Segment: no - ob - ob
	baseSegment.pickups.clear();
	baseSegments.push_back(baseSegment);

	// Segment: pi - no - no
	baseSegment.length = 3.0f;
	baseSegment.obstacles.clear();
	baseSegment.pickups.clear();
	basePickup.lane = 1;
	baseSegment.pickups.push_back(basePickup);
	baseSegments.push_back(baseSegment);

	// Segment: no - pi - no
	baseSegment.length = 3.0f;
	baseSegment.obstacles.clear();
	baseSegment.pickups.clear();
	basePickup.lane = 2;
	baseSegment.pickups.push_back(basePickup);
	baseSegments.push_back(baseSegment);

	// Segment: no - no - pi
	baseSegment.length = 3.0f;
	baseSegment.obstacles.clear();
	baseSegment.pickups.clear();
	basePickup.lane = 3;
	baseSegment.pickups.push_back(basePickup);
	baseSegments.push_back(baseSegment);

	// Segment: ob - no - no
	baseSegment.length = 6.0f;
	baseSegment.obstacles.clear();
	baseSegment.pickups.clear();
	baseSegment.obstacles.push_back(baseObstacle);
	baseSegments.push_back(baseSegment);

	// Segment: no - ob - no
	baseSegment.length = 6.0f;
	baseSegment.obstacles.clear();
	baseSegment.pickups.clear();
	baseSegment.obstacles.push_back(baseObstacle2);
	baseSegments.push_back(baseSegment);

	// Segment: no - no - ob
	baseSegment.length = 6.0f;
	baseSegment.obstacles.clear();
	baseSegment.pickups.clear();
	baseSegment.obstacles.push_back(baseObstacle3);
	baseSegments.push_back(baseSegment);

	// Setting base segment
	game.baseSegments = baseSegments;

	// ----------------
	// Ball
	Model ball;
	ball.LoadModelFromFile("models/ball.obj");
	ball.material.texture = ballTexture;
	ball.material.ambient = glm::vec3(0.2, 0.2, 0.2);
	ball.material.shininess = 40.0f;
	ball.Buffer();

	game.ball = ball;

	// ----------------
	// GameOver text
	Model gameover;
	gameover.LoadModelFromFile("models/gameover.obj");
	gameover.material.ambient = glm::vec3(1, 1, 1);
	gameover.material.diffuse = glm::vec3(1, 1, 1);
	gameover.material.specular = glm::vec3(0.9, 0.9, 0.9);
	gameover.material.ignoreTextures = true;
	gameover.material.shininess = 50;

	gameover.Buffer();

	game.gameoverText = gameover;

	// ----------------
	// Setting up first frame
	game.PostSetup();
}

// ------------------------------------------------
void setupTextures() {
	// Generating and binding texture identifier
	glGenTextures(nTextures, textures);

	// Setting up all 2d textures
	for (int i = 0; i < nTextures; i++) {
		setupTexture(GL_TEXTURE_2D, textures[i], textureFiles[i]);
	}

	// Assigning textures
	wallTexture.diffuse = textures[0];
	wallTexture.specular = textures[1];
	wallTexture.normal = textures[2];
	
	floorTexture.diffuse = textures[3];
	floorTexture.specular = textures[4];
	floorTexture.normal = textures[5];

	obstacleTexture.diffuse = textures[6];
	obstacleTexture.specular = textures[7];
	obstacleTexture.normal = textures[8];

	ballTexture.diffuse = textures[9];
	ballTexture.specular = textures[10];
	ballTexture.normal = textures[11];

	ceilingTexture.diffuse = textures[12];
	ceilingTexture.specular = textures[13];
	ceilingTexture.normal = textures[14];

	// Unbinding texture
	glBindTexture(GL_TEXTURE_2D, 0);

	// Cubemap
	glGenTextures(1, &tex_cube);
	glBindTexture(GL_TEXTURE_CUBE_MAP, tex_cube);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	loadTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_X, L"pos_x.png");
	loadTexture(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, L"neg_x.png");
	loadTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, L"pos_y.png");
	loadTexture(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, L"neg_y.png");
	loadTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, L"pos_z.png");
	loadTexture(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, L"neg_z.png");

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	GLfloat fLargest;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
	std::cout << fLargest;
}

void setupTexture(GLenum target, GLuint texture, const wchar_t *fileName) {
	// Binding texture
	glBindTexture(target, texture);

	// Setting parameters
	glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4.0f);

	// Loading texture from file
	loadTexture(target, fileName);

	glGenerateMipmap(target);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
}

// Loads a texture from file
void loadTexture(GLuint tex, const wchar_t *fileName) {
	// Initializing devil
	ilInit();
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
	
	// Generating and binding image name
	ILuint imageName;
	ilGenImages(1, &imageName);
	ilBindImage(imageName);

	// Loading texture from file
	if (!ilLoadImage(fileName))
	{
		// Error reporting
		ILenum err = ilGetError();
		std::wstring file(fileName);
		std::string str(file.begin(), file.end());

		std::cerr << "Error while loading " << str << ": " << err << std::endl;
		std::cerr << "      " << ilGetString(err) << std::endl;

		ilBindImage(0);
		ilDeleteImages(1, &imageName);
	}

	// Defining texture
	glTexImage2D(tex, 0, ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_TYPE), ilGetData());

	// Unbinding image name
	ilBindImage(0);

	// Deleting image from memory
	ilDeleteImages(1, &imageName);
}