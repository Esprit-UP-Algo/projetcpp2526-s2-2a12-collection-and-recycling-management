#include "ourlabib.h"
#include "connection.h"
#include "ui_ourlabib.h"
#include <QApplication>
#include <QDebug>
#include <QMessageBox>
ourlabib::ourlabib(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::ourlabib) {
  ui->setupUi(this);
  // Commencer par la page de login
  ui->stackedWidget->setCurrentIndex(0);
}

ourlabib::~ourlabib() { delete ui; }









