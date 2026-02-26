#include "connection.h"
#include "ourlabib.h"
#include "ui_ourlabib.h"
#include <QDebug>
#include <QMessageBox>
#include <QStandardItem>
#include <QStandardItemModel>

// --- Calendrier et Mail ---

void ourlabib::on_btnCalendrier_clicked() {
    ui->stackedWidget->setCurrentIndex(9); // Page Calendrier
}

void ourlabib::on_btnMail_clicked() {
    ui->stackedWidget->setCurrentIndex(10); // Page Mail
}

void ourlabib::on_btnRetourCalendrier_clicked() {
    ui->stackedWidget->setCurrentIndex(6); // Retour page Missions
}

void ourlabib::on_btnRetourMail_clicked() {
    ui->stackedWidget->setCurrentIndex(6); // Retour page Missions
}

void ourlabib::on_btnEnvoyerMail_clicked() {
    QString dest = ui->mailDestinataire->text().trimmed();
    QString sujet = ui->mailObjet->text().trimmed();
    QString corps = ui->mailCorps->toPlainText().trimmed();

    if (dest.isEmpty() || sujet.isEmpty() || corps.isEmpty()) {
        QMessageBox::warning(this, "Erreur",
                             "Veuillez remplir tous les champs du mail.");
        return;
    }

    // Simulation d'envoi
    QMessageBox::information(this, "Mail Envoyé",
                             "Le mail automatique a été envoyé avec succès à : " +
                                 dest);

    // Reset et retour
    ui->mailDestinataire->clear();
    ui->mailObjet->clear();
    ui->mailCorps->clear();
    ui->stackedWidget->setCurrentIndex(6);
}

void ourlabib::on_btnMissions_clicked() {
  ui->stackedWidget->setCurrentIndex(6); // Page Missions

  // Interface sans base de données (Mock)
  QStandardItemModel *model = new QStandardItemModel(this);
  model->setColumnCount(8);
  model->setHeaderData(0, Qt::Horizontal, "ID");
  model->setHeaderData(1, Qt::Horizontal, "Titre");
  model->setHeaderData(2, Qt::Horizontal, "Description");
  model->setHeaderData(3, Qt::Horizontal, "Date Début");
  model->setHeaderData(4, Qt::Horizontal, "Date Fin");
  model->setHeaderData(5, Qt::Horizontal, "Statut");
  model->setHeaderData(6, Qt::Horizontal, "Équipe");
  model->setHeaderData(7, Qt::Horizontal, "Zone");

  // Ajouter des données fictives
  QList<QStandardItem *> rowData;
  rowData << new QStandardItem("1") << new QStandardItem("Nettoyage Printemps")
          << new QStandardItem("Nettoyage complet du secteur Nord")
          << new QStandardItem("2024-03-01") << new QStandardItem("2024-03-05")
          << new QStandardItem("En cours") << new QStandardItem("Équipe Alpha")
          << new QStandardItem("Secteur Nord");
  model->appendRow(rowData);

  ui->tableViewMission->setModel(model);
  ui->tableViewMission->resizeColumnsToContents();
}

void ourlabib::on_btnRetourMissions_clicked() {
  ui->stackedWidget->setCurrentIndex(1);
}

void ourlabib::on_btnTrierMissionsHeader_clicked() {
  ui->stackedWidget->setCurrentIndex(18);
}

void ourlabib::on_btnStatsMissionsHeader_clicked() {
  ui->stackedWidget->setCurrentIndex(16);
}

void ourlabib::on_btnExportMissionsHeader_clicked() {
  ui->stackedWidget->setCurrentIndex(17);
}

void ourlabib::on_btnRetourStatsMission_clicked() {
  ui->stackedWidget->setCurrentIndex(6);
}

void ourlabib::on_btnRetourExportMission_clicked() {
  ui->stackedWidget->setCurrentIndex(6);
}

void ourlabib::on_btnRetourTrierMission_clicked() {
  ui->stackedWidget->setCurrentIndex(6);
}

void ourlabib::on_btnStatistiquesMissions_clicked() {
  ui->stackedWidget->setCurrentIndex(16);
}

void ourlabib::on_btnExportMission_clicked() {
  ui->stackedWidget->setCurrentIndex(17);
}

void ourlabib::on_btnTrierMission_clicked() {
  ui->stackedWidget->setCurrentIndex(18);
}
