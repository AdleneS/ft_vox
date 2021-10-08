#include "../gl3w/include/GL/gl3w.h"
#include "stb_image.hpp"
#include "cube.hpp"
#include <iostream>
#include <vector>

class Cubemap
{
private:
    /* data */
public:
    unsigned int textureID;
    int width;
    int height;
    int nrChannels;
    unsigned char *data;
    unsigned int cubemapTexture;
    unsigned int skyboxVAO;
    unsigned int skyboxVBO;
    std::vector<std::string> faces{
        "../resources/cubemap/right.jpg",
        "../resources/cubemap/left.jpg",
        "../resources/cubemap/top.jpg",
        "../resources/cubemap/bottom.jpg",
        "../resources/cubemap/front.jpg",
        "../resources/cubemap/back.jpg"};
    Cubemap();
    ~Cubemap();

    unsigned int loadCubemap(std::vector<std::string> faces);
};

//inline Cubemap::Cubemap()
//{
//}
//
//inline Cubemap::~Cubemap()
//{
//}
