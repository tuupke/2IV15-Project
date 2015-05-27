// TinkerToy.cpp : Defines the entry point for the console application.
//

#include "Particle.h"
#include "Force.h"
#include "SpringForce.h"
#include "AngularForce.h"
#include "MouseForce.h"
#include "Gravity.h"
#include "RodConstraint.h"
#include "CircularWireConstraint.h"
#include "imageio.h"

#include <vector>
#include <stdlib.h>
//#include <stdio.h>
#include <cstdio>
#include <GL/glut.h>
#include <cmath>

/* macros */

/* external definitions (from solver) */
extern void simulation_step( std::vector<Particle*> pVector, std::vector<Force*> fVector, float dt, std::vector<Constraint*> fConstraint);
int solver = 1; // The solver to use: 1: Euler, 2: Midpoint, 3: Runge-Kutta

/* global variables */

static int N;
static float dt, d;
static int dsim;
static int dump_frames;
static int frame_number;

// static Particle *pList;
static std::vector<Particle*> pVector;

static int win_id;
static int win_x, win_y;
static int mouse_down[3];
static int mouse_release[3];
static int mouse_shiftclick[3];
static int omx, omy, mx, my;
static int hmx, hmy;

static std::vector<Force*> fVector;
static std::vector<Constraint*> fConstraint;
static RodConstraint * delete_this_dummy_rod = NULL;
static CircularWireConstraint * delete_this_dummy_wire = NULL;
static MouseForce * mouse_force = NULL;


/*
----------------------------------------------------------------------
free/clear/allocate simulation data
----------------------------------------------------------------------
*/

static void free_data ( void )
{
	pVector.clear();

	if (delete_this_dummy_rod) {
		delete delete_this_dummy_rod;
		delete_this_dummy_rod = NULL;
	}

	fVector.clear();
	fConstraint.clear();

	if (delete_this_dummy_wire) {
		delete delete_this_dummy_wire;
		delete_this_dummy_wire = NULL;
	}
}

static void clear_data ( void )
{
	int ii, size = pVector.size();

	for(ii=0; ii<size; ii++){
		pVector[ii]->reset();
	}
}

static void create_grid(int size, bool diagonals)
{
	float screen_size = 1.8;
	float ks_xy = 0.5;
	float ks_diag = 0.7;
	float particle_weight = 1.0;
	Vec2f position;

	pVector.clear();

	for (int y = 0; y < size; y++) {
		for (int x = 0; x < size; x++) {
			position = Vec2f((screen_size/(size-1)*x) - 0.9, (screen_size/(size-1)*y) - 0.9);
			pVector.push_back(new Particle(position, particle_weight));
		}
	}
	// X-springs
	for (int y = 0; y < size; y++) {
		for (int x = 0; x < size-1; x++) {
			fVector.push_back(new SpringForce(pVector[x + y*size], 
							  pVector[x+1 + y*size], 
							  screen_size/(size-1), ks_xy, 0.5));
		}
	}
	// Y-springs
	for (int y = 0; y < size-1; y++) {
		for (int x = 0; x < size; x++) {
			fVector.push_back(new SpringForce(pVector[x + y*size], 
							  pVector[x + (y+1)*size], 
							  screen_size/(size-1), ks_xy, 0.5));
		}
	}
	// diagonal springs
	if (diagonals) {
		for (int y = 0; y < size-1; y++) {
			for (int x = 0; x < size-1; x++) {
				fVector.push_back(new SpringForce(pVector[x + y*size], 
								  pVector[x+1 + (y+1)*size], 
								  sqrt(2*pow((screen_size/(size-1)), 2)), 
								  ks_diag, 0.5));
				fVector.push_back(new SpringForce(pVector[x+1 + y*size], 
								  pVector[x + (y+1)*size], 
								  sqrt(2*pow((screen_size/(size-1)), 2)), 
								  ks_diag, 0.5));
			}
		}
	}
}

