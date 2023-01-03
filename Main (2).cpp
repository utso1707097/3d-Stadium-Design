#include <GL/glut.h>

#include <iostream>

#include "Scene.h"

// Window parameters
const unsigned int W_WIDTH = 1280u;
const unsigned int W_HEIGHT = 720u;
const std::string W_TITLE = "Cricket Stadium";

// Texture parameters
const unsigned int texture_count = 8u;
unsigned int textures[texture_count];

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	int screenWidth = glutGet(GLUT_SCREEN_WIDTH);
	int screenHeight = glutGet(GLUT_SCREEN_HEIGHT);
	glutInitWindowPosition((screenWidth - W_WIDTH) / 2, (screenHeight - W_HEIGHT) / 2);
	glutInitWindowSize(W_WIDTH, W_HEIGHT);
	glutCreateWindow(W_TITLE.c_str());

	glShadeModel(GL_SMOOTH);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHTING);
	
	std::cout << "OpenGL Vendor:   " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "OpenGL Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "OpenGL Version:  " << glGetString(GL_VERSION) << std::endl;

	gluOrtho2D(0, W_WIDTH, 0, W_HEIGHT);

	glutPassiveMotionFunc(NULL);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutDisplayFunc(display);
	glutIdleFunc(idle);

	glutMainLoop();
}
