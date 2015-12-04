// ----------------------------------------------
// Informatique Graphique 3D & Réalité Virtuelle.
// Travaux Pratiques
// Algorithmes de Rendu
// Copyright (C) 2015 Tamy Boubekeur
// All rights reserved.
// ----------------------------------------------

#include <iostream>
#include <vector>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <cmath>
#include <GL/glut.h>

#include "Vec3.h"
#include "Camera.h"
#include "Mesh.h"

using namespace std;

static const unsigned int DEFAULT_SCREENWIDTH = 1024;
static const unsigned int DEFAULT_SCREENHEIGHT = 768;
static const string DEFAULT_MESH_FILE ("models/man.off");

static string appTitle ("Informatique Graphique & Realite Virtuelle - Travaux Pratiques - Algorithmes de Rendu");
static GLint window;
static unsigned int FPS = 0;
static bool fullScreen = false;

static Camera camera;
static Mesh mesh;

void printUsage () {
	std::cerr << std::endl 
		 << appTitle << std::endl
         << "Author: Tamy Boubekeur" << std::endl << std::endl
         << "Usage: ./main [<file.off>]" << std::endl
         << "Commands:" << std::endl 
         << "------------------" << std::endl
         << " ?: Print help" << std::endl
		 << " w: Toggle wireframe mode" << std::endl
         << " <drag>+<left button>: rotate model" << std::endl 
         << " <drag>+<right button>: move model" << std::endl
         << " <drag>+<middle button>: zoom" << std::endl
         << " q, <esc>: Quit" << std::endl << std::endl; 
}

void init (const char * modelFilename) {
    glCullFace (GL_BACK);     // Specifies the faces to cull (here the ones pointing away from the camera)
    glEnable (GL_CULL_FACE); // Enables face culling (based on the orientation defined by the CW/CCW enumeration).
    glDepthFunc (GL_LESS); // Specify the depth test for the z-buffer
    glEnable (GL_DEPTH_TEST); // Enable the z-buffer in the rasterization
    glLineWidth (2.0); // Set the width of edges in GL_LINE polygon mode
    glClearColor (0.0f, 0.0f, 0.0f, 1.0f); // Background color
    glClearColor (0.0f, 0.0f, 0.0f, 1.0f);
	
    mesh.loadOFF (modelFilename);
    camera.resize (DEFAULT_SCREENWIDTH, DEFAULT_SCREENHEIGHT);
}

void drawScene () {
    glBegin (GL_TRIANGLES);
    for (unsigned int i = 0; i < mesh.T.size (); i++) 
        for (unsigned int j = 0; j < 3; j++) {
            const Vertex & v = mesh.V[mesh.T[i].v[j]];
            // EXERCISE : the following color response shall be replaced with a proper reflectance evaluation/shadow test/etc.

            Vec3<float> normal = Vec3<float>(v.n[0], v.n[1], v.n[2]);
            Vec3<float> light_dir = Vec3<float>(0.0f, 0.0f, -1.0f);

            float cam_x, cam_y, cam_z;
            camera.getPos(cam_x, cam_y, cam_z);
            Vec3<float> camera_dir = Vec3<float>(cam_x - v.p[0], cam_y- v.p[1], cam_z- v.p[2]);
            camera_dir.normalize();

            float Kd = 0.7f;
            float diffuse_term = Kd/3.14f;

            // Implementation of BRDF (Blinn-Phong)

            // float Ks = 0.5f;
            // float S = 2.0f;

            // Vec3<float> r = 2.0f * normal * dot(normal, light_dir) - light_dir;
            // float specular_term = Ks * pow(dot(r, camera_dir), S);

            // End of BRDF (Blinn-Phong)

            // Implementation of BRDF "à micro-facettes" CookTorrance

            // Vec3<float> Wh = camera_dir + light_dir;
            // Wh.normalize();

            // float alpha = 0.1f;
            // float F0 = 0.9;

            // float D = 1.0f/(3.14f * pow(alpha, 2) * pow(dot(normal, Wh), 4));
            // D *= exp((pow(dot(normal, Wh), 2) -1)/(pow(alpha, 2) * pow(dot(normal, Wh), 2)));

            // float F = F0 + (1.0f - F0) * pow((1.0f - max(0.0f, dot(light_dir, Wh))), 5);

            // float G = min(
            //               min(1.0f,
            //                   2.0f * dot(normal, Wh) * dot(normal, light_dir) / dot(camera_dir, Wh)
            //                   ),
            //               2.0f * dot(normal, Wh) * dot(normal, camera_dir) / dot(camera_dir, Wh)
            //               );

            // float specular_term = D * F * G;
            // specular_term /= 4.0f * dot(normal, light_dir) * dot(normal, camera_dir);

            // End of BRDF "à micro-facettes" CookTorrance

            // Implementation of BRDF "à micro-facettes" GGX

            Vec3<float> Wh = camera_dir + light_dir;
            Wh.normalize();

            float alpha = 0.1f;
            float F0 = 0.92f;

            float D = pow(alpha, 2) / 3.14f;
            D /= pow(1 + (pow(alpha, 2) - 1) * pow(dot(normal, Wh), 2), 2);

            float F = F0 + (1.0f - F0) * pow((1.0f - max(0.0f, dot(light_dir, Wh))), 5);

            float Gi = 2.0f * dot(normal, light_dir);
            Gi /= dot(normal, light_dir) +
              pow(pow(alpha, 2) + (1.0f - pow(alpha, 2)) * pow(dot(normal, light_dir), 2), 0.5);

            float Go = 2.0f * dot(normal, camera_dir);
            Gi /= dot(normal, camera_dir) +
              pow(pow(alpha, 2) + (1.0f - pow(alpha, 2)) * pow(dot(normal, camera_dir), 2), 0.5);

            float G = Gi * Go;

            float specular_term = D * F * G;
            specular_term /= 4.0f * dot(normal, light_dir) * dot(normal, camera_dir);

            // End of BRDF "à micro-facettes" GGX


            float BRDF = 1.0f * (diffuse_term + specular_term) * (dot(normal, light_dir));
            glColor3f (BRDF, BRDF, BRDF);

            glNormal3f (v.n[0], v.n[1], v.n[2]); // Specifies current normal vertex
            glVertex3f (v.p[0], v.p[1], v.p[2]); // Emit a vertex (one triangle is emitted each time 3 vertices are emitted)
        }
    glEnd (); 
}

