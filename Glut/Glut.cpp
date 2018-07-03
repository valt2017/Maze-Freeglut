// Glut.cpp : Defines the entry point for the console application.
// Created according tutorials on
// http://www.lighthouse3d.com/

#include "stdafx.h"
#include <GL/glut.h>
#include <GL/freeglut.h>
#include "math.h"

// angle of rotation for the camera direction
float angle = 0.0f;
// actual vector representing the camera's direction
float lx = 0.5f, lz = -1.0f;
// XZ position of the camera
float x = 0.0f, z = 5.0f, y = 1.0f;
// the key states. These variables will be zero
//when no key is being presses
float deltaAngle = 0.0f;
float deltaMove = 0;
bool* keyStates = new bool[256]; // Create an array of boolean values of length 256 (0-255)  

COORD Cur = { 0,0 };
COORD End = { 0,0 };
const char CURSOR = 'O';
const char END = 'C';
const char WALL = '#';
const char FREE = ' ';
int iX, iY;
char ** Array;

void changeSize(int w, int h) {
	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if (h == 0)
		h = 1;
	float ratio = w * 1.0 / h;
	// Use the Projection Matrix
	glMatrixMode(GL_PROJECTION);
	// Reset Matrix
	glLoadIdentity();
	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);
	// Set the correct perspective.
	gluPerspective(45.0f, ratio, 0.1f, 100.0f);
	// Get Back to the Modelview
	glMatrixMode(GL_MODELVIEW);
}

void computePos(float deltaMove) {
	x += deltaMove * lx * 0.1f;
	z += deltaMove * lz * 0.1f;
}

void computeDir(float deltaAngle) {
	angle += deltaAngle;
	lx = sin(angle);
	lz = -cos(angle);
}

void renderScene(void) {
	if (deltaMove)
		computePos(deltaMove);
	if (deltaAngle)
		computeDir(deltaAngle);
	// Clear Color and Depth Buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Reset transformations
	glLoadIdentity();
	// Set the camera
	gluLookAt(x, y, z, x + lx, 1.0f, z + lz, 0.0f, 1.0f, 0.0f);
	// Draw ground
//	glColor3f(0.9f, 0.9f, 0.9f);
//	glBegin(GL_QUADS);
//	glVertex3f(-iX*10.0f, -5.0f, -100.0f);
//	glVertex3f(-100.0f, -5.0f, 100.0f);
//	glVertex3f(100.0f, -5.0f, 100.0f);
//	glVertex3f(100.0f, -5.0f, -100.0f);
//	glEnd();
	// Draw Walls 
	for (int j = 0; j < iY; j++)
		for (int i = 0; i < iX; i++) {
			glPushMatrix();
			glTranslatef(i*10.0, 0, j*10.0);
			if (Array[j][i] == WALL) {
				glColor3f(0.0f, 0.0f, 1.0f);
				glTranslatef(0.0f, 0.75f, 0.0f);
				glutWireCube(10.0f);
			}
			if (Array[j][i] == END) {
				glColor3f(1.0f, 0.0f, 0.0f);
				glTranslatef(0.0f, 0.75f, 0.0f);
				glutSolidSphere(0.75f, 20, 20);
			}
			if (Array[j][i] == CURSOR) {
				glColor3f(0.0f, 1.0f, 0.0f);
				glTranslatef(0.0f, 0.75f, 0.0f);
				glutSolidSphere(0.75f, 20, 20);
			}
			if (Array[j][i] == FREE) {
				glColor3f(0.0f, 0.0f, 1.0f);
				glTranslatef(0.0f, 0.75f, 0.0f);
				glutSolidSphere(0.75f, 20, 20);
			}
			glPopMatrix();
		}
	glutSwapBuffers();
}

