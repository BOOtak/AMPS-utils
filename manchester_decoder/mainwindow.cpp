#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QProgressDialog>
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

void MainWindow::on_openFilePushButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open WAV file"), QStandardPaths::standardLocations(QStandardPaths::HomeLocation).at(0), tr("WAV files (*.wav)"));
    this->ui->inputFilePathLineEdit->setText(fileName);
    this->on_inputFilePathLineEdit_returnPressed();
}

void MainWindow::on_inputFilePathLineEdit_returnPressed()
{
    QString fileName = this->ui->inputFilePathLineEdit->text();
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly)) {
        this->setEnabledFileInfo(false);
        this->clearFileInfo();
        this->setEnabledDecodeOptions(false);
        this->inputfilePath = "";
        return;
    }

    this->inputfilePath = fileName;

    WavFile wavFile = WavFile();
    wavFile.readHeader(file);

    QString time;

    this->setEnabledFileInfo(true);
    this->ui->sampleRateLine->setText(QString::number(wavFile.format().sampleRate()));
    this->ui->channelCountLine->setText(QString::number(wavFile.format().channelCount()));
    this->ui->bitsPerFrameLine->setText(QString::number(wavFile.format().bytesPerFrame() * 8));
    this->ui->lengthLine->setText(
                time.sprintf(
                    "%lli:%02lli",
                    wavFile.dataLength() / wavFile.format().bytesForDuration(1000000) / 60,
                    wavFile.dataLength() / wavFile.format().bytesForDuration(1000000) % 60));

}

void MainWindow::clearFileInfo() {
    this->ui->sampleRateLine->clear();
    this->ui->channelCountLine->clear();
    this->ui->bitsPerFrameLine->clear();
    this->ui->lengthLine->clear();
}

void MainWindow::setEnabledFileInfo(bool enabled) {
    this->ui->sampleRateLabel->setEnabled(enabled);
    this->ui->channelCountLabel->setEnabled(enabled);
    this->ui->bitsPerFrameLabel->setEnabled(enabled);
    this->ui->lengthLabel->setEnabled(enabled);
    this->ui->outputFilePathLineEdit->setEnabled(enabled);
    this->ui->saveFilePushButton->setEnabled(enabled);
    this->ui->selectOutputFileLabel->setEnabled(enabled);
}

void MainWindow::setEnabledDecodeOptions(bool enabled) {
    this->ui->decodePushButton->setEnabled(enabled);
    this->ui->toInvert->setEnabled(enabled);
}

void MainWindow::on_decodePushButton_clicked()
{
//    this->ui->resultTextEdit->clear();
    QString inputFileName = this->inputfilePath;
    QFile inputFile(inputFileName);

    if (!inputFile.open(QIODevice::ReadOnly)) {
        return;
    }

    QString outputFileName = this->outputFilePath;
    QFile outputFile(outputFileName);

    if (!outputFile.open(QIODevice::WriteOnly)) {
        return;
    }

    WavFile wavFile = WavFile();
    wavFile.readHeader(inputFile);

    int BUFLEN = 65536 * wavFile.format().bytesPerFrame();
    QByteArray buffer = QByteArray(BUFLEN, 0);

    int total_errors = 0;

    QByteArray partial = QByteArray();
    int offset = 0;
    int oddBit = -1;

    QString result = QString();
    QProgressDialog dialog("Decoding...", "Abort", 0, wavFile.dataLength(), this);
    dialog.setWindowModality(Qt::WindowModal);

    qint64 total_readed = 0;

    for (;;) {
        if(dialog.wasCanceled()) {
            break;
        }

        qint64 readed = wavFile.readData(inputFile, buffer, wavFile.format());
        total_readed += readed;

        dialog.setValue(total_readed);

        QByteArray compressed = QByteArray();
        compressed.insert(0, partial);
        partial.truncate(0);
        for (int i = 0; i < readed; i += wavFile.format().bytesPerFrame()) {
            if (wavFile.format().byteOrder() == QAudioFormat::LittleEndian) {
                compressed.append((buffer.at(i) << 8) + buffer.at(i + 1));
            } else {
                compressed.append((buffer.at(i + 1) << 8) + buffer.at(i));
            }
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

        bool inversed = this->ui->toInvert->isChecked();
        int errors = 0;
        QVector<char> logicalBits = rawBitsToLogicalBits(rawBits, inversed, &errors, &oddBit);

        outputFile.write(logicalBits.data(), logicalBits.size());

        this->setUpdatesEnabled(false);
//        this->ui->resultTextEdit->insertPlainText(QString(logicalBits.data(), logicalBits.size()));
        setUpdatesEnabled(true);

        total_errors += errors;
        if (readed < BUFLEN) {
            break;
        }
        QApplication::processEvents(0, 0);
    }

    std::cout << "here" << std::endl;

    inputFile.close();
    outputFile.close();

    std::cout << "Total errors: " << total_errors << std::endl;
    buffer.setRawData(0, BUFLEN);
}

void MainWindow::on_outputFilePathLineEdit_returnPressed()
{
    QString fileName = this->ui->outputFilePathLineEdit->text();
    QFile file(fileName);

    if (!file.open(QIODevice::WriteOnly)) {
        this->setEnabledDecodeOptions(false);
        this->outputFilePath = "";
        return;
    }
    this->outputFilePath = fileName;
    this->setEnabledDecodeOptions(true);
}

void MainWindow::on_saveFilePushButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save output file"), QStandardPaths::standardLocations(QStandardPaths::HomeLocation).at(0), tr("Text files (*.txt)"));
    this->ui->outputFilePathLineEdit->setText(fileName);
    this->on_outputFilePathLineEdit_returnPressed();
}
