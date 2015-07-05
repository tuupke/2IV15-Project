// StableFluids.cpp : Defines the entry point for the console application.
//

#include "RigidBody.h"
#include "Rectangle.h"
#include "ScalarField.h"
#include "VectorField.h"
#include "FieldToolbox.h"
#include "particles/Particle.h"
#include "particles/Force.h"
#include "particles/SpringForce.h"
#include "particles/Gravity.h"
#include "particles/RodConstraint.h"
#include "particles/CircularWireConstraint.h"
#include "imageio.h"

#include <stdlib.h>
#include <stdio.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <vector>

/* macros */

#define IX(i, j) ((i)+(N+2)*(j))

/* global variables */

static int N;
static float dt, diff, visc;
static float force, source;
static int dvel;
static int dump_frames;
static int frame_number;
bool drawLine = true;

static VectorField *VelocityField, *PrevVelocityField;
static ScalarField *DensityField, *PrevDensityField;
static std::vector< RigidBody * > bodies;

static int win_id;
static int win_x, win_y;
static int mouse_down[3];
static int omx, omy, mx, my;

extern void simulation_step(std::vector< Particle * > pVector, std::vector< Force * > fVector, float dt, std::vector< Constraint * > fConstraint, VectorField *VelocityField);

static std::vector< Particle * > pVector;
static std::vector< Force * > fVector;
static std::vector< Constraint * > fConstraint;


/*
----------------------------------------------------------------------
free/clear/allocate simulation data
----------------------------------------------------------------------
*/


static void free_data(void) {
    if (VelocityField) delete (VelocityField);
    if (PrevVelocityField) delete (PrevVelocityField);
    if (DensityField) delete (DensityField);
    if (PrevDensityField) delete (PrevDensityField);
}

static void clear_data(void) {
    int i, size = (N + 2) * (N + 2);

    for (i = 0; i < size; i++) {
        (*VelocityField)[i][0] = (*VelocityField)[i][1] = 0.0f;
        (*PrevVelocityField)[i][0] = (*PrevVelocityField)[i][1] = 0.0f;
        (*DensityField)[i] = 0.0f;
        (*PrevDensityField)[i] = 0.0f;
    }

    for (i = 0; i < pVector.size(); i++)
	    pVector[i]->reset();
}

static void create_grid(int N)
{
	bool diagonals = 0;

	float screen_size = 0.9;
	float ks_xy = 0.6;
	float ks_diag = 1.0;
	float particle_weight = 0.8;
	float x, y, h;
	int i, j;
	Vec2f position;

	pVector.clear();

	h = 1.0f / N;

	for (i = 1; i <= N; i++) {
		x = (i - 0.5f) * h;
		for (j = 1; j <= N; j++) {
			y = (j - 0.5f) * h;
			position = Vec2f(x, y);

			printf("Position (%d, %d): %f, %f\n", i, j, x, y);

			pVector.push_back(new Particle(position, particle_weight));
		}
	}

	// X-springs
	for (int y = 0; y < N; y++) {
		for (int x = 0; x < N - 1; x++) {
			fVector.push_back(new SpringForce(pVector[x + y * N],
						pVector[x + 1 + y * N],
						screen_size / (N - 1), ks_xy, 0.5));
		}
 	}

	// Y-springs
	for (int y = 0; y < N - 1; y++) {
		for (int x = 0; x < N; x++) {
			fVector.push_back(new SpringForce(pVector[x + y * N],
						pVector[x + (y + 1) * N],
						screen_size / (N - 1), ks_xy, 0.5));
		}
	}

	// diagonal springs
	if (diagonals) {
		for (int y = 0; y < N - 1; y++) {
			for (int x = 0; x < N - 1; x++) {
				fVector.push_back(new SpringForce(pVector[x + y * N],
						  pVector[x + 1 + (y + 1) * N],
						  sqrt(2 * pow((screen_size / (N - 1)), 2)),
						  ks_diag, 0.5));
				fVector.push_back(new SpringForce(pVector[x + 1 + y * N],
						  pVector[x + (y + 1) * N],
						  sqrt(2 * pow((screen_size / (N - 1)), 2)),
						  ks_diag, 0.5));
			}
		}
	}
}

