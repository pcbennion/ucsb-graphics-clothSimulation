
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <iostream>
#include "Vector3.h"


using namespace std;

#define PI 3.14159265

// view info
int height, width;
Vector3 camerapos;
Vector3 focus;

// timer var
clock_t t;

// verticies
Vector3 vert[1000];
Vector3 v[1000];
Vector3 dv[1000];
int numVerts;
int n;

// springs
float length[1000][8];
int dst[1000][8];
int numSprings[1000];

// force constants
Vector3 gravity;
float mass;
float k;
float damping;

void sumForces(int index)
{
    // start out by setting gravity
    dv[index] = gravity;

    // now sum up all spring forces
    float L, S;
    Vector3 d;
    float kx;
    for(int i=0; i<numSprings[index]; i++)
    {
    	// get current length of spring
    	d = vert[index]-vert[dst[index][i]];
    	L = d.length();
    	// get displacement of spring and magnitude of force
    	S = L - length[index][i];
    	kx = -k*S;
    	// add to dv
    	dv[index]+=(d/L)*kx;
    }

    // add damping force
    dv[index]+=-(v[index]*damping);

    // F = ma --> a = F/m
    dv[index] /= mass;
}

void init(void)
{
    glEnable(GL_POINT_SMOOTH);
    glPointSize( 6.0 );

    glClearColor (0.0, 0.0, 0.0, 0.0);
    glEnable(GL_DEPTH_TEST);
}

void resize (int w, int h)
{
    if (h==0)  h=1;// don't divide by 0
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    height = h; width = w;
}

void display (void)
{
    // clear buffers
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set camera view
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotatef(180, 0, 1, 0);
    glTranslatef(-camerapos[0], -camerapos[1], -camerapos[2]);

    // render scene (verts and lines)
    int i, j;
    int Render_Verts = 0;
    // step through all verts
    if(Render_Verts)
    {
		for(i=0; i<numVerts; i++)
		{
			// draw vert
			glBegin(GL_POINTS);
				glColor3f(0.0, 1.0, 1.0); // cyan
				glVertex3f(vert[i][0], vert[i][1], vert[i][2]);
			glEnd();
			// step through all connected springs
			for(j=0; j<numSprings[i]; j++)
			{
				// draw spring
				glBegin(GL_LINES);
					glColor3f(1,1,1); // white
					glVertex3f(vert[i][0], vert[i][1], vert[i][2]);
					glVertex3f(vert[dst[i][j]][0], vert[dst[i][j]][1], vert[dst[i][j]][2]);
				glEnd();
			}
		}
    } else {
		// render scene (triangles)
		for(i=0; i<numVerts; i++)
		{
			glBegin(GL_TRIANGLE_FAN);
                glColor3f(1, 1, 1); // white
				glVertex3f(vert[i][0], vert[i][1], vert[i][2]);
			for(j=0; j<numSprings[i]; j++)
				glVertex3f(vert[dst[i][j]][0], vert[dst[i][j]][1], vert[dst[i][j]][2]);
			glEnd();
		}
    }

    // done with scene
    glutSwapBuffers();
}

//////////////////////////////////////////////////////////////////////////////////
// Handles keyboard events
//////////////////////////////////////////////////////////////////////////////////
void keyboard(unsigned char key, int x, int y)
{
  switch (key)
  {
      // the escape key and 'q' quit the program
      case 27:
      case 'q':
          exit(0);
          break;
  }
  glutPostRedisplay();
}

//////////////////////////////////////////////////////////////////////////////////
// Periodically check to see if animation is needed
//////////////////////////////////////////////////////////////////////////////////
void idle()
{
    float dt = ((float)(clock()-t))/((float)CLOCKS_PER_SEC);
    if(dt<0.020) return;// framerate of 60
    t=clock();

    int i;
    // calculate forces for prepare for euler
    for(i=0; i<numVerts; i++)
        sumForces(i);

    // execute forward euler
    for(i=0; i<numVerts; i++)
    {
        if(i!=0&&i!=n-1)
    	{
			v[i] = v[i]+(dv[i]*dt);
			vert[i]=vert[i]+(v[i]*dt);
    	}
    }

    // render changes
    glutPostRedisplay();
}

int main (int argc, char **argv)
{
    // Initialize view
    width = 800; height = 600;
    camerapos=Vector3(0.5, 0.3, 0.5);

    // Initialize timer
    t = clock();

    // Physics constants
    gravity = Vector3(0, -0.1, 0);
    mass = 0.075;
    k = 20;
    damping = 0.025;

    // Initialize verticies and springs
    numVerts = 0;
    n = 20;
    float r = 1/(float)n;
    float rDiag = r * 2 / sqrt(2);
	int a, b, i, j;
	int index;
    for(i=0; i<n; i++)
    {
        for(j=0; j<n; j++)
    	{
    		index = j*n + i;
			vert[index]=Vector3(i*r, 1, 0.5-j*r); v[index] = Vector3(0, 0, 0); dv[index] = Vector3(0, 0, 0);
				numSprings[index] = 0;
				numVerts++;
			// springs:
			if(i>0)
			{
				a = j*n + (i-1); b = j*n + (i);
				dst[a][index=numSprings[a]]=b; length[a][index] = r; numSprings[a]++;
				dst[b][index=numSprings[b]]=a; length[b][index] = r; numSprings[b]++;
				if(j>0)
				{
					a = (j-1)*n + (i-1); b = (j)*n + (i);
					dst[a][index=numSprings[a]]=b; length[a][index] = rDiag; numSprings[a]++;
					dst[b][index=numSprings[b]]=a; length[b][index] = rDiag; numSprings[b]++;
					if(j<n)
					{
						a = (j)*n + (i-1); b = (j-1)*n + (i);
						dst[b][index=numSprings[b]]=a; length[b][index] = rDiag; numSprings[b]++;
						dst[a][index=numSprings[a]]=b; length[a][index] = rDiag; numSprings[a]++;
					}
				}
			}
			if(j>0)
			{
				a = (j-1)*n + (i); b = j*n + (i);
				dst[a][index=numSprings[a]]=b; length[a][index] = r; numSprings[a]++;
				dst[b][index=numSprings[b]]=a; length[b][index] = r; numSprings[b]++;
			}
    	}
    }

   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
   glutInitWindowSize(width, height);
   glutInitWindowPosition(50, 50);
   glutCreateWindow("CMPSC 180, Homework 6");
   init();

   glutDisplayFunc(display);
   glutKeyboardFunc(keyboard);
   glutReshapeFunc(resize);
   glutIdleFunc(idle);

   glutMainLoop();

   return 0;
}

