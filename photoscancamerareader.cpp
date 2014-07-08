#include "photoscancamerareader.h"
#include <opencv/cv.h>
#include <fstream>

using namespace photoscan;

PhotoScanCameraReader::PhotoScanCameraReader()
{
}

int PhotoScanCameraReader::readXML(const char *fileName, CameraList &cameraList)
{

    cout << "Reading camera parameters...\n";

    map<string, map<string,float> > sensors;
    map<string, cv::Point2i> cameras_size;

    ifstream camFile(fileName);
    if(camFile.is_open()){

        camFile.seekg(0, camFile.end);
        int camFileLength = camFile.tellg();
        camFile.seekg(0, camFile.beg);

        char* camFileBuff = new char[camFileLength + 1];

        camFile.read(camFileBuff, camFileLength);

        camFileBuff[camFileLength] = 0;

        xml_document<> doc;
        doc.parse<0>(camFileBuff);

        xml_node<> *root_node = doc.first_node("document");
        if(!root_node){
            cerr << "Error on reading XML cameras file" << endl;
            return -1;
        }

        root_node = root_node->first_node("chunk");
        if(!root_node){
            cerr << "Error on reading XML cameras file" << endl;
            return -1;
        }

        xml_node<> *sensors_node = root_node->first_node("sensors");
        if(!sensors_node){
            cerr << "Error on reading XML cameras file...(sensors node missing)" << endl;
            return -1;
        }

        xml_node<> *cameras_node = root_node->first_node("cameras");
        if(!cameras_node){
            cerr << "Error on reading XML cameras file...(cameras node missing)" << endl;
            return -1;
        }

        xml_node<> *node = sensors_node->first_node("sensor");

        while(node){

            float fx, fy, cx, cy, k1, k2, k3, focal_l, w, h, pixel_w, pixel_h;
            xml_attribute<> *att_id = node->first_attribute("id");

            xml_node<> *prop_node = node->first_node("property");
            while(prop_node){

                char* name = prop_node->first_attribute("name")->value();
                char* value = prop_node->first_attribute("value")->value();

                if(strcmp(name, "pixel_width") == 0){

                       sscanf(value, "%f", &pixel_w);

                }else if(strcmp(name, "pixel_height") == 0){

                       sscanf(value, "%f", &pixel_h);

                }else if(strcmp(name, "focal_length") == 0){

                       sscanf(value, "%f", &focal_l);

                }

                prop_node = prop_node->next_sibling("property");
            }

            xml_node<> *calib_node = node->first_node("calibration");
            if(!calib_node){
                cerr << "Error on reading XML cameras file" << endl;
                //return -1;
                node = node->next_sibling("sensor");
                continue;
            }

            xml_node<> *param_node = calib_node->first_node("resolution");
            if(param_node){
                sscanf(param_node->first_attribute("width")->value(), "%f", &w);
                sscanf(param_node->first_attribute("height")->value(), "%f", &h);
            }else{
                cerr << "Error on reading XML cameras file...(resolution missing)" << endl;
                return -1;
            }

            param_node = calib_node->first_node("fx");
            if(param_node){
                sscanf(param_node->value(), "%f", &fx);
            }else{
                cerr << "Error on reading XML cameras file...(fx missing)" << endl;
                return -1;
            }

            param_node = calib_node->first_node("fy");
            if(param_node){
                sscanf(param_node->value(), "%f", &fy);
            }else{
                cerr << "Error on reading XML cameras file...(fy missing)" << endl;
                return -1;
            }

            param_node = calib_node->first_node("cx");
            if(param_node){
                sscanf(param_node->value(), "%f", &cx);
            }else{
                cerr << "Error on reading XML cameras file...(cx missing)" << endl;
                return -1;
            }

            param_node = calib_node->first_node("cy");
            if(param_node){
                sscanf(param_node->value(), "%f", &cy);
            }else{
                cerr << "Error on reading XML cameras file...(cy missing)" << endl;
                return -1;
            }

            param_node = calib_node->first_node("k1");
            if(param_node){
                sscanf(param_node->value(), "%f", &k1);
            }else{
                cerr << "Error on reading XML cameras file...(k1 missing)" << endl;
                return -1;
            }

            param_node = calib_node->first_node("k2");
            if(param_node){
                sscanf(param_node->value(), "%f", &k2);
            }else{
                cerr << "Error on reading XML cameras file...(k2 missing)" << endl;
                return -1;
            }

            param_node = calib_node->first_node("k3");
            if(param_node){
                sscanf(param_node->value(), "%f", &k3);
            }else{
                cerr << "Error on reading XML cameras file...(k3 missing)" << endl;
                return -1;
            }

            map<string, float> entry;
            entry.insert(pair<string, float>("fx",fx)); entry.insert(pair<string, float>("fy",fy));
            entry.insert(pair<string, float>("cx",cx)); entry.insert(pair<string, float>("cy",cy));
            entry.insert(pair<string, float>("k1",k1));
            entry.insert(pair<string, float>("k2",k2));
            entry.insert(pair<string, float>("k3",k3));
            entry.insert(pair<string, float>("w",w)); entry.insert(pair<string, float>("h",h));
            entry.insert(pair<string, float>("pixel_w",pixel_w));
            entry.insert(pair<string, float>("pixel_h",pixel_h));
            entry.insert(pair<string, float>("focal_l",focal_l));

            string sensor_id (att_id->value());
            sensors.insert(pair<string, map<string, float > > (sensor_id, entry));

            node = node->next_sibling("sensor");
        }

        node = cameras_node->first_node("camera");

        while(node){
            float id;
            char label[100], sensor_id[100];
            float t0, t1, t2,
                    r00, r01, r02,
                    r10, r11, r12,
                    r20, r21, r22;

            Camera camera;

            xml_attribute<> *att_id = node->first_attribute("id");
            xml_attribute<> *att_label = node->first_attribute("label");
            xml_attribute<> *att_sensor_id = node->first_attribute("sensor_id");

            sscanf(att_id->value(), "%f", &id);
            sscanf(att_label->value() , " %s", label);
            sscanf(att_sensor_id->value(), "%s", sensor_id);

            camera.label = label;

            string sensor_id_str(sensor_id);
            map<string, float> sensor = sensors.at(sensor_id_str);

            try{camera.sensor.cx = sensor.at("cx");}catch(exception e){camera.sensor.cx = PARAM_UNDIFINED ;cerr << "camera " << label << " : cx not present" << endl;}
            try{camera.sensor.cy = sensor.at("cy");}catch(exception e){camera.sensor.cy = PARAM_UNDIFINED ;cerr << "camera " << label << " : cy not present" << endl;}
            try{camera.sensor.fx = sensor.at("fx");}catch(exception e){camera.sensor.fx = PARAM_UNDIFINED ;cerr << "camera " << label << " : fx not present" << endl;}
            try{camera.sensor.fy = sensor.at("fy");}catch(exception e){camera.sensor.fy = PARAM_UNDIFINED ;cerr << "camera " << label << " : fy not present" << endl;}
            try{camera.sensor.k1 = sensor.at("k1");}catch(exception e){camera.sensor.k1 = PARAM_UNDIFINED ;cerr << "camera " << label << " : k1 not present" << endl;}
            try{camera.sensor.k2 = sensor.at("k2");}catch(exception e){camera.sensor.k2 = PARAM_UNDIFINED ;cerr << "camera " << label << " : k2 not present" << endl;}
            try{camera.sensor.k3 = sensor.at("k3");}catch(exception e){camera.sensor.k3 = PARAM_UNDIFINED ;cerr << "camera " << label << " : k3 not present" << endl;}
            try{camera.sensor.width = sensor.at("w");}catch(exception e){camera.sensor.width = PARAM_UNDIFINED ;cerr << "camera " << label << " : width not present" << endl;}
            try{camera.sensor.height = sensor.at("h");}catch(exception e){camera.sensor.height = PARAM_UNDIFINED; cerr << "camera " << label << " : height not present" << endl;}
            try{camera.sensor.pix_width = sensor.at("pixel_w");}catch(exception e){camera.sensor.pix_width = PARAM_UNDIFINED; cerr << "camera " << label << " : pixel width not present" << endl;}
            try{camera.sensor.pix_height = sensor.at("pixel_h");}catch(exception e){camera.sensor.pix_height = PARAM_UNDIFINED; cerr << "camera " << label << " : pixle height not present" << endl;}
            try{camera.sensor.focal_length = sensor.at("focal_l");}catch(exception e){camera.sensor.focal_length = PARAM_UNDIFINED; cerr << "camera " << label << " : focal length not present" << endl;}

            xml_node<> *trans_node = node->first_node("transform");
            if(trans_node){
                sscanf(trans_node->value(), "%f %f %f %f %f %f %f %f %f %f %f %f ",
                       &r00, &r01, &r02, &t0,
                       &r10, &r11, &r12, &t1,
                       &r20, &r21, &r22, &t2);
            }else{
                cerr << "Error on reading XML cameras file" << endl;
                return -1;
            }

            camera.rotation[0][0] = r00;
            camera.rotation[0][1] = r01;
            camera.rotation[0][2] = r02;
            camera.rotation[1][0] = r10;
            camera.rotation[1][1] = r11;
            camera.rotation[1][2] = r12;
            camera.rotation[2][0] = r20;
            camera.rotation[2][1] = r21;
            camera.rotation[2][2] = r22;

            camera.translation[0] = t0;
            camera.translation[1] = t1;
            camera.translation[2] = t2;


            cameraList.insert(pair<string, Camera >(label, camera));

            node = node->next_sibling("camera");

        }

    }else{
        return -1;
    }

    return 1;

}
