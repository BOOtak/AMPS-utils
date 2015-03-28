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

    void setEnabledFileInfo(bool enabled);
    void clearFileInfo();
    void setEnabledDecodeOptions(bool enabled);
private slots:

    void on_decodePushButton_clicked();
    
    void on_openFilePushButton_clicked();

    void on_inputFilePathLineEdit_returnPressed();

    void on_outputFilePathLineEdit_returnPressed();

    void on_saveFilePushButton_clicked();

private:
    Ui::MainWindow *ui;
    QString inputfilePath;
    QString outputFilePath;
};

#endif // MAINWINDOW_H
