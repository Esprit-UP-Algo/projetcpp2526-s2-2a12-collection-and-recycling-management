#include "ourlabib.h"
#include "gestion_equipe.h"
#include "ui_ourlabib.h"
#include "connection.h"

#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QDebug>
#include <QString>
#include <QDate>
#include <QShowEvent>
#include <QAbstractItemModel>
#include <QFileDialog>
#include <QPrinter>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextTable>
#include <QTextTableFormat>
#include <QTextCharFormat>

#include <QSqlQuery>
#include <QMessageBox>
#include <QList>
#include <QStringList>
#include <QColor>
#include <QMargins>
#include <QFont>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QLegend>
#include <QTimer>


ourlabib::ourlabib(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ourlabib)
{
    ui->setupUi(this);
    // Commencer par la page de login
    ui->stackedWidget->setCurrentIndex(0);
    QTimer::singleShot(100, this, [this]() {
        ui->tableViewZones->setModel(z.afficher());
    });
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
    /*
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
    ui->tableViewEQ->resizeColumnsToContents();*/
}

void ourlabib::on_btnZones_clicked() {
    ui->stackedWidget->setCurrentIndex(4); // Page Zones

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
        ui->chatHistory->append("<b>Assistant:</b> Bonjour ! Comment puis-je vous aider ?");
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
/*****************************Crud Gestion Equipe****************************/

void ourlabib::on_enrgstEQ_clicked()
{
    int id_Eqpe=ui->idEQ->text().toInt();
    QString nom=ui->nomEQ->text();
    QString zone=ui->zoneCEQ->text();
    QString chefEq= ui->chefEQ->text();
    QDate dateM= ui->dateEQ->date();
    int nb=ui->nbMembresEQ->text().toInt();
    QString statut =ui->statutEQ->currentText();
    Gestion_equipe equipe(id_Eqpe,nom,zone,chefEq,nb,dateM,statut);
    bool test=equipe.ajouter();
    if (test) {
        ui->tableViewEQ->setModel(equipe.afficher());
        ui->zoneCEQ->clear();
        ui->idEQ->clear();
        ui->nomEQ->clear();
        ui->chefEQ->clear();
        ui->dateEQ->clear();
        ui->nbMembresEQ->clear();

        QMessageBox::information(this, "Succès", "Équipe ajoutée avec succès ");
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de l'ajout ");
    }

}
void ourlabib::showEvent(QShowEvent *event) {
    QMainWindow::showEvent(event);
    ui->tableViewEQ->setModel(equipe.afficher());
}

void ourlabib::on_supEQ_clicked()
{
    int id=0;
    QModelIndex index=ui->tableViewEQ->selectionModel()->currentIndex();

    if(index.isValid()){
        id = ui->tableViewEQ->model()->data(ui->tableViewEQ->model()->index(index.row(), 0)).toInt();
    }
    else if(!ui->idEQ->text().isEmpty()){
        id = ui->idEQ->text().toInt();
    }

    if(id==0) return;

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(
        nullptr,
        QObject::tr("Confirmer"),
        QObject::tr("Êtes-vous sûr(e) de vouloir supprimer l'équipe ID %1 ?").arg(id),
        QMessageBox::Yes | QMessageBox::No
        );

    if(reply == QMessageBox::Yes){
        if(equipe.supprimer(id)){
            ui->zoneCEQ->clear();
            ui->idEQ->clear();
            ui->nomEQ->clear();
            ui->chefEQ->clear();
            ui->dateEQ->clear();
            ui->statutEQ->setCurrentIndex(0);
            ui->nbMembresEQ->clear();
            ui->tableViewEQ->setModel(equipe.afficher());
            QMessageBox::information(nullptr, QObject::tr("Succès"), QObject::tr("Équipe supprimée avec succès."));
        }
        else{
            QMessageBox::critical(nullptr, QObject::tr("Erreur"), QObject::tr("Échec de la suppression."));
        }
    }
}


void ourlabib::on_modifEQ_clicked()
{
    if (ui->idEQ->text().isEmpty()) {
        QMessageBox::warning(this, QObject::tr("Erreur"),
                             QObject::tr("Veuillez sélectionner un enregistrement."),
                             QMessageBox::Ok);
        return;
    }

    int id = ui->idEQ->text().toInt();
    QString nom = ui->nomEQ->text();
    QString zone = ui->zoneCEQ->text();
    QString chef = ui->chefEQ->text();
    int nb = ui->nbMembresEQ->text().toInt();
    QDate date = ui->dateEQ->date();
    QString statut = ui->statutEQ->currentText();

    if (equipe.modifier(id, nom, zone, chef, nb, date, statut))
    {
        ui->tableViewEQ->setModel(equipe.afficher());
        ui->idEQ->clear();
        ui->nomEQ->clear();
        ui->zoneCEQ->clear();
        ui->chefEQ->clear();
        ui->nbMembresEQ->clear();
        ui->statutEQ->setCurrentIndex(0);

        QMessageBox::information(this, QObject::tr("Succès"),
                                 QObject::tr("Modification réussie."),
                                 QMessageBox::Ok);
    }
    else
    {
        QMessageBox::critical(this, QObject::tr("Erreur"),
                              QObject::tr("Échec de la modification."),
                              QMessageBox::Ok);
    }
}

void ourlabib::on_tableViewEQ_clicked(const QModelIndex &index)
{
    if (!index.isValid()) return;

    QAbstractItemModel* model = ui->tableViewEQ->model();

    int id = model->data(model->index(index.row(), 0)).toInt();
    QString nom = model->data(model->index(index.row(), 1)).toString();
    QString zone = model->data(model->index(index.row(), 2)).toString();
    QString chef = model->data(model->index(index.row(), 3)).toString();
    int nb = model->data(model->index(index.row(), 4)).toInt();
    QDate date = model->data(model->index(index.row(), 5)).toDate();

    QString statut = model->data(model->index(index.row(), 6)).toString();

    ui->idEQ->setText(QString::number(id));
    ui->nomEQ->setText(nom);
    ui->zoneCEQ->setText(zone);
    ui->chefEQ->setText(chef);
    ui->nbMembresEQ->setText(QString::number(nb));
    if (date.isValid()) {
        ui->dateEQ->setDate(date);
    } else {
        ui->dateEQ->setDate(QDate::fromString(model->data(model->index(index.row(), 5)).toString(), "yyyy-MM-dd"));
    }
    int idx = ui->statutEQ->findText(statut);
    if (idx != -1) {
        ui->statutEQ->setCurrentIndex(idx);
    } else {

        ui->statutEQ->addItem(statut);
        ui->statutEQ->setCurrentText(statut);
    }
}

void ourlabib::on_anulEQ_clicked()
{
    ui->zoneCEQ->clear();
    ui->idEQ->clear();
    ui->nomEQ->clear();
    ui->chefEQ->clear();
    ui->dateEQ->clear();
    ui->statutEQ->setCurrentIndex(0);
    ui->nbMembresEQ->clear();
}
void ourlabib::on_trieEQ_clicked() {
    ui->tableViewEQ->setModel(equipe.trier());
}

void ourlabib::on_refreshEQ_clicked()
{
    ui->tableViewEQ->setModel(equipe.afficher());
}


void ourlabib::on_rechercheEQ_clicked()
{
    if (ui->rechEQ->text().isEmpty()) {
        QMessageBox::warning(this, tr("Attention"),tr("Veuillez entrer l'ID de l'équipe à rechercher."));
        return;
    }

    bool ok = false;
    int idRecherche = ui->rechEQ->text().toInt(&ok);
    if (!ok) {
        QMessageBox::warning(this, tr("Erreur"),tr("L'ID doit être un nombre valide."));
        return;
    }
    QSqlQueryModel* filteredModel = equipe.rechercher(idRecherche);
    if (filteredModel) {
        if (filteredModel->rowCount() > 0) {
            ui->tableViewEQ->setModel(filteredModel);
            QMessageBox::information(this, tr("Recherche Équipe"),tr("Équipe trouvée avec l'ID : %1").arg(idRecherche));
        } else {
            QMessageBox::warning(this, tr("Erreur"),
                                 tr("Aucune équipe trouvée avec cet ID."));
            delete filteredModel;
        }
    } else {
        QMessageBox::critical(this, tr("Erreur"),tr("Erreur lors de la recherche."));
    }
    ui->rechEQ->clear();
}

void ourlabib::on_exportEQ_clicked()
{
    QAbstractItemModel *model = ui->tableViewEQ->model();
    if (!model) {
        QMessageBox::critical(this, tr("Erreur"), tr("Le modèle du tableau n'est pas disponible."));
        return;
    }
    if (model->rowCount() == 0) {
        QMessageBox::warning(this, tr("Attention"), tr("Le tableau est vide !"));
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this, tr("Exporter en PDF"), "", tr("Fichiers PDF (*.pdf)"));
    if (fileName.isEmpty()) return;
    if (!fileName.endsWith(".pdf", Qt::CaseInsensitive)) fileName += ".pdf";

    QTextDocument doc;
    QTextCursor cursor(&doc);
    cursor.insertHtml("<h2 style='text-align:center;'>Liste des Equipes </h2><br>");
    int rows = model->rowCount();
    int cols = model->columnCount();
    QTextTableFormat tableFormat;
    tableFormat.setBorder(1);
    tableFormat.setCellPadding(5);
    tableFormat.setCellSpacing(0);
    tableFormat.setAlignment(Qt::AlignCenter);
    QTextTable *table = cursor.insertTable(rows + 1, cols, tableFormat);
    for (int col = 0; col < cols; col++) {
        QVariant headerData = model->headerData(col, Qt::Horizontal, Qt::DisplayRole);
        QString text = headerData.toString();
        QTextCursor cellCursor = table->cellAt(0, col).firstCursorPosition();
        cellCursor.insertText(text, QTextCharFormat()); // tu peux styliser si besoin
    }
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            QVariant data = model->data(model->index(row, col), Qt::DisplayRole);
            QString text = data.toString();
            QTextCursor cellCursor = table->cellAt(row + 1, col).firstCursorPosition();
            cellCursor.insertText(text);
        }
    }
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);

    doc.print(&printer);

    QMessageBox::information(this, tr("Succès"), tr("Export PDF terminé !"));
}

