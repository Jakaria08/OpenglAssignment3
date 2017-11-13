
// motionViewer <motion.bvh>

#ifndef __APPLE__
#  include <GL/glew.h>
#endif
#include <GL/freeglut.h>

#include "skeleton.h"
#include "motion.h"
#include "timer.h"
#include "camera.h"
#include "light.h"
#include "texture.h"
#include "attachment.h"
#include "animation.h"


int main(int, char**);
void setup(char *fileName, char *motion, char *attachmnt);
void drawScene(void);
void idle(void);
void resize(int, int);
void keyInput(unsigned char, int, int);
void toggle();

/* global variables */
camera cam;
skeleton skel;
motion mot;
timer tim;
mesh obj;
lighting light;
texture tex;
animation anm;
bool interpolate(true);
int toggle1 = 1;
// taken from attachment viewer
attachment att(&obj, &skel);
int highlightBone(0);
bool highlightMode = false;
Vector3f initialPosition(0.0, 0.0, -1.0);
float boneRadii[] = {
  0.5, 2.0, 1.0, 0.5, 0.25, 0.0,
  0.5, 2.0, 1.0, 0.5, 0.25, 0.0,
  0.5, 2.0, 1.5, 1.5, 0.5, 0.5, 0.5,
  2.0, 1.0, 1.0, 0.5, 0.5, 0.5, 0.25, 0.0, 0.0,
  2.0, 1.0, 1.0, 0.5, 0.5, 0.5, 0.25, 0.0, 0.0
};

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitContextVersion(2, 1);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("personViewer");
	glutDisplayFunc(drawScene);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyInput);
	glutIdleFunc(idle);
	if (argc != 4) {
		cerr << "Usage: personViewer <meshfile.obj> <motionfile.bvh> <attachfile.att>" << endl;
		exit(1);
	}
	setup(argv[1],argv[2],argv[3]);
	glutMainLoop();
	return 0;
}


void setup(char *fileName, char *motion, char *attachmnt)
{
    //obj mesh
    obj.readObjFile(fileName);
    obj.normalize();
    obj.createVertexArray();
    obj.glCreateDisplayList();

    // Local storage for bmp image data.
    BitMapFile *image[1];
    // Load the image.
    image[0] = tex.getbmp("./a3files/mesh/skin.bmp");
    tex.loadExternalTextures(image[0]);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	// skeleton
	skel.readBvhSkeleton(motion);
	skel.determineRadius();
	skel.recoverBones();


	// motion
	mot.readBvhMotion(motion, skel);
	mot.determineRange(skel.translationIndices);

	// attachment
	att.distancesVisibility(boneRadii);
	att.readW(attachmnt);

	// timer
	tim.initialize(true, mot.sequence.size(), mot.defaultGapTime);

	// camera
	cam.initialize(persp, -0.1, 0.1, -0.1, 0.1, 0.1, 150.0);
	cam.positionMotion(mot.range, skel.radius);
    //lighting
    light.setup_lighting();
    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// camera view volume
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	cam.glVolume();

	// gl
	glClearColor(0.0, 0.0, 0.0, 0.0);
}


void drawScene(void)
{

    //taken from text book chapter 11
    // Material property vectors.
    float matAmbAndDif1[] = {0.0, 0.0, 0.0, 1.0};
    float matSpec[] = {1.0, 1.0, 1.0, 1.0};
    float matShine[] = {50.0};

    // Material properties
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, matAmbAndDif1);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpec);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, matShine);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// camera
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	cam.glPosition();

	// skeleton
	skel.glColor();
	skel.interpolatePose(&mot, tim.loopFrac, interpolate);
	skel.glDraw();

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	tex.bindTex();

	//drawing
	obj.glColor();
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
	obj.glCallDisplayList();

	glutSwapBuffers();
}


void idle(void)
{
	tim.glIdle();
}


void resize(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
}


void keyInput(unsigned char key, int x, int y)
{
	switch(key)
	{
		case 'q': exit(0);            break; // quit
		case 'w': // write bvh file
			skel.writeBvhSkeleton("skeletonout.bvh");
			mot.writeBvhMotion("skeletonout.bvh");
			obj.writeObjFile("meshout.obj");
			att.writeMatrixXfSparse(att.W, "attachout.att");
			break;
        case 's':
        toggle();
        break;
		case 'x': // reset
			tim.initialize(true, mot.sequence.size(), mot.defaultGapTime);
			cam.initialize(persp, -0.2, 0.2, -0.2, 0.2, 0.2, 100.0);
			cam.positionMotion(mot.range, skel.radius);
			break;
		default: break;
	}
	cam.keyInput(key); // camera controls
	tim.keyInput(key); // timer controls

	glutPostRedisplay();
}

void toggle()
{
    if(toggle1 == 5)
    {
        toggle1 = 1;
    }

    if(toggle1 == 1)
    {
        glDisable(GL_TEXTURE_2D);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else if(toggle1 == 2)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glShadeModel(GL_FLAT);
    }
    else if(toggle1 == 3)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glShadeModel(GL_SMOOTH);
    }

    else if(toggle1 == 4)
    {
         glEnable(GL_TEXTURE_2D);
    }

    toggle1++;

}

