#include "filter_photoscan.h"
#include <QtGui>
#include <stdlib.h>
#include <QFileDialog>
//#include "meshfilter.h"


using namespace photoscan;

FilterPhotoScan::FilterPhotoScan()
{
    typeList << FP_PHOTOSCAN;

  foreach(FilterIDType tt , types())
      actionList << new QAction(filterName(tt), this);
}

QString     FilterPhotoScan::filterName( FilterIDType id ) const{ return QString("Photoscan");}

QString     FilterPhotoScan::filterInfo( FilterIDType id ) const{ return QString("Imports cameras parameters from Photoscan XML file and Applies these to the rasters shots");}

FilterPhotoScan::FilterClass FilterPhotoScan::getClass(QAction *act){
    return MeshFilterInterface::Camera;
}

int FilterPhotoScan::getRequirements(QAction *act) {
    return MeshModel::MM_VERTCOORD ;
}

void        FilterPhotoScan::initParameterSet( QAction *act,
                                      MeshDocument &md,
                                      RichParameterSet &par ){

//    par.addParam( new RichString( "XML file",
//                                  "",
//                               "XML cameras file ",
//                               "The XML file generated from photoscan when exported cameras" ) );
}

//virtual int         postCondition( QAction *act ) const;

bool        FilterPhotoScan::applyFilter( QAction *act,
                                 MeshDocument &md,
                                 RichParameterSet &par,
                                 vcg::CallBackPos *cb ){

    QString cameraFileName = QFileDialog::getOpenFileName(0,
                                                          tr("Open Photoscan-XML file"), "", tr("XML document (*.xml)"));

    std::string filename = QFile::encodeName(cameraFileName).constData ();

    CameraList cameras;

    PhotoScanCameraReader::readXML(filename.c_str(),cameras);

    if( md.rasterList.size() > cameras.size() ){
        this->errorMessage = "The number of cameras found in the XML file should be equal or superior to the number of opened rasters";
        return false;
    }

    for(int i = 0 ; i < md.rasterList.size() ; i++){
        photoscan::Camera camera;
        try{
            camera = cameras.at(md.rasterList.at(i)->label().toStdString());
        }catch(std::exception ex){
            this->errorMessage = "The raster : " + md.rasterList.at(i)->label() + " does not match any camera in the Photoscan camera file!";
            continue;
        }
        cout << camera.label << std::endl;

        RasterModel *rm = md.rasterList.at(i);

        vcg::Matrix44f transform_mat;
        vcg::Matrix44f rot;
        vcg::Point3f trans;

        for(int i = 0 ; i < 3 ; i++){
            for(int j =0 ; j < 3 ; j++){
                transform_mat[i][j] = camera.rotation[j][i];
            }
            transform_mat[i][3] = camera.translation[i];
            transform_mat[3][i] = 0;

            rot[i][3] = 0;
            rot[3][i] = 0;
        }
        transform_mat[3][3] = 1;
        rot[3][3] = 1;

//        transform_mat = vcg::Inverse(transform_mat);

        for(int i = 0 ; i < 3 ; i++){
            for(int j = 0 ; j < 3 ; j++){
                rot[i][j] = transform_mat[i][j];
            }
            trans[i] = transform_mat[i][3];
        }

//        rot = vcg::Inverse(rot);
        for(int i = 0 ; i < 3 ; i++){
            rot[2][i] = -rot[2][i];
        }

        rm->shot.Extrinsics.SetRot(rot);
        rm->shot.Extrinsics.SetTra(trans);

//        float ratio = 0.05 / trans.Norm();

        rm->shot.Intrinsics.ViewportPx = vcg::Point2i(1308 , 1950 );//camera.sensor.width, camera.sensor.height);
        rm->shot.Intrinsics.CenterPx   = vcg::Point2f((float)1308 / 2, (float)1950 / 2 ); //camera.sensor.cx, camera.sensor.cy);
        rm->shot.Intrinsics.PixelSizeMm[0]= camera.sensor.pix_width;
        rm->shot.Intrinsics.PixelSizeMm[1]= camera.sensor.pix_height;
        rm->shot.Intrinsics.FocalMm = 50 ;//camera.sensor.fx * camera.sensor.pix_width;
//        rm->shot.Intrinsics.k[0] = camera.sensor.k1;
//        rm->shot.Intrinsics.k[1] = camera.sensor.k2;





    }

}

MESHLAB_PLUGIN_NAME_EXPORTER(FilterPhotoScan)