void ourlabib::on_statEQ_clicked()
{
    QBarSet *set = new QBarSet("Membres");
    set->setColor(QColor("#9BE15D"));

    QStringList noms;
    QSqlQuery query("SELECT NOM_EQUIPE, NOMBRE_MEMBRES FROM EQUIPES ORDER BY NOMBRE_MEMBRES DESC");
    while (query.next()) {
        noms << query.value(0).toString();
        *set << query.value(1).toInt();
    }

    if (noms.isEmpty()) {
        QMessageBox::information(this, tr("Information"), tr("Aucune équipe trouvée!"));
        return;
    }

    QBarSeries *series = new QBarSeries();
    series->append(set);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Statistiques des Équipes");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(noms);
    axisX->setTitleText("Équipes");
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Nombre de Membres");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    QWidget *window = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(window);
    layout->addWidget(chartView);
    window->setLayout(layout);
    window->resize(800, 500);
    window->setWindowTitle("Statistiques des Équipes");
    window->show();
}
//*******************CRUD ZONES*******************
void ourlabib::on_enregistrerzone_clicked()
{

    int id  = ui->idzone->text().toInt();
    QString nom= ui->nomzone->text();
    QString local= ui->localisationzone->text();
    QString prior= ui->prioritzone->currentText();
    int pop = ui->population->text().toInt();
    QString super = ui->superfice->text();
    int nb = ui->nbpoubelle->text().toInt();

    zones z(id, nom, local, prior, pop, super, nb);
    bool test = z.ajouter();

    if (test)
    {
        ui->tableViewZones->setModel(z.afficher());
        ui->idzone->clear();
        ui->nomzone->clear();
        ui->localisationzone->clear();
        ui->prioritzone->setCurrentIndex(0);
        ui->population->clear();
        ui->superfice->clear();
        ui->nbpoubelle->clear();

        QMessageBox::information(nullptr, QObject::tr("OK"), QObject::tr("Zone ajoutée avec succès."), QMessageBox::Ok);
    }
    else
    {
        QMessageBox::critical(nullptr, QObject::tr("Erreur"), QObject::tr("Ajout non effectué. (ID existant ou erreur base de données)"), QMessageBox::Ok);
    }


}


