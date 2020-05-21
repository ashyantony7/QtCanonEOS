## Remote Photography Qt App for Canon EOS Cameras (C++)

This is a basic working app with good performance live view capture, manual focus control and photo capture which you can use to develop a more complex application. To use this app you need to 

1. Get the Canaon EOS SDK. Steps available [here](https://developers.canon-europe.com/s/article/How-to-get-access-camera)

2. Have Qt version 4 or greater 

3. Edit the LIBS, INCLUDEPATH, and DEPENDPATH in the CameraApp.pro file to your local path to the SDK library in Windows/Mac

The 64 bit version of the EOS SDK does not work properly for me, but the 32 bit version worked fine. There are other limitations such as the need to wait for a certain duration for the camera to process requests. Current solution is to use threaded delays.
