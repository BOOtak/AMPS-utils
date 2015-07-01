#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

int updateCRC(int data, int crc) {
    int temp = data;
    for (int i = 0; i < 12; i++) {
        crc = crc << 1;
        if ((temp & 0x800) != 0) {
            crc = crc | 0x1;
            temp = temp & 0x7ff;
        }
        temp = temp << 1;
        if ((crc & 0x1000) != 0) {
            crc = crc & 0xfff;
            crc = crc ^ 0x539;
        }
    }
    return crc;
}

QString calculateCRC(QString source) {
    QString data = QString("").fill('0', 36 - source.size()).append(source);
    int f = data.mid(0, 12).toInt(NULL, 2);
    int s = data.mid(12, 12).toInt(NULL, 2);
    int t = data.mid(24, 12).toInt(NULL, 2);

//    std::cout << data.toUtf8().constData() << std::endl;
//    std::cout << f << " " << s << " " << t << std::endl;

//    crc = 0
//    crc = update_crc(first_word, crc)
//    crc = update_crc(second_word, crc)
//    crc = update_crc(third_word, crc)
//    # print("{0:b}".format(crc))
//    for i in range(0, 0xfff):
//        new_crc = update_crc(i, crc)
//        if new_crc == 0:
//            print("{0:b}".format(i))

    int crc = 0;
    crc = updateCRC(f, crc);
    crc = updateCRC(s, crc);
    crc = updateCRC(t, crc);

    for (int i = 0; i < 0xfff; i++) {
        int new_crc = updateCRC(i, crc);
        if (new_crc == 0) {
            return QString::number(i, 2);
        }
    }
    return "";
}

QString MainWindow::validateInput(const QString raw, int* removedSymbols) {
    QString formatted_text = raw;
    QRegExp rexp = QRegExp("[^0-1]");
    *removedSymbols = formatted_text.count(rexp);
    formatted_text.remove(rexp);

    if (formatted_text.size() > MAX_DATA_LENGTH) {
        formatted_text.resize(MAX_DATA_LENGTH);
    }

    return formatted_text;
}

void MainWindow::on_lineEdit_textChanged(const QString &arg1)
{
    int curPos = this->ui->lineEdit->cursorPosition();
    int count = 0;
    QString formattedString = validateInput(arg1, &count);

    this->ui->lineEdit->setText(formattedString);
    this->ui->lineEdit->setCursorPosition(curPos - count);
    QString crc = calculateCRC(formattedString);
    QString formattedCRC = QString("").fill('0', 12 - crc.size()).append(crc);
    this->ui->lineEdit_2->setText(formattedCRC);
}
