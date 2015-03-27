#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QStandardPaths>
#include <iostream>
#include "decoder.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Manchester decoder");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_browsePushButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open WAV file"), QStandardPaths::standardLocations(QStandardPaths::HomeLocation).at(0), tr("WAV files (*.wav)"));
    this->ui->filePathLineEdit->setText(fileName);
    this->on_filePathLineEdit_returnPressed();
}

void MainWindow::on_filePathLineEdit_returnPressed()
{
    QString fileName = this->ui->filePathLineEdit->text();
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }

    WavFile wavFile = WavFile();
    wavFile.readHeader(file);

    this->ui->fileInfoLabel->setText(QString(FILE_INFO_FORMAT).arg(QString::number(wavFile.format().sampleRate()),
                                                                 QString::number(wavFile.format().channelCount()),
                                                                 QString::number(wavFile.format().bytesPerFrame() * 8)));
}

void MainWindow::on_decodePushButton_clicked()
{
    QString fileName = this->ui->filePathLineEdit->text();
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }

    WavFile wavFile = WavFile();
    wavFile.readHeader(file);

    int BUFLEN = 4096 * wavFile.format().bytesPerFrame();
    QByteArray buffer = QByteArray(BUFLEN, 0);

    int total_errors = 0;
    for (;;) {
        qint64 readed = wavFile.readData(file, buffer, wavFile.format());

        QByteArray compressed = QByteArray(BUFLEN / 4, 0);
        for (int i = 0; i < compressed.size(); i++) {
            compressed[i] = buffer.at(i * 4 + 1);
        }

        QVector<int> squares = amplitudesToSquares(compressed);
        QVector<int> rawBits = squaresToRawBits(squares);
        int errors = 0;
        QVector<int> logicalBits = rawBitsToLogicalBits(rawBits, &errors);

        total_errors += errors;
        if (readed < BUFLEN) {
            break;
        }
    }

    std::cout << "Total errors: " << total_errors << std::endl;
    buffer.setRawData(0, BUFLEN);
}
