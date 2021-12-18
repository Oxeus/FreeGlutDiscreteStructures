/* Nick Insalata
Sources:https://www.youtube.com/watch?v=8Qkpaewj-7Y 
		https://www3.ntu.edu.sg/home/ehchua/programming/opengl/CG_Examples.html
		(taken from NeHe OpenGL Tutorials)
		http://www.swiftless.com/tutorials/opengl/keyboard.html
		http://stackoverflow.com/questions/15435715/opengl-glut-buttons-and-keys
		http://stackoverflow.com/questions/2417697/gluperspective-was-removed-in-opengl-3-1-any-replacements
Notes:
This will draw a rotatable and zoomable 3D object using openGL.
Your 3D object is meant to be drawn inside a 2x2x2 cube whose center is the origin.
You can zoom in and out with 'z' and 'x', and rotate with "ijkl".
*/


#include <GL/glut.h>		// GLUT, includes glu.h and gl.h
#include <cmath>
#include <time.h>
#include <iostream>
using namespace std;


struct Point {
	double x;
	double y;
	double z;
};

 
// Global variables and consts
const double PI = 3.14159265359;
const int RES = 100;		// resolution (precision) of my 3d cubic grid
const int MILS = 20;		// refresh interval in milliseconds
double zoom   = -4.0;		// zoom in or out from object
double xAngle = 0.0;		// rotational angle on x-axis
double yAngle = 0.0;		// rotational angle on x-axis
bool keys[256] = {0};		// holds key states for keyboard buffering


const int V = 125;
bool edge[V][V];
Point vert[V];
const int N = 10;




// function prototypes (descriptions above function definitions)
void display();			
void keyUp(unsigned char key, int x, int y);
void keyDown(unsigned char key, int x, int y);
void checkKeyboard();
void timer(int value);
void reshape(GLsizei width, GLsizei height);
void initG();
void cylinder(Point p1, Point p2, double r, int n);
Point norm(Point v);
Point cross(Point a, Point b);
double dot(Point a, Point b);


void cylinder(Point p1, Point p2, double r) {
	// create v, a vector from p1 to p2, and normalize it
	Point v = {p2.x-p1.x, p2.y-p1.y, p2.z-p1.z};
	v = norm(v);

	// create a, a vector perpendicular to v, and normalize it
	// being careful not to divide by 0!!!!!!!!!!!!!!!!!!
	Point a;
	if(v.z != 0) {
		a.x = 1;
		a.y = 1;
		a.z = (-v.x-v.y)/v.z;
	} else if(v.y != 0) {
		a.x = 1;
		a.y = (-v.x-v.z)/v.y;
		a.z = 1;
	} else {
		a.x = (-v.y-v.z)/v.x;
		a.y = 1;
		a.z = 1;
	}
	a = norm(a);
	
	// create b, perpendicular to both v and a (already normalized)
	Point b = cross(v, a);
	
	// make points of circle, centered at origin
	double c, s, x, y, z;
	Point circle[N];
	for(int i = 0; i < N; i++) {
		c = r*cos(2*i*PI/N);
		s = r*sin(2*i*PI/N);
		circle[i].x = c*a.x + s*b.x;
		circle[i].y = c*a.y + s*b.y;
		circle[i].z = c*a.z + s*b.z;
	}
	
	glBegin(GL_TRIANGLES);
		// draw end cap at p1, listing all verts as triangles
		for(int i = 0; i < N; i++) {
			glVertex3f(p1.x+circle[i].x, p1.y+circle[i].y, p1.z+circle[i].z);
			glVertex3f(p1.x, p1.y, p1.z);
			glVertex3f(p1.x+circle[(i+1)%N].x, p1.y+circle[(i+1)%N].y, p1.z+circle[(i+1)%N].z);
		}
		
		// draw end cap at p2, listing all verts as triangles
		for(int i = 0; i < N; i++) {
			glVertex3f(p2.x+circle[i].x, p2.y+circle[i].y, p2.z+circle[i].z);
			glVertex3f(p2.x, p2.y, p2.z);
			glVertex3f(p2.x+circle[(i+1)%N].x, p2.y+circle[(i+1)%N].y, p2.z+circle[(i+1)%N].z);
		}
		
		// draw shell, with rectangles as two triangles
		for(int i = 0; i < N; i++) {
			glVertex3f(p1.x+circle[i].x, p1.y+circle[i].y, p1.z+circle[i].z);
			glVertex3f(p2.x+circle[(i+1)%N].x, p2.y+circle[(i+1)%N].y, p2.z+circle[(i+1)%N].z);
			glVertex3f(p1.x+circle[(i+1)%N].x, p1.y+circle[(i+1)%N].y, p1.z+circle[(i+1)%N].z);
			
			glVertex3f(p2.x+circle[i].x, p2.y+circle[i].y, p2.z+circle[i].z);
			glVertex3f(p2.x+circle[(i+1)%N].x, p2.y+circle[(i+1)%N].y, p2.z+circle[(i+1)%N].z);
			glVertex3f(p1.x+circle[i].x, p1.y+circle[i].y, p1.z+circle[i].z);
		}
	glEnd();
		
}


double dot(Point a, Point b) {
	return a.x*b.x + a.y*b.y + a.z+b.z;
}


