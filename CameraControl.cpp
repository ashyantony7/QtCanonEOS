#include "CameraControl.h"

// Initialize SDK
bool CameraControlInit(){

    EdsError err = EDS_ERR_OK;

    //Initialize SDK
    err = EdsInitializeSDK();
    if(err == EDS_ERR_OK) {
        return true;
    } else{
        return false;
    }
}

void CameraControlEnd(){
    EdsTerminateSDK();
}

Camera::Camera(){
    EdsCameraListRef cameraList = NULL;
    EdsUInt32 count = 0;

    // Get camera list
    mErr = EdsGetCameraList(&cameraList);

    // Get number of cameras
    if(mErr == EDS_ERR_OK)
    {
        mErr = EdsGetChildCount(cameraList, &count);
        if(count == 0)
        {
            mErr = EDS_ERR_DEVICE_NOT_FOUND;
            errorInfo = "Device not found";
        }
    }

    // Get first camera retrieved
    if(mErr == EDS_ERR_OK)
    {
        mErr = EdsGetChildAtIndex(cameraList , 0 , &mCameraRef);
        mErr = EdsGetDeviceInfo(mCameraRef, &mInfo);

        mDeviceDes = mInfo.szDeviceDescription;
        cameraInfo = std::string(mDeviceDes);
    }

    // Release camera list
    if(cameraList != NULL)
    {
        EdsRelease(cameraList);
        cameraList = NULL;
    }
}

bool Camera::okay(){
    return mErr==EDS_ERR_OK;
}

void Camera::takePicture(){

    // take picture
    if (mErr == EDS_ERR_OK){
        mErr = EdsSendCommand(mCameraRef, kEdsCameraCommand_PressShutterButton, kEdsCameraCommand_ShutterButton_Completely_NonAF);
        mErr = EdsSendCommand(mCameraRef, kEdsCameraCommand_PressShutterButton, kEdsCameraCommand_ShutterButton_OFF);
    }
    if(mErr!=EDS_ERR_OK){
        errorInfo = "Couldn't take picture";
        mErr=EDS_ERR_OK;
    }
}

void Camera::openSession(){
      mErr = EdsOpenSession(mCameraRef);
      if (mErr!= EDS_ERR_OK){
          errorInfo = "Couldn't open session";
      }
}

void Camera::closeSession(){
    mErr = EdsCloseSession(mCameraRef);
    if (mErr!= EDS_ERR_OK){
        errorInfo = "Couldn't close session";
    }
}

void Camera::createStream(){

    // create memory stream
    mErr = EdsCreateMemoryStream(0, &mStream);

    // create Evf image ref
    mErr = EdsCreateEvfImageRef(mStream, &mImageRef);

    if (mErr!= EDS_ERR_OK){
        errorInfo = "stream not created";
    }
}

void Camera::releaseStream(){

    // release memory stream
    if(mStream != NULL)
    {
        mErr = EdsRelease(mStream);
        mStream = NULL;
    }

    // release Evf image
    if(mImageRef != NULL)
    {
        EdsRelease(mImageRef);
        mImageRef = NULL;
    }
}

void Camera::startLive(){

    // open session if not open
    if (!mSessionOpen){
        openSession();
        mSessionOpen = true;
    }

    // Get the output device for the live view image
    EdsUInt32 device;
    mErr = EdsGetPropertyData(mCameraRef, kEdsPropID_Evf_OutputDevice, 0 , sizeof(device), &device);

    // PC live view starts by setting the PC as the output device for the live view image.
    if(mErr == EDS_ERR_OK)
    {
        device |= kEdsEvfOutputDevice_PC;
        mErr = EdsSetPropertyData(mCameraRef, kEdsPropID_Evf_OutputDevice, 0 , sizeof(device), &device);
    }

    QThread::msleep(1500);

    if (mErr!=EDS_ERR_OK){
        errorInfo = "property not set";
    }else{
        // create stream if not present
        if (!mStreamOpen){
            createStream();
            mStreamOpen = true;
        }

        live = true;
    }
}

QImage Camera::grabLiveFrame(){

    // dowload image
    mErr = EdsDownloadEvfImage(mCameraRef, mImageRef);

    EdsUInt64 length;
    const char* image_data;
    EdsGetLength(mStream, &length);

    // get pointer to stream and convert it to byte array
    EdsGetPointer(mStream, (EdsVoid**)&image_data);
    QByteArray bytes = QByteArray(image_data, length);

    if (mErr!=EDS_ERR_OK){
        errorInfo = "Couldn't download image";
        QImage temp;
        return temp;
    }else{
        // create an image and return it
        return QImage::fromData(bytes, "JPG");
    }

}

void Camera::endLive(){

    // Get the output device for the live view image
    EdsUInt32 device;

    mErr = EdsGetPropertyData(mCameraRef, kEdsPropID_Evf_OutputDevice, 0 , sizeof(device), &device);

    // PC live view ends if the PC is disconnected from the live view image output device.
    if(mErr == EDS_ERR_OK)
    {
        device &= ~kEdsEvfOutputDevice_PC;
        mErr = EdsSetPropertyData(mCameraRef, kEdsPropID_Evf_OutputDevice, 0 , sizeof(device), &device);
    }

    if (mErr!=EDS_ERR_OK){
        errorInfo = "property not set";
    }else{
        // release stream if not present
        if (mStreamOpen){
            releaseStream();
            mStreamOpen = false;
        }

        live = false;
    }
}

void Camera::focusUpdate(int f){

    switch(f){
        case 1:
            mErr = EdsSendCommand(mCameraRef, kEdsCameraCommand_DriveLensEvf, kEdsEvfDriveLens_Far1);
            break;
        case 2:
            mErr = EdsSendCommand(mCameraRef, kEdsCameraCommand_DriveLensEvf, kEdsEvfDriveLens_Far2);
            break;
        case 3:
            mErr = EdsSendCommand(mCameraRef, kEdsCameraCommand_DriveLensEvf, kEdsEvfDriveLens_Far3);
            break;
        case -1:
            mErr = EdsSendCommand(mCameraRef, kEdsCameraCommand_DriveLensEvf, kEdsEvfDriveLens_Near1);
            break;
        case -2:
            mErr = EdsSendCommand(mCameraRef, kEdsCameraCommand_DriveLensEvf, kEdsEvfDriveLens_Near2);
            break;
        case -3:
            mErr = EdsSendCommand(mCameraRef, kEdsCameraCommand_DriveLensEvf, kEdsEvfDriveLens_Near3);
            break;
    }
}


Camera::~Camera(){

    // close live if open
    if (live){
        endLive();
    }

    // close stream if open
    if (mStreamOpen){
        releaseStream();
    }

    // close session if open
    if (mSessionOpen){
        closeSession();
    }
}
