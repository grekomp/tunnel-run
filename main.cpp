#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <IL\il.h>
#include <vector>
#include <iostream>

#include "Common.h"
#include "shaders.h"
#include "Model.h"

// Config
const int WIDTH = 768;
const int HEIGHT = 576;
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
void setupLocations(GLuint shaderProgram);
void setupBuffers();
void setupTextures();
void setupTexture(GLenum target, GLuint texture, const wchar_t *fileName);
void loadTexture(GLuint texture, const wchar_t *fileName);

//******************************************************************************************
GLuint shaderPrograms[2];

// Matrices
glm::mat4 projMatrix;
glm::mat4 viewMatrix;

// Textures
std::vector<const wchar_t *> textureFiles = {
	L"bricks2.jpg",
	L"bricks2_spec.jpg",
	L"bricks2_norm.jpg",
	L"bricks2_disp.jpg"
};
const int nTextures = 4;
GLuint textures[nTextures];

// Light parameters
glm::vec4 lightPosition = glm::vec4(20.0f, 20.0f, 15.0f, 1.0f);
glm::vec3 lightAmbient = glm::vec3(0.2f, 0.2f, 0.2f);
glm::vec3 lightDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3 lightSpecular = glm::vec3(1.0, 1.0, 1.0);

// Material parameters
glm::vec3 materialAmbient = glm::vec3(0.2f, 0.2f, 0.2f);
glm::vec3 materialDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
glm::vec3 materialSpecular = glm::vec3(0.8f, 0.8f, 0.8f);
float shininess = 10.2f;

// Options
float fovy = 45.0f;
float aspectRatio = (float)WIDTH / HEIGHT;

// ---------------
Model models[5];

//******************************************************************************************
int main(int argc, char *argv[])
{
	atexit( onShutdown );

    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );

	glutInitContextVersion( 3, 3 );
	glutInitContextFlags( GLUT_DEBUG );
	glutInitContextProfile( GLUT_CORE_PROFILE );

	glutSetOption( GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS );

    glutInitWindowPosition( 0, 0 );
    glutInitWindowSize( WIDTH, HEIGHT );
    glutCreateWindow( "OpenGL (Core Profile) - Transformations" );

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
    
    glutReshapeFunc( changeSize );
    glutDisplayFunc( renderScene );

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
	std::cout << "GLEW = "		  << glewGetString( GLEW_VERSION ) << std::endl;
	std::cout << "GL_VENDOR = "	  << glGetString( GL_VENDOR ) << std::endl;
	std::cout << "GL_RENDERER = " << glGetString( GL_RENDERER ) << std::endl;
	std::cout << "GL_VERSION = "  << glGetString( GL_VERSION ) << std::endl;
	std::cout << "GLSL = "		  << glGetString( GL_SHADING_LANGUAGE_VERSION ) << std::endl << std::endl;

	std::cout << "w, s, a, d, q, z - local rotation" << std::endl;
	std::cout << "o, k - local scale" << std::endl;
	std::cout << "p, l - global scale" << std::endl;
	std::cout << "+, - - change FOV" << std::endl;
}

void initGL()
{
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_PRIMITIVE_RESTART);

	setupShaders();

	setupBuffers();

	setupTextures();

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
	projMatrix = glm::perspective( glm::radians( fovy ), aspectRatio, 0.1f, 100.0f );
}

void renderScene()
{
	// Clearing the screen
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// Picking and using a shader program
	glUseProgram( shaderPrograms[0] );

	// Creating the view matrix
	viewMatrix = glm::lookAt(
		glm::vec3( 0, 0, 8 ), 
		glm::vec3( 0, 0, -1 ), 
		glm::vec3( 0, 1, 0 ) 
	);

	// Sending the projection matrix
	glUniformMatrix4fv( PROJECTION_MATRIX_LOCATION, 1, GL_FALSE, glm::value_ptr( projMatrix ) );

	// Recalculating light position to eye coordinates
	glm::vec4 lightPos = viewMatrix * lightPosition;
	glUniform4fv(LIGHT_POSITION_LOCATION, 1, glm::value_ptr(lightPos));

	// Sending light options
	glUniform3fv(LIGHT_AMBIENT_LOCATION, 1, glm::value_ptr(lightAmbient));
	glUniform3fv(LIGHT_DIFFUSE_LOCATION, 1, glm::value_ptr(lightDiffuse));
	glUniform3fv(LIGHT_SPECULAR_LOCATION, 1, glm::value_ptr(lightSpecular));
	
    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y)
{
	switch( key )
	{
		case 27: // ESC
			exit( 0 );
			break;
		case '+':
		case '=':
			fovy /= 1.1f;
			updateProjectionMatrix();
			break;

		case '-':
			if (1.1f * fovy < 180.0f)
			{
				fovy *= 1.1f;
				updateProjectionMatrix();
			}
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
	}

	glutPostRedisplay();
}

void setupShaders()
{
	// Setting up shaders with textures
	if (!setupShaders("shaders/vertex-texture.vert", "shaders/fragment-texture.frag", shaderPrograms[0])) {
		std::cout << "Failed to setup shaders - texture" << std::endl;
	}
}

void setupBuffers()
{
	// TODO: Add model loading, setupSegments
}

// ------------------------------------------------
void setupTextures() {
	// Generating and binding texture identifier
	glGenTextures(nTextures, textures);

	// Setting up all 2d textures
	for (int i = 0; i < nTextures; i++) {
		setupTexture(GL_TEXTURE_2D, textures[i], textureFiles[i]);
	}

	// Unbinding texture
	glBindTexture(GL_TEXTURE_2D, 0);
}

void setupTexture(GLenum target, GLuint texture, const wchar_t *fileName) {
	// Binding texture
	glBindTexture(target, texture);

	// Setting parameters
	glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// Loading texture from file
	loadTexture(target, fileName);
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
		std::cerr << "Blad: " << err << std::endl;
		std::cerr << "      " << ilGetString(err) << std::endl;

		ilBindImage(0);
		ilDeleteImages(1, &imageName);

		exit(1);
	}

	// Defining texture
	glTexImage2D(tex, 0, ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_TYPE), ilGetData());

	// Unbinding image name
	ilBindImage(0);

	// Deleting image from memory
	ilDeleteImages(1, &imageName);
}