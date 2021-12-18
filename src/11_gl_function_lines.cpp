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

// structures
struct vec {
	double x;
	double y;
	double z;
};
 
// Global variables and consts
const int MILS = 20;		// refresh interval in milliseconds
double zoom   = -10.0;		// zoom in or out from object
double xAngle = 0.0;		// rotational angle on x-axis
double yAngle = 0.0;		// rotational angle on x-axis
bool* keys = new bool[256];	// holds key states for keyboard buffering
int animate = 0;


// function prototypes (descriptions above function definitions)
void drawObjectHere();
void display();			
void keyUp(unsigned char key, int x, int y);
void keyDown(unsigned char key, int x, int y);
void checkKeyboard();
void timer(int value);
void reshape(GLsizei width, GLsizei height);
void lighting();
double f(double x, double y);


// Performs application specific initialization.  It defines lighting
// parameters for light source GL_LIGHT0: black for ambient, yellow for
// diffuse, white for specular, and makes it a directional source
// shining along <-1, -1, -1>.  It also sets a couple material properties
// to make cyan colored objects with a fairly low shininess value.  Lighting
// and depth buffer hidden surface removal are enabled here.
void lighting() {
  GLfloat black[] = { 0.0, 0.0, 0.0, 1.0 };
  GLfloat yellow[] = { 1.0, 1.0, 0.0, 1.0 };
  GLfloat cyan[] = { 0.0, 1.0, 1.0, 1.0 };
  GLfloat white[] = { 1.0, 1.0, 1.0, 1.0 };
  GLfloat position[] = { 0.0, 0.0, 1.0};

  glMaterialfv(GL_FRONT, GL_AMBIENT, cyan);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, white);
  glMaterialfv(GL_FRONT, GL_SPECULAR, yellow);
  glMaterialf(GL_FRONT, GL_SHININESS, 60);

  glLightfv(GL_LIGHT0, GL_AMBIENT, black);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, yellow);
  glLightfv(GL_LIGHT0, GL_SPECULAR, white);
  glLightfv(GL_LIGHT0, GL_POSITION, position);

  glEnable(GL_LIGHTING);                // so the renderer considers light
  glEnable(GL_LIGHT0);                  // turn LIGHT0 on
  glEnable(GL_DEPTH_TEST);              // so the renderer considers depth
}


vec normal(vec a, vec b, vec c) {
	// compute difference of a and b, c and b
	vec i, j, n;
	i.x = a.x - b.x;
	i.y = a.y - b.y;
	i.z = a.z - b.z;
	j.x = c.x - b.x;
	j.y = c.y - b.y;
	j.z = c.z - b.z;
	
	// compute the cross product
	n.x = (i.y * j.z) - (i.z * j.y);	// n = i x j
    n.y = (i.z * j.x) - (i.x * j.z);
    n.z = (i.x * j.y) - (i.y * j.x);	// len = length(n)
	
	// normalize the normal (not reduntant!!! ;)
	double len = sqrt((n.x * n.x) + (n.y * n.y) + (n.z * n.z));
    if (len < 0.00000001) len = 0.0001f; // avoiding division by 0
    n.x /= len;		// scale the normal to unit length
    n.y /= len;
    n.z /= len;
	
	return n;
}


double f(double x, double y) {
	x*=50;
	y*=50;
	return cos( 0.5*sqrt(x*x+y*y)+6*animate)/(0.5*(x*x+y*y)+10+2*animate);
}


