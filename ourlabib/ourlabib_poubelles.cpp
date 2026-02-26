#include "connection.h"
#include "ourlabib.h"
#include "ui_ourlabib.h"
#include <QDebug>
#include <QMessageBox>
#include <QStandardItem>
#include <QStandardItemModel>


// --- QR Code (Poubelles) ---

void ourlabib::on_btnQRCode_clicked() {
    ui->stackedWidget->setCurrentIndex(12); // Page QR Code
}

void ourlabib::on_btnRetourQRCode_clicked() {
    ui->stackedWidget->setCurrentIndex(5); // Retour page Poubelles
}

void ourlabib::on_btnPoubelles_clicked() {
  ui->stackedWidget->setCurrentIndex(5); // Page Poubelles

  // Interface sans base de données (Mock)
  QStandardItemModel *model = new QStandardItemModel(this);
  model->setColumnCount(6);
  model->setHeaderData(0, Qt::Horizontal, "ID");
  model->setHeaderData(1, Qt::Horizontal, "Localisation");
  model->setHeaderData(2, Qt::Horizontal, "Niveau");
  model->setHeaderData(3, Qt::Horizontal, "Capacité");
  model->setHeaderData(4, Qt::Horizontal, "Type");
  model->setHeaderData(5, Qt::Horizontal, "Zone");

  // Ajouter des données fictives
  QList<QStandardItem *> rowData;
  rowData << new QStandardItem("1") << new QStandardItem("Bâtiment A")
          << new QStandardItem("50%") << new QStandardItem("100L")
          << new QStandardItem("Recyclage")
          << new QStandardItem("Secteur Nord");
  model->appendRow(rowData);

  ui->tableViewPoubelles->setModel(model);
  ui->tableViewPoubelles->resizeColumnsToContents();
}

void ourlabib::on_btnRetourPoubelles_clicked() {
  ui->stackedWidget->setCurrentIndex(1);
}

void ourlabib::on_btnChatbotPoubelles_clicked() {
  lastPageIndex = 5;                     // Page Poubelles
  ui->stackedWidget->setCurrentIndex(8); // Page Chatbot partagée
  if (ui->chatHistory->toPlainText().isEmpty()) {
    ui->chatHistory->append("<b>Assistant:</b> Bonjour ! Comment puis-je vous "
                            "aider avec la gestion des poubelles ?");
  }
}

void ourlabib::on_btnTrierPoubellesHeader_clicked() {
  ui->stackedWidget->setCurrentIndex(27);
}

void ourlabib::on_btnStatsPoubellesHeader_clicked() {
  ui->stackedWidget->setCurrentIndex(25);
}

void ourlabib::on_btnExportPoubellesHeader_clicked() {
  ui->stackedWidget->setCurrentIndex(26);
}

void ourlabib::on_btnRetourStatsPoubelle_clicked() {
  ui->stackedWidget->setCurrentIndex(5);
}

void ourlabib::on_btnRetourExportPoubelle_clicked() {
  ui->stackedWidget->setCurrentIndex(5);
}

void ourlabib::on_btnRetourTrierPoubelle_clicked() {
  ui->stackedWidget->setCurrentIndex(5);
}

void ourlabib::on_btnStatistiquesPoubelles_clicked() {
  ui->stackedWidget->setCurrentIndex(25);
}

void ourlabib::on_btnExportPoubelle_clicked() {
  ui->stackedWidget->setCurrentIndex(26);
}

void ourlabib::on_btnTrierPoubelle_clicked() {
  ui->stackedWidget->setCurrentIndex(27);
}