static int allocate_data(void) {
    FieldToolbox::Create();
    VelocityField = new VectorField(N, visc, dt);
    PrevVelocityField = new VectorField(N, visc, dt);
    DensityField = new ScalarField(N, diff, dt);
    PrevDensityField = new ScalarField(N, diff, dt);

    bodies.push_back(new Rectangle(Vec2f(0.5f, 0.5f), 0.0f, 0.3f, 0.2f));
    create_grid(8);

    if (!VelocityField || !PrevVelocityField || !DensityField || !PrevDensityField) {
        fprintf(stderr, "cannot allocate data\n");
        return (0);
    }

    return (1);
}

/*
----------------------------------------------------------------------
OpenGL specific drawing routines
----------------------------------------------------------------------
*/

static void pre_display(void) {
    glViewport(0, 0, win_x, win_y);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, 1.0, 0.0, 1.0);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

static void post_display(void) {
    // Write frames if necessary.
    if (dump_frames) {
        const int FRAME_INTERVAL = 4;
        if ((frame_number % FRAME_INTERVAL) == 0) {
            const unsigned int w = glutGet(GLUT_WINDOW_WIDTH);
            const unsigned int h = glutGet(GLUT_WINDOW_HEIGHT);
            unsigned char *buffer = (unsigned char *) malloc(w * h * 4 * sizeof(unsigned char));
            if (!buffer)
                exit(-1);
            // glRasterPos2i(0, 0);
            glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
            char filename[13];
            sprintf(filename, "img%.5i.png", frame_number / FRAME_INTERVAL);
            printf("Dumped %s.\n", filename);
            saveImageRGBA(filename, buffer, w, h);
        }
    }
    frame_number++;

    glutSwapBuffers();
}

static void draw_velocity(void) {
    int i, j;
    float x, y, h;

    h = 1.0f / N;

    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(1.0f);

    glBegin(GL_LINES);

    for (i = 1; i <= N; i++) {
        x = (i - 0.5f) * h;
        for (j = 1; j <= N; j++) {
            y = (j - 0.5f) * h;

            glVertex2f(x, y);
            glVertex2f(x + (*VelocityField)[IX(i, j)][0], y + (*VelocityField)[IX(i, j)][1]);
        }
    }

    glEnd();
}

static void draw_density(void) {
    int i, j;
    float x, y, h, d00, d01, d10, d11;

    h = 1.0f / N;

    glBegin(GL_QUADS);

    for (i = 0; i <= N; i++) {
        x = (i - 0.5f) * h;
        for (j = 0; j <= N; j++) {
            y = (j - 0.5f) * h;

            d00 = (*DensityField)[IX(i, j)];
            d01 = (*DensityField)[IX(i, j + 1)];
            d10 = (*DensityField)[IX(i + 1, j)];
            d11 = (*DensityField)[IX(i + 1, j + 1)];

            glColor3f(d00, d00, d00);
            glVertex2f(x, y);
            glColor3f(d10, d10, d10);
            glVertex2f(x + h, y);
            glColor3f(d11, d11, d11);
            glVertex2f(x + h, y + h);
            glColor3f(d01, d01, d01);
            glVertex2f(x, y + h);
        }
    }

    glEnd();
}

static void draw_particles(void) {
    int size = pVector.size();

    for (int ii = 0; ii < size; ii++) {
        pVector[ii]->draw();
    }
}

static void draw_forces(void) {
    for (int i = 0; i < fVector.size(); i++)
        fVector[i]->draw();
}

static void draw_constraints(void) {
    for (int i = 0; i < fConstraint.size(); i++) {
        fConstraint[i]->draw();
    }
}
 
/*
----------------------------------------------------------------------
relates mouse movements to forces sources
----------------------------------------------------------------------
*/

static void get_from_UI(ScalarField *d, VectorField *u_v) {
    int i, j, size = (N + 2) * (N + 2);

    for (i = 0; i < size; i++) {
        (*u_v)[i][0] = (*u_v)[i][1] = (*d)[i] = 0.0f;
    }

    if (!mouse_down[0] && !mouse_down[2]) return;

    i = (int) ((mx / (float) win_x) * N + 1);
    j = (int) (((win_y - my) / (float) win_y) * N + 1);

    if (i < 1 || i > N || j < 1 || j > N) return;

    if (mouse_down[0]) {
        (*u_v)[IX(i, j)][0] = force * (mx - omx);
        (*u_v)[IX(i, j)][1] = force * (omy - my);
    }

    if (mouse_down[2]) {
        (*d)[IX(i, j)] = source;
    }

    omx = mx;
    omy = my;

    return;
}

