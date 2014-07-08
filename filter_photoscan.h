#ifndef FILTER_PHOTOSCAN_H
#define FILTER_PHOTOSCAN_H

#include <QObject>
#include <QStringList>
#include <QList>
#include <common/interfaces.h>
#include "photoscancamerareader.h"

class FilterPhotoScan : public QObject, public MeshFilterInterface
{

    Q_OBJECT
    MESHLAB_PLUGIN_IID_EXPORTER(MESH_FILTER_INTERFACE_IID)
    Q_INTERFACES(MeshFilterInterface)

public:

        enum {FP_PHOTOSCAN};
    FilterPhotoScan();

    virtual QString pluginName(void) const { return "Photoscan"; }

    virtual QString     filterName( FilterIDType id ) const;
    virtual QString     filterInfo( FilterIDType id ) const;

    virtual FilterClass getClass( QAction *act );

    virtual void        initParameterSet( QAction *act,
                                          MeshDocument &md,
                                          RichParameterSet &par );

    virtual int         getRequirements( QAction *act );
    //virtual int         postCondition( QAction *act ) const;

    virtual bool        applyFilter( QAction *act,
                                     MeshDocument &md,
                                     RichParameterSet &par,
                                     vcg::CallBackPos *cb );

};

#endif