static void init_system(void)
{
	const double dist = 0.2;
	const Vec2f center(0.0, 0.0);
	const Vec2f offset(dist, 0.0);
	const Vec2f offset1(0.0, dist);

	// Create three particles, attach them to each other, then add a
	// circular wire constraint to the first.
	
	pVector.push_back(new Particle(center - offset, 1));
//	pVector.push_back(new Particle(center + offset + offset, 1));
//	pVector.push_back(new Particle(center + offset + offset + offset, 2));
//	pVector.push_back(new Particle(center + offset + offset1, 2));
	
//	fVector.push_back(new SpringForce(pVector[0], pVector[1], 2*dist, 0.5, 0.1));
//	fVector.push_back(new SpringForce(pVector[3], pVector[0], dist, 0.5, 0.1));
//	fVector.push_back(new AngularForce(pVector[3], pVector[0], pVector[1], 0.2*3.14159265, 0.5, 1.0));

	char choice;
	std::cout << "Diagonal springs? [y]/n" << std::endl;
	std::cin >> choice;

	bool diagonals = !(choice == 'n');

	create_grid(10, diagonals);

	std::vector<int> ids;
	ids.push_back(0);
//	fConstraint.push_back(new CircularWireConstraint(pVector[0], center, dist, ids));
 	for (int i = 0; i < pVector.size(); i++) {
//  		fVector.push_back(new Gravity(pVector[i], Vec2f(0,-0.01)));
  		fVector.push_back(new Drag(pVector[i], 0.10));
  		fVector.push_back(new Gravity(pVector[i], Vec2f(0,-0.01)));
//  		fVector.push_back(new Drag(pVector[i], 0.10));

  	}
  
	mouse_force = new MouseForce(pVector, 0.05, 0.30, 1.5, 0.2);
	fVector.push_back(mouse_force);
 
//	delete_this_dummy_rod = new RodConstraint(pVector[1], pVector[2], dist);
//	delete_this_dummy_wire = new CircularWireConstraint(pVector[0], center, dist);
}

/*
----------------------------------------------------------------------
OpenGL specific drawing routines
----------------------------------------------------------------------
*/

static void pre_display ( void )
{
	glViewport ( 0, 0, win_x, win_y );
	glMatrixMode ( GL_PROJECTION );
	glLoadIdentity ();
	gluOrtho2D ( -1.0, 1.0, -1.0, 1.0 );
	glClearColor ( 0.0f, 0.0f, 0.0f, 1.0f );
	glClear ( GL_COLOR_BUFFER_BIT );
}

static void post_display ( void )
{
	// Write frames if necessary.
	if (dump_frames) {
		const int FRAME_INTERVAL = 4;
		if ((frame_number % FRAME_INTERVAL) == 0) {
			const unsigned int w = glutGet(GLUT_WINDOW_WIDTH);
			const unsigned int h = glutGet(GLUT_WINDOW_HEIGHT);
			unsigned char * buffer = (unsigned char *) malloc(w * h * 4 * sizeof(unsigned char));
			if (!buffer)
				exit(-1);
			// glRasterPos2i(0, 0);
			glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
			static char filename[80];
			sprintf(filename, "snapshots/img%.5i.png", frame_number / FRAME_INTERVAL);
			printf("Dumped %s.\n", filename);
			saveImageRGBA(filename, buffer, w, h);
			
			free(buffer);
		}
	}
	frame_number++;
	
	glutSwapBuffers ();
}

static void draw_particles ( void )
{
	int size = pVector.size();

	for(int ii=0; ii< size; ii++)
	{
		pVector[ii]->draw();
	}
}

static void draw_forces ( void )
{
	for (int i = 0; i < fVector.size(); i++)
		fVector[i]->draw();
}

static void draw_constraints ( void )
{
	for(int i = 0; i < fConstraint.size(); i++){
		fConstraint[i]->draw();
	}
}

/*
----------------------------------------------------------------------
relates mouse movements to tinker toy construction
----------------------------------------------------------------------
*/

static void get_from_UI ()
{
	int i, j;
	// int size, flag;
//	int hi, hj;
	// float x, y;
	if ( !mouse_down[0] && !mouse_down[2] && !mouse_release[0] 
	&& !mouse_shiftclick[0] && !mouse_shiftclick[2] ) return;

	i = (int)((       mx /(float)win_x)*N);
	j = (int)(((win_y-my)/(float)win_y)*N);

	if ( i<1 || i>N || j<1 || j>N ) return;

	if ( mouse_down[0] ) {

	}

	if ( mouse_down[2] ) {
	}

//	hi = (int)((       hmx /(float)win_x)*N);
//	hj = (int)(((win_y-hmy)/(float)win_y)*N);

	if( mouse_release[0] ) {
	}

	omx = mx;
	omy = my;
}

static void remap_GUI()
{
	int ii, size = pVector.size();
	for(ii=0; ii<size; ii++)
	{
		pVector[ii]->m_Position[0] = pVector[ii]->m_ConstructPos[0];
		pVector[ii]->m_Position[1] = pVector[ii]->m_ConstructPos[1];
	}
}

/*
----------------------------------------------------------------------
GLUT callback routines
----------------------------------------------------------------------
*/

