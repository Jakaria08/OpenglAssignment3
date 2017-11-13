#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cfloat>
#include <cmath>
#include <vector>
#include <Eigen/Dense>


#ifndef __APPLE__
#  include <GL/glew.h>
#endif
#include <GL/freeglut.h>

#ifndef TEXTURE_H
#define TEXTURE_H

// taken from text book chapter 12
struct BitMapFile
{
   int sizeX;
   int sizeY;
   unsigned char *data;
};


class texture
{

public:

  unsigned int tex[1];
  void setup();
  void loadExternalTextures(BitMapFile * image);
  BitMapFile *getbmp(std::string filename);


  void bindTex()
  {
    // Activate texture object.
    glBindTexture(GL_TEXTURE_2D, tex[0]);
  }

};

#endif
