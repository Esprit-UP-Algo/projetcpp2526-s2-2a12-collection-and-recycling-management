#include "connection.h"
#include "ourlabib.h"
#include "ui_ourlabib.h"
#include <QDebug>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlTableModel>

void ourlabib::on_btnEmployes_clicked() {
  ui->stackedWidget->setCurrentIndex(2); // Page Employés

  // Interface sans base de données (Mock)
  ui->tableEmployes->setRowCount(0);
  ui->tableEmployes->setColumnCount(7);
  ui->tableEmployes->setHorizontalHeaderLabels(
      {"ID", "Nom", "Prénom", "Email", "Téléphone", "Poste", "Date Embauche"});

  // Exemple de fausse donnée
  ui->tableEmployes->insertRow(0);
  ui->tableEmployes->setItem(0, 0, new QTableWidgetItem("1"));
  ui->tableEmployes->setItem(0, 1, new QTableWidgetItem("Admin"));
  ui->tableEmployes->setItem(0, 2, new QTableWidgetItem("Admin"));
  ui->tableEmployes->setItem(0, 3, new QTableWidgetItem("admin@ourlabib.com"));
  ui->tableEmployes->setItem(0, 4, new QTableWidgetItem("000000000"));
  ui->tableEmployes->setItem(0, 5, new QTableWidgetItem("Gérant"));
  ui->tableEmployes->setItem(0, 6, new QTableWidgetItem("2024-01-01"));
}

void ourlabib::on_btnRetourEmployes_clicked() {
  ui->stackedWidget->setCurrentIndex(1);
}

void ourlabib::on_btnTrierEmployeHeader_clicked() {
  ui->stackedWidget->setCurrentIndex(15);
}

void ourlabib::on_btnStatsEmployeHeader_clicked() {
  ui->stackedWidget->setCurrentIndex(13);
}

void ourlabib::on_btnExportEmployeHeader_clicked() {
  ui->stackedWidget->setCurrentIndex(14);
}

void ourlabib::on_btnStatistiquesEmployes_clicked() {
  ui->stackedWidget->setCurrentIndex(13);
}

void ourlabib::on_btnExportEmploye_clicked() {
  ui->stackedWidget->setCurrentIndex(14);
}

void ourlabib::on_btnTrierEmploye_clicked() {
  ui->stackedWidget->setCurrentIndex(15);
}

void ourlabib::on_btnRetourStatsEmploye_clicked() {
  ui->stackedWidget->setCurrentIndex(2);
}

void ourlabib::on_btnRetourExportEmploye_clicked() {
  ui->stackedWidget->setCurrentIndex(2);
}

void ourlabib::on_btnRetourTrierEmploye_clicked() {
  ui->stackedWidget->setCurrentIndex(2);
}
