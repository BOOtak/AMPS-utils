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

    QByteArray partial = QByteArray();
    int offset = 0;
    int oddBit = -1;

    for (;;) {
        qint64 readed = wavFile.readData(file, buffer, wavFile.format());

        QByteArray compressed = QByteArray();
        compressed.insert(0, partial);
        partial.truncate(0);
        for (int i = 0; i < readed; i += 4) {
            compressed.append(buffer.at(i + 1));
        }

        QVector<int> squares = amplitudesToSquares(compressed, &offset);
        if (offset > 0) {
            partial = partial.insert(0, compressed.right(offset));
            offset = 0;
        }

        QVector<int> rawBits = QVector<int>();
        if (oddBit != -1) {
            rawBits.append(oddBit);
            oddBit = -1;
        }
        rawBits = rawBits + squaresToRawBits(squares);

        int errors = 0;
        QVector<int> logicalBits = rawBitsToLogicalBits(rawBits, &errors, &oddBit);

        total_errors += errors;
        if (readed < BUFLEN) {
            break;
        }
    }

    std::cout << "Total errors: " << total_errors << std::endl;
    buffer.setRawData(0, BUFLEN);
}
