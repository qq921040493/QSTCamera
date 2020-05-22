#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->quitButton, SIGNAL(clicked()), this, SLOT(clickQuitButton()));
    connect(ui->photoButton, SIGNAL(clicked()), this, SLOT(clickPhotoButton()));

}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::clickQuitButton(void)
{
    this->close();
}

void MainWindow::clickPhotoButton(void)
{
    v4l2.V4l2_Init();
    v4l2.V4l2_Malloc();
    v4l2.V4l2_capturing();
    v4l2.Take_photo();
    v4l2.V4l2_Close();
}