void reshape(int w, int h) {
    camera.resize (w, h);
}

void display () {
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    camera.apply (); 
    drawScene ();
    glFlush ();
    glutSwapBuffers (); 
}

void key (unsigned char keyPressed, int x, int y) {
    switch (keyPressed) {
    case 'f':
        if (fullScreen) {
            glutReshapeWindow (camera.getScreenWidth (), camera.getScreenHeight ());
            fullScreen = false;
        } else {
            glutFullScreen ();
            fullScreen = true;
        }      
        break;
    case 'q':
    case 27:
        exit (0);
        break;
    case 'w':
        GLint mode[2];
		glGetIntegerv (GL_POLYGON_MODE, mode);
		glPolygonMode (GL_FRONT_AND_BACK, mode[1] ==  GL_FILL ? GL_LINE : GL_FILL);
        break;
        break;
    default:
        printUsage ();
        break;
    }
}

void mouse (int button, int state, int x, int y) {
    camera.handleMouseClickEvent (button, state, x, y);
}

void motion (int x, int y) {
    camera.handleMouseMoveEvent (x, y);
}

void idle () {
    static float lastTime = glutGet ((GLenum)GLUT_ELAPSED_TIME);
    static unsigned int counter = 0;
    counter++;
    float currentTime = glutGet ((GLenum)GLUT_ELAPSED_TIME);
    if (currentTime - lastTime >= 1000.0f) {
        FPS = counter;
        counter = 0;
        static char winTitle [128];
        unsigned int numOfTriangles = mesh.T.size ();
        sprintf (winTitle, "Number Of Triangles: %d - FPS: %d", numOfTriangles, FPS);
        glutSetWindowTitle (winTitle);
        lastTime = currentTime;
    }
    glutPostRedisplay (); 
}

int main (int argc, char ** argv) {
    if (argc > 2) {
        printUsage ();
        exit (1);
    }
    glutInit (&argc, argv);
    glutInitDisplayMode (GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize (DEFAULT_SCREENWIDTH, DEFAULT_SCREENHEIGHT);
    window = glutCreateWindow (appTitle.c_str ());
    init (argc == 2 ? argv[1] : DEFAULT_MESH_FILE.c_str ());
    glutIdleFunc (idle);
    glutReshapeFunc (reshape);
    glutDisplayFunc (display);
    glutKeyboardFunc (key);
    glutMotionFunc (motion);
    glutMouseFunc (mouse);
    printUsage ();  
    glutMainLoop ();
    return 0;
}

