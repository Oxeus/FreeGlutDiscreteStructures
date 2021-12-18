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


struct vec {
	double x;
	double y;
	double z;
};
 
 
// Global variables and consts
const int RES = 100;		// resolution of our cubic grid
const int MILS = 20;		// refresh interval in milliseconds
double zoom   = -10.0;		// zoom in or out from object
double xAngle = 0.0;		// rotational angle on x-axis
double yAngle = 0.0;		// rotational angle on x-axis
bool* keys = new bool[256];
// holds key states for keyboard buffering
double pi = 3.1415926535897;


// function prototypes (descriptions above function definitions)
vec normal(vec a, vec b, vec c);
void drawTri(vec a, vec b, vec c);
void drawCube(vec pos, double side);
bool R(double x, double y, double z);
bool intersects(double i, double j, double k, double side);
void drawObjectHere();
void display();			
void keyUp(unsigned char key, int x, int y);
void keyDown(unsigned char key, int x, int y);
void checkKeyboard();
void timer(int value);
void reshape(GLsizei width, GLsizei height);
void lighting();


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
  GLfloat position[] = { 1.0, 1.0, 1.0};

  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, cyan);
  glMaterialfv(GL_FRONT, GL_SPECULAR, white);
  glMaterialf(GL_FRONT, GL_SHININESS, 30);

  glLightfv(GL_LIGHT0, GL_AMBIENT, black);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, yellow);
  glLightfv(GL_LIGHT0, GL_SPECULAR, white);
  glLightfv(GL_LIGHT0, GL_POSITION, position);

  glEnable(GL_LIGHTING);                // so the renderer considers light
  glEnable(GL_LIGHT0);                  // turn LIGHT0 on
  glEnable(GL_DEPTH_TEST);              // so the renderer considers depth
}


// computes the normal (perpendicular) vector to a flat triangular surface
// so that the lighting function can compute light reflections correctly
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

// draws a triangle between the points a, b and c
void drawTri(vec a, vec b, vec c) {
	glBegin(GL_TRIANGLES);
		vec n = normal(a, c, b);
		glNormal3f(n.x, n.y, n.z); // set the normal
		glVertex3f(a.x, a.y, a.z);
		glVertex3f(b.x, b.y, b.z);
		glVertex3f(c.x, c.y, c.z);
	glEnd();
}


// draws a vertically oriented lit cube centered on 'pos' 
// with size 'side' length per side using two triangles per face
void drawCube(vec pos, double side) {
	// build 8 vertices of the cube (naming: left right, bottom top, back front)
	vec lbb = { pos.x - side / 2, pos.y - side / 2, pos.z - side / 2 };
	vec lbf = { pos.x - side / 2, pos.y - side / 2, pos.z + side / 2 };
	vec ltb = { pos.x - side / 2, pos.y + side / 2, pos.z - side / 2 };
	vec ltf = { pos.x - side / 2, pos.y + side / 2, pos.z + side / 2 };
	vec rbb = { pos.x + side / 2, pos.y - side / 2, pos.z - side / 2 };
	vec rbf = { pos.x + side / 2, pos.y - side / 2, pos.z + side / 2 };
	vec rtb = { pos.x + side / 2, pos.y + side / 2, pos.z - side / 2 };
	vec rtf = { pos.x + side / 2, pos.y + side / 2, pos.z + side / 2 };

	// draw two tri per cube face (making sure vertices are listed ccw)
	drawTri(ltf, lbf, rtf); // upper front
	drawTri(rbf, rtf, lbf); // lower front

	drawTri(ltb, rtb, lbb); // upper back
	drawTri(rbb, lbb, rtb); // lower back

	drawTri(lbf, lbb, rbf); // upper bottom
	drawTri(rbb, rbf, lbb); // lower bottom

	drawTri(ltb, ltf, rtb); // upper top
	drawTri(rtf, rtb, ltf); // lower top

	drawTri(rtb, rtf, rbb); // upper right
	drawTri(rbf, rbb, rtf); // lower right

	drawTri(ltf, ltb, lbf); // upper left
	drawTri(lbb, lbf, ltb); // lower left
}

void drawObjectHere() {
	// set cube size
	double side = 2.0 / RES;

	// touch each cube in the grid
	for (int i = 0; i < RES; i++) {
		for (int j = 0; j < RES; j++) {
			for (int k = 0; k < RES; k++) {
				// if relation intersects cube location
				if (intersects(2.0*i / RES - 1, 2.0*j / RES - 1, 2.0*k / RES - 1, side)) {
					// draw the cube
					vec pos = { 2.0*i / RES - 1, 2.0*j / RES - 1, 2.0*k / RES - 1 };
					drawCube(pos, side);
				}
			}
		}
	}
}

// our relation returns true if (x, y, z) is
// in the volume, false if it isn't
bool R(double x, double y, double z) {
	double sphere = 0.99;
	double hole = 0.90;
	double diag = 1.8;
	// return x*x + y*y + z*z <= 0.9; // a sphere
	// return x + y + z < 0; // a diagonal plane
	// return y < 0; // a flat vertical plane
	// return x*x + y*y + z*z <= 0.9 && y > 0; // northern hemisphere (an intersection)

	// return y > 0 || y < -0.5; // union of two planes
	// return y > 0 || y > 0.5; // but this doesn't do what you think!!!
	// return y > 0 || y < 0.5; // nor does this
	// what is going on and why isn't this working?
	// how can you make a successful union???

	// here's some fun stuff using only && and || without fixing union
	// return x*x + y*y < 0.25 || x*x + z*z < 0.25 || y*y + z*z < 0.25; // here's a PVC connector
	// and here are some caps for the pipes :)
	//return ((x > 0.85 || x < -0.85) || (y > 0.85 || y < -0.85) || (z > 0.85 || z < -0.85)) && (x*x + y*y + z*z < 0.9);
	// see if you can make a shape with intersections and unions of at least 10 shapes
	return (x*x + y * y + z * z < sphere) || (y > hole) || (x > hole) || (x < -hole) || (y < -hole) || (z > hole) || (z < -hole) || 
		x + y + z > diag || -x + -y + -z > diag || x + -y + -z > diag || -x + y + -z > diag || -x + -y + z > diag || -x + y + z > diag || x + -y + z > diag || x + y + -z > diag;
}

// checks to see if some cube vertices are in
// the relation while others are not
bool intersects(double i, double j, double k, double side) {
	// count up how many verts are in the surface
	int count = 0;
	count += R(i - side / 2, j - side / 2, k - side / 2);
	count += R(i - side / 2, j - side / 2, k + side / 2);
	count += R(i - side / 2, j + side / 2, k - side / 2);
	count += R(i - side / 2, j + side / 2, k + side / 2);
	count += R(i + side / 2, j - side / 2, k - side / 2);
	count += R(i + side / 2, j - side / 2, k + side / 2);
	count += R(i + side / 2, j + side / 2, k - side / 2);
	count += R(i + side / 2, j + side / 2, k + side / 2);
	
	// check if some verts are in surface and others aren't
	return count > 0 && count < 8;
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

