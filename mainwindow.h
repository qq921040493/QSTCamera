#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include"v4l2.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    

private slots:
    void clickQuitButton(void);
    void clickPhotoButton(void);

private:
    Ui::MainWindow *ui;
    V4L2 v4l2;
};

#endif // MAINWINDOW_H
