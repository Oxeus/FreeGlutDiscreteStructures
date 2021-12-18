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
 
 
// Global variables and consts
const int RES = 200;		// resolution (precision) of my 3d cubic grid
const int MILS = 20;		// refresh interval in milliseconds
double zoom   = -4.0;		// zoom in or out from object
double xAngle = 0.0;		// rotational angle on x-axis
double yAngle = 0.0;		// rotational angle on x-axis
bool* keys = new bool[256];	// holds key states for keyboard buffering
bool arr[RES][RES][RES] = {0};   // holds which cubes are drawn and which are not


// function prototypes (descriptions above function definitions)
void drawObjectHere();
void display();			
void keyUp(unsigned char key, int x, int y);
void keyDown(unsigned char key, int x, int y);
void checkKeyboard();
void timer(int value);
void reshape(GLsizei width, GLsizei height);
void cube(double x, double y, double z, double s);
void initArr(bool arr[RES][RES][RES]);
bool onSurface(double i, double j, double k);
bool f(double i, double j, double k);



// function definitions
void initArr(bool arr[RES][RES][RES]) {
	for(int i = 0; i < RES; i++) {
		for(int j = 0; j < RES; j++) {
			for(int k = 0; k < RES; k++) {
				arr[i][j][k] = onSurface(2.0*i/RES-1, 2.0*j/RES-1, 2.0*k/RES-1);
			}
		}
	}
}


bool f(double i, double j, double k) {
	return i*i*i + j*j*j + k*k*k < 1;
}


bool onSurface(double i, double j, double k) {
	double s = 1.0/RES;
	return !(f(i-s, j-s, k-s) == f(i-s, j-s, k+s) &&
			f(i-s, j-s, k-s) == f(i-s, j+s, k-s) &&
			f(i-s, j-s, k-s) == f(i-s, j+s, k+s) &&
			f(i-s, j-s, k-s) == f(i+s, j-s, k-s) &&
			f(i-s, j-s, k-s) == f(i+s, j-s, k+s) &&
			f(i-s, j-s, k-s) == f(i+s, j+s, k-s) &&
			f(i-s, j-s, k-s) == f(i+s, j+s, k+s));
}


void drawObjectHere() {
	// Render a color-cube consisting of 6 quads with different colors
	glBegin(GL_QUADS);                // Begin drawing the color cube with 6 quads
		for(int i = 0; i < RES; i++) {
			for(int j = 0; j < RES; j++) {
				for(int k = 0; k < RES; k++) {
					if(arr[i][j][k]) {
						cube(2.0*i/RES-1, 2.0*j/RES-1, 2.0*k/RES-1, 1.0/RES);
					}
				}
			}
		}
	glEnd();  // End of drawing color-cube
}


void cube(double x, double y, double z, double s) {
	// Top face (y = 1.0f)
		// Define vertices in counter-clockwise (CCW) order with normal pointing out
		glColor3f(0.0f, 1.0f, 0.0f);     // Green
		glVertex3f(x+s, y+s, z-s);
		glVertex3f(x-s, y+s, z-s);
		glVertex3f(x-s, y+s, z+s);
		glVertex3f(x+s, y+s, z+s);
		
		// Bottom face (y = -1.0f)
		glColor3f(1.0f, 0.5f, 0.0f);     // Orange
		glVertex3f(x+s, y-s, z+s);
		glVertex3f(x-s, y-s, z+s);
		glVertex3f(x-s, y-s, z-s);
		glVertex3f(x+s, y-s, z-s);
		
		// Front face  (z = 1.0f)
		glColor3f(1.0f, 0.0f, 0.0f);     // Red
		glVertex3f(x+s, y+s, z+s);
		glVertex3f(x-s, y+s, z+s);
		glVertex3f(x-s, y-s, z+s);
		glVertex3f(x+s, y-s, z+s);
		
		// Back face (z = -1.0f)
		glColor3f(1.0f, 1.0f, 0.0f);     // Yellow
		glVertex3f(x+s, y-s, z-s);
		glVertex3f(x-s, y-s, z-s);
		glVertex3f(x-s, y+s, z-s);
		glVertex3f(x+s, y+s, z-s);
		
		// Left face (x = -1.0f)
		glColor3f(0.0f, 0.0f, 1.0f);     // Blue
		glVertex3f(x-s, y+s, z+s);
		glVertex3f(x-s, y+s, z-s);
		glVertex3f(x-s, y-s, z-s);
		glVertex3f(x-s, y-s, z+s);
		
		// Right face (x = 1.0f)
		glColor3f(1.0f, 0.0f, 1.0f);     // Magenta
		glVertex3f(x+s, y+s, z-s);
		glVertex3f(x+s, y+s, z+s);
		glVertex3f(x+s, y-s, z+s);
		glVertex3f(x+s, y-s, z-s);
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
	initArr(arr);						// determine which cubes are on
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

