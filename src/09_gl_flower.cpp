#include <GL/freeglut.h>
#include <iostream>
#include <cmath>
#include <fstream>
using namespace std;


// struct
struct Point {
	int x;
	int y;
	int r;
	int g;
	int b;
};


// consts
const int WRESX = 1000;
const int WRESY = 1000;
const int ARESX = 1000;
const int ARESY = 1000;
const double PHI = (1.0+sqrt(5))/2.0;
const int N = 1000;


// functions
void display();
void fill(unsigned char arr[ARESY][ARESX][3], Point points[N]);
void print(unsigned char arr[ARESY][ARESX][3]);
int nearest(Point points[N], int r, int c);
void init(Point points[N]);
double dist(int x0, int y0, int x1, int y1);


int main(int argc, char** argv) {
    // set up displays and mode (we won't change this!)
	glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
    
	// set window size
	glutInitWindowSize (WRESX, WRESY); 
    
	// set window position on screen
	glutInitWindowPosition (0, 0);

	// set window title
    glutCreateWindow ("test");
    
    // set the display function to the one we made below
	glutDisplayFunc(display);

	// set up the display function to loop and make an animation
	// glutIdleFunc(glutPostRedisplay);
	
	// required to make the thing go 
    glutMainLoop();

	// exit
    return 0;
}


void display() {
	//  clear all pixels
    glClear (GL_COLOR_BUFFER_BIT);

	// make and fill in array
	static unsigned char arr[ARESY][ARESX][3] = {0};
	Point points[N];
	init(points);
	fill(arr, points);

	// resize the array to the size of the window RES
	glPixelZoom((float)WRESX/ARESX, (float)WRESY/ARESY);
	
	// unpack wrong??
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
	glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);

	// draw the array to the screen
	glDrawPixels(ARESX, ARESY, GL_RGB, GL_UNSIGNED_BYTE, arr);

	// start processing buffered OpenGL routines 
    glFlush ();
}


void init(Point points[N]) {
	for(int n = 0; n < N; n++) {
		// set x and y with polar coords for each point
		double r = 20*sqrt(n/M_PI);
		double theta = n * PHI * 2 * M_PI;
		points[n].x = ARESX/2 + round(r*cos(theta));
		points[n].y = ARESY/2 + round(r*sin(theta));
		
		// set color of each point
		points[n].r = 128.0 * n / N + 128;
		points[n].g = 256.0 * n / N + 128;
		points[n].b = 128;
	}

}


double dist(int x0, int y0, int x1, int y1) {
	return sqrt((x1-x0)*(x1-x0) + (y1-y0)*(y1-y0));
}


int nearest(Point points[N], int r, int c) {
	// init our min values
	double d;
	double minDist = ARESX+ARESY;
	int minN = 0;

	// loop through all of the points to see which is nearest
	for(int i = 0; i < N; i++) {
		d = dist(points[i].x, points[i].y, c, r);
		if(d < minDist) {
			minDist = d;
			minN = i;
		}
	}

	return minN;
}


void fill(unsigned char arr[ARESY][ARESX][3], Point points[N]) {
	// traverse the screen
	for(int r = 0; r < ARESY; r++) {
		for(int c = 0; c < ARESX; c++) {
			// find the nearest point to (r, c)
			int n = nearest(points, r, c);
			
			// color it
			arr[r][c][0] = points[n].r / (1+dist(points[n].x, points[n].y, c, r)/10);
			arr[r][c][1] = points[n].g / (1+dist(points[n].x, points[n].y, c, r)/10);
			arr[r][c][2] = points[n].b / (1+dist(points[n].x, points[n].y, c, r)/10);
		}
	}
}



