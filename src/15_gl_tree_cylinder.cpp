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
#include <fstream>
#include <iomanip>
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
double zoom   = -20.0;		// zoom in or out from object
double xAngle = 0.0;		// rotational angle on x-axis
double yAngle = 0.0;		// rotational angle on x-axis
bool keys[256] = {0};		// holds key states for keyboard buffering
bool printNow = false;		// make sure it only prints once
const int N = 20;			// N-gon of cylinder (number of points on cylinder cap)
ofstream file;
int seed = 0;


const int MAX_DEPTH = 4;
const double LENGTHSCALE = 0.25;
const double WIDTHSCALE = 0.5;
const int NUMBRANCHES = 5;


// function prototypes (descriptions above function definitions)
void display();			
void keyUp(unsigned char key, int x, int y);
void keyDown(unsigned char key, int x, int y);
void checkKeyboard();
void timer(int value);
void reshape(GLsizei width, GLsizei height);
void lighting();

Point norm(Point v);
Point cross(Point a, Point b);
double dot(Point a, Point b);
Point normal(Point a, Point b, Point c);
void print(Point i, Point j, Point k, Point n);
void cylinder(Point p1, Point p2, double r);
void tree(int depth, Point start);
double rand(double low, double high);




double rand(double low, double high) {
	return (high-low)*rand()/RAND_MAX + low;
}




void tree(int depth, Point start) {
	// start at left-most branch
	double angle = 0;
	
	// number of branches at node determined by for loop
	for(int i = 0; i < NUMBRANCHES; i++) {
		//if(depth > 2 && rand(0,1) < (double)depth/MAX_DEPTH) continue; // random linear thinning as depth increases
		//if(depth > 2 && rand(0, 40) > pow(2, start.x*start.x+start.z*start.z)) continue; // random thinning of branches closer to y-axis
		
		// draw a cylinder (need to know: start and end and radius)
		double length = 1 / (LENGTHSCALE * depth);
		double width = 1 / (WIDTHSCALE * (depth+1));
		Point end;
		end.x = cos(angle + rand(-1, 1))*length + start.x;
		end.y = length/rand(.5, 2) + start.y;
		end.z = sin(angle + rand(-1, 1))*length + start.z;
		cylinder(start, end, 0.2*width);
		
		// if we want to recurse
		if(depth < MAX_DEPTH) {
			tree(depth+1, end);
		} else {
			// put in a leaf in the same orientation as the branch
			double x = end.x - start.x;
			double y = end.y - start.y;
			double z = end.z - start.z;
			Point close = {x, y, z};
			close = norm(close);
			close.x = close.x*0.1 + end.x;
			close.y = close.y*0.1 + end.y;
			close.z = close.z*0.1 + end.z;

			// put in a leaf facing upward
			close = end;
			close.y += 0.1;
			cylinder(end, close, width);
		}

		// update to new angle
		angle += 2*PI / (NUMBRANCHES);
	}
}


void drawObjectHere() {
	if(printNow) {
		file.open("C:/Users/alafl/Desktop/cylinder.stl");
		file << "solid TETRA" << endl;
	}
	
	Point a = {0, -7, 0};
	Point b = {0, -2, 0};
	cylinder(a, b, 0.1*1/WIDTHSCALE);
	
	//drawLine(400, 800, 400, 500);
	srand(seed);
	tree(1, b);
	
	if(printNow) file << "endsolid TETRA" << endl;
	if(printNow) cout << "print done" << endl;
	printNow = false;
}



void print(Point i, Point j, Point k, Point n) {
	file << scientific << setprecision(6);
	file << "facet normal " << n.x << " " << n.y << " " << n.z << endl
		<< "outer loop" << endl
		<< "vertex " << i.x << " " << i.y << " " << i.z << endl
		<< "vertex " << j.x << " " << j.y << " " << j.z << endl
		<< "vertex " << k.x << " " << k.y << " " << k.z << endl
		<< "endloop" << endl
		<< "endfacet" << endl;
}



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
  GLfloat position[] = { 0.0, 0.0, 10.0};

  glMaterialfv(GL_FRONT, GL_AMBIENT, cyan);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, yellow);
  glMaterialfv(GL_FRONT, GL_SPECULAR, white);
  glMaterialf(GL_FRONT, GL_SHININESS, 60);

  glLightfv(GL_LIGHT0, GL_AMBIENT, black);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, yellow);
  glLightfv(GL_LIGHT0, GL_SPECULAR, white);
  glLightfv(GL_LIGHT0, GL_POSITION, position);

  glEnable(GL_LIGHTING);                // so the renderer considers light
  glEnable(GL_LIGHT0);                  // turn LIGHT0 on
  glEnable(GL_DEPTH_TEST);              // so the renderer considers depth
}


