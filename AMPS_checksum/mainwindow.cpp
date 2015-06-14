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

void MainWindow::on_lineEdit_textChanged(const QString &arg1)
{
    int curPos = this->ui->lineEdit->cursorPosition();
    QString formatted_text = arg1;
    QRegExp rexp = QRegExp("[^0-1]");
    int count = formatted_text.count(rexp);
    formatted_text.remove(rexp);
    curPos -= count;

    if (formatted_text.size() > MAX_DATA_LENGTH) {
        formatted_text.resize(MAX_DATA_LENGTH);
    }
    this->ui->lineEdit->setText(formatted_text);
    this->ui->lineEdit->setCursorPosition(curPos);

    std::cout << arg1.toStdString() << std::endl;
}

void calculateCRC(QString source) {

}