Point cross(Point u, Point v) {
	 Point r = {u.y*v.z-u.z*v.y, u.z*v.x-u.x*v.z, u.x*v.y-u.y*v.x};
	 return r;
}


Point norm(Point v) {
	double s = sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
	v.x /= s;
	v.y /= s;
	v.z /= s;
	return v;
}


void initG() {
	srand(time(NULL));
	
	// set up vert positions
	int count = 0;
	for(double i = -1; i <= 1; i+=1) {
		for(double j = -1; j <= 1; j+=1) {
			for(double k = -1; k <= 1; k+=1) {
				vert[count].x = pow(i, k);
				vert[count].y = (j * i + k);
				vert[count].z = (k / j + i);
				count++;
			}
		}
	}
	
	// set up edge array to make a complete graph
	for(int i = 0; i < V; i++) {
		for(int j = i+1; j < V; j++) 
		{
				edge[i][j] = 1;
		}
	}
}


void drawObjectHere() {
	//glutWireSphere(1, 100, 100);
	double r = 0.01;
	
	for(int i = 0; i < V; i++) {
		for(int j = 0; j < V; j++) {
			if(edge[i][j]) {
				// draw a cylinder from vert[i] to vert[j]
				cylinder(vert[i], vert[j], r);
			}
		}
	}
}



// Main function: GLUT runs as a console application starting at main()
int main(int argc, char** argv) {
	glutInit(&argc, argv);				// Initialize GLUT
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);	// Enable double buffered mode
	glutInitWindowSize(700, 700);		// Set the window's initial width & height
	glutInitWindowPosition(50, 50);		// Position the window's initial top-left corner
	glutCreateWindow("GRAPHICS!");		// Create window with the given title
	glutDisplayFunc(display);			// Register callback handler for window re-paint event
	glutKeyboardFunc(keyDown);			// Register callback handler for key press event
	glutKeyboardUpFunc(keyUp);			// Register callback handler for key release event
	glutReshapeFunc(reshape);			// Register callback handler for window re-size event
	glutTimerFunc(0, timer, 0);			// First timer call immediately
	glClearColor(0.0, 0.0, 0.0, 1.0);	// Set background color to black and opaque
	glClearDepth(1.0);					// Set background depth to farthest
	glEnable(GL_DEPTH_TEST);			// Enable depth testing for z-culling
	glDepthFunc(GL_LEQUAL);				// Set the type of depth-test
	glShadeModel(GL_SMOOTH);			// Enable smooth shading
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // Nice perspective corrections
	initG();
	glutMainLoop();						// Enter the infinite event-processing loop
	return 0;
}


// Handler for window-repaint event
void display() {
	// setup view for drawing
	checkKeyboard();					// deal with any keypresses
	glMatrixMode(GL_MODELVIEW);			// To operate on model-view matrix
	glLoadIdentity();					// Reset the model-view matrix
	glTranslatef(0.0, 0.0, zoom);		// Move right and into the screen
	glRotatef(xAngle, 1.0, 0.0, 0.0);	// Rotate about (1,0,0)-axis (in degrees)
	glRotatef(yAngle, 0.0, 1.0, 0.0);	// Rotate about (0,1,0)-axis (in degrees)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear color and depth buffers
	drawObjectHere();					// actually draws your shape
	glutSwapBuffers();  				// Swap the front and back frame buffers (double buffering)
}


// does the key operations (with buffering for smooth performance and multiple key presses)
void checkKeyboard() {
	if(keys['x']) zoom -= 0.1;	// zoom out
	if(keys['z']) zoom += 0.1;	// zoom in
	if(keys['i']) xAngle -= 1;	// rotate object up
	if(keys['k']) xAngle += 1;	// rotate object down
	if(keys['j']) yAngle -= 1;	// rotate object left
	if(keys['l']) yAngle += 1;	// rotate object right
}
// called when user presses a key
void keyDown(unsigned char key, int x, int y) {
	keys[key] = true;	// record the key press in the key buffer 'keys'
}
// called when user releases a key
void keyUp(unsigned char key, int x, int y) {
	keys[key] = false;	// record the key release in the key buffer 'keys'
}


// Called back when timer expired
void timer(int value) {
   glutPostRedisplay();				// Post re-paint request to activate display()
   glutTimerFunc(MILS, timer, 0);	// next timer call milliseconds later
}


// Handler for window re-size event
void reshape(GLsizei width, GLsizei height) {  // GLsizei for non-negative integer
	// Compute aspect ratio of the new window (right way is with GL MATH)
	if (height == 0) height = 1;                // To prevent divide by 0
	GLfloat zNear = 0.1;
	GLfloat zFar = 100.0;
	GLfloat fieldOfView = 45.0;
	GLfloat aspect = (GLfloat)width / (GLfloat)height;
	GLfloat fH = tan( float(fieldOfView / 360.0f * 3.14159f) ) * zNear;
	GLfloat fW = fH * aspect;
 
   // Set the viewport to cover the new window
   glViewport(0, 0, width, height);
 
   // Set the aspect ratio of the clipping volume to match the viewport
   glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
   glLoadIdentity();             // Reset
   
   // Enable perspective projection with fovy, aspect, zNear and zFar	
	//glFrustum( -fW, fW, -fH, fH, zNear, zFar);
	
	// old way:
	 gluPerspective(45.0, aspect, 0.1, 100.0);
}