Point normal(Point a, Point c, Point b) {
	// compute difference of a and b, c and b
	Point i, j, n;
	i.x = a.x - c.x;
	i.y = a.y - c.y;
	i.z = a.z - c.z;
	j.x = b.x - c.x;
	j.y = b.y - c.y;
	j.z = b.z - c.z;
	
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


void cylinder(Point p1, Point p2, double r) {
	// create v, a vector from p1 to p2, and normalize it
	Point v = {p2.x-p1.x, p2.y-p1.y, p2.z-p1.z};
	v = norm(v);

	// create p, a vector perpendicular to v, and normalize it
	// being careful not to divide by 0!!!!!!!!!!!!!!!!!!
	Point p;
	if(v.z != 0) {
		p.x = 1;
		p.y = 1;
		p.z = (-v.x-v.y)/v.z;
	} else if(v.y != 0) {
		p.x = 1;
		p.y = (-v.x-v.z)/v.y;
		p.z = 1;
	} else {
		p.x = (-v.y-v.z)/v.x;
		p.y = 1;
		p.z = 1;
	}
	p = norm(p);
	
	// create cr, perpendicular to both v and a (already normalized)
	Point cr = cross(v, p);
	
	// make points of circle, centered at origin
	double co, si, x, y, z;
	Point circle[N];
	for(int i = 0; i < N; i++) {
		co = r*cos(2*i*PI/N);
		si = r*sin(2*i*PI/N);
		circle[i].x = co*p.x + si*cr.x;
		circle[i].y = co*p.y + si*cr.y;
		circle[i].z = co*p.z + si*cr.z;
	}
	
	glBegin(GL_TRIANGLES);
		// draw end cap at p1, listing all verts as triangles
		Point a, b, c, n;
		for(int i = 0; i < N; i++) {
			a = {p1.x+circle[i].x, p1.y+circle[i].y, p1.z+circle[i].z};
			b = {p1.x+circle[(i+1)%N].x, p1.y+circle[(i+1)%N].y, p1.z+circle[(i+1)%N].z};
			n = normal(a, b, p1);
			glNormal3f(n.x, n.y, n.z);
			glVertex3f(a.x, a.y, a.z);
			glVertex3f(p1.x, p1.y, p1.z);
			glVertex3f(b.x, b.y, b.z);
			if(printNow) print(a, b, p1, n);
		}
		
		// draw end cap at p2, listing all verts as triangles
		for(int i = 0; i < N; i++) {
			a = {p2.x+circle[i].x, p2.y+circle[i].y, p2.z+circle[i].z};
			b = {p2.x+circle[(i+1)%N].x, p2.y+circle[(i+1)%N].y, p2.z+circle[(i+1)%N].z};
			n = normal(b, a, p2); // reversed since cap points other way
			glNormal3f(n.x, n.y, n.z);
			glVertex3f(a.x, a.y, a.z);
			glVertex3f(p2.x, p2.y, p2.z);
			glVertex3f(b.x, b.y, b.z);
			if(printNow) print(b, a, p2, n);
		}
		
		// draw shell, with rectangles as two triangles
		for(int i = 0; i < N; i++) {
			a = {p1.x+circle[i].x, p1.y+circle[i].y, p1.z+circle[i].z};
			b = {p2.x+circle[(i+1)%N].x, p2.y+circle[(i+1)%N].y, p2.z+circle[(i+1)%N].z};
			c = {p1.x+circle[(i+1)%N].x, p1.y+circle[(i+1)%N].y, p1.z+circle[(i+1)%N].z};
			n = normal(a, b, c);
			glNormal3f(n.x, n.y, n.z);
			glVertex3f(a.x, a.y, a.z);
			glVertex3f(b.x, b.y, b.z);
			glVertex3f(c.x, c.y, c.z);
			if(printNow) print(a, b, c, n);
			
			a = {p2.x+circle[i].x, p2.y+circle[i].y, p2.z+circle[i].z};
			b = {p2.x+circle[(i+1)%N].x, p2.y+circle[(i+1)%N].y, p2.z+circle[(i+1)%N].z};
			c = {p1.x+circle[i].x, p1.y+circle[i].y, p1.z+circle[i].z};
			n = normal(a, b, c);
			glNormal3f(n.x, n.y, n.z);
			glVertex3f(a.x, a.y, a.z);
			glVertex3f(b.x, b.y, b.z);
			glVertex3f(c.x, c.y, c.z);
			if(printNow) print(a, b, c, n);
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



// Main function: GLUT runs as a console application starting at main()
int main(int argc, char** argv) {
	glutInit(&argc, argv);				// Initialize GLUT
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);	// Enable double buffered mode
	glutInitWindowSize(1000, 1000);		// Set the window's initial width & height
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
	lighting();
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
	if(keys['r']) cout << seed++ << endl;
	if(keys['p']) printNow = true;
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


/*


void tree(int depth, Point start, double angle) {
	// start at left-most branch
	angle -= ANGLE/2;
	
	// number of branches at node determined by for loop
	for(int i = 0; i < NUMBRANCHES; i++) {
		//if(depth > 2 && rand(0,1) < (double)depth/MAX_DEPTH) continue; // random linear thinning as depth increases
		if(depth > 2 && rand(0, 40) > pow(2, start.x*start.x+start.z*start.z)) continue; // random thinning of branches closer to y-axis
		
		// draw a cylinder (need to know: start and end and radius)
		double length = 1 / (LENGTHSCALE * depth);
		double width = 1 / (WIDTHSCALE * (depth+1));
		Point end;
		end.x = cos(angle+rand(-0.3, 0.3))*length + start.x;
		end.y = length*rand(0.5, 1.5) + start.y;
		end.z = sin(angle+rand(-0.3, 0.3))*length + start.z;
		cylinder(start, end, 0.1*width);
		
		// if we want to recurse
		if(depth < MAX_DEPTH) {
			tree(depth+1, end, angle);
		} else {
			// put in a leaf
		}

		// update to new angle
		angle += ANGLE / (NUMBRANCHES-1);
	}
}
*/

