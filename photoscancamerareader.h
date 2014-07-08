#ifndef PHOTOSCANCAMERAREADER_H
#define PHOTOSCANCAMERAREADER_H

#include <iostream>
#include <map>
#include <vector>
#include "rapidxml.hpp"

#define PARAM_UNDIFINED FLT_MAX;

using namespace std;
using namespace rapidxml;

namespace photoscan{

typedef struct Sensor{
    float fx, fy, cx, cy, k1, k2, k3, focal_length, pix_width, pix_height,width, height;
} Sensor;

typedef struct Camera{
    std::string label;
    Sensor sensor;
    std::vector< std::vector<float> > rotation;
    std::vector<float> translation;

    Camera(){
        rotation.resize(3);
        for(int i = 0  ; i < 3 ; i++) rotation[i].resize(3);
        translation.resize(3);
    }
} Camera;

typedef map<string, Camera > CameraList;

class PhotoScanCameraReader
{
public:
    PhotoScanCameraReader();

    static int readXML(const char* fileName, CameraList& cameras);
};

}


#endif // PHOTOSCANCAMERAREADER_H