void ourlabib::on_tableViewZones_activated(const QModelIndex &index)
{
    QAbstractItemModel *model = ui->tableViewZones->model();
    if (!model) return;
    int row = index.row();
    ui->idzone->setText(model->data(model->index(row, 0)).toString());
    ui->nomzone->setText(model->data(model->index(row, 1)).toString());
    ui->localisationzone->setText(model->data(model->index(row, 2)).toString());
    ui->prioritzone->setCurrentText(model->data(model->index(row, 3)).toString());
    ui->population->setText(model->data(model->index(row, 4)).toString());
    ui->superfice->setText(model->data(model->index(row, 5)).toString());
    ui->nbpoubelle->setText(model->data(model->index(row, 6)).toString());
}


void ourlabib::on_refreshzone_clicked()
{
    ui->tableViewZones->setModel(z.afficher());
}
/*
void ourlabib::afficherZones(QShowEvent *event) {
    QMainWindow::showEvent(event);
    ui->tableViewZones->setModel(z.afficher());
}
*/

void ourlabib::on_modifzone_clicked()
{
    int id= ui->idzone->text().toInt();
    QString nom= ui->nomzone->text();
    QString local= ui->localisationzone->text();
    QString prior= ui->prioritzone->currentText();
    int pop= ui->population->text().toInt();
    QString super = ui->superfice->text();
    int nb= ui->nbpoubelle->text().toInt();
    bool test = z.modifier(id, nom, local, prior, pop, super, nb);
    if (test)
    {
        ui->tableViewZones->setModel(z.afficher());
        ui->idzone->clear();
        ui->nomzone->clear();
        ui->localisationzone->clear();
        ui->prioritzone->setCurrentIndex(0);
        ui->population->clear();
        ui->superfice->clear();
        ui->nbpoubelle->clear();
        QMessageBox::information(nullptr, QObject::tr("OK"), QObject::tr("Zone modifiée avec succès."), QMessageBox::Ok);
    }
    else
    {
        QMessageBox::critical(nullptr, QObject::tr("Erreur"), QObject::tr("Modification non effectuée."), QMessageBox::Ok);
    }
}
void ourlabib::on_supzone_clicked()
{
    int id = 0;
    QModelIndex index = ui->tableViewZones->selectionModel()->currentIndex();

    if (index.isValid()) {
        id = ui->tableViewZones->model()->data(ui->tableViewZones->model()->index(index.row(), 0)).toInt();
    }
    else if (!ui->idzone->text().isEmpty()) {
        id = ui->idzone->text().toInt();
    }

    if (id == 0) return;

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(
        nullptr,
        QObject::tr("Confirmer"),
        QObject::tr("Êtes-vous sûr(e) de vouloir supprimer la zone ID %1 ?").arg(id),
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        if (z.supprimer(id)) {
            ui->tableViewZones->setModel(z.afficher());
            ui->idzone->clear();
            ui->nomzone->clear();
            ui->localisationzone->clear();
            ui->prioritzone->setCurrentIndex(0);
            ui->population->clear();
            ui->superfice->clear();
            ui->nbpoubelle->clear();
            QMessageBox::information(nullptr, QObject::tr("OK"), QObject::tr("Zone supprimée avec succès."), QMessageBox::Ok);
        } else {
            QMessageBox::critical(nullptr, QObject::tr("Erreur"), QObject::tr("Suppression non effectuée."), QMessageBox::Ok);
        }
    }
}