// function definitions
void drawObjectHere() {
	//glutSolidTorus(0.5, 2, 100, 100);
	
	// draw a wire-frame surface of a function
	// resolution of grid 
	const int XRES = 200;
	const int YRES = 200;
	
	// make a 2D array to hold the points and fill it
	double arr[YRES][XRES] = {0};
	for(int r = 0; r < YRES; r++) {
		for(int c = 0; c < XRES; c++) {
			arr[r][c] = f(2.0*c/XRES - 1, 2.0*r/YRES - 1);
		}
	}
	
	// draw a solid surface
	glBegin(GL_TRIANGLES);
		for(int r = 0; r < YRES-1; r++) {
			for(int c = 0; c < XRES-1; c++) {
				// draw top left triangle
				vec i = {2.0*c/XRES-1, 2.0*r/YRES-1, arr[r][c]};
				vec j = {2.0*(c)/XRES-1, 2.0*(r+1)/YRES-1, arr[r+1][c]};
				vec k = {2.0*(c+1)/XRES-1, 2.0*r/YRES-1, arr[r][c+1]};
				vec n = normal(i, j, k);
				glNormal3f(n.x, n.y, n.z); // set the normal
				glVertex3f(i.x, i.y, i.z);
				glVertex3f(j.x, j.y, j.z);
				glVertex3f(k.x, k.y, k.z);
				
				// draw bottom right triangle
				vec s = {2.0*(c+1)/XRES-1, 2.0*(r+1)/YRES-1, arr[r+1][c+1]};
				vec t = {2.0*(c+1)/XRES-1, 2.0*r/YRES-1, arr[r][c+1]};
				vec u = {2.0*(c)/XRES-1, 2.0*(r+1)/YRES-1, arr[r+1][c]};
				n = normal(s, t, u);
				glNormal3f(n.x, n.y, n.z); // set the normal
				glVertex3f(s.x, s.y, s.z);
				glVertex3f(t.x, t.y, t.z);
				glVertex3f(u.x, u.y, u.z);
				
			}
		}
	glEnd();
	
	/* draw lines
	glBegin(GL_LINES);
		// draw the coordinate axes
		glColor4f(1, 0, 0, 1);
		glVertex3f(-10, 0, 0);
		glVertex3f(10, 0, 0);
		glColor4f(0, 1, 0, 1);
		glVertex3f(0, -10, 0);
		glVertex3f(0, 10, 0);
		glColor4f(0, 0, 1, 1);
		glVertex3f(0, 0, -10);
		glVertex3f(0, 0, 10);
		
		// draw our wire-frame surface
		glColor4f(1, 1, 1, 1);
		for(int r = 0; r < YRES-1; r++) {
			for(int c = 0; c < XRES-1; c++) {
				// conversion: 2.0*c/XRES-1, 2.0*r/YRES-1
				// draw horizontal lines
				glVertex3f(2.0*c/XRES-1, 2.0*r/YRES-1, arr[r][c]);
				glVertex3f(2.0*(c+1)/XRES-1, 2.0*r/YRES-1, arr[r][c+1]);
				// draw vertical lines
				glVertex3f(2.0*c/XRES-1, 2.0*r/YRES-1, arr[r][c]);
				glVertex3f(2.0*(c)/XRES-1, 2.0*(r+1)/YRES-1, arr[r+1][c]);
			}
		}
		
		// get the last lines 
		for(int r = 0; r < YRES-1; r++) {
			int c = XRES-1;
			glVertex3f(2.0*c/XRES-1, 2.0*r/YRES-1, arr[r][c]);
			glVertex3f(2.0*(c)/XRES-1, 2.0*(r+1)/YRES-1, arr[r+1][c]);
		}
		for(int c = 0; c < XRES-1; c++) {
			int r = YRES-1;
			glVertex3f(2.0*c/XRES-1, 2.0*r/YRES-1, arr[r][c]);
			glVertex3f(2.0*(c+1)/XRES-1, 2.0*r/YRES-1, arr[r][c+1]);
		}
	glEnd();*/
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
	lighting();							// do the lighting function
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
	static int t = 0;
	t++;
   animate = (animate>20) ? 0 : animate+1;
   glutPostRedisplay();				// Post re-paint request to activate display()
   glutTimerFunc(MILS, timer, 0);	// next timer call milliseconds later
   
}


// Handler for window re-size event
void reshape(GLsizei w, GLsizei h) { 	// GLsizei for non-negative integer
	glViewport(0, 0, w, h);				// Set the viewport to cover the new window
	glMatrixMode(GL_PROJECTION);  		// To operate on the Projection matrix
	glLoadIdentity();             		// Reset (what?)
	gluPerspective(45, w/h, 0.1, 100);	// fov, aspect, zNear and zFar
}

