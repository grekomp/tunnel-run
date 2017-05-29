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
void setupTexture();
void loadTexture(GLuint texture, const wchar_t *fileName);

//******************************************************************************************
GLuint shaderPrograms[2];

// Uniform locations
GLuint parallaxStrengthLoc;

// Matrices
glm::mat4 projMatrix;
glm::mat4 viewMatrix;

// Textures
GLuint textures[4];

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

// Parallax strength modifier
float parallaxStrength = 0.2f;

// Options
float fovy = 45.0f;
float aspectRatio = (float)WIDTH / HEIGHT;

//******************************************************************************************
int main(int argc, char *argv[])
{
	atexit( onShutdown );

    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );

	glutInitContextVersion( 3, 1 );
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

	if( !GLEW_VERSION_3_1 )
	{
		std::cerr << "Brak obslugi OpenGL 3.1\n";
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

	setupTexture();

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

	// Creating the view matrix
	viewMatrix = glm::lookAt(
		glm::vec3( 0, 0, 8 ), 
		glm::vec3( 0, 0, -1 ), 
		glm::vec3( 0, 1, 0 ) 
	);

	// Picking and using a shader program
	glUseProgram( shaderPrograms[0] );

	// Setting parallax strength
	glUniform1f(parallaxStrengthLoc, parallaxStrength);

	// Sending the projection matrix
	glUniformMatrix4fv( PROJECTION_MATRIX_LOCATION, 1, GL_FALSE, glm::value_ptr( projMatrix ) );

	// Recalculating light position to eye coordinates
	glm::vec4 lightPos = viewMatrix * lightPosition;
	glUniform4fv(LIGHT_POSITION_LOCATION, 1, glm::value_ptr(lightPos));

	// Sending light and material options
	glUniform3fv(LIGHT_AMBIENT_LOCATION, 1, glm::value_ptr(lightAmbient));
	glUniform3fv(MATERIAL_AMBIENT_LOCATION, 1, glm::value_ptr(materialAmbient));

	glUniform3fv(LIGHT_DIFFUSE_LOCATION, 1, glm::value_ptr(lightDiffuse));
	glUniform3fv(MATERIAL_DIFFUSE_LOCATION, 1, glm::value_ptr(materialDiffuse));

	glUniform3fv(LIGHT_SPECULAR_LOCATION, 1, glm::value_ptr(lightSpecular));
	glUniform3fv(MATERIAL_SPECULAR_LOCATION, 1, glm::value_ptr(materialSpecular));
	glUniform1f(MATERIAL_SHININESS_LOCATION, shininess);

	// Normal transform matrix
	glm::mat3 normalMat = glm::inverseTranspose(glm::mat3(viewMatrix));
	glUniformMatrix3fv(NORMAL_MATRIX_LOCATION, 1, GL_FALSE, glm::value_ptr(normalMat));

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
		//exit(3);
	}

	// Setting up uniform locations
	setupLocations(shaderPrograms[0]);
}

void setupLocations(GLuint shaderProgram) {
	// Parallax strength
	parallaxStrengthLoc = glGetUniformLocation(shaderProgram, "parallaxStrength");
}

void setupBuffers()
{
	
}

// ------------------------------------------------
void setupTexture() {
	// Generating and binding texture identifier
	glGenTextures(4, textures);

	// ----------------
	// Diffuse texture
	glBindTexture(GL_TEXTURE_2D, textures[0]);

	// Setting parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// Loading texture from file
	loadTexture(GL_TEXTURE_2D, L"bricks2.jpg");

	// ----------------
	// Specular texture
	glBindTexture(GL_TEXTURE_2D, textures[1]);

	// Setting parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// Loading texture from file
	loadTexture(GL_TEXTURE_2D, L"bricks2_spec.jpg");

	// ----------------
	// Normal texture
	glBindTexture(GL_TEXTURE_2D, textures[2]);

	// Setting parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// Loading texture from file
	loadTexture(GL_TEXTURE_2D, L"bricks2_norm.jpg");

	// ----------------
	// Displacemet texture
	glBindTexture(GL_TEXTURE_2D, textures[3]);

	// Setting parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// Loading texture from file
	loadTexture(GL_TEXTURE_2D, L"bricks2_disp.jpg");

	// Unbinding texture
	glBindTexture(GL_TEXTURE_2D, 0);
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