void ourlabib::on_triezone_clicked()
{
    ui->tableViewZones->setModel(z.trier());
}


void ourlabib::on_annlzone_clicked()
{
    ui->idzone->clear();
    ui->nomzone->clear();
    ui->localisationzone->clear();
    ui->prioritzone->setCurrentIndex(0);
    ui->population->clear();
    ui->superfice->clear();
    ui->nbpoubelle->clear();
}


void ourlabib::on_rechzone_clicked()
{
    int id = ui->recherchezone->text().toInt();
    if (ui->recherchezone->text().isEmpty()) {
        ui->tableViewZones->setModel(z.afficher());
        ui->recherchezone->clear();
        return;
    }
    QSqlQueryModel* model = z.rechercher(id);
    ui->tableViewZones->setModel(model);
    if (model->rowCount() > 0) {
        QMessageBox::information(nullptr, QObject::tr("Trouvé"),
                                 QObject::tr("Zone ID %1 trouvée !").arg(id), QMessageBox::Ok);
    } else {
        QMessageBox::warning(nullptr, QObject::tr("Non trouvé"),
                             QObject::tr("Aucune zone avec l'ID %1 n'existe !").arg(id), QMessageBox::Ok);
    }
    ui->recherchezone->clear();
}

void ourlabib::on_statzone_clicked()
{
    QBarSet *set = new QBarSet("Population");
    set->setColor(QColor("#4CAF50"));

    QStringList noms;
    QSqlQuery query("SELECT NOM_ZONE, POPULATION FROM ZONES ORDER BY POPULATION DESC");
    while (query.next()) {
        noms << query.value(0).toString();
        *set << query.value(1).toInt();
    }

    if (noms.isEmpty()) {
        QMessageBox::information(this, tr("Information"), tr("Aucune zone trouvée!"));
        return;
    }

    QBarSeries *series = new QBarSeries();
    series->append(set);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Statistiques des Zones");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(noms);
    axisX->setTitleText("Zones");
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Population");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    QWidget *window = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(window);
    layout->addWidget(chartView);
    window->setLayout(layout);
    window->resize(800, 500);
    window->setWindowTitle("Statistiques des Zones");
    window->show();
}
void ourlabib::on_exportzone_clicked() {
    QAbstractItemModel *model = ui->tableViewZones->model();
    if (!model) {
        QMessageBox::critical(this, "Erreur", "Le modèle du tableau n'est pas disponible.");
        return;
    }
    if(model->rowCount() == 0) {
        QMessageBox::warning(this, "Attention", "Le tableau est vide !");
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this, "Exporter en PDF", "", "*.pdf");
    if(fileName.isEmpty()) return;
    if(!fileName.endsWith(".pdf")) fileName += ".pdf";

    QTextDocument doc;
    QTextCursor cursor(&doc);
    cursor.insertHtml("<h2 align='center'>Liste des Zones</h2><br>");

    int rows = model->rowCount();
    int cols = model->columnCount();

    QTextTableFormat tableFormat;
    tableFormat.setBorder(1);
    tableFormat.setCellPadding(3);
    tableFormat.setCellSpacing(0);
    QTextTable *table = cursor.insertTable(rows + 1, cols, tableFormat);

    for(int col = 0; col < cols; col++) {
        QVariant headerData = model->headerData(col, Qt::Horizontal, Qt::DisplayRole);
        QString text = headerData.toString();
        if (table->cellAt(0, col).isValid()) {
            table->cellAt(0, col).firstCursorPosition().insertText(text);
        }
    }

    for(int row = 0; row < rows; row++) {
        for(int col = 0; col < cols; col++) {
            QVariant data = model->data(model->index(row, col), Qt::DisplayRole);
            QString text = data.toString();
            if (table->cellAt(row + 1, col).isValid()) {
                table->cellAt(row + 1, col).firstCursorPosition().insertText(text);
            }
        }
    }

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    doc.print(&printer);

    QMessageBox::information(this, "Succès", "Export PDF terminé !");

}


