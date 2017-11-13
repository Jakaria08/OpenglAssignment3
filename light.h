#ifndef LIGHT_H
#define LIGHT_H


class lighting
{

public:

  void setup_lighting()
  {
    // Turn on OpenGL lighting.
    glEnable(GL_LIGHTING);

    // Light property vectors.
    float lightAmb[] = {0.0, 0.0, 0.0, 1.0};
    float lightDifAndSpec[] = {1.0, 1.0, 1.0, 1.0};
    float lightPos[] = {0.0, 1.5, 3.0, 1.0};
    float globAmb[] = {0.2, 0.2, 0.2, 1.0};

    // Light properties.
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDifAndSpec);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightDifAndSpec);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    glEnable(GL_LIGHT0); // Enable particular light source.
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globAmb); // Global ambient light.
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE); // Enable two-sided lighting.
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE); // Enable local viewpoint.


  }


};

#endif
