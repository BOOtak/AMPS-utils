#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "wavfile.h"

namespace Ui {
class MainWindow;
}

#define FILE_INFO_FORMAT "Rate: %1, Channels: %2, %3bit per frame"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_browsePushButton_clicked();

    void on_filePathLineEdit_returnPressed();

    void on_decodePushButton_clicked();
    
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