static void key_func ( unsigned char key, int x, int y )
{
	switch ( key )
	{
	case 'c':
	case 'C':
		clear_data ();
		break;

	case 'd':
	case 'D':
		dump_frames = !dump_frames;
		break;

	case 'q':
	case 'Q':
		free_data ();
		exit ( 0 );
		break;
	
	case 'e':
	case 'E':
		solver = 1;
		printf("Solver: Euler\n");
		break;
	case 'm':
	case 'M':
		solver = 2;
		printf("Solver: Midpoint\n");
		break;
	case 'r':
	case 'R':
		solver = 3;
		printf("Solver: Runge-Kutta 4\n");
		break;

	case '+':
		dt += 0.01;
		printf("dt: %f\n", dt);
		break;
	case '-':
		dt -= 0.01;
		printf("dt: %f\n", dt);
		break;
	case '>':
		dt += 0.1;
		printf("dt: %f\n", dt);
		break;
	case '<':
		dt -= 0.1;
		printf("dt: %f\n", dt);
		break;

	case 'a':
	case 'A':
		mouse_force->m_radius += 0.05;
		printf("radius: %f\n", mouse_force->m_radius);
		break;
	case 'z':
	case 'Z':
		mouse_force->m_radius -= 0.05;
		printf("radius: %f\n", mouse_force->m_radius);
		break;


	case ' ':
		dsim = !dsim;
		break;
	}
}

static void mouse_func ( int button, int state, int x, int y )
{
	float mouse_x = (double)2*(x - win_x/2)/(double)win_x;
	float mouse_y = (double)2*(win_y/2 - y)/(double)win_y;

	omx = mx = x;
	omx = my = y;

	if(!mouse_down[0]){hmx=x; hmy=y;}
	if(mouse_down[button]) mouse_release[button] = state == GLUT_UP;
	if(mouse_down[button]) mouse_shiftclick[button] = glutGetModifiers()==GLUT_ACTIVE_SHIFT;
	mouse_down[button] = state == GLUT_DOWN;

	if (state == GLUT_UP) {
		mouse_force->disable();
	}
	if (state == GLUT_DOWN) {
		mouse_force->enable(mouse_x, mouse_y);
	}
}

static void motion_func ( int x, int y )
{
	float mouse_x = (double)2*(x - win_x/2)/(double)win_x;
	float mouse_y = (double)2*(win_y/2 - y)/(double)win_y;
	mx = x;
	my = y;

	mouse_force->enable(mouse_x, mouse_y);
}

static void reshape_func ( int width, int height )
{
	glutSetWindow ( win_id );
	glutReshapeWindow ( width, height );

	win_x = width;
	win_y = height;
}

static void idle_func ( void )
{
	if ( dsim ) simulation_step( pVector, fVector, dt, fConstraint );
	else        {get_from_UI();remap_GUI();}

	glutSetWindow ( win_id );
	glutPostRedisplay ();
}

static void display_func ( void )
{
	pre_display ();

	draw_forces();
	draw_constraints();
	draw_particles();

	post_display ();
}


/*
----------------------------------------------------------------------
open_glut_window --- open a glut compatible window and set callbacks
----------------------------------------------------------------------
*/

static void open_glut_window ( void )
{
	glutInitDisplayMode ( GLUT_RGBA | GLUT_DOUBLE );

	glutInitWindowPosition ( 0, 0 );
	glutInitWindowSize ( win_x, win_y );
	win_id = glutCreateWindow ( "Tinkertoys!" );

	glClearColor ( 0.0f, 0.0f, 0.0f, 1.0f );
	glClear ( GL_COLOR_BUFFER_BIT );
	glutSwapBuffers ();
	glClear ( GL_COLOR_BUFFER_BIT );
	glutSwapBuffers ();

	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);

	pre_display ();

	glutKeyboardFunc ( key_func );
	glutMouseFunc ( mouse_func );
	glutMotionFunc ( motion_func );
	glutReshapeFunc ( reshape_func );
	glutIdleFunc ( idle_func );
	glutDisplayFunc ( display_func );
}


/*
----------------------------------------------------------------------
main --- main routine
----------------------------------------------------------------------
*/

int main ( int argc, char ** argv )
{
	glutInit ( &argc, argv );

	if ( argc == 1 ) {
		N = 64;
		dt = 0.1f;
		d = 5.f;
		fprintf ( stderr, "Using defaults : N=%d dt=%g d=%g\n",
			N, dt, d );
	} else {
		N = atoi(argv[1]);
		dt = atof(argv[2]);
		d = atof(argv[3]);
	}

	printf ( "\n\nHow to use this application:\n\n" );
	printf ( "\t Toggle construction/simulation display with the spacebar key\n" );
	printf ( "\t Dump frames by pressing the 'd' key\n" );
	printf ( "\t Quit by pressing the 'q' key\n" );
	printf ( "\t 'e' = Euler\n" );
	printf ( "\t 'm' = Midpoint\n" );
	printf ( "\t 'r' = Runge Kutta\n\n" );
	printf ( "\t '+, >' = Increase dt with 0.01, 0.1\n" );
	printf ( "\t '-, <' = Decrease dt with 0.01, 0.1\n" );
	printf ( "\t 'a, z' = Increase, decrease mouse radius\n" );

	dsim = 0;
	dump_frames = 0;
	frame_number = 0;
	
	init_system();
	
	win_x = 512;
	win_y = 512;
	open_glut_window ();

	glutMainLoop ();

	exit ( 0 );
}