void keyPressed(unsigned char key, int x, int y) {
	keyStates[key] = true; // Set the state of the current key to pressed
	switch (key) {
	case 'd':
		if (Array[Cur.Y][Cur.X + 1] != WALL) {
			Array[Cur.Y][Cur.X] = FREE;
			Array[Cur.Y][++Cur.X] = CURSOR;
		}
		break;
	case 'a':
		if (Array[Cur.Y][Cur.X - 1] != WALL) {
			Array[Cur.Y][Cur.X] = FREE;
			Array[Cur.Y][--Cur.X] = CURSOR;
		}
		break;
	case 'w':
		if (Array[Cur.Y - 1][Cur.X] != WALL) {
			Array[Cur.Y][Cur.X] = FREE;
			Array[--Cur.Y][Cur.X] = CURSOR;
		}
		break;
	case 's':
		if (Array[Cur.Y + 1][Cur.X] != WALL) {
			Array[Cur.Y][Cur.X] = FREE;
			Array[++Cur.Y][Cur.X] = CURSOR;
		}
		break;
	}
	if (Cur.Y == End.Y && Cur.X == End.X)
	{
		glutLeaveMainLoop();
	}
}

void keyUp(unsigned char key, int x, int y) {
	keyStates[key] = false; // Set the state of the current key to not pressed  
}

void pressKey(int key, int xx, int yy) {
	switch (key) {
		case GLUT_KEY_F1: if (y == 1.0f) y = 100.0f; else y = 1.0f;  break;
		case GLUT_KEY_LEFT: deltaAngle = -0.002f; break;
		case GLUT_KEY_RIGHT: deltaAngle = 0.002f; break;
		case GLUT_KEY_UP: deltaMove = 0.1f; break;
		case GLUT_KEY_DOWN: deltaMove = -0.1f; break;
	}
}

void releaseKey(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_LEFT:
	case GLUT_KEY_RIGHT: deltaAngle = 0.0f; break;
	case GLUT_KEY_UP:
	case GLUT_KEY_DOWN: deltaMove = 0; break;
	}
}

char ** LoadArray(char * File, int * pX, int * pY){
	int i = 0, iRozX = 0, iRozY = 0;
	char ** pIn;
	std::ifstream pFin(File);

	if (!pFin){
		std::cout << "File open error: " << File << std::endl;
		exit(EXIT_FAILURE);
	}
	pFin >> iRozX >> iRozY;
	while (pFin.get() != '\n')
		continue;
	*pX = iRozX;
	*pY = iRozY;
	pIn = new char*[iRozY * sizeof(char*)];
	if (pIn == NULL){
		std::cout << "Matrix allocation error\n";
		pFin.close();
		return NULL;
	}
	for (i = 0; i < iRozY; i++)	{
		pIn[i] = new char[(iRozX + 2) * sizeof(char)]; // '\n'+ '\0'
		if (pIn[i] == NULL)	{
			std::cout << "Matrix allocation error\n";
			pFin.close();
			return NULL;
		}
		pFin.getline(pIn[i], iRozX + 2); // '\n'+ '\0'
		pIn[i][iRozX] = '\0';
	}
	pFin.close();
	return pIn;
}

void FreeArray(char ** pIn, int iRozY){
	int i;
	for (i = 0; i < iRozY; i++)
		delete pIn[i];
	delete pIn;
}

COORD Find(char ** pIn, int iX, int iY, char znak)
{
	int i, j;
	COORD lokal = { 1,1 };
	for (j = 0; j < iY; j++)
		for (i = 0; i < iX; i++)
			if (pIn[j][i] == znak)
			{
				lokal.X = i;
				lokal.Y = j;
				return lokal;
			}
	return lokal;
}

int main(int argc, char **argv) {
	if (argc != 2)	{
		std::cout << "Error: level not found.\n";
		exit(EXIT_FAILURE);
	}
	Array = LoadArray(argv[1], &iX, &iY);
	Cur = Find(Array, iX, iY, CURSOR);
	End = Find(Array, iX, iY, END);
	// init GLUT and create window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(640, 480);
	glutCreateWindow("Maze");
	// register callbacks
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutIdleFunc(renderScene);
	glutKeyboardFunc(keyPressed);   
	glutSpecialFunc(pressKey);
	glutIgnoreKeyRepeat(1);
	glutSpecialUpFunc(releaseKey);
	glutKeyboardUpFunc(keyUp);   
	// OpenGL init
	glEnable(GL_DEPTH_TEST);
	// enter GLUT event processing cycle
	glutMainLoop();
	FreeArray(Array, iY);
	return EXIT_SUCCESS;
}