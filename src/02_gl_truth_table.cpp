#include <GL/freeglut.h>
#include <iostream>
#include <cmath>
using namespace std;


// consts
const int WRES = 512;
const int ARES = 8;
const int VARS = 3;


// functions
void display();
void fill(unsigned char arr[ARES][ARES][3]);
void printArr(unsigned char arr[ARES][ARES][3]);
bool isTrue(int r, int c);



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
	for(int r = 0; r < ARES; r++) {
		for(int c = 0; c < ARES; c++) {
			arr[r][c][0] = rand()%255 * isTrue(r, c); // red
			arr[r][c][1] = rand()%255 * isTrue(r, c); // green
			arr[r][c][2] = rand()%255 * isTrue(r, c); // blue
		}
	}
}

bool isTrue(int r, int c) {
	// convert r to binary
	bool binNum[VARS] = {0};
	int newR = r;
	for(int count = VARS; count > 0; count--) {
		binNum[count-1] = newR%2;
		newR /= 2;
	}

	// see if location is a variable or formula
	if(c < VARS) {
		return binNum[c];
	} else {
		// do a logical formula
		switch(c) {
			case VARS:
				return !(binNum[0] && binNum[1]); // ~(A and B)
			case VARS+1:
				return binNum[0] || binNum[1]; // A or B
			case VARS+2:
				return binNum[1] != binNum[2]; 
			case VARS+3:
				return (binNum[2] && binNum[1]); // ~(A and B)
			case VARS+4:
				return binNum[0] || binNum[1] && binNum[2]; // A or B
		}
		return false;
	}
}

