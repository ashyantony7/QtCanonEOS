#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // check for SDK loaded
    SDK_loaded = CameraControlInit();
    if (!SDK_loaded) {
        QMessageBox msgBox;
        msgBox.setText("CameraSDK could not be initialized");
        msgBox.exec();
    }else{
     ui->currentStatus->setText("No cameras connected");
    }

    // disable other buttons
    ui->pushButton->setEnabled(false);
    ui->Live->setEnabled(false);
    ui->focusPlus->setEnabled(false);
    ui->focusPlus2->setEnabled(false);
    ui->focusPlus3->setEnabled(false);
    ui->focusMinus->setEnabled(false);
    ui->focusMinus2->setEnabled(false);
    ui->focusMinus3->setEnabled(false);
}

MainWindow::~MainWindow()
{
    if (*live_on){
        *live_on = false;
        QThread::currentThread()->msleep(1500);
    }
    delete live_on;
    delete focus;
    delete shoot;
    CameraControlEnd();
    delete ui;
    delete cam;
}

void MainWindow::on_Connect_clicked()
{
    // if SDK is loaded
    if (SDK_loaded){
        cam = new Camera();

        // camera found
        if (cam->okay()){

               // show the connected camera details
               QString text = QString::fromUtf8(cam->cameraInfo.data(), cam->cameraInfo.size());
               ui->currentStatus->setText(text);

               // enable live view button
               ui->Live->setEnabled(true);
               ui->Live->setStyleSheet("color: rgb(237, 237, 237);");
         }
         else{
                ui->currentStatus->setText("No devices found");
         }
    }else{
        QMessageBox msgBox;
        msgBox.setText("CameraSDK could not be initialized");
        msgBox.exec();
    }
}

void MainWindow::on_pushButton_clicked()
{
   *shoot=true;
}


void enableFocusButtons(Ui::MainWindow* ui){
    ui->focusPlus->setEnabled(true);
    ui->focusPlus->setStyleSheet("color: rgb(237, 237, 237);");
    ui->focusPlus2->setEnabled(true);
    ui->focusPlus2->setStyleSheet("color: rgb(237, 237, 237);");
    ui->focusPlus3->setEnabled(true);
    ui->focusPlus3->setStyleSheet("color: rgb(237, 237, 237);");
    ui->focusMinus->setEnabled(true);
    ui->focusMinus->setStyleSheet("color: rgb(237, 237, 237);");
    ui->focusMinus2->setEnabled(true);
    ui->focusMinus2->setStyleSheet("color: rgb(237, 237, 237);");
    ui->focusMinus3->setEnabled(true);
    ui->focusMinus3->setStyleSheet("color: rgb(237, 237, 237);");
}

void disableFocusButtons(Ui::MainWindow* ui){
    ui->focusPlus->setEnabled(false);
    ui->focusPlus->setStyleSheet("color: rgb(100, 100, 100);");
    ui->focusPlus2->setEnabled(false);
    ui->focusPlus2->setStyleSheet("color: rgb(100, 100, 100);");
    ui->focusPlus3->setEnabled(false);
    ui->focusPlus3->setStyleSheet("color: rgb(100, 100, 100);");
    ui->focusMinus->setEnabled(false);
    ui->focusMinus->setStyleSheet("color: rgb(100, 100, 100);");
    ui->focusMinus2->setEnabled(false);
    ui->focusMinus2->setStyleSheet("color: rgb(100, 100, 100);");
    ui->focusMinus3->setEnabled(false);
    ui->focusMinus3->setStyleSheet("color: rgb(100, 100, 100);");
}

void showLive(Camera* cam, Ui::MainWindow* ui, bool* live, int* focus, bool* shoot){

    QImage temp_img;

    while(*live){

        if(cam->okay()){

            // grab live frame
            temp_img = cam->grabLiveFrame();
            ui->imageview->setPixmap(QPixmap::fromImage(temp_img));

            // check for change in focus
            if (*focus!=0){
                cam->focusUpdate(*focus);
                *focus=0;
            }

            // check for photo request
            if(*shoot){
                cam->takePicture();
                *shoot=false;
                QThread::msleep(1000);
                cam->endLive();
                QThread::msleep(1000);
                cam->startLive();
            }

        }else{

            //exit live view with error message
            *live=false;
            break;
        }
    }

    // exit live view
    cam->endLive();
}

void MainWindow::on_Live_clicked()
{
    // start live if not live
    if (!(*live_on)){

        // initiate live view session in cam
        if (!cam->live){
            cam->startLive();
        }

        // start a new thread to perfrom live operations
        if (cam->okay()){
            *live_on = true;
            liveDispThread = QtConcurrent::run(showLive, cam, ui, live_on, focus, shoot);
            ui->Live->setText("Stop Live");
        }

        // enable focus buttons
        enableFocusButtons(ui);
        // enable shoot button
        ui->pushButton->setEnabled(true);
        ui->pushButton->setStyleSheet("color: rgb(237, 237, 237);");

    }else{

        // stop live view
        *live_on = false;
        ui->Live->setText("Live View");

        // disable focus buttons
        disableFocusButtons(ui);

        // disable shoot button
        ui->pushButton->setEnabled(false);
        ui->pushButton->setStyleSheet("color: rgb(100, 100, 100);");
    }
}

void MainWindow::on_focusPlus_clicked()
{
    *focus =1;
}

void MainWindow::on_focusMinus_clicked()
{
    *focus=-1;
}

void MainWindow::on_focusPlus3_clicked()
{
    *focus=3;
}

void MainWindow::on_focusPlus2_clicked()
{
    *focus=2;
}

void MainWindow::on_focusMinus2_clicked()
{
    *focus=-2;
}

void MainWindow::on_focusMinus3_clicked()
{
    *focus=-3;
}
