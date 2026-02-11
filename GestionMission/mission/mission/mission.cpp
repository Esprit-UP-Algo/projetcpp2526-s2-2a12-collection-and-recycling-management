#include "mission.h"
#include "ui_mission.h"

GestionMission::GestionMission(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GestionMission)
{
    ui->setupUi(this);
    connect(ui->btnRetour, &QPushButton::clicked, this, &GestionMission::on_btnRetour_clicked);
    connect(ui->btnQuitter, &QPushButton::clicked, this, &GestionMission::on_btnQuitter_clicked);
}

GestionMission::~GestionMission()
{
    delete ui;
}

void GestionMission::on_btnRetour_clicked()
{
    this->close();
}

void GestionMission::on_btnQuitter_clicked()
{
    this->close();
}
