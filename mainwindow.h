#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QFileDialog>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QImage>
#include <QPixmap>
#include <QCloseEvent>
#include <QMessageBox>
#include <QtConcurrent/QtConcurrent>
#include "CameraControl.h"
#include <iostream>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_Connect_clicked();

    void on_pushButton_clicked();

    void on_Live_clicked();

    void on_focusPlus_clicked();

    void on_focusMinus_clicked();

    void on_focusPlus3_clicked();

    void on_focusPlus2_clicked();

    void on_focusMinus2_clicked();

    void on_focusMinus3_clicked();

private:

    Ui::MainWindow *ui;

    QFuture<void> liveDispThread;

    // camera object
    Camera* cam;

    // camera values
    bool SDK_loaded = false;
    bool* live_on = new bool(false);

    // camera controls
    int* focus= new int(0);
    bool* shoot= new bool(false);
};
#endif // MAINWINDOW_H
