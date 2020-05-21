#ifndef CAMERACONTROL_H
#define CAMERACONTROL_H

#include "EDSDK.h"
#include "EDSDKErrors.h"
#include "EDSDKTypes.h"
#include <string>
#include <QImage>
#include <QByteArray>
#include <QThread>

bool CameraControlInit();

void CameraControlEnd();

class Camera{
public:
    Camera();

    bool okay();

    void takePicture();

    // live operations
    void startLive();
    void endLive();

    QImage grabLiveFrame();

    // focus operations
    void focusUpdate(int f);

    ~Camera();

    std::string cameraInfo;
    std::string errorInfo;
    bool live = false;

private:

    // session operations
    void openSession();
    void closeSession();

    // stream operations
    void createStream();
    void releaseStream();

    bool mSessionOpen=false;
    bool mStreamOpen = false;

    EdsCameraRef mCameraRef;
    EdsDeviceInfo mInfo;
    EdsChar* mDeviceDes;
    EdsStreamRef mStream = NULL;
    EdsEvfImageRef mImageRef = NULL;
    EdsError mErr=EDS_ERR_OK;
};

#endif // CAMERACONTROL_H
