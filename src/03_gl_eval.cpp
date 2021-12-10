#include <GL/freeglut.h>
#include <iostream>
#include <cmath>
using namespace std;


// consts
const int WRES = 512;
const int ARES = 128;
const int RULE = 40;

// functions
void display();
void fill(unsigned char arr[ARES][ARES][3]);
void printArr(unsigned char arr[ARES][ARES][3]);
bool findColor(unsigned char arr[ARES][ARES][3], int r, int c);


int main(int argc, char** argv) {
    // set up displays and mode (we won't change this!)
	glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
    
	// set window size
	glutInitWindowSize (WRES, WRES); 
    
	// set window position on screen
	glutInitWindowPosition (0, 0);

	// set window title
    glutCreateWindow ("test");
    
    // set the display function to the one we made below
	glutDisplayFunc(display);
	
	// required to make the thing go 
    glutMainLoop();

	// exit
    return 0;
}


void display() {
	//  clear all pixels
    glClear (GL_COLOR_BUFFER_BIT);

	// make and fill in array
	static unsigned char arr[ARES][ARES][3] = {0};
	arr[ARES-1][ARES/2][0] = 255;	// set a true square in the middle of the top row
	arr[ARES-1][ARES/2][1] = 255;
	arr[ARES-1][ARES/2][2] = 255;
	fill(arr);

	// resize the array to the size of the window RES
	glPixelZoom((float)WRES/ARES, (float)WRES/ARES);
	
	// unpack wrong??
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
	glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);

	// draw the array to the screen
	glDrawPixels(ARES, ARES, GL_RGB, GL_UNSIGNED_BYTE, arr);

	// start processing buffered OpenGL routines 
    glFlush ();
}


void fill(unsigned char arr[ARES][ARES][3]) {
	// puts a color at every array location
	for(int r = ARES-2; r >= 0; r--) {
		for(int c = 0; c < ARES; c++) {
			bool color = findColor(arr, r+1, c);

			arr[r][c][0] = 255 * color; // red
			arr[r][c][1] = 255 * color; // green
			arr[r][c][2] = 255 * color; // blue
		}
	}
}

bool findColor(unsigned char arr[ARES][ARES][3], int r, int c) {
	// vars (with row wrap)
	bool A = arr[r][(c-1+ARES)%ARES][0];
	bool B = arr[r][c][0];
	bool C = arr[r][(c+1)%ARES][0];

	// bin arrary representation of RULE
	bool bin[8] = {0};
	int rule = RULE;
	for(int i = 7; i >= 0; i--) {
		bin[i] = rule % 2;
		rule /= 2;
	}

	// look for 1's, and do logic based on that
	for(int i = 0; i < 8; i++) {
		if(bin[i]) {	// do a switch???
	}
}

	// eval nbrs with rule...
	return (A&&B&&C) && (A&&!B&&C) && (!A&&!B&&C) || (!A&&!B&&!C);
}
