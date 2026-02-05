#include "gestionequipe.h"
#include "ui_gestionequipe.h"

GestionEquipe::GestionEquipe(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GestionEquipe)
{
    ui->setupUi(this);
}

GestionEquipe::~GestionEquipe()
{
    delete ui;
}

void GestionEquipe::on_retourne_2_clicked()
{
    ui->tabWidgetEQ->setCurrentIndex(0);
}


void GestionEquipe::on_quitEQ_clicked()
{
    this->close();
}


void GestionEquipe::on_chatEQ_clicked()
{
    ui->tabWidgetEQ->setCurrentIndex(1);
}

