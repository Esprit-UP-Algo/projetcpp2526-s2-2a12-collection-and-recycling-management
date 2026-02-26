#include "connection.h"
#include "ourlabib.h"
#include "ui_ourlabib.h"
#include <QDebug>
#include <QMessageBox>
#include <QStandardItem>
#include <QStandardItemModel>

// --- Map et Chatbot (General) ---

void ourlabib::on_btnMap_clicked() {
    ui->stackedWidget->setCurrentIndex(7); // Page Map
}

void ourlabib::on_btnChatbot_clicked() {
    lastPageIndex = 4;                     // Page Zones
    ui->stackedWidget->setCurrentIndex(8); // Page Chatbot
    if (ui->chatHistory->toPlainText().isEmpty()) {
        ui->chatHistory->append("<b>Assistant:</b> Bonjour ! Comment puis-je vous "
                                "aider avec la gestion du laboratoire OurLabib ?");
    }
}

void ourlabib::on_btnRetourMap_clicked() {
    ui->stackedWidget->setCurrentIndex(4); // Retour page Zones
}

void ourlabib::on_btnRetourChatbot_clicked() {
    ui->stackedWidget->setCurrentIndex(lastPageIndex); // Retour dynamique
}

void ourlabib::on_btnSendMessage_clicked() {
    QString message = ui->chatInput->text().trimmed();
    if (message.isEmpty())
        return;

    ui->chatHistory->append("<b>Vous:</b> " + message);
    ui->chatInput->clear();

    // Réponse simulée du bot
    QString response;
    if (message.contains("zone", Qt::CaseInsensitive)) {
        response = "Vous pouvez gérer les zones dans l'onglet 'Zones'. Vous y "
                   "trouverez la liste des secteurs et leur priorité.";
    } else if (message.contains("poubelle", Qt::CaseInsensitive)) {
        response = "La gestion des poubelles vous permet de suivre le niveau de "
                   "remplissage en temps réel.";
    } else {
        response = "Je transmets votre demande concernant : '" + message +
                   "'. Un administrateur vous répondra sous peu.";
    }

    ui->chatHistory->append("<b>Assistant:</b> " + response);
}


void ourlabib::on_btnZones_clicked() {
  ui->stackedWidget->setCurrentIndex(4); // Page Zones

  // Interface sans base de données (Mock)
  QStandardItemModel *model = new QStandardItemModel(this);
  model->setColumnCount(5);
  model->setHeaderData(0, Qt::Horizontal, "ID");
  model->setHeaderData(1, Qt::Horizontal, "Nom Zone");
  model->setHeaderData(2, Qt::Horizontal, "Localisation");
  model->setHeaderData(3, Qt::Horizontal, "Priorité");
  model->setHeaderData(4, Qt::Horizontal, "Statut");

  // Ajouter des données fictives
  QList<QStandardItem *> rowData;
  rowData << new QStandardItem("1") << new QStandardItem("Secteur Nord")
          << new QStandardItem("Bâtiment A") << new QStandardItem("Haute")
          << new QStandardItem("Active");
  model->appendRow(rowData);

  ui->tableViewZones->setModel(model);
  ui->tableViewZones->resizeColumnsToContents();
}

void ourlabib::on_btnRetourZones_clicked() {
  ui->stackedWidget->setCurrentIndex(1);
}

void ourlabib::on_btnTrierZonesHeader_clicked() {
  ui->stackedWidget->setCurrentIndex(24);
}

void ourlabib::on_btnStatsZonesHeader_clicked() {
  ui->stackedWidget->setCurrentIndex(22);
}

void ourlabib::on_btnExportZonesHeader_clicked() {
  ui->stackedWidget->setCurrentIndex(23);
}

void ourlabib::on_btnRetourStatsZone_clicked() {
  ui->stackedWidget->setCurrentIndex(4);
}

void ourlabib::on_btnRetourExportZone_clicked() {
  ui->stackedWidget->setCurrentIndex(4);
}

void ourlabib::on_btnRetourTrierZone_clicked() {
  ui->stackedWidget->setCurrentIndex(4);
}

void ourlabib::on_statzone_clicked() { ui->stackedWidget->setCurrentIndex(22); }

void ourlabib::on_exportzone_clicked() {
  ui->stackedWidget->setCurrentIndex(23);
}

void ourlabib::on_triezone_clicked() { ui->stackedWidget->setCurrentIndex(24); }
