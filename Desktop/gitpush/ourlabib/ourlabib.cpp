#include "ourlabib.h"
#include "ui_ourlabib.h"
#include "connection.h"
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QDebug>

ourlabib::ourlabib(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ourlabib)
{
    ui->setupUi(this);
    // Commencer par la page de login
    ui->stackedWidget->setCurrentIndex(0);
}

ourlabib::~ourlabib()
{
    delete ui;
}

// --- Navigation Logique ---

void ourlabib::on_pushButton_login_clicked() {
    // Récupérer les valeurs de login
    QString username = ui->lineEdit_username->text().trimmed();
    QString password = ui->lineEdit_password->text().trimmed();
    bool isNotRobot = ui->checkBox->isChecked();
    
    // Vérifier que les champs ne sont pas vides
    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez remplir tous les champs!");
        return;
    }
    
    // Vérifier le checkbox
    if (!isNotRobot) {
        QMessageBox::warning(this, "Erreur", "Veuillez cocher 'I'm not a robot'!");
        return;
    }
    
    // Vérifier l'authentification avec la base de données
    Connection* c = Connection::instance();
    if (c->authenticateEmploye(username, password)) {
        // Authentification réussie
        QMessageBox::information(this, "Succès", "Connexion réussie!");
        
        // Effacer les champs
        ui->lineEdit_username->clear();
        ui->lineEdit_password->clear();
        ui->checkBox->setChecked(false);
        
        // Aller au menu principal
        ui->stackedWidget->setCurrentIndex(1);
    } else {
        // Authentification échouée
        QMessageBox::critical(this, "Erreur", "Nom d'utilisateur ou mot de passe incorrect!");
        ui->lineEdit_password->clear();
    }
}

void ourlabib::on_btnLogout_clicked() {
    ui->stackedWidget->setCurrentIndex(0); // Page Login
}

void ourlabib::on_btnEmployes_clicked() {
    ui->stackedWidget->setCurrentIndex(2); // Page Employés
    
    // Charger les données dans la table
    Connection* c = Connection::instance();
    QSqlQuery query = c->getEmployes();
    
    // Configurer et remplir la table
    ui->tableEmployes->setRowCount(0);
    ui->tableEmployes->setColumnCount(7);
    ui->tableEmployes->setHorizontalHeaderLabels(
        {"ID", "Nom", "Prénom", "Email", "Téléphone", "Poste", "Date Embauche"}
    );
    
    int row = 0;
    while (query.next()) {
        ui->tableEmployes->insertRow(row);
        ui->tableEmployes->setItem(row, 0, new QTableWidgetItem(query.value("id_employe").toString()));
        ui->tableEmployes->setItem(row, 1, new QTableWidgetItem(query.value("nom").toString()));
        ui->tableEmployes->setItem(row, 2, new QTableWidgetItem(query.value("prenom").toString()));
        ui->tableEmployes->setItem(row, 3, new QTableWidgetItem(query.value("email").toString()));
        ui->tableEmployes->setItem(row, 4, new QTableWidgetItem(query.value("telephone").toString()));
        ui->tableEmployes->setItem(row, 5, new QTableWidgetItem(query.value("poste").toString()));
        ui->tableEmployes->setItem(row, 6, new QTableWidgetItem(query.value("date_embauche").toString()));
        row++;
    }
}

void ourlabib::on_btnEquipes_clicked() {
    ui->stackedWidget->setCurrentIndex(3); // Page Équipes
    
    Connection* c = Connection::instance();
    
    // Créer un modèle SQL pour Equipes
    QSqlTableModel* model = new QSqlTableModel(this, c->getDatabase());
    model->setTable("Equipes");
    model->select();
    model->setHeaderData(0, Qt::Horizontal, "ID");
    model->setHeaderData(1, Qt::Horizontal, "Nom Équipe");
    model->setHeaderData(2, Qt::Horizontal, "Description");
    model->setHeaderData(3, Qt::Horizontal, "Chef");
    model->setHeaderData(4, Qt::Horizontal, "Membres");
    
    // Assigner le modèle à la vue
    ui->tableViewEQ->setModel(model);
    ui->tableViewEQ->resizeColumnsToContents();
}