/*
----------------------------------------------------------------------
GLUT callback routines
----------------------------------------------------------------------
*/

static void key_func(unsigned char key, int x, int y) {
    switch (key) {
        case 'c':
        case 'C':
            clear_data();
            break;

        case 'd':
        case 'D':
            dump_frames = !dump_frames;
            break;

        case 'q':
        case 'Q':
            free_data();
            exit(0);
            break;

        case 'v':
        case 'V':
            dvel = !dvel;
            break;
        case 'f':
            drawLine = !drawLine;
            break;
    }
}

static void mouse_func(int button, int state, int x, int y) {
    omx = mx = x;
    omx = my = y;

    if (glutGetModifiers() & GLUT_ACTIVE_SHIFT) {
        button = 2;
    }

    mouse_down[button] = state == GLUT_DOWN;
}

static void motion_func(int x, int y) {
    mx = x;
    my = y;
}

static void reshape_func(int width, int height) {
    glutSetWindow(win_id);
    glutReshapeWindow(width, height);

    win_x = width;
    win_y = height;
}

static void idle_func ( void )
{
	get_from_UI( PrevDensityField, PrevVelocityField );
	VelocityField->TimeStep( VelocityField, PrevVelocityField, bodies);
	DensityField->TimeStep( DensityField, PrevDensityField, VelocityField );
	simulation_step(pVector, fVector, dt, fConstraint, VelocityField);

    glutSetWindow(win_id);
    glutPostRedisplay();
}

static void display_func(void) {
    pre_display();

    if (dvel) draw_velocity();
    else draw_density();

    draw_forces();
    draw_constraints();
    draw_particles();

    for(int i = 0; i < bodies.size(); i++){
        bodies[i]->draw();
    }

    post_display();
}


/*
----------------------------------------------------------------------
open_glut_window --- open a glut compatible window and set callbacks
----------------------------------------------------------------------
*/

static void open_glut_window(void) {
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);

    glutInitWindowPosition(0, 0);
    glutInitWindowSize(win_x, win_y);
    win_id = glutCreateWindow("Stable Fluids!");

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glutSwapBuffers();
    glClear(GL_COLOR_BUFFER_BIT);
    glutSwapBuffers();

    pre_display();

    glutKeyboardFunc(key_func);
    glutMouseFunc(mouse_func);
    glutMotionFunc(motion_func);
    glutReshapeFunc(reshape_func);
    glutIdleFunc(idle_func);
    glutDisplayFunc(display_func);
}


/*
----------------------------------------------------------------------
main --- main routine
----------------------------------------------------------------------
*/

int main(int argc, char **argv) {
    glutInit(&argc, argv);

    if (argc != 1 && argc != 7) {
        fprintf(stderr, "usage : %s N dt diff visc force source\n", argv[0]);
        fprintf(stderr, "where:\n");\
            fprintf(stderr, "\t N      : grid resolution\n");
        fprintf(stderr, "\t dt     : time step\n");
        fprintf(stderr, "\t diff   : diffusion rate of the density\n");
        fprintf(stderr, "\t visc   : viscosity of the fluid\n");
        fprintf(stderr, "\t force  : scales the mouse movement that generate a force\n");
        fprintf(stderr, "\t source : amount of density that will be deposited\n");
        exit(1);
    }

    if (argc == 1) {
        N = 64;
        dt = 0.1f;
        diff = 0.0f;
        visc = 0.0f;
        force = 5.0f;
        source = 100.0f;
        fprintf(stderr, "Using defaults : N=%d dt=%g diff=%g visc=%g force = %g source=%g\n",
                N, dt, diff, visc, force, source);
    } else {
        N = atoi(argv[1]);
        dt = atof(argv[2]);
        diff = atof(argv[3]);
        visc = atof(argv[4]);
        force = atof(argv[5]);
        source = atof(argv[6]);
    }

    printf("\n\nHow to use this demo:\n\n");
    printf("\t Add densities with the right mouse button\n");
    printf("\t Add velocities with the left mouse button and dragging the mouse\n");
    printf("\t Toggle density/velocity display with the 'v' key\n");
    printf("\t Clear the simulation by pressing the 'c' key\n");
    printf("\t Quit by pressing the 'q' key\n");

    dvel = 0;

    if (!allocate_data()) exit(1);
    clear_data();

    win_x = 512;
    win_y = 512;
    open_glut_window();

    glutMainLoop();

    exit(0);
}
