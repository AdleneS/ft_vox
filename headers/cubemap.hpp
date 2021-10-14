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
    //std::vector<std::string> faces{
    //    "./resources/cubemap/bedu/p_R.jpg",
    //    "./resources/cubemap/bedu/n_L.jpg",
    //    "./resources/cubemap/bedu/p_U.jpg",
    //    "./resources/cubemap/bedu/n_D.jpg",
    //    "./resources/cubemap/bedu/p_F.jpg",
    //    "./resources/cubemap/bedu/n_B.jpg"};
    std::vector<std::string> faces{
        "./resources/cubemap/blue_sky/right.jpg",
        "./resources/cubemap/blue_sky/left.jpg",
        "./resources/cubemap/blue_sky/top.jpg",
        "./resources/cubemap/blue_sky/bottom.jpg",
        "./resources/cubemap/blue_sky/front.jpg",
        "./resources/cubemap/blue_sky/back.jpg"};
    Cubemap();
    ~Cubemap();

    unsigned int loadCubemap(std::vector<std::string> faces);
};