void ourlabib::on_btnZones_clicked() {
    ui->stackedWidget->setCurrentIndex(4); // Page Zones
    
    Connection* c = Connection::instance();
    
    // Créer un modèle SQL pour Zones
    QSqlTableModel* model = new QSqlTableModel(this, c->getDatabase());
    model->setTable("Zones");
    model->select();
    model->setHeaderData(0, Qt::Horizontal, "ID");
    model->setHeaderData(1, Qt::Horizontal, "Nom Zone");
    model->setHeaderData(2, Qt::Horizontal, "Localisation");
    model->setHeaderData(3, Qt::Horizontal, "Priorité");
    model->setHeaderData(4, Qt::Horizontal, "Statut");
    
    // Assigner le modèle à la vue
    ui->tableViewZones->setModel(model);
    ui->tableViewZones->resizeColumnsToContents();
}

void ourlabib::on_btnPoubelles_clicked() {
    ui->stackedWidget->setCurrentIndex(5); // Page Poubelles
    
    Connection* c = Connection::instance();
    
    // Créer un modèle SQL pour Poubelles
    QSqlTableModel* model = new QSqlTableModel(this, c->getDatabase());
    model->setTable("Poubelles");
    model->select();
    model->setHeaderData(0, Qt::Horizontal, "ID");
    model->setHeaderData(1, Qt::Horizontal, "Localisation");
    model->setHeaderData(2, Qt::Horizontal, "Niveau");
    model->setHeaderData(3, Qt::Horizontal, "Capacité");
    model->setHeaderData(4, Qt::Horizontal, "Type");
    model->setHeaderData(5, Qt::Horizontal, "Zone");
    
    // Assigner le modèle à la vue
    ui->tableViewPoubelles->setModel(model);
    ui->tableViewPoubelles->resizeColumnsToContents();
}

void ourlabib::on_btnMissions_clicked() {
    ui->stackedWidget->setCurrentIndex(6); // Page Missions
    
    Connection* c = Connection::instance();
    
    // Créer un modèle SQL pour Missions
    QSqlTableModel* model = new QSqlTableModel(this, c->getDatabase());
    model->setTable("Missions");
    model->select();
    model->setHeaderData(0, Qt::Horizontal, "ID");
    model->setHeaderData(1, Qt::Horizontal, "Titre");
    model->setHeaderData(2, Qt::Horizontal, "Description");
    model->setHeaderData(3, Qt::Horizontal, "Date Début");
    model->setHeaderData(4, Qt::Horizontal, "Date Fin");
    model->setHeaderData(5, Qt::Horizontal, "Statut");
    model->setHeaderData(6, Qt::Horizontal, "Équipe");
    model->setHeaderData(7, Qt::Horizontal, "Zone");
    
    // Assigner le modèle à la vue
    ui->tableViewMission->setModel(model);
    ui->tableViewMission->resizeColumnsToContents();
}

// --- Boutons Retour ---

void ourlabib::on_btnRetourEmployes_clicked() {
    ui->stackedWidget->setCurrentIndex(1);
}

void ourlabib::on_btnRetourZones_clicked() {
    ui->stackedWidget->setCurrentIndex(1);
}

void ourlabib::on_btnRetourPoubelles_clicked() {
    ui->stackedWidget->setCurrentIndex(1);
}

void ourlabib::on_btnRetourEquipes_clicked() {
    ui->stackedWidget->setCurrentIndex(1);
}

void ourlabib::on_btnRetourMissions_clicked() {
    ui->stackedWidget->setCurrentIndex(1);
}

void ourlabib::on_btnQuitApp_clicked() {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Quitter", "Voulez-vous vraiment quitter l'application ?",
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        QApplication::quit();
    }
}

// --- Map et Chatbot ---

void ourlabib::on_btnMap_clicked() {
    ui->stackedWidget->setCurrentIndex(7); // Page Map
}

