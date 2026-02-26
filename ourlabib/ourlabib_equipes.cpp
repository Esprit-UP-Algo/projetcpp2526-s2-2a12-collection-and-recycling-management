#include "connection.h"
#include "ourlabib.h"
#include "ui_ourlabib.h"
#include <QDebug>
#include <QMessageBox>
#include <QStandardItem>
#include <QStandardItemModel>

// --- SMS (Equipes) ---

void ourlabib::on_btnSMS_clicked() {
    ui->stackedWidget->setCurrentIndex(11); // Page SMS
}

void ourlabib::on_btnRetourSMS_clicked() {
    ui->stackedWidget->setCurrentIndex(3); // Retour page Equipes
}

void ourlabib::on_btnEnvoyerSMS_clicked() {
    QString num = ui->smsNumero->text().trimmed();
    QString msg = ui->smsCorps->toPlainText().trimmed();

    if (num.isEmpty() || msg.isEmpty()) {
        QMessageBox::warning(this, "Erreur",
                             "Veuillez saisir un numéro et un message.");
        return;
    }

    QMessageBox::information(this, "SMS Envoyé",
                             "SMS envoyé avec succès au : " + num);

    ui->smsNumero->clear();
    ui->smsCorps->clear();
    ui->stackedWidget->setCurrentIndex(3);
}

void ourlabib::on_btnEquipes_clicked() {
  ui->stackedWidget->setCurrentIndex(3); // Page Équipes

  // Interface sans base de données (Mock)
  QStandardItemModel *model = new QStandardItemModel(this);
  model->setColumnCount(5);
  model->setHeaderData(0, Qt::Horizontal, "ID");
  model->setHeaderData(1, Qt::Horizontal, "Nom Équipe");
  model->setHeaderData(2, Qt::Horizontal, "Description");
  model->setHeaderData(3, Qt::Horizontal, "Chef");
  model->setHeaderData(4, Qt::Horizontal, "Membres");

  // Ajouter des données fictives
  QList<QStandardItem *> rowData;
  rowData << new QStandardItem("1") << new QStandardItem("Équipe Alpha")
          << new QStandardItem("Nettoyage Nuit") << new QStandardItem("Bob")
          << new QStandardItem("5");
  model->appendRow(rowData);

  ui->tableViewEQ->setModel(model);
  ui->tableViewEQ->resizeColumnsToContents();
}

void ourlabib::on_btnRetourEquipes_clicked() {
  ui->stackedWidget->setCurrentIndex(1);
}

void ourlabib::on_btnChatbotEquipes_clicked() {
  lastPageIndex = 3;                     // Page Equipes
  ui->stackedWidget->setCurrentIndex(8); // Page Chatbot partagée
  if (ui->chatHistory->toPlainText().isEmpty()) {
    ui->chatHistory->append("<b>Assistant:</b> Bonjour ! Comment puis-je vous "
                            "aider avec la gestion des équipes ?");
  }
}

void ourlabib::on_btnTrierEquipeHeader_clicked() {
  ui->stackedWidget->setCurrentIndex(21);
}

void ourlabib::on_btnStatsEquipeHeader_clicked() {
  ui->stackedWidget->setCurrentIndex(19);
}

void ourlabib::on_btnExportEquipeHeader_clicked() {
  ui->stackedWidget->setCurrentIndex(20);
}

void ourlabib::on_btnRetourStatsEquipe_clicked() {
  ui->stackedWidget->setCurrentIndex(3);
}

void ourlabib::on_btnRetourExportEquipe_clicked() {
  ui->stackedWidget->setCurrentIndex(3);
}

void ourlabib::on_btnRetourTrierEquipe_clicked() {
  ui->stackedWidget->setCurrentIndex(3);
}

void ourlabib::on_statEQ_clicked() { ui->stackedWidget->setCurrentIndex(19); }

void ourlabib::on_exportEQ_clicked() { ui->stackedWidget->setCurrentIndex(20); }

void ourlabib::on_trieEQ_clicked() { ui->stackedWidget->setCurrentIndex(21); }