void ourlabib::on_btnChatbot_clicked() {
    lastPageIndex = 4; // Page Zones
    ui->stackedWidget->setCurrentIndex(8); // Page Chatbot
    if (ui->chatHistory->toPlainText().isEmpty()) {
        ui->chatHistory->append("<b>Assistant:</b> Bonjour ! Comment puis-je vous aider avec la gestion du laboratoire OurLabib ?");
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
    if (message.isEmpty()) return;

    ui->chatHistory->append("<b>Vous:</b> " + message);
    ui->chatInput->clear();

    // Réponse simulée du bot
    QString response;
    if (message.contains("zone", Qt::CaseInsensitive)) {
        response = "Vous pouvez gérer les zones dans l'onglet 'Zones'. Vous y trouverez la liste des secteurs et leur priorité.";
    } else if (message.contains("poubelle", Qt::CaseInsensitive)) {
        response = "La gestion des poubelles vous permet de suivre le niveau de remplissage en temps réel.";
    } else {
        response = "Je transmets votre demande concernant : '" + message + "'. Un administrateur vous répondra sous peu.";
    }

    ui->chatHistory->append("<b>Assistant:</b> " + response);
}

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
        QMessageBox::warning(this, "Erreur", "Veuillez remplir tous les champs du mail.");
        return;
    }

    // Simulation d'envoi
    QMessageBox::information(this, "Mail Envoyé", "Le mail automatique a été envoyé avec succès à : " + dest);
    
    // Reset et retour
    ui->mailDestinataire->clear();
    ui->mailObjet->clear();
    ui->mailCorps->clear();
    ui->stackedWidget->setCurrentIndex(6);
}

// --- Equipes Chatbot et SMS ---

void ourlabib::on_btnChatbotEquipes_clicked() {
    lastPageIndex = 3; // Page Equipes
    ui->stackedWidget->setCurrentIndex(8); // Page Chatbot partagée
    if (ui->chatHistory->toPlainText().isEmpty()) {
        ui->chatHistory->append("<b>Assistant:</b> Bonjour ! Comment puis-je vous aider avec la gestion des équipes ?");
    }
}

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
        QMessageBox::warning(this, "Erreur", "Veuillez saisir un numéro et un message.");
        return;
    }

    QMessageBox::information(this, "SMS Envoyé", "SMS envoyé avec succès au : " + num);
    
    ui->smsNumero->clear();
    ui->smsCorps->clear();
    ui->stackedWidget->setCurrentIndex(3);
}

// --- Poubelles Chatbot et QR Code ---

void ourlabib::on_btnChatbotPoubelles_clicked() {
    lastPageIndex = 5; // Page Poubelles
    ui->stackedWidget->setCurrentIndex(8); // Page Chatbot partagée
    if (ui->chatHistory->toPlainText().isEmpty()) {
        ui->chatHistory->append("<b>Assistant:</b> Bonjour ! Comment puis-je vous aider avec la gestion des poubelles ?");
    }
}

void ourlabib::on_btnQRCode_clicked() {
    ui->stackedWidget->setCurrentIndex(12); // Page QR Code
}

void ourlabib::on_btnRetourQRCode_clicked() {
    ui->stackedWidget->setCurrentIndex(5); // Retour page Poubelles
}

// --- Actions Management (Sort, Stats, Export) ---

// Employés
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

// Zones
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

// Équipes
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

// Poubelles
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

// Missions
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

// --- Boutons Action dans les Tables ---

// Équipes
void ourlabib::on_statEQ_clicked() {
    ui->stackedWidget->setCurrentIndex(19);
}

void ourlabib::on_exportEQ_clicked() {
    ui->stackedWidget->setCurrentIndex(20);
}

void ourlabib::on_trieEQ_clicked() {
    ui->stackedWidget->setCurrentIndex(21);
}

// Zones
void ourlabib::on_statzone_clicked() {
    ui->stackedWidget->setCurrentIndex(22);
}

void ourlabib::on_exportzone_clicked() {
    ui->stackedWidget->setCurrentIndex(23);
}

void ourlabib::on_triezone_clicked() {
    ui->stackedWidget->setCurrentIndex(24);
}

// Poubelles
void ourlabib::on_btnStatistiquesPoubelles_clicked() {
    ui->stackedWidget->setCurrentIndex(25);
}

void ourlabib::on_btnExportPoubelle_clicked() {
    ui->stackedWidget->setCurrentIndex(26);
}

void ourlabib::on_btnTrierPoubelle_clicked() {
    ui->stackedWidget->setCurrentIndex(27);
}

// Missions
void ourlabib::on_btnStatistiquesMissions_clicked() {
    ui->stackedWidget->setCurrentIndex(16);
}

void ourlabib::on_btnExportMission_clicked() {
    ui->stackedWidget->setCurrentIndex(17);
}

void ourlabib::on_btnTrierMission_clicked() {
    ui->stackedWidget->setCurrentIndex(18);
}
