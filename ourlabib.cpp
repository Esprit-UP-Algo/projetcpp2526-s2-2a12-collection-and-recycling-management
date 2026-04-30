#include "ourlabib.h"
#include "connection.h"
#include "gestion_equipe.h"
#include "mapzones.h"
#include "qrender.h"
#include "poubelles.h"
#include "zones.h"
#include "missions.h"
#include "employes.h"

// groqApiKey removed (using local keys)
#include "ui_ourlabib.h"

#ifdef USE_OPENCV
#include <opencv2/opencv.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#endif

#include <QDebug>

#include <QFileDialog>
#include <QIntValidator>
#include <QItemSelectionModel>
#include <QImage>
#include <QMessageBox>
#include <QRandomGenerator> 
#include <QModelIndexList>
#include <QPrinter>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QSortFilterProxyModel>
#include <QPdfWriter>
#include <QDesktopServices>
#include <QUrl>
#include <QPageLayout>
#include <QPageSize>
#include <QPainter>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QStandardItemModel>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextTable>
#include <QTextTableFormat>
#include <QtCharts>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QChartView>
#include <QCalendarWidget>





















#include <QCalendarWidget>


// ============================================================
// CONSTRUCTEUR
// ============================================================

// ========== VARIABLES STATIQUES ET UTILITAIRES (EMPLOYÃ‰ES & Ã‰QUIPES) ==========
static Employe empTmp;
static int selectedEmployeId = -1;

static void rafraichirTableEmployes(Ui::ourlabib *ui) {
  QSqlQueryModel *model = empTmp.afficher();
  ui->tableEmployes->setModel(model);
  ui->tableEmployes->resizeColumnsToContents();
  ui->tableEmployes->horizontalHeader()->setStretchLastSection(true);
}

ourlabib::ourlabib(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::ourlabib) {
  ui->setupUi(this);

  // --- Validateurs pour la saisie des Ã‰quipes ---
  QIntValidator *idValidator = new QIntValidator(1, 99999999, this);
  ui->idEQ->setValidator(idValidator);
  ui->nbMembresEQ->setRange(1, 100);
  ui->nomEQ->setMaxLength(30);
  QRegularExpression regex("^[A-Za-z0-9À-ÿ\\s'-]+$");
  QRegularExpressionValidator *textValidator =
      new QRegularExpressionValidator(regex, this);
  ui->nomEQ->setValidator(textValidator);

  ui->chefEQ->setEditable(true);
  ui->zoneCEQ->setEditable(true);
  ui->chefEQ->setValidator(textValidator);

  // --- Validateurs pour la saisie des Zones ---
  QIntValidator *idValidatorZone = new QIntValidator(1, 99999999, this);
  ui->idzone->setValidator(idValidatorZone);
  ui->nomzone->setMaxLength(40);
  ui->localisationzone->setMaxLength(50);
  ui->population->setValidator(idValidatorZone);
  ui->nbpoubelle->setValidator(idValidatorZone);

  // --- Mot de passe oubli? ---
  ui->label_forgot->setOpenExternalLinks(false);
  connect(ui->label_forgot, &QLabel::linkActivated, this, &ourlabib::on_label_forgot_linkActivated);

  // --- Configuration de la table Poubelles ---
  ui->tableViewPoubelles->setSelectionMode(
      QAbstractItemView::ExtendedSelection);
  ui->tableViewPoubelles->setSelectionBehavior(QAbstractItemView::SelectRows);

  // Arduino initialization
  arduino = nullptr;
  arduinoConnected = false;
  connectArduino();

  // Page de dÃ©marrage
  ui->stackedWidget->setCurrentIndex(0);

  //chatbotzone
  zonechatbot = new ZoneChatbot(this);
  connect(zonechatbot, &ZoneChatbot::responseReceived, this, &ourlabib::onResponseReceived);
  connect(zonechatbot, &ZoneChatbot::errorOccurred, this, &ourlabib::onErrorOccurred);
  connect(zonechatbot, &ZoneChatbot::processingFinished, this, &ourlabib::onChatbotProcessingFinished);
  connect(zonechatbot, &ZoneChatbot::insertRequestReady, this, &ourlabib::executeZoneInsertion);

  
  //twillio SMS
// ID Twilio supprim�
  networkManager = new QNetworkAccessManager(this);
// ID Twilio supprim�
// ID Twilio supprim�
// ID Twilio supprim�
// ID Twilio supprim�
// ID Twilio supprim�
// ID Twilio supprim�

  // chatbotEquipe
  chatbotEQ = new chatbotEquie(this);
  connect(chatbotEQ, &chatbotEquie::responseReceived, this, &ourlabib::onEquipeChatbotResponse);
  connect(chatbotEQ, &chatbotEquie::processingFinished, this, &ourlabib::onEquipeChatbotFinished);
  connect(chatbotEQ, &chatbotEquie::updateRequested, this, &ourlabib::onEquipeUpdateRequested);

  // Maps
}

ourlabib::~ourlabib() { delete ui; }

// ============================================================
// NAVIGATION & LOGIN
// Navigation slots
void ourlabib::on_btnEmployes_clicked() {
  if (!hasAccess("EMPLOYES")) {
    QMessageBox::warning(this, "AccÃ¨s refusÃ©", "Vous n'avez pas les droits pour accÃ©der Ã  la gestion des employÃ©s.");
    return;
  }
  ui->stackedWidget->setCurrentIndex(2);
  loadEquipes();
  rafraichirTableEmployes(ui);
}

void ourlabib::on_btnEquipes_clicked() {
  ui->stackedWidget->setCurrentIndex(3);
  refreshEquipesTable();
}

void ourlabib::on_btnZones_clicked() {
  ui->stackedWidget->setCurrentIndex(4);
  refreshZonesTable();
}

void ourlabib::on_btnPoubelles_clicked() {
  ui->stackedWidget->setCurrentIndex(5);
  refreshPoubellesTable();
}

void ourlabib::on_btnMissions_clicked() {
  ui->stackedWidget->setCurrentIndex(6);
  loadMissionsCombos();
    refreshMissionsTable();
}

// ============================================================
// CALENDRIER & MAIL
// ============================================================

void ourlabib::on_btnCalendrier_clicked() {
    // === AFFICHAGE IMMÉDIAT DU CALENDRIER ===
    ui->stackedWidget->setCurrentWidget(ui->pageCalendrier); // pageCalendrier = index 9

    if (ui->calendarMissions) {
        ui->calendarMissions->setSelectedDate(QDate::currentDate());
        highlightCalendarDates();
    }

    // === PLANIFICATION IA ===
    Connection *c = Connection::instance();
    if (!c->getDatabase().isOpen()) return;

    QSqlQuery queryPoubelle(c->getDatabase());
    if (!queryPoubelle.exec("SELECT ID_POUBELLE, ID_ZONE FROM SAFWCARM.POUBELLE WHERE NIVEAU_REMPLISSAGE >= 80 OR UPPER(ETAT) = 'PLEINE'")) {
        queryPoubelle.exec("SELECT ID_POUBELLE, ID_ZONE FROM POUBELLE WHERE NIVEAU_REMPLISSAGE >= 80 OR UPPER(ETAT) = 'PLEINE'");
    }

    int countMissions = 0;
    QDate today = QDate::currentDate();

    while (queryPoubelle.next()) {
        int idPoubelle = queryPoubelle.value(0).toInt();
        int idZone     = queryPoubelle.value(1).toInt();

        QSqlQuery checkMission(c->getDatabase());
        checkMission.prepare("SELECT COUNT(*) FROM MISSION WHERE TYPE LIKE :s AND TO_CHAR(DATE_MISSION,'YYYY-MM-DD') = :d");
        checkMission.bindValue(":s", "%P" + QString::number(idPoubelle) + "%");
        checkMission.bindValue(":d", today.toString("yyyy-MM-dd"));

        if (checkMission.exec() && checkMission.next() && checkMission.value(0).toInt() == 0) {
            QSqlQuery qEq(c->getDatabase());
            int idEquipe = -1;
            if (qEq.exec("SELECT ID_EQUIPE FROM EQUIPES WHERE ROWNUM = 1") && qEq.next())
                idEquipe = qEq.value(0).toInt();

            if (idEquipe != -1) {
                QSqlQuery ins(c->getDatabase());
                ins.prepare("INSERT INTO MISSION (TYPE, DATE_MISSION, DUREE, ETAT, PRIORITE, ID_EQUIPE, ID_ZONE) VALUES (:t, :d, 60, 'Planifiée', 'Critique', :e, :z)");
                ins.bindValue(":t", "Collecte Urgence (P" + QString::number(idPoubelle) + ")");
                ins.bindValue(":d", today);
                ins.bindValue(":e", idEquipe);
                ins.bindValue(":z", idZone);
                if (ins.exec()) countMissions++;
            }
        }
    }

    if (ui->calendarMissions) highlightCalendarDates();
    if (countMissions > 0) {
        QMessageBox::information(this, "Planification IA",
            QString::number(countMissions) + " mission(s) d'urgence générée(s) !");
        loadMissionsCombos();
        refreshMissionsTable();
    }
}

void ourlabib::on_btnRetourCalendrier_clicked() {
    ui->stackedWidget->setCurrentWidget(ui->pageMissions);
}

void ourlabib::on_btnMail_clicked() {
    int idEquipe = 0;
    QString nomEquipe = "Équipe sélectionnée";
    if (ui->equipeMission->currentData().isValid()) {
        idEquipe  = ui->equipeMission->currentData().toInt();
        nomEquipe = ui->equipeMission->currentText();
    }

    QString emails, nomChef = "Chef d'équipe";
    Connection *c = Connection::instance();

    if (idEquipe > 0) {
        QSqlQuery qChef(c->getDatabase());
        // Robustesse : Essai EQUIPES puis EQUIPE
        if (!qChef.exec("SELECT CHEF_EQUIPE FROM EQUIPES WHERE ID_EQUIPE = " + QString::number(idEquipe))) {
            qChef.exec("SELECT CHEF_EQUIPE FROM EQUIPE WHERE ID_EQUIPE = " + QString::number(idEquipe));
        }
        if (qChef.next()) nomChef = qChef.value(0).toString();

        QSqlQuery qEmail(c->getDatabase());
        qEmail.prepare("SELECT EMAIL FROM EMPLOYE WHERE NOM = :nom");
        qEmail.bindValue(":nom", nomChef);
        if (qEmail.exec() && qEmail.next()) emails = qEmail.value(0).toString();
    }

    // --- STRUCTURE DE L'EMAIL DEMANDÉE ---
    QString corps = "ORDRE DE MISSION - OURLABIB\n\n";
    corps += "À : " + nomChef + "\n";
    corps += "Date : " + QDate::currentDate().toString("dd/MM/yyyy") + "\n\n";

    if (idEquipe > 0) {
        QSqlQuery qM(c->getDatabase());
        // Sélection des missions de l'équipe pour aujourd'hui
        qM.prepare("SELECT TYPE, PRIORITE, DESCRIPTION FROM MISSION "
                   "WHERE ID_EQUIPE = :id AND TO_CHAR(DATE_MISSION, 'YYYY-MM-DD') = :date");
        qM.bindValue(":id", idEquipe);
        qM.bindValue(":date", QDate::currentDate().toString("yyyy-MM-dd"));
        
        if (qM.exec()) {
            bool hasMissions = false;
            while (qM.next()) {
                hasMissions = true;
                QString type = qM.value(0).toString();
                QString prio = qM.value(1).toString();
                QString desc = qM.value(2).toString();
                
                corps += "- " + type + " [" + prio + "]\n";
                corps += "  Desc: " + (desc.isEmpty() ? "Aucune description fournie" : desc) + "\n\n";
            }
            if (!hasMissions) {
                corps += "Aucune mission planifiée pour aujourd'hui.\n";
            }
        }
    }

    ui->mailDestinataire->setText(emails);
    ui->mailObjet->setText("Ordre de Mission - " + nomEquipe);
    ui->mailCorps->setPlainText(corps);
    ui->stackedWidget->setCurrentWidget(ui->pageMail); // pageMail = index 10
}

void ourlabib::on_btnRetourMail_clicked() {
    ui->stackedWidget->setCurrentWidget(ui->pageMissions);
}

void ourlabib::on_btnEnvoyerMail_clicked() {
    QString dest  = ui->mailDestinataire->text().trimmed();
    QString sujet = ui->mailObjet->text().trimmed();
    QString corps = ui->mailCorps->toPlainText().trimmed();
    if (dest.isEmpty() || sujet.isEmpty() || corps.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez remplir tous les champs.");
        return;
    }
    QString corpsHtml = "<div style='font-family:monospace;white-space:pre-wrap'>" + corps + "</div>";
    envoyerEmailResend(dest, sujet, corpsHtml);
}



// ============================================================
// ACCÃˆS AUX PAGES PRINCIPALES
// ============================================================

// Navigation slots removed to avoid redefinition
void ourlabib::on_btnRetourEquipes_clicked() {
  ui->stackedWidget->setCurrentIndex(1);
}
void ourlabib::on_btnRetourZones_clicked() {
  ui->stackedWidget->setCurrentIndex(1);
}
void ourlabib::on_btnRetourPoubelles_clicked() {
  ui->stackedWidget->setCurrentIndex(1);
}
void ourlabib::on_btnRetourMissions_clicked() {
  ui->stackedWidget->setCurrentIndex(1);
}

// ============================================================
// GESTION DES ÉQUIPES (CRUD complet)
// ============================================================

Gestion_equipe equipe;

bool ourlabib::saisieEquipeValide()
{
    if(ui->idEQ->text().isEmpty() || ui->idEQ->text().toInt()<= 0)
    {
        QMessageBox::warning(this, "Erreur", "L'ID doit être un nombre supérieur à 0");
        return false;
    }
    if(ui->nomEQ->text().isEmpty())
    {
        QMessageBox::warning(this, "Erreur", "Le nom est obligatoire");
        return false;
    }
    if(ui->zoneCEQ->currentText().isEmpty())
    {
        QMessageBox::warning(this, "Erreur", "La zone couverte est obligatoire");
        return false;
    }
    if(ui->chefEQ->currentText().isEmpty())
    {
        QMessageBox::warning(this, "Erreur", "le chef d'équipe est obligatoire");
        return false;
    }
    if(ui->nbMembresEQ->value() <= 0)
    {
        QMessageBox::warning(this, "Erreur", "Le nombre d'équipes doit être un nombre supérieur à 0");
        return false;
    }
    return true;
}

void ourlabib::on_enrgstEQ_clicked()
{
    if (!saisieEquipeValide())
        return;
    int id_Eqpe=ui->idEQ->text().toInt();
    QString nom=ui->nomEQ->text();
    QString zone=ui->zoneCEQ->currentText();
    QString chefEq= ui->chefEQ->currentText();
    QDate dateM= ui->dateEQ->date();
    int nb=ui->nbMembresEQ->value();
    QString statut =ui->statutEQ->currentText();
    Gestion_equipe eq(id_Eqpe,nom,zone,chefEq,nb,dateM,statut);
    bool test=eq.ajouter();
    if (test) {
        ui->tableViewEQ->setModel(equipe.afficher());
        ui->zoneCEQ->clear();
        ui->idEQ->clear();
        ui->nomEQ->clear();
        ui->chefEQ->clear();
        ui->dateEQ->clear();
        ui->nbMembresEQ->setValue(1);

        QMessageBox::information(this, "Succès", "Équipe ajoutée avec succès ");
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de l'ajout ");
    }
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
            ui->nbMembresEQ->setValue(1);
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
    QString zone = ui->zoneCEQ->currentText();
    QString chef = ui->chefEQ->currentText();
    int nb = ui->nbMembresEQ->value();
    QDate date = ui->dateEQ->date();
    QString statut = ui->statutEQ->currentText();

    if (equipe.modifier(id, nom, zone, chef, nb, date, statut))
    {
        ui->tableViewEQ->setModel(equipe.afficher());
        ui->idEQ->clear();
        ui->nomEQ->clear();
        ui->zoneCEQ->clear();
        ui->chefEQ->clear();
        ui->nbMembresEQ->setValue(1);
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
    ui->zoneCEQ->setCurrentText(zone);
    ui->chefEQ->setCurrentText(chef);
    ui->nbMembresEQ->setValue(nb);
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
    ui->nbMembresEQ->setValue(1);
}

void ourlabib::clearEquipeForm() {
    on_anulEQ_clicked();
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
    QString val = ui->rechEQ->text().trimmed();
    if (val.isEmpty()) {
        QMessageBox::warning(this, tr("Attention"), tr("Veuillez entrer le nom ou l'ID de l'équipe à rechercher."));
        return;
    }

    QSqlQueryModel* filteredModel = equipe.rechercher(val);
    if (filteredModel) {
        if (filteredModel->rowCount() > 0) {
            ui->tableViewEQ->setModel(filteredModel);
            QMessageBox::information(this, tr("Recherche Équipe"), tr("Résultat(s) trouvé(s) pour : %1").arg(val));
        } else {
            QMessageBox::warning(this, tr("Erreur"), tr("Aucune équipe trouvée avec ce nom ou ID."));
            delete filteredModel;
        }
    } else {
        QMessageBox::critical(this, tr("Erreur"), tr("Erreur lors de la recherche."));
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
        cellCursor.insertText(text, QTextCharFormat());
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

void ourlabib::refreshEquipesTable() {
    ui->tableViewEQ->setModel(equipe.afficher());
    ui->tableViewEQ->resizeColumnsToContents();
    loadZonesCombo();
    loadChefsEquipe();
}

// ============================================================
// CHARGEMENT ZONES DANS COMBO "Zone Couverte" (formulaire Équipes)
// ============================================================
void ourlabib::loadZonesCombo() {
    Connection *c = Connection::instance();
    if (!c->getDatabase().isOpen()) return;

    QSqlQuery query(c->getDatabase());
    QString sql = "SELECT NOM_ZONE FROM ZONES ORDER BY NOM_ZONE";

    if (!query.exec(sql)) {
        sql = "SELECT NOM_ZONE FROM ZONE ORDER BY NOM_ZONE";
        if (!query.exec(sql)) {
            qDebug() << "Erreur loadZonesCombo:" << query.lastError().text();
            return;
        }
    }

    ui->zoneCEQ->clear();
    ui->zoneCEQ->addItem("-- Sélectionner une zone --", "");
    while (query.next()) {
        ui->zoneCEQ->addItem(query.value(0).toString());
    }
}

// ============================================================
// CHARGEMENT CHEFS D'ÉQUIPE DANS COMBO "Chef Équipe" (formulaire Équipes)
// ============================================================
void ourlabib::loadChefsEquipe() {
    Connection *c = Connection::instance();
    if (!c->getDatabase().isOpen()) return;

    QSqlQuery query(c->getDatabase());
    // On récupère les employés dont le poste est "Chef d'équipe" ou "Superviseur"
    QString sql = "SELECT NOM FROM EMPLOYE WHERE UPPER(POSTE) IN ('CHEF D''EQUIPE', 'SUPERVISEUR', 'CHEF D''ÉQUIPE') ORDER BY NOM";

    if (!query.exec(sql)) {
        // Fallback : tous les employés
        sql = "SELECT NOM FROM EMPLOYE ORDER BY NOM";
        if (!query.exec(sql)) {
            qDebug() << "Erreur loadChefsEquipe:" << query.lastError().text();
            return;
        }
    }

    ui->chefEQ->clear();
    ui->chefEQ->addItem("-- Sélectionner un chef --", "");
    while (query.next()) {
        ui->chefEQ->addItem(query.value(0).toString());
    }

    // Si aucun résultat, charger tous les employés
    if (ui->chefEQ->count() <= 1) {
        ui->chefEQ->clear();
        QSqlQuery q2(c->getDatabase());
        if (q2.exec("SELECT NOM FROM EMPLOYE ORDER BY NOM")) {
            ui->chefEQ->addItem("-- Sélectionner un chef --", "");
            while (q2.next()) {
                ui->chefEQ->addItem(q2.value(0).toString());
            }
        }
    }
}


// ============================================================
// GESTION DES ZONES (CRUD complet)
// ============================================================

zones z;

bool ourlabib::saisieZoneValide()
{
    if(ui->idzone->text().isEmpty() || ui->idzone->text().toInt()<= 0)
    {
        QMessageBox::warning(this, "Erreur", "L'ID doit être un nombre supérieur à 0");
        return false;
    }
    if(ui->nbpoubelle->text().isEmpty() || ui->nbpoubelle->text().toInt()<= 0)
    {
        QMessageBox::warning(this, "Erreur", "Le nombre du poubelle doit être un nombre supérieur à 0");
        return false;
    }
    if(ui->nomzone->text().isEmpty())
    {
        QMessageBox::warning(this, "Erreur", "Le nom de la zone est obligatoire");
        return false;
    }
    if(ui->localisationzone->text().isEmpty())
    {
        QMessageBox::warning(this, "Erreur", "La localisation de la zone est obligatoire");
        return false;
    }
    if(ui->population->text().isEmpty() || ui->population->text().toInt()<= 0)
    {
        QMessageBox::warning(this, "Erreur", "La population doit être un nombre supérieur à 0");
        return false;
    }
    if(ui->superfice->text().isEmpty())
    {
        QMessageBox::warning(this, "Erreur", "La superfice  est obligatoire");
        return false;
    }
    return true;
}

void ourlabib::on_enregistrerzone_clicked()
{
    if (!saisieZoneValide())
        return;
    int id  = ui->idzone->text().toInt();
    QString nom= ui->nomzone->text();
    QString local= ui->localisationzone->text();
    QString prior= ui->prioritzone->currentText();
    int pop = ui->population->text().toInt();
    QString super = ui->superfice->text();
    int nb = ui->nbpoubelle->text().toInt();

    zones zObj(id, nom, local, prior, pop, super, nb);
    bool test = zObj.ajouter();

    if (test)
    {
        refreshZonesTable();
        on_annlzone_clicked();
        QMessageBox::information(nullptr, QObject::tr("OK"), QObject::tr("Zone ajoutée avec succès."), QMessageBox::Ok);
    }
    else
    {
        QMessageBox::critical(nullptr, QObject::tr("Erreur"), QObject::tr("Ajout non effectué. (ID existant ou erreur base de données)"), QMessageBox::Ok);
    }
}


void ourlabib::on_tableViewZones_activated(const QModelIndex &index)
{
    on_tableViewZones_clicked(index);
}

void ourlabib::on_tableViewZones_clicked(const QModelIndex &index)
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
    refreshZonesTable();
}


void ourlabib::on_modifzone_clicked()
{
    if (ui->idzone->text().isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner une zone à modifier.");
        return;
    }

    int id= ui->idzone->text().toInt();
    QString nom= ui->nomzone->text();
    QString local= ui->localisationzone->text();
    QString prior= ui->prioritzone->currentText();
    int pop= ui->population->text().toInt();
    QString super = ui->superfice->text();
    int nb= ui->nbpoubelle->text().toInt();

    if (z.modifier(id, nom, local, prior, pop, super, nb))
    {
        refreshZonesTable();
        on_annlzone_clicked();
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
            refreshZonesTable();
            on_annlzone_clicked();
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

void ourlabib::clearZoneForm() {
    on_annlzone_clicked();
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

void ourlabib::refreshZonesTable() {
    ui->tableViewZones->setModel(z.afficher());
    ui->tableViewZones->resizeColumnsToContents();
}

// ============================================================
// GESTION DES POUBELLES (CRUD complet)
// ============================================================

Poubelle poubelleTmp;

void ourlabib::refreshPoubellesTable() {
  ui->tableViewPoubelles->setModel(poubelleTmp.afficher());
  ui->tableViewPoubelles->resizeColumnsToContents();

  ui->zonePoubelle->clear();
  QSqlQuery qZone("SELECT ID_ZONE, NOM_ZONE FROM ZONES ORDER BY ID_ZONE");
  while (qZone.next()) {
      ui->zonePoubelle->addItem(qZone.value(0).toString() + " - " + qZone.value(1).toString());
  }
}

void ourlabib::clearPoubelleForm() {
  ui->idPoubelle->clear();
  ui->localisationPoubelle->clear();
  ui->capacitePoubelle->setValue(0);
  ui->remplissagePoubelle->setValue(0);
  ui->recherchePoubelle->clear();
  ui->statutPoubelle->setCurrentIndex(0);
  ui->datePoubelle->setDate(QDate::currentDate());
  ui->idPoubelle->setDisabled(false);
}

void ourlabib::on_btnEnregistrerPoubelle_clicked() {
  QString idStr = ui->idPoubelle->text();
  QString loc = ui->localisationPoubelle->text();
  int id = idStr.toInt();
  int cap = ui->capacitePoubelle->value();
  int niv = ui->remplissagePoubelle->value();
  QString type = ui->typePoubelle->currentText();
  int zone = ui->zonePoubelle->currentText().split(" - ").first().toInt();

  if (idStr.isEmpty() || loc.trimmed().isEmpty()) {
    QMessageBox::warning(this, "Erreur",
                         "Veuillez remplir l'ID et la Localisation.");
    return;
  }
  if (id <= 0) {
    QMessageBox::warning(this, "Erreur", "L'ID doit Ãªtre un nombre positif.");
    return;
  }

  QSqlQuery checkQuery;
  checkQuery.prepare("SELECT ID_POUBELLE FROM POUBELLE WHERE ID_POUBELLE = :id");
  checkQuery.bindValue(":id", id);
  if (checkQuery.exec() && checkQuery.next()) {
    QMessageBox::critical(this, "Erreur", "Une poubelle avec cet ID existe dÃ©jÃ  !");
    return;
  }

  QString etat = ui->statutPoubelle->currentText();
  QDate date = ui->datePoubelle->date();

  Poubelle p(id, loc, niv, cap, type, zone, etat, date);
  if (p.ajouter()) {
    refreshPoubellesTable();
    clearPoubelleForm();
    QMessageBox::information(this, "SuccÃ¨s", "Poubelle ajoutÃ©e avec succÃ¨s !");
  } else {
    QMessageBox::critical(this, "Erreur", "Ã‰chec de l'ajout.");
  }
}

void ourlabib::on_btnModifierPoubelle_clicked() {
  QString idStr = ui->idPoubelle->text();
  if (idStr.isEmpty()) {
    QMessageBox::warning(this, "Erreur",
                         "Veuillez sÃ©lectionner une poubelle Ã  modifier");
    return;
  }

  int id = idStr.toInt();
  QString loc = ui->localisationPoubelle->text();
  int niv = ui->remplissagePoubelle->value();
  int cap = ui->capacitePoubelle->value();
  QString type = ui->typePoubelle->currentText();
  int zone = ui->zonePoubelle->currentText().split(" - ").first().toInt();
  QString etat = ui->statutPoubelle->currentText();
  QDate date = ui->datePoubelle->date();

  if (loc.trimmed().isEmpty()) {
    QMessageBox::warning(this, "Erreur",
                         "La localisation ne peut pas Ãªtre vide.");
    return;
  }

  Poubelle p(id, loc, niv, cap, type, zone, etat, date);
  if (p.modifier(id)) {
    refreshPoubellesTable();
    clearPoubelleForm();
    QMessageBox::information(this, "SuccÃ¨s", "Poubelle modifiÃ©e avec succÃ¨s !");
  } else {
    QMessageBox::critical(this, "Erreur", "Ã‰chec de la modification.");
  }
}

void ourlabib::on_btnSupprimerPoubelle_clicked() {
  QModelIndexList selection =
      ui->tableViewPoubelles->selectionModel()->selectedRows();

  if (selection.isEmpty()) {
    QMessageBox::warning(this, "Attention",
                         "Veuillez sÃ©lectionner au moins une poubelle.");
    return;
  }

  QString msg = (selection.size() > 1)
                    ? "Supprimer ces " + QString::number(selection.size()) +
                          " poubelles ?"
                    : "Supprimer cette poubelle ?";

  if (QMessageBox::question(this, "Suppression", msg,
                            QMessageBox::Yes | QMessageBox::No) ==
      QMessageBox::Yes) {
    bool success = true;
    for (const QModelIndex &index : selection) {
      int id = index.data(Qt::DisplayRole).toInt();
      if (!poubelleTmp.supprimer(id)) {
        success = false;
      }
    }
    refreshPoubellesTable();
    clearPoubelleForm();
    if (success) {
      QMessageBox::information(this, "SuccÃ¨s", "Suppression terminÃ©e.");
    } else {
      QMessageBox::critical(
          this, "Erreur", "Certaines poubelles n'ont pas pu Ãªtre supprimÃ©es.");
    }
  }
}

void ourlabib::on_tableViewPoubelles_clicked(const QModelIndex &index) {
    int row = index.row();
    auto model = ui->tableViewPoubelles->model();

    ui->idPoubelle->setText(model->index(row, 0).data().toString());
    ui->idPoubelle->setDisabled(true);
    ui->typePoubelle->setCurrentText(model->index(row, 1).data().toString());
    ui->capacitePoubelle->setValue(model->index(row, 2).data().toInt());
    ui->remplissagePoubelle->setValue(model->index(row, 3).data().toInt());
    ui->localisationPoubelle->setText(model->index(row, 4).data().toString()); // ← AJOUTER
    ui->statutPoubelle->setCurrentText(model->index(row, 5).data().toString());
    ui->datePoubelle->setDate(model->index(row, 6).data().toDate());

    QString idZoneStr = model->index(row, 7).data().toString();
    for(int i = 0; i < ui->zonePoubelle->count(); i++) {
        if(ui->zonePoubelle->itemText(i).startsWith(idZoneStr + " - ") ||
            ui->zonePoubelle->itemText(i) == idZoneStr) {
            ui->zonePoubelle->setCurrentIndex(i);
            break;
        }
    }
}

void ourlabib::on_btnAnnulerPoubelle_clicked() { clearPoubelleForm(); }

void ourlabib::on_btnRechercherPoubelle_clicked() {
  QString texte = ui->recherchePoubelle->text();

  if (texte.isEmpty()) {
    refreshPoubellesTable();
    return;
  }

  QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel(this);
  proxyModel->setSourceModel(ui->tableViewPoubelles->model());
  proxyModel->setFilterKeyColumn(-1); // Cherche dans toutes les colonnes
  proxyModel->setFilterFixedString(texte);
  proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

  if (proxyModel->rowCount() == 0) {
    QMessageBox::information(this, "Recherche",
                             "Aucune poubelle trouvÃ©e avec cet ID.");
    refreshPoubellesTable();
  } else {
    ui->tableViewPoubelles->setModel(proxyModel);
    QModelIndex index = ui->tableViewPoubelles->model()->index(0, 0);
    ui->tableViewPoubelles->setCurrentIndex(index);
    on_tableViewPoubelles_clicked(index);
  }
  ui->recherchePoubelle->clear();
}

void ourlabib::on_btnRefreshPoubelle_clicked() {
  refreshPoubellesTable();
  clearPoubelleForm();
  ui->idPoubelle->setDisabled(false);
}

void ourlabib::on_btnTrierPoubelle_clicked() {
    Connection *c = Connection::instance();

    QSqlQueryModel *model = new QSqlQueryModel();
    model->setQuery(
        "SELECT ID_POUBELLE, TYPE, CAPACITE_MAX, NIVEAU_REMPLISSAGE, "
        "LOCALISATION, ETAT, DATE_VIDANGE, ID_ZONE "
        "FROM POUBELLE "
        "ORDER BY NIVEAU_REMPLISSAGE DESC",   // ← du plus rempli au moins rempli
        c->getDatabase()
        );

    model->setHeaderData(0, Qt::Horizontal, "ID");
    model->setHeaderData(1, Qt::Horizontal, "Type");
    model->setHeaderData(2, Qt::Horizontal, "Capacité Max (L)");
    model->setHeaderData(3, Qt::Horizontal, "Remplissage (%)");
    model->setHeaderData(4, Qt::Horizontal, "Localisation");
    model->setHeaderData(5, Qt::Horizontal, "Statut");
    model->setHeaderData(6, Qt::Horizontal, "Date Vidange");
    model->setHeaderData(7, Qt::Horizontal, "ID Zone");

    ui->tableViewPoubelles->setModel(model);
    ui->tableViewPoubelles->resizeColumnsToContents();
}

void ourlabib::on_btnStatistiquesPoubelles_clicked() {
    Connection *c = Connection::instance();

    // Requête : moyenne du remplissage par zone
    QSqlQuery query(c->getDatabase());
    query.exec(
        "SELECT z.NOM_ZONE, "
        "       ROUND(AVG(p.NIVEAU_REMPLISSAGE), 1) AS MOY_REMPLISSAGE, "
        "       COUNT(p.ID_POUBELLE) AS NB_POUBELLES "
        "FROM POUBELLE p "
        "JOIN ZONES z ON p.ID_ZONE = z.ID_ZONE "
        "GROUP BY z.NOM_ZONE "
        "ORDER BY MOY_REMPLISSAGE DESC"
        );

    QStringList zones;
    QBarSet *setMoy  = new QBarSet("Remplissage moyen (%)");
    QBarSet *setNb   = new QBarSet("Nb poubelles");
    setMoy->setColor(QColor("#FF6B35"));
    setNb->setColor(QColor("#4A90D9"));

    bool hasData = false;
    while (query.next()) {
        hasData = true;
        zones    << query.value(0).toString();
        *setMoy  << query.value(1).toDouble();
        *setNb   << query.value(2).toInt();
    }

    if (!hasData) {
        QMessageBox::information(this, "Information",
                                 "Aucune donnee trouvee.\nVerifiez que des poubelles sont associees a des zones.");
        return;
    }

    // --- Graphique barres groupées ---
    QBarSeries *series = new QBarSeries();
    series->append(setMoy);
    series->append(setNb);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Poubelles par Zone — Remplissage moyen & Nombre");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(zones);
    axisX->setTitleText("Zones");
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(0, 100);
    axisY->setTitleText("Valeur");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    // --- Texte résumé : zone la plus critique ---
    QString zoneMax = zones.first();  // déjà trié DESC donc la 1ère = la plus remplie
    double  moyMax  = setMoy->at(0);
    QLabel *labelResume = new QLabel(
        QString("🔴 Zone la plus critique : <b>%1</b> — Remplissage moyen : <b>%2%</b>")
            .arg(zoneMax).arg(moyMax)
        );
    labelResume->setAlignment(Qt::AlignCenter);
    labelResume->setStyleSheet("font-size: 14px; color: #CC0000; padding: 8px;");

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    // --- Fenêtre ---
    QWidget *window = new QWidget;
    window->setWindowTitle("Statistiques Poubelles par Zone");
    window->resize(900, 550);

    QVBoxLayout *layout = new QVBoxLayout(window);
    layout->addWidget(labelResume);
    layout->addWidget(chartView);
    window->setLayout(layout);
    window->show();
}

void ourlabib::on_btnExportPoubelle_clicked() {
  QAbstractItemModel *model = ui->tableViewPoubelles->model();
  if (!model || model->rowCount() == 0) {
    QMessageBox::warning(this, "Attention", "Le tableau est vide !");
    return;
  }

  QString fileName = QFileDialog::getSaveFileName(this, "Exporter en PDF", "",
                                                  "Fichiers PDF (*.pdf)");
  if (fileName.isEmpty())
    return;

  QTextDocument doc;
  QTextCursor cursor(&doc);
  cursor.insertHtml(
      "<h2 style='text-align:center;'>Liste des Poubelles</h2><br>");

  int rows = model->rowCount();
  int cols = model->columnCount();

  QTextTableFormat tableFormat;
  tableFormat.setBorder(1);
  tableFormat.setCellPadding(5);
  tableFormat.setCellSpacing(0);
  QTextTable *table = cursor.insertTable(rows + 1, cols, tableFormat);

  for (int col = 0; col < cols; col++) {
    QString text =
        model->headerData(col, Qt::Horizontal, Qt::DisplayRole).toString();
    table->cellAt(0, col).firstCursorPosition().insertText(text);
  }

  for (int row = 0; row < rows; row++) {
    for (int col = 0; col < cols; col++) {
      QString text =
          model->data(model->index(row, col), Qt::DisplayRole).toString();
      table->cellAt(row + 1, col).firstCursorPosition().insertText(text);
    }
  }

  QPrinter printer(QPrinter::HighResolution);
  printer.setOutputFormat(QPrinter::PdfFormat);
  printer.setOutputFileName(fileName);
  doc.print(&printer);

  QMessageBox::information(this, "SuccÃ¨s", "Export PDF terminÃ© !");
}

// ============================================================
// POSTES SUPPLÃ‰MENTAIRES (Map, Chatbot, SMS, etc.)
// ============================================================

void ourlabib::on_btnMap_clicked() {
    ui->stackedWidget->setCurrentIndex(7);
    mapZones.openMap();   // ouvre la carte dans le navigateur
}
void ourlabib::on_btnRetourMap_clicked() {
  ui->stackedWidget->setCurrentIndex(4);
}

void ourlabib::on_btnChatbot_clicked() {
  lastPageIndex = 4;
  ui->stackedWidget->setCurrentIndex(8);
  if (ui->chatHistory->toPlainText().isEmpty()) {
    ui->chatHistory->append(
        "<b>Assistant:</b> Bonjour ! Comment puis-je vous aider ?");
  }
}

void ourlabib::on_btnChatbotEquipes_clicked() {
  lastPageIndex = 3;
  ui->stackedWidget->setCurrentIndex(8);
  if (ui->chatHistory->toPlainText().isEmpty()) {
    ui->chatHistory->append(
        "<b>Assistant:</b> Bonjour ! Aide pour les Ã©quipes ?");
  }
}

void ourlabib::on_btnChatbotPoubelles_clicked() {
  lastPageIndex = 5;
  ui->stackedWidget->setCurrentIndex(8);
  if (ui->chatHistory->toPlainText().isEmpty()) {
    ui->chatHistory->append(
        "<b>Assistant:</b> Bonjour ! Gestion des poubelles ?");
  }
}

void ourlabib::on_btnRetourChatbot_clicked() {
  ui->stackedWidget->setCurrentIndex(lastPageIndex);
}

void ourlabib::appendBotMessage(const QString &message) {
    ui->chatHistory->append("<b>Assistant:</b> " + message);
}

void ourlabib::appendErrorMessage(const QString &message) {
    ui->chatHistory->append("<b style='color:red;'>Erreur:</b> " + message);
}

void ourlabib::onResponseReceived(const QString &response) {
    appendBotMessage(response);
}

void ourlabib::onErrorOccurred(const QString &error) {
    appendErrorMessage(error);
}

void ourlabib::onChatbotProcessingFinished() {
    // Si vous avez un bouton d'envoi ou un champ de saisie, vous pouvez le réactiver ici
    // Exemple: ui->btnSendMessage->setEnabled(true);
}


// Deleted on_btnSendMessage_clicked

void ourlabib::on_btnSMS_clicked() { ui->stackedWidget->setCurrentIndex(10); }
void ourlabib::on_btnRetourSMS_clicked() {
  ui->stackedWidget->setCurrentIndex(3);
}



void ourlabib::on_btnQRCode_clicked() {
    QModelIndex idx = ui->tableViewPoubelles->currentIndex();
    if (!idx.isValid()) {
        QMessageBox::warning(this, "Attention", "Veuillez d'abord sélectionner une poubelle.");
        return;
    }

    auto model = ui->tableViewPoubelles->model();
    int row = idx.row();

    QString idPoubelle   = model->index(row, 0).data().toString();
    QString type         = model->index(row, 1).data().toString();
    QString capacite     = model->index(row, 2).data().toString();
    QString remplissage  = model->index(row, 3).data().toString();
    QString localisation = model->index(row, 4).data().toString();
    QString etat         = model->index(row, 5).data().toString();
    QString dateVidange  = model->index(row, 6).data().toString();

    // URL Google Maps
    QString locEncoded = QUrl::toPercentEncoding(localisation);
    QString mapUrl = "https://www.google.com/maps/search/" + locEncoded;

    // --- Fenêtre d'affichage ---
    QWidget *win = new QWidget;
    win->setWindowTitle(QString("QR Code — Poubelle #%1").arg(idPoubelle));
    win->setFixedSize(600, 420);
    win->setStyleSheet("background-color: #f5f5f5;");

    QHBoxLayout *mainLayout = new QHBoxLayout(win);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setSpacing(20);

    // Label pour le QR Code (avec indicateur de chargement)
    QLabel *qrLabel = new QLabel("Chargement du QR Code...");
    qrLabel->setAlignment(Qt::AlignCenter);
    qrLabel->setFixedSize(300, 300);
    qrLabel->setStyleSheet("border: 2px solid #ccc; background: white; border-radius: 8px; color: #666; font-style: italic;");
    mainLayout->addWidget(qrLabel);

    // Layout pour les infos
    QVBoxLayout *infoLayout = new QVBoxLayout;
    infoLayout->setSpacing(6);

    auto addInfo = [&](QString lbl, QString val, QString color = "#2c3e50") {
        if (val.isEmpty()) val = "N/A";
        QLabel *l = new QLabel(QString("<b>%1</b><br><span style='color:%3'>%2</span>").arg(lbl, val, color));
        l->setStyleSheet("background:white; border-radius:6px; padding:6px; font-size:11px; color:#2c3e50; border: 1px solid #e0e0e0;");
        l->setFixedWidth(240);
        infoLayout->addWidget(l);
    };

    addInfo("🗑️ ID",          idPoubelle);
    addInfo("♻️ Type",        type);
    addInfo("📦 Capacité",    capacite + " L");
    addInfo("📊 Remplissage", remplissage + "%", remplissage.toInt() >= 80 ? "#e74c3c" : (remplissage.toInt() >= 50 ? "#f39c12" : "#27ae60"));
    addInfo("📍 Localisation", localisation);
    addInfo("🔧 État",         etat, (etat=="Plein"||etat=="Hors service") ? "#e74c3c" : "#27ae60");
    addInfo("📅 Vidange",      dateVidange);

    QPushButton *btnSave = new QPushButton("💾 Sauvegarder le QR Code");
    btnSave->setEnabled(false); // Activé une fois chargé
    btnSave->setStyleSheet("background:#95a5a6; color:white; font-weight:bold; padding:8px; border-radius:6px;");
    infoLayout->addStretch();
    infoLayout->addWidget(btnSave);
    mainLayout->addLayout(infoLayout);

    // --- APPEL API (QRServer) ---
    // Utilisation de l'API pour un scan 100% garanti
    QString apiUrl = QString("https://api.qrserver.com/v1/create-qr-code/?size=300x300&data=%1").arg(QUrl::toPercentEncoding(mapUrl));
    
    QNetworkAccessManager *manager = new QNetworkAccessManager(win);
    QNetworkRequest request((QUrl(apiUrl)));
    
    QNetworkReply *reply = manager->get(request);

    // Image temporaire pour la sauvegarde
    QImage *qrImage = new QImage();

    QObject::connect(reply, &QNetworkReply::finished, [=]() {
        if (reply->error() == QNetworkReply::NoError) {
            qrImage->loadFromData(reply->readAll());
            qrLabel->setPixmap(QPixmap::fromImage(*qrImage));
            qrLabel->setStyleSheet("border: 2px solid #2ecc71; background: white; border-radius: 8px; padding: 5px;");
            
            btnSave->setEnabled(true);
            btnSave->setStyleSheet("background:#2ecc71; color:white; font-weight:bold; padding:8px; border-radius:6px;");
        } else {
            qrLabel->setText("Erreur de chargement\n(Vérifiez votre connexion)");
            qrLabel->setStyleSheet("border: 2px solid #e74c3c; background: white; border-radius: 8px; color: red;");
        }
        reply->deleteLater();
    });

    QObject::connect(btnSave, &QPushButton::clicked, [=]() {
        QString fileName = QFileDialog::getSaveFileName(win, "Sauvegarder", QString("QRCode_Poubelle_%1.png").arg(idPoubelle), "Images PNG (*.png)");
        if (!fileName.isEmpty() && !qrImage->isNull()) {
            qrImage->save(fileName, "PNG");
            QMessageBox::information(win, "Succès", "QR Code sauvegardé !");
        }
    });

    win->show();
}
void ourlabib::on_btnRetourQRCode_clicked() {
  ui->stackedWidget->setCurrentIndex(5);
}

// ============================================================
// BOUTONS DES HEADERS (Statistiques, Export, Tri)
// ============================================================

void ourlabib::on_btnTrierEquipeHeader_clicked() { on_trieEQ_clicked(); }
void ourlabib::on_btnStatsEquipeHeader_clicked() { on_statEQ_clicked(); }
void ourlabib::on_btnExportEquipeHeader_clicked() { on_exportEQ_clicked(); }
void ourlabib::on_btnRetourStatsEquipe_clicked() {
  ui->stackedWidget->setCurrentIndex(3);
}
void ourlabib::on_btnRetourExportEquipe_clicked() {
  ui->stackedWidget->setCurrentIndex(3);
}
void ourlabib::on_btnRetourTrierEquipe_clicked() {
  ui->stackedWidget->setCurrentIndex(3);
}

void ourlabib::on_btnTrierZonesHeader_clicked() { on_triezone_clicked(); }
void ourlabib::on_btnStatsZonesHeader_clicked() { on_statzone_clicked(); }
void ourlabib::on_btnExportZonesHeader_clicked() { on_exportzone_clicked(); }
void ourlabib::on_btnRetourStatsZone_clicked() {
  ui->stackedWidget->setCurrentIndex(4);
}
void ourlabib::on_btnRetourExportZone_clicked() {
  ui->stackedWidget->setCurrentIndex(4);
}
void ourlabib::on_btnRetourTrierZone_clicked() {
  ui->stackedWidget->setCurrentIndex(4);
}

void ourlabib::on_btnTrierPoubellesHeader_clicked() {
  on_btnTrierPoubelle_clicked();
}
void ourlabib::on_btnStatsPoubellesHeader_clicked() {
  on_btnStatistiquesPoubelles_clicked();
}
void ourlabib::on_btnExportPoubellesHeader_clicked() {
  on_btnExportPoubelle_clicked();
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
    afficherStatsMissions();
}

void ourlabib::on_btnExportMission_clicked() {
    ui->stackedWidget->setCurrentIndex(17);
    exporterMissionsPDF();
}

void ourlabib::on_btnTrierMission_clicked() {
    ui->tableViewMission->setModel(missionTmp.trier("PRIORITE"));
    QMessageBox::information(this, "Tri Réussi", "Les missions ont été triées par priorité.");
}

// ========== GESTION MISSIONS (MVC - VUE) ==========

void ourlabib::on_btnEnregistrerMission_clicked() {
    if (!saisieMissionValide()) return;

    int id = ui->idMission->text().toInt();
    QString type = ui->typeMission->currentText();
    QDate date = ui->dateMission->date();
    QTime duree = ui->dureeMission->time();
    int equipe = ui->equipeMission->currentData().toInt();
    int zone = ui->zoneMission->currentData().toInt();
    QString etat = ui->etatMission->currentText();
    QString priorite = ui->prioriteMission->currentText();
    QString desc = ui->descriptionMission->toPlainText();

    Missions m(id, type, date, duree, equipe, zone, etat, priorite, desc);
    if (m.ajouter()) {
        loadMissionsCombos();
    refreshMissionsTable();
        clearMissionForm();
        QMessageBox::information(this, "SuccÃ¨s", "Mission ajoutÃ©e avec succÃ¨s");
    } else {
        QMessageBox::critical(this, "Erreur", "Ã‰chec de l'ajout (ID peut-Ãªtre dÃ©jÃ  utilisÃ©)");
    }
}

void ourlabib::on_btnModifierMission_clicked() {
    if (ui->idMission->text().isEmpty()) {
        QMessageBox::warning(this, "Erreur", "SÃ©lectionnez une mission Ã  modifier");
        return;
    }
    if (!saisieMissionValide()) return;

    missionTmp.setId(ui->idMission->text().toInt());
    missionTmp.setType(ui->typeMission->currentText());
    missionTmp.setDate(ui->dateMission->date());
    missionTmp.setDuree(ui->dureeMission->time());
    missionTmp.setEquipe(ui->equipeMission->currentData().toInt());
    missionTmp.setZone(ui->zoneMission->currentData().toInt());
    missionTmp.setEtat(ui->etatMission->currentText());
    missionTmp.setPriorite(ui->prioriteMission->currentText());
    missionTmp.setDescription(ui->descriptionMission->toPlainText());

    if (missionTmp.modifier()) {
        loadMissionsCombos();
    refreshMissionsTable();
        clearMissionForm();
        QMessageBox::information(this, "SuccÃ¨s", "Mission modifiÃ©e");
    } else {
        QMessageBox::critical(this, "Erreur", "Ã‰chec de la modification");
    }
}

void ourlabib::on_btnSupprimerMission_clicked() {
    int id = 0;
    QModelIndex index = ui->tableViewMission->selectionModel()->currentIndex();
    if (index.isValid()) {
        id = ui->tableViewMission->model()->data(ui->tableViewMission->model()->index(index.row(), 0)).toInt();
    } else if (!ui->idMission->text().isEmpty()) {
        id = ui->idMission->text().toInt();
    }

    if (id == 0) {
        QMessageBox::warning(this, "Erreur", "SÃ©lectionnez une mission");
        return;
    }

    if (QMessageBox::question(this, "Confirmation", "Supprimer la mission ID " + QString::number(id) + " ?",
                              QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        if (missionTmp.supprimer(id)) {
            loadMissionsCombos();
    refreshMissionsTable();
            clearMissionForm();
            QMessageBox::information(this, "SuccÃ¨s", "Mission supprimÃ©e");
        } else {
            QMessageBox::critical(this, "Erreur", "Ã‰chec de la suppression");
        }
    }
}

void ourlabib::on_tableViewMission_clicked(const QModelIndex &index) {
    if (!index.isValid()) return;
    int row = index.row();
    QAbstractItemModel* model = ui->tableViewMission->model();

    ui->idMission->setText(model->data(model->index(row, 0)).toString());
    ui->typeMission->setCurrentText(model->data(model->index(row, 1)).toString());
    ui->dateMission->setDate(model->data(model->index(row, 2)).toDate());
    ui->dureeMission->setTime(model->data(model->index(row, 3)).toTime());
    ui->etatMission->setCurrentText(model->data(model->index(row, 4)).toString());
    ui->prioriteMission->setCurrentText(model->data(model->index(row, 5)).toString());
    
    QString idEquipeStr = model->data(model->index(row, 6)).toString();
    for(int i = 0; i < ui->equipeMission->count(); i++) {
        if(ui->equipeMission->itemText(i).startsWith(idEquipeStr + " - ") || ui->equipeMission->itemData(i).toString() == idEquipeStr || ui->equipeMission->itemText(i) == idEquipeStr) {
            ui->equipeMission->setCurrentIndex(i);
            break;
        }
    }
    
    QString idZoneStr = model->data(model->index(row, 7)).toString();
    for(int i = 0; i < ui->zoneMission->count(); i++) {
        if(ui->zoneMission->itemText(i).startsWith(idZoneStr + " - ") || ui->zoneMission->itemData(i).toString() == idZoneStr || ui->zoneMission->itemText(i) == idZoneStr) {
            ui->zoneMission->setCurrentIndex(i);
            break;
        }
    }
}

void ourlabib::on_btnAnnulerMission_clicked() { clearMissionForm(); }
void ourlabib::on_btnRefreshMission_clicked() { loadMissionsCombos();
    refreshMissionsTable(); }
void ourlabib::on_btnRechercherMission_clicked() {
    QString val = ui->rechercheMission->text();
    if (val.isEmpty()) {
        loadMissionsCombos();
    refreshMissionsTable();
        return;
    }
    QSqlQueryModel* model = missionTmp.rechercher(val);
    if (model->rowCount() == 0) {
        QMessageBox::information(this, "Recherche", "Aucune mission trouvee pour : " + val);
        loadMissionsCombos();
    refreshMissionsTable();
    } else {
        ui->tableViewMission->setModel(model);
    }
}

void ourlabib::refreshMissionsTable() {
    ui->tableViewMission->setModel(missionTmp.afficher());
}

void ourlabib::clearMissionForm() {
    ui->idMission->clear();
    ui->typeMission->setCurrentIndex(0);
    ui->dateMission->setDate(QDate::currentDate());
    ui->dureeMission->setTime(QTime(1, 0));
    ui->equipeMission->setCurrentIndex(0);
    ui->zoneMission->setCurrentIndex(0);
    ui->etatMission->setCurrentIndex(0);
    ui->prioriteMission->setCurrentIndex(0);
    ui->descriptionMission->clear();
}

bool ourlabib::saisieMissionValide() {
    if (ui->idMission->text().isEmpty() || ui->idMission->text().toInt() <= 0) {
        QMessageBox::warning(this, "Erreur", "ID invalide");
        return false;
    }
    return true;
}void ourlabib::on_btnTrierEmployeHeader_clicked() {
  ui->stackedWidget->setCurrentIndex(15);
}
void ourlabib::on_btnStatsEmployeHeader_clicked() {
  afficherStatsEmployes();
  ui->stackedWidget->setCurrentIndex(13);
}
void ourlabib::on_btnExportEmployeHeader_clicked() {
  ui->stackedWidget->setCurrentIndex(14);
}

// ========== GESTION EMPLOYÃ‰S (VUE) ==========

// Bloc dÃ©placÃ© au dÃ©but du fichier pour visibilitÃ© globale

void ourlabib::loadEquipes() {
    Connection *c = Connection::instance();
    if (!c->getDatabase().isOpen()) {
        qDebug() << "Base de données non ouverte!";
        return;
    }

    QSqlQuery query(c->getDatabase());
    QString queryStr = "SELECT ID_EQUIPE, NOM_EQUIPE FROM EQUIPES ORDER BY NOM_EQUIPE";
    
    if (!query.exec(queryStr)) {
        // Fallback singular
        queryStr = "SELECT ID_EQUIPE, NOM_EQUIPE FROM EQUIPE ORDER BY NOM_EQUIPE";
        if (!query.exec(queryStr)) {
            qDebug() << "Erreur chargement équipes:" << query.lastError().text();
            return;
        }
    }

    ui->comboBoxEquipeEmploye->clear();
    while (query.next()) {
        int id = query.value(0).toInt();
        QString nom = query.value(1).toString();
        ui->comboBoxEquipeEmploye->addItem(nom, id);
    }

    if (ui->comboBoxEquipeEmploye->count() == 0) {
        ui->comboBoxEquipeEmploye->addItem("Aucune équipe", 0);
    }
}

// ============================================================
// MÃ‰THODE POUR AFFICHER LES STATISTIQUES AVEC GRAPHIQUE Ã€ BARRES
// ============================================================
void ourlabib::on_btnStatistiquesEmployes_clicked() {
  ui->stackedWidget->setCurrentIndex(13); // Page des statistiques
  afficherStatsEmployes();                // Appeler la mÃ©thode d'affichage
}

// ============================================================
// TOUS LES AUTRES SLOTS EXISTANTS (gardez votre code actuel)
// ============================================================

void ourlabib::on_btnRetourEmployes_clicked() {
  ui->stackedWidget->setCurrentIndex(1);
}

// Nous avons replacÃ© on_btnExportEmploye_clicked et on_btnTrierEmploye_clicked
// Ã  la fin du fichier.

void ourlabib::on_btnRetourStatsEmploye_clicked() {
  ui->stackedWidget->setCurrentIndex(2);
}

void ourlabib::on_btnRetourExportEmploye_clicked() {
  ui->stackedWidget->setCurrentIndex(2);
}

void ourlabib::on_btnRetourTrierEmploye_clicked() {
  ui->stackedWidget->setCurrentIndex(2);
}

void ourlabib::on_tableEmployes_clicked(const QModelIndex &index) {
  QSqlQueryModel *model =
      qobject_cast<QSqlQueryModel *>(ui->tableEmployes->model());
  if (!model)
    return;

  int row = index.row();
  selectedEmployeId = model->data(model->index(row, 0)).toInt();
  ui->idEmploye->setText(model->data(model->index(row, 0)).toString());

  ui->nomEmploye->setText(model->data(model->index(row, 1)).toString());
  ui->emailEmploye->setText(model->data(model->index(row, 2)).toString());

  QString poste = model->data(model->index(row, 3)).toString();
  ui->posteEmploye->setCurrentText(poste);

  QString dispo = model->data(model->index(row, 4)).toString();
  ui->disponibiliteEmploye->setCurrentText(dispo);

  ui->heuresTravailEmploye->setValue(model->data(model->index(row, 5)).toInt());
  ui->salaireEmploye->setValue(model->data(model->index(row, 6)).toDouble());

   QString sexe = model->data(model->index(row, 7)).toString();
   if (sexe == "Homme")
     ui->sexeEmploye->setCurrentText("Masculin");
   else if (sexe == "Femme")
     ui->sexeEmploye->setCurrentText("Féminin");
   else
     ui->sexeEmploye->setCurrentText("Non spécifié");

   // RFID UID
   ui->rfidUidEmploye->setText(model->data(model->index(row, 8)).toString());

   QString nomEquipe = model->data(model->index(row, 9)).toString();
  int indexEquipe = ui->comboBoxEquipeEmploye->findText(nomEquipe);
  if (indexEquipe >= 0)
    ui->comboBoxEquipeEmploye->setCurrentIndex(indexEquipe);
}

void ourlabib::on_btnEnregistrerEmploye_clicked() {
   QString nom = ui->nomEmploye->text().trimmed();
   QString email = ui->emailEmploye->text().trimmed();
   QString poste = ui->posteEmploye->currentText();
   QString dispo = ui->disponibiliteEmploye->currentText();
   int heures = ui->heuresTravailEmploye->value();
   double sal = ui->salaireEmploye->value();

   QString sexe;
   if (ui->sexeEmploye->currentText() == "Masculin") {
     sexe = "Homme";
   } else if (ui->sexeEmploye->currentText() == "Féminin") {
     sexe = "Femme";
   } else {
     sexe = "Homme";
   }

   QString rfid = ui->rfidUidEmploye->text().trimmed();

   int idEquipe = ui->comboBoxEquipeEmploye->currentData().toInt();

   if (nom.isEmpty() || email.isEmpty() || poste.isEmpty()) {
     QMessageBox::warning(this, "Champs manquants",
                          "Les champs Nom, Email et Poste sont obligatoires !");
     return;
   }

  if (!email.contains("@") || !email.contains(".")) {
    QMessageBox::warning(this, "Email invalide",
                         "Veuillez saisir une adresse email valide.");
    return;
  }

  if (sal <= 0) {
    QMessageBox::warning(this, "Salaire invalide",
                         "Le salaire doit être supérieur à 0.");
    return;
  }

  if (idEquipe == 0) {
    QMessageBox::warning(this, "Équipe invalide",
                         "Veuillez sélectionner une équipe valide.");
    return;
  }

   QSqlQuery checkEmail(Connection::instance()->getDatabase());
   checkEmail.prepare("SELECT COUNT(*) FROM EMPLOYE WHERE EMAIL = :email");
   checkEmail.bindValue(":email", email);
   checkEmail.exec();
   if (checkEmail.next() && checkEmail.value(0).toInt() > 0) {
     QMessageBox::warning(this, "Email existant",
                          "Cet email est déjà utilisé par un autre employé!");
     return;
   }

   // Check duplicate RFID UID if provided
   if (!rfid.isEmpty()) {
       QSqlQuery checkRfid(Connection::instance()->getDatabase());
       checkRfid.prepare("SELECT COUNT(*) FROM EMPLOYE WHERE RFID_UID = :rfid");
       checkRfid.bindValue(":rfid", rfid);
       checkRfid.exec();
       if (checkRfid.next() && checkRfid.value(0).toInt() > 0) {
           QMessageBox::warning(this, "RFID existant",
                                "Cette carte RFID est déjà assignée à un autre employé!");
           return;
       }
   }

   Employe emp(0, nom, email, poste, dispo, heures, sal, sexe, rfid, idEquipe);
   bool ok = emp.ajouter();

   if (ok) {
     QMessageBox::information(this, "Succès", "Employé ajouté avec succès !");

     // Add RFID to Arduino's authorized list if provided
     if (!rfid.isEmpty()) {
         arduino->write_to_arduino("ADD_CARD:" + rfid + ":" + nom + "\n");
     }

     rafraichirTableEmployes(ui);
     ui->idEmploye->clear();
     ui->nomEmploye->clear();
     ui->emailEmploye->clear();
     ui->rfidUidEmploye->clear();
     ui->heuresTravailEmploye->setValue(35);
     ui->salaireEmploye->setValue(2500.0);
   } else {
    QMessageBox::critical(this, "Erreur",
                          "Échec de l'ajout. Vérifiez les données saisies.\n"
                          "(Email dupliqué ou contrainte BD non respectée)");
  }
}

void ourlabib::on_btnSupprimerEmploye_clicked() {
  if (selectedEmployeId == -1) {
    QMessageBox::warning(this, "Aucune sélection",
                         "Veuillez sélectionner un employé dans le tableau.");
    return;
  }

  QMessageBox::StandardButton rep = QMessageBox::question(
      this, "Confirmation",
      "Voulez-vous vraiment supprimer cet employé (ID = " +
          QString::number(selectedEmployeId) + ") ?",
      QMessageBox::Yes | QMessageBox::No);
  if (rep != QMessageBox::Yes)
    return;

  bool ok = empTmp.supprimer(selectedEmployeId);

  if (ok) {
    QMessageBox::information(this, "Succès", "Employé supprimé avec succès !");
    selectedEmployeId = -1;
    rafraichirTableEmployes(ui);
    ui->idEmploye->clear();
    ui->nomEmploye->clear();
    ui->emailEmploye->clear();
    ui->heuresTravailEmploye->setValue(35);
    ui->salaireEmploye->setValue(2500.0);
  } else {
    QMessageBox::critical(this, "Erreur",
                          "Échec de la suppression.\n"
                          "(L'employé est peut-être lié à une équipe)");
  }
}

void ourlabib::on_btnModifierEmploye_clicked() {
  if (selectedEmployeId == -1) {
    QMessageBox::warning(
        this, "Aucune sélection",
        "Veuillez d'abord cliquer sur un employé dans le tableau.");
    return;
  }

  QString nom = ui->nomEmploye->text().trimmed();
  QString email = ui->emailEmploye->text().trimmed();
  QString poste = ui->posteEmploye->currentText();
  QString dispo = ui->disponibiliteEmploye->currentText();
  int heures = ui->heuresTravailEmploye->value();
  double sal = ui->salaireEmploye->value();

  QString sexe;
  if (ui->sexeEmploye->currentText() == "Masculin") {
    sexe = "Homme";
  } else if (ui->sexeEmploye->currentText() == "Féminin") {
    sexe = "Femme";
  } else {
    sexe = "Homme";
  }

   int idEquipe = ui->comboBoxEquipeEmploye->currentData().toInt();
   QString rfid = ui->rfidUidEmploye->text().trimmed();

   if (nom.isEmpty() || email.isEmpty() || poste.isEmpty()) {
     QMessageBox::warning(this, "Champs manquants",
                          "Nom, Email et Poste sont obligatoires !");
     return;
   }

   // Check duplicate RFID UID if provided (excluding current employee)
   if (!rfid.isEmpty()) {
       QSqlQuery checkRfid(Connection::instance()->getDatabase());
       checkRfid.prepare("SELECT COUNT(*) FROM EMPLOYE WHERE RFID_UID = :rfid AND ID_EMPLOYE != :id");
       checkRfid.bindValue(":rfid", rfid);
       checkRfid.bindValue(":id", selectedEmployeId);
       checkRfid.exec();
       if (checkRfid.next() && checkRfid.value(0).toInt() > 0) {
           QMessageBox::warning(this, "RFID existant",
                                "Cette carte RFID est déjà assignée à un autre employé!");
           return;
       }
   }

   Employe emp(selectedEmployeId, nom, email, poste, dispo, heures, sal, sexe,
               rfid, idEquipe);
   bool ok = emp.modifier(selectedEmployeId);

   if (ok) {
     QMessageBox::information(this, "Succès", "Employé modifié avec succès !");

     // Update RFID in Arduino's authorized list if changed
     if (!rfid.isEmpty()) {
         arduino->write_to_arduino("ADD_CARD:" + rfid + ":" + nom + "\n");
     }

     rafraichirTableEmployes(ui);
   } else {
    QMessageBox::critical(this, "Erreur",
                          "Échec de la modification.\n"
                          "Vérifiez les données saisies.");
  }
}

void ourlabib::on_btnRechercherEmploye_clicked() {
  QString valeur = ui->rechercheEmploye->text().trimmed();

  if (valeur.isEmpty()) {
    rafraichirTableEmployes(ui);
    return;
  }

  QSqlQueryModel *model = empTmp.rechercher(valeur, "TOUT");
  if (model) {
      ui->tableEmployes->setModel(model);
      int nbResultats = model->rowCount();
      if (nbResultats == 0) {
          QMessageBox::information(this, "Recherche", "Aucun employé trouvé pour : " + valeur);
      }
  } else {
      QMessageBox::critical(this, "Erreur", "Problème lors de la recherche dans la base de données.");
  }
}

void ourlabib::on_btnRefreshEmploye_clicked() {
  ui->rechercheEmploye->clear();
  ui->idEmploye->clear();
  ui->nomEmploye->clear();
  ui->emailEmploye->clear();
  ui->heuresTravailEmploye->setValue(35);
  ui->salaireEmploye->setValue(2500.0);
  selectedEmployeId = -1;

  rafraichirTableEmployes(ui);
}

void ourlabib::on_btnAnnulerEmploye_clicked() {
   ui->idEmploye->clear();
   ui->nomEmploye->clear();
   ui->emailEmploye->clear();
   ui->rfidUidEmploye->clear();
   ui->heuresTravailEmploye->setValue(35);
   ui->salaireEmploye->setValue(2500.0);
   selectedEmployeId = -1;
}

void ourlabib::on_btnExportEmploye_clicked() {
  QString fileName = QFileDialog::getSaveFileName(this, "Exporter en PDF", "",
                                                  "Fichiers PDF (*.pdf)");
  if (fileName.isEmpty())
    return;

  QPdfWriter pdf(fileName);
  pdf.setPageSize(QPageSize(QPageSize::A4));
  pdf.setPageOrientation(QPageLayout::Landscape);
  QPainter painter(&pdf);

  painter.setFont(QFont("Arial", 20, QFont::Bold));
  painter.drawText(3000, 1000, "Liste des Employés");
  painter.setFont(QFont("Arial", 10));

  QSqlQueryModel *model = empTmp.afficher();
  int y = 2000;
  painter.drawText(500, y, "Nom");
  painter.drawText(2500, y, "Email");
  painter.drawText(5500, y, "Poste");
  painter.drawText(7000, y, "Disponibilité");
  painter.drawText(8500, y, "Heures");
  y += 500;

  for (int i = 0; i < model->rowCount(); ++i) {
    painter.drawText(500, y, model->data(model->index(i, 1)).toString());
    painter.drawText(2500, y, model->data(model->index(i, 2)).toString());
    painter.drawText(5500, y, model->data(model->index(i, 3)).toString());
    painter.drawText(7000, y, model->data(model->index(i, 4)).toString());
    painter.drawText(8500, y, model->data(model->index(i, 5)).toString());
    y += 400;
    if (y > 9000) {
      pdf.newPage();
      y = 500;
    }
  }

  painter.end();
  QMessageBox::information(this, "Export PDF", "L'exportation PDF a réussi !");
  QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
}

void ourlabib::on_btnTrierEmploye_clicked() {
  QSqlQueryModel *model = empTmp.trier("HEURES_TRAVAIL", "ASC");
  ui->tableEmployes->setModel(model);
  QMessageBox::information(
      this, "Tri réussi",
      "Les employés ont été triés selon leurs heures de travail.");
}

void ourlabib::afficherStatsMissions() {
    Connection *c = Connection::instance();
    if (!c->getDatabase().isOpen()) return;

    QSqlQuery query(c->getDatabase());
    // Fallback MISSIONS puis MISSION
    if (!query.exec("SELECT PRIORITE, COUNT(*) FROM MISSIONS GROUP BY PRIORITE")) {
        query.exec("SELECT PRIORITE, COUNT(*) FROM MISSION GROUP BY PRIORITE");
    }

    QPieSeries *series = new QPieSeries();
    QString prioriteMax = "";
    int maxCount = 0;

    while (query.next()) {
        QString label = query.value(0).toString().trimmed();
        if (label.isEmpty()) label = "Non définie";
        int count = query.value(1).toInt();
        series->append(label, count);

        if (count > maxCount) {
            maxCount = count;
            prioriteMax = label;
        }
    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Répartition des Missions par Priorité");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignRight);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    // --- Texte résumé : priorité majoritaire ---
    QLabel *labelResume = new QLabel(
        QString("📌 Priorité majoritaire : <b>%1</b> — Nombre de missions : <b>%2</b>")
            .arg(prioriteMax).arg(maxCount)
    );
    labelResume->setAlignment(Qt::AlignCenter);
    labelResume->setStyleSheet("font-size: 14px; color: #CC0000; padding: 8px;");

    // --- Fenêtre d'affichage ---
    QWidget *window = new QWidget;
    window->setWindowTitle("Statistiques Missions par Priorité");
    window->resize(800, 500);

    QVBoxLayout *layout = new QVBoxLayout(window);
    layout->addWidget(labelResume);
    layout->addWidget(chartView);
    window->setLayout(layout);
    window->show();
}

void ourlabib::exporterMissionsPDF() {
    QString fileName = QFileDialog::getSaveFileName(this, "Exporter Missions en PDF", "", "*.pdf");
    if (fileName.isEmpty()) return;
    if (!fileName.endsWith(".pdf")) fileName += ".pdf";

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);

    QTextDocument doc;
    QString html = "<h1 align='center'>Liste des Missions</h1><br>"
                   "<table border='1' width='100%' cellpadding='5' cellspacing='0'>"
                   "<tr><th>ID</th><th>Type</th><th>Date</th><th>Etat</th><th>Priorité</th></tr>";

    QSqlQuery query("SELECT ID_MISSION, TYPE, DATE_MISSION, ETAT, PRIORITE FROM MISSION ORDER BY ID_MISSION");
    while (query.next()) {
        html += QString("<tr><td>%1</td><td>%2</td><td>%3</td><td>%4</td><td>%5</td></tr>")
                .arg(query.value(0).toString())
                .arg(query.value(1).toString())
                .arg(query.value(2).toDate().toString("dd/MM/yyyy"))
                .arg(query.value(3).toString())
                .arg(query.value(4).toString());
    }
    html += "</table>";
    doc.setHtml(html);
    doc.print(&printer);
    QMessageBox::information(this, "Succès", "Export PDF réussi !");
}

void ourlabib::afficherStatsEmployes() {
    Connection *c = Connection::instance();
    if (!c->getDatabase().isOpen()) return;

    QSqlQuery query(c->getDatabase());
    // Fallback EMPLOYES puis EMPLOYE
    if (!query.exec("SELECT POSTE, COUNT(*) FROM EMPLOYES GROUP BY POSTE")) {
        query.exec("SELECT POSTE, COUNT(*) FROM EMPLOYE GROUP BY POSTE");
    }

    QPieSeries *series = new QPieSeries();
    QString posteMax = "";
    int maxCount = 0;

    while (query.next()) {
        QString label = query.value(0).toString().trimmed();
        if (label.isEmpty()) label = "Non défini";
        int count = query.value(1).toInt();
        series->append(label, count);

        if (count > maxCount) {
            maxCount = count;
            posteMax = label;
        }
    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Répartition des Employés par Poste");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignRight);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    // --- Texte résumé : poste majoritaire ---
    QLabel *labelResume = new QLabel(
        QString("📌 Poste majoritaire : <b>%1</b> — Nombre d'employés : <b>%2</b>")
            .arg(posteMax).arg(maxCount)
    );
    labelResume->setAlignment(Qt::AlignCenter);
    labelResume->setStyleSheet("font-size: 14px; color: #CC0000; padding: 8px;");

    // --- Fenêtre d'affichage ---
    QWidget *window = new QWidget;
    window->setWindowTitle("Statistiques Employés par Poste");
    window->resize(800, 500);

    QVBoxLayout *layout = new QVBoxLayout(window);
    layout->addWidget(labelResume);
    layout->addWidget(chartView);
    window->setLayout(layout);
    window->show();
}

void ourlabib::loadMissionsCombos() {
    Connection *c = Connection::instance();
    if (!c->getDatabase().isOpen()) return;

    // ── Chargement des équipes ──────────────────────────────────────────────
    QSqlQuery qEquipe(c->getDatabase());
    ui->equipeMission->clear();
    ui->equipeMission->addItem("-- Sélectionner une équipe --", -1);

    // Essai EQUIPES (pluriel) puis EQUIPE (singulier)
    if (!qEquipe.exec("SELECT ID_EQUIPE, NOM_EQUIPE FROM EQUIPES ORDER BY NOM_EQUIPE")) {
        qEquipe.exec("SELECT ID_EQUIPE, NOM_EQUIPE FROM EQUIPE ORDER BY NOM_EQUIPE");
    }
    while (qEquipe.next()) {
        ui->equipeMission->addItem(qEquipe.value(1).toString(), qEquipe.value(0).toInt());
    }

    // ── Chargement des zones ────────────────────────────────────────────────
    QSqlQuery qZone(c->getDatabase());
    ui->zoneMission->clear();
    ui->zoneMission->addItem("-- Sélectionner une zone --", -1);

    // Essai ZONES (pluriel) puis ZONE (singulier)
    if (!qZone.exec("SELECT ID_ZONE, NOM_ZONE FROM ZONES ORDER BY NOM_ZONE")) {
        qZone.exec("SELECT ID_ZONE, NOM_ZONE FROM ZONE ORDER BY NOM_ZONE");
    }
    while (qZone.next()) {
        ui->zoneMission->addItem(qZone.value(1).toString(), qZone.value(0).toInt());
    }
}

void ourlabib::on_btnGestionMDP_clicked() {
    ui->stackedWidget->setCurrentWidget(ui->pageGestionMDP);
    refreshDemandesTable();
}

void ourlabib::on_btnRetourMDP_clicked() {
    ui->stackedWidget->setCurrentIndex(1);
}

void ourlabib::refreshDemandesTable() {
    QSqlQueryModel *model = new QSqlQueryModel();
    
    model->setQuery("SELECT NOM_UTILISATEUR, DATE_DEMANDE FROM DEMANDES_RESET WHERE REAL_PASS IS NULL ORDER BY DATE_DEMANDE DESC");
    
    model->setHeaderData(0, Qt::Horizontal, "Identifiant");
    model->setHeaderData(1, Qt::Horizontal, "Date d'envoi");

    ui->tableViewDemandes->setModel(model);
    ui->tableViewDemandes->resizeColumnsToContents();
    ui->tableViewDemandes->horizontalHeader()->setStretchLastSection(true);
}

void ourlabib::on_tableViewDemandes_clicked(const QModelIndex &index) {
    if (!index.isValid()) return;
    QString user = ui->tableViewDemandes->model()->data(ui->tableViewDemandes->model()->index(index.row(), 0)).toString();
    ui->lineEdit_selectedUser->setText(user);
    ui->lineEdit_newPass->clear();
}

void ourlabib::on_btnValiderReset_clicked() {
    QString user = ui->lineEdit_selectedUser->text();
    QString realPass = ui->lineEdit_newPass->text(); 

    if (user.isEmpty() || realPass.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner un utilisateur et saisir son nouveau mot de passe.");
        return;
    }

    Connection *c = Connection::instance();
    QSqlQuery query(c->getDatabase());
    query.prepare("UPDATE DEMANDES_RESET SET REAL_PASS = :real "
                  "WHERE NOM_UTILISATEUR = :user AND REAL_PASS IS NULL");
    query.bindValue(":real", realPass);
    query.bindValue(":user", user);

    if (query.exec()) {
        QSqlQuery commitQuery(c->getDatabase());
        commitQuery.exec("COMMIT");
        
        QMessageBox::information(this, "Succès", 
            "La demande de réinitialisation a été validée avec succès.\n"
            "Un mot de passe temporaire a été généré et sera affiché à l'utilisateur "
            "lorsqu'il vérifiera l'état de sa demande.");

        refreshDemandesTable();
        ui->lineEdit_selectedUser->clear();
        ui->lineEdit_newPass->clear();
    } else {
        QMessageBox::critical(this, "Erreur", "Échec : " + query.lastError().text());
    }
}

void ourlabib::on_btnSupprimerDemande_clicked() {
    QString user = ui->lineEdit_selectedUser->text();
    if (user.isEmpty()) return;

    if (QMessageBox::question(this, "Suppression", "Voulez-vous supprimer cette demande ?",
                                  QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        Connection *c = Connection::instance();
        QSqlQuery delQuery(c->getDatabase());
        delQuery.prepare("DELETE FROM DEMANDES_RESET WHERE NOM_UTILISATEUR = :user");
        delQuery.bindValue(":user", user);
        
        if (delQuery.exec()) {
            QSqlQuery commitQuery(c->getDatabase());
            commitQuery.exec("COMMIT");
            refreshDemandesTable();
            ui->lineEdit_selectedUser->clear();
        }
    }
}
void ourlabib::on_btnConfigFaceID_clicked() {
#ifdef USE_OPENCV
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        QMessageBox::critical(this, "Erreur Caméra", "Impossible d'accéder à la webcam.");
        return;
    }

    cv::CascadeClassifier faceCascade;
    if (!faceCascade.load("C:/Users/LENOVO/Desktop/qt/ourlabib/OpenCV-MinGW-Build-OpenCV-4.5.5-x64/OpenCV-MinGW-Build-OpenCV-4.5.5-x64/etc/haarcascades/haarcascade_frontalface_default.xml")) {
        QMessageBox::warning(this, "Erreur", "Modèle de détection faciale introuvable.");
        return;
    }

    QMessageBox::information(this, "Enregistrement Face ID", "Veuillez fixer la caméra. Appuyez sur 'Espace' pour enregistrer votre visage.");

    cv::Mat frame, faceROI;
    while (true) {
        cap >> frame;
        if (frame.empty()) break;

        cv::Mat gray;
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
        std::vector<cv::Rect> faces;
        faceCascade.detectMultiScale(gray, faces, 1.1, 4, 0, cv::Size(100, 100));

        for (const auto& r : faces) {
            cv::rectangle(frame, r, cv::Scalar(0, 255, 0), 2);
            faceROI = gray(r);
        }

        cv::imshow("Enregistrement Face ID", frame);
        char key = (char)cv::waitKey(30);
        if (key == 27) break; // Echap
        if (key == ' ' && !faceROI.empty()) { // Espace
            cv::resize(faceROI, faceROI, cv::Size(200, 200));

            std::vector<uchar> buf;
            cv::imencode(".png", faceROI, buf);
            QByteArray data = QByteArray::fromRawData(reinterpret_cast<const char*>(buf.data()), buf.size());

            Connection *c = Connection::instance();
            QSqlQuery query(c->getDatabase());
            query.prepare("UPDATE UTILISATEUR SET FACE_DATA = :face WHERE NOM_UTILISATEUR = :user");
            query.bindValue(":face", data);
            query.bindValue(":user", currentUser);

            if (query.exec()) {
                QSqlQuery commit("COMMIT");
                commit.exec();
                QMessageBox::information(this, "Succès", "Votre visage a été enregistré avec succès !");
            } else {
                QMessageBox::critical(this, "Erreur DB", "Échec de l'enregistrement : " + query.lastError().text());
            }
            break;
        }
    }
    cv::destroyAllWindows();
    cap.release();
#else
    QMessageBox::warning(this, "Indisponible", "Veuillez activer OpenCV pour cette fonctionnalité.");
#endif
}

void ourlabib::envoyerEmailResend(QString dest, QString sujet, QString corpsHtml) {
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QUrl url("https://api.resend.com/emails");
    QNetworkRequest request(url);
    QString apiKey = "re_SHJeCokf_94GVt5ncK3aZMYdZ1ZeQQG2P";
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", "Bearer " + apiKey.toUtf8());
    QJsonObject emailData;
    emailData["from"] = "OurLabib <onboarding@resend.dev>";
    emailData["to"] = dest;
    emailData["subject"] = sujet;
    emailData["html"] = corpsHtml;
    QJsonDocument doc(emailData);
    QByteArray data = doc.toJson();
    QNetworkReply *reply = manager->post(request, data);
    connect(reply, &QNetworkReply::finished, this, [this, reply, manager]() {
        if (reply->error() == QNetworkReply::NoError) {
            QMessageBox::information(this, "Succès", "L'email a été envoyé automatiquement via Resend !");
            ui->mailDestinataire->clear();
            ui->mailObjet->clear();
            ui->mailCorps->clear();
            ui->stackedWidget->setCurrentIndex(6);
        } else {
            QString errorMsg = reply->errorString();
            QByteArray details = reply->readAll();
            QMessageBox::critical(this, "Erreur Resend", "L'envoi a échoué.\nErreur : " + errorMsg + "\nDétails : " + details);
        }
        reply->deleteLater();
        manager->deleteLater();
    });
}

void ourlabib::highlightCalendarDates() {
    if (!ui->calendarMissions) return;

    ui->calendarMissions->setDateTextFormat(QDate(), QTextCharFormat());

    QTextCharFormat format;
    format.setBackground(QColor("#e74c3c"));
    format.setForeground(Qt::white);
    format.setFontWeight(QFont::Bold);

    Connection *c = Connection::instance();
    QSqlQuery query(c->getDatabase());
    query.prepare("SELECT DISTINCT TO_CHAR(DATE_MISSION, 'YYYY-MM-DD') FROM MISSION");
    if (query.exec()) {
        while (query.next()) {
            QDate missionDate = QDate::fromString(query.value(0).toString(), "yyyy-MM-dd");
            if (missionDate.isValid()) {
                ui->calendarMissions->setDateTextFormat(missionDate, format);
            }
        }
    }
}

void ourlabib::on_calendarMissions_clicked(const QDate &date) {
    Connection *c = Connection::instance();
    QSqlQuery query(c->getDatabase());
    query.prepare("SELECT TYPE, PRIORITE, ETAT FROM MISSION WHERE TO_CHAR(DATE_MISSION, 'YYYY-MM-DD') = :date");
    query.bindValue(":date", date.toString("yyyy-MM-dd"));
    
    QString info = "Missions planifiées pour le " + date.toString("dd/MM/yyyy") + " :\n\n";
    int count = 0;
    
    if (query.exec()) {
        while (query.next()) {
            count++;
            info += "🔸 " + query.value(0).toString() + " [" + query.value(1).toString() + "]\n";
            info += "   État: " + query.value(2).toString() + "\n";
        }
    }
    
    if (count > 0) {
        QMessageBox::information(this, "Détails de la date (OURLABIB)", info);
    } else {
        QMessageBox::information(this, "Détails de la date (OURLABIB)", 
                                "📅 Aucune mission planifiée pour cette date.\n\n"
                                "Les dates avec des missions sont marquées en rouge sur le calendrier.");
     }
}

// ============================================================
// ARDUINO & RFID FUNCTIONS
// ============================================================

void ourlabib::connectArduino() {
    if (!arduino) {
        arduino = new Arduino();
    }

    int result = arduino->connect_arduino();
    if (result == 0) {
        arduinoConnected = true;
        qDebug() << "Arduino connecté avec succès sur le port" << arduino->getarduino_port_name();

        // Clear existing cards on Arduino
        arduino->write_to_arduino("CLEAR_CARDS\n");

        // Load all RFID UIDs from database into Arduino's authorized list
        QSqlQuery query(Connection::instance()->getDatabase());
        query.prepare("SELECT RFID_UID, NOM FROM EMPLOYE WHERE RFID_UID IS NOT NULL AND RFID_UID != ''");
        if (query.exec()) {
            int count = 0;
            while (query.next()) {
                QString uid = query.value(0).toString();
                QString name = query.value(1).toString();
                // Send ADD_CARD command: ADD_CARD:UID:Name
                arduino->write_to_arduino("ADD_CARD:" + uid + ":" + name + "\n");
                count++;
            }
            qDebug() << count << "cartes RFID chargées depuis la base de données";
        }

        // Signal to Arduino that we're ready to process RFID scans
        arduino->write_to_arduino("READY\n");

        // Connecter le signal readyRead du port série
        connect(arduino->getserial(), &QSerialPort::readyRead, this, [this]() {
            static QByteArray buffer;
            buffer.append(arduino->read_from_arduino());
            int newlineIndex = buffer.indexOf('\n');
            while (newlineIndex != -1) {
                QByteArray line = buffer.left(newlineIndex);
                buffer.remove(0, newlineIndex + 1);
                QString uid = QString::fromUtf8(line).trimmed();
                if (!uid.isEmpty()) {
                    onRfidRead(uid);
                }
                newlineIndex = buffer.indexOf('\n');
            }
        });

        QMessageBox::information(this, "Arduino", "Arduino connecté avec succès");
    } else {
        arduinoConnected = false;
        qDebug() << "Échec de connexion Arduino";
        QMessageBox::warning(this, "Arduino", "Arduino non détecté. Vérifiez la connexion.");
    }
}

void ourlabib::on_btnScanRfidEmploye_clicked() {
    if (!arduinoConnected) {
        QMessageBox::warning(this, "Erreur", "Arduino n'est pas connecté. Veuillez vérifier la connexion.");
        return;
    }

    ui->rfidUidEmploye->setText("En attente de scan...");
    qDebug() << "En attente de scanned RFID card...";
    // Update LCD to show scanning prompt
    arduino->write_to_arduino("LCD:Scan new card:Please present card\n");
}

void ourlabib::onRfidRead(const QString &uid) {
    if (ui->rfidUidEmploye && ui->rfidUidEmploye->text() == "En attente de scan...") {
        ui->rfidUidEmploye->setText(uid);
        arduino->write_to_arduino("LCD:Card Scanned:UID stored\n");
        return;
    } else if (ui->rfidUidEmploye && ui->rfidUidEmploye->text() == uid) {
        // Ignore duplicate scan
        return;
    }

    Employe emp;
    QSqlQuery query(Connection::instance()->getDatabase());
    query.prepare("SELECT ID_EMPLOYE, NOM FROM EMPLOYE WHERE RFID_UID = :rfid");
    query.bindValue(":rfid", uid);

    if (query.exec() && query.next()) {
        int employeeId = query.value(0).toInt();
        QString employeeName = query.value(1).toString();
        QString mission = getEmployeeMission(employeeId);
        displayWelcomeOnLcd(employeeName, mission.isEmpty() ? "Aucune mission assignée" : mission);
        QMessageBox::information(this, "Pointage", 
                                 "👤 " + employeeName + "\n" +
                                 "📋 Mission: " + mission);
    } else {
        arduino->write_to_arduino("LCD:ACCES REFUSE:Carte inconnue");
        QMessageBox::warning(this, "Pointage", "Carte RFID non enregistrée");
    }
}

QString ourlabib::getEmployeeMission(int employeeId) {
    QSqlQuery query(Connection::instance()->getDatabase());
    // Get the most recent mission for the employee's team regardless of state
    query.prepare("SELECT m.TYPE, m.DATE_MISSION "
                  "FROM EMPLOYE e "
                  "JOIN MISSION m ON e.ID_EQUIPE = m.ID_EQUIPE "
                  "WHERE e.ID_EMPLOYE = :idEmp "
                  "ORDER BY m.DATE_MISSION DESC");
    query.bindValue(":idEmp", employeeId);

    if (!query.exec()) {
        qDebug() << "SQL ERROR in getEmployeeMission:" << query.lastError().text();
        return "Erreur BD";
    }

    if (query.next()) {
        QString type = query.value(0).toString();
        QVariant dateVar = query.value(1);
        QString dateStr;
        if (dateVar.typeId() == QMetaType::QDate || dateVar.typeId() == QMetaType::QDateTime) {
            dateStr = dateVar.toDate().toString("dd/MM/yyyy");
        } else {
            // Sometime Oracle dates come as strings
            dateStr = dateVar.toString();
            // Just take the first 10 chars if it's a long datetime string
            if (dateStr.length() > 10) dateStr = dateStr.left(10);
        }
        return type + " (" + dateStr + ")";
    }
    
    return "Aucune mission planifiée";
}

void ourlabib::displayWelcomeOnLcd(const QString &name, const QString &mission) {
    if (!arduinoConnected) return;
    arduino->write_to_arduino("WELCOME:" + name + ":" + mission);
}

void ourlabib::startRfidListening() {
    if (!arduinoConnected) return;
    qDebug() << "Écoute RFID démarrée";
}

void ourlabib::stopRfidListening() {
    if (arduino && arduino->getserial()->isOpen()) {
        arduino->getserial()->close();
    }
    arduinoConnected = false;
}

//==============================================================
// CHATBOT ZONES
//==============================================================
void ourlabib::executeZoneInsertion(const QString &nom, const QString &loc, int pop, const QString &surface)
{
    Connection *c = Connection::instance();
    if (!c->getDatabase().isOpen()) {
        appendErrorMessage("⚠️ Connexion à la base de données non disponible.");
        return;
    }

    QString baseNom = nom;
    QString baseLoc = loc;

    int newId = 1;
    QSqlQuery queryMax(c->getDatabase());
    // Essai avec ZONES (pluriel) puis ZONE (singulier)
    if (queryMax.exec("SELECT MAX(ID_ZONE) FROM ZONES") && queryMax.next()) {
        newId = queryMax.value(0).toInt() + 1;
    } else if (queryMax.exec("SELECT MAX(ID_ZONE) FROM ZONE") && queryMax.next()) {
        newId = queryMax.value(0).toInt() + 1;
    }

    QString finalNom  = baseNom.isEmpty()  ? QString("Zone_Auto_%1").arg(newId) : baseNom;
    QString finalLoc  = baseLoc.isEmpty()  ? "Non spécifié" : baseLoc;
    QString finalSurf = surface.isEmpty()  ? "100.0" : surface;

    zones newZone(newId, finalNom, finalLoc, "Moyenne", pop, finalSurf, 5);
    if (newZone.ajouter()) {
        appendBotMessage("✨ La zone '" + finalNom + "' a été insérée avec succès en base de données !");
        refreshZonesTable();
    } else {
        appendErrorMessage("⚠️ Erreur lors de l'insertion de la zone. Vérifiez les données saisies.");
    }
}
void ourlabib::on_ZoneChatbot_clicked() {
    ui->stackedWidget->setCurrentIndex(4);
}
//==============================================================
// ID Twilio supprim�
//==============================================================
void ourlabib::on_btnEnvoyerSMS_clicked()
{
    QString msg = ui->smsCorps->toPlainText();
    if(msg.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez entrer un message.");
        return;
    }

    QString phone = ui->smsNumero->text().trimmed();

    // Si le champ numéro est vide, on essaie de le récupérer depuis la BD via rechEQ (fallback)
    if (phone.isEmpty()) {
        int idRecherche = ui->rechEQ->text().toInt();
        if (idRecherche > 0) {
            QSqlQuery query(Connection::instance()->getDatabase());
            query.prepare("SELECT TELEPHONE FROM EQUIPE WHERE ID_EQUIPE = :id");
            query.bindValue(":id", idRecherche);
            if(query.exec() && query.next()){
                phone = query.value(0).toString();
            }
        }
    }

    if (phone.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez entrer un numéro ou sélectionner une équipe valide.");
        return;
    }

// ID Twilio supprim�
    QString finalPhone = phone;
    if (!finalPhone.startsWith("+")) {
        QString digitsOnly = finalPhone;
        digitsOnly.remove(QRegularExpression("[^0-9]"));
        if (digitsOnly.length() == 8) {
            finalPhone = "+216" + digitsOnly;
        } else if (digitsOnly.length() == 11 && digitsOnly.startsWith("216")) {
            finalPhone = "+" + digitsOnly;
        }
    }

// ID Twilio supprim�
}

bool ourlabib::isTunisianMobileNumber(int phone_int)
{
    QString phone_str = QString::number(phone_int);
    return (phone_str.length() == 8 && (phone_str.startsWith("2") || phone_str.startsWith("5") || phone_str.startsWith("9")));
}

QString ourlabib::convertIntToInternational(int phone_int)
{
    return "+216" + QString::number(phone_int);
}

void ourlabib::sendWelcomeMessage(int phone_int, const QString& name)
{
    if(isTunisianMobileNumber(phone_int)) {
        QString internationalPhone = convertIntToInternational(phone_int);
        QString message = QString("Bienvenue dans le système de gestion des équipes, %1 !").arg(name);
// ID Twilio supprim�
    }
}

void ourlabib::onSmsSent(const QString& sid)
{
    QMessageBox::information(this, "Succès", "Message envoyé avec succès. SID: " + sid);
    ui->smsCorps->clear();
}

void ourlabib::onSmsError(const QString& errorMsg, int statusCode)
{
    QString detailedError = QString("Erreur d'envoi. Code: %1\nMessage: %2").arg(statusCode).arg(errorMsg);
    QMessageBox::critical(this, "Erreur", detailedError);
}
// ID Twilio supprim�
{
    if (reply->error() == QNetworkReply::NoError) {
        QString response = reply->readAll();
// ID Twilio supprim�
    } else {
// ID Twilio supprim�
        QString response = reply->readAll();
// ID Twilio supprim�
    }
    reply->deleteLater();
}

//==============================================================
// CHATBOT EQUIPE
//==============================================================

void ourlabib::on_chatBtnequipe_clicked()
{
    QLineEdit *input = this->findChild<QLineEdit*>("chatQstEQ");
    QTextEdit *history = this->findChild<QTextEdit*>("chatequipe");
    
    if (!input || !history) {
        qDebug() << "Widgets chatbot non trouvés dans l'onglet Équipe";
        return;
    }

    QString message = input->text().trimmed();
    if (message.isEmpty()) return;

    QString htmlUser = QString("<div style='text-align: right; color: blue;'>"
                               "<b>Vous:</b> %1</div>").arg(message.toHtmlEscaped());
    history->append(htmlUser);
    input->clear();

    chatbotEQ->sendMessage(message);
}

void ourlabib::on_chatBtnZone_clicked()
{
    QString message = ui->chatQstZone->text().trimmed();
    if (message.isEmpty()) return;

    ui->chatQstZone->clear();
    ui->chatHistory->append("<b>Vous:</b> " + message);

    zonechatbot->sendMessage(message);
}

void ourlabib::onEquipeChatbotResponse(const QString &response)
{
    QTextEdit *history = this->findChild<QTextEdit*>("chatequipe");
    if (!history) return;

    QString formattedMsg = response;
    formattedMsg.replace("\n", "<br>");
    QString htmlBot = QString("<div style='text-align: left; color: green;'>"
                              "<b>Chatbot:</b> %1</div>").arg(formattedMsg);
    history->append(htmlBot);
}

void ourlabib::onEquipeChatbotFinished()
{
    QLineEdit *input = this->findChild<QLineEdit*>("chatQstEQ");
    if (input) {
        input->setEnabled(true);
        input->setFocus();
    }
}

void ourlabib::onEquipeUpdateRequested(const QString &nomEquipe, const QString &champ, const QString &valeur)
{
    QSqlQuery qRech;
    qRech.prepare("SELECT ID_EQUIPE FROM EQUIPES WHERE NOM_EQUIPE = :nom");
    qRech.bindValue(":nom", nomEquipe);
    if (!qRech.exec() || !qRech.next()) {
        onEquipeChatbotResponse("Désolé, je ne trouve pas l'équipe '" + nomEquipe + "'.");
        return;
    }

    int id = qRech.value(0).toInt();
    QString sqlUpdate = QString("UPDATE EQUIPES SET %1 = :val WHERE ID_EQUIPE = :id").arg(champ);
    QSqlQuery qUp;
    qUp.prepare(sqlUpdate);
    qUp.bindValue(":val", valeur);
    qUp.bindValue(":id", id);

    if (qUp.exec()) {
        onEquipeChatbotResponse("Mise à jour réussie : " + champ + " de " + nomEquipe + " = " + valeur);
        refreshEquipesTable();
    } else {
        onEquipeChatbotResponse("Erreur BD lors de la mise à jour.");
    }
}

//==============================================================
// MAPS
//==============================================================

void ourlabib::initMap() {
    loadZonesOnMap();
}

void ourlabib::loadZonesOnMap() {

    // ── 1. Lire les zones depuis la BD ──────────────────────────────────────
    Connection *c = Connection::instance();
    struct ZoneInfo { QString nom, loc, prio, pop, surf, nb; };
    QList<ZoneInfo> zones_list;

    QSqlQuery query(c->getDatabase());
    QString sql = "SELECT NOM_ZONE, LOCALISATION, PRIORITE, POPULATION, SUPERFICE, NOMBRE_POUBELLE FROM ZONES";
    if (!query.exec(sql)) {
        query.exec("SELECT NOM_ZONE, LOCALISATION, PRIORITE, POPULATION, SUPERFICE, NOMBRE_POUBELLE FROM ZONE");
    }
    while (query.next()) {
        ZoneInfo z;
        z.nom  = query.value(0).toString();
        z.loc  = query.value(1).toString();
        z.prio = query.value(2).toString();
        z.pop  = query.value(3).toString();
        z.surf = query.value(4).toString();
        z.nb   = query.value(5).toString();
        zones_list.append(z);
    }

    // ── 2. Construire le JS des marqueurs ───────────────────────────────────
    QString markersJS;
    for (const ZoneInfo &z : zones_list) {
        double lat = 0, lng = 0;
        bool hasGPS = false;

        // Essai 1 : coordonnées GPS dans LOCALISATION (format "lat,lng")
        QStringList parts = z.loc.split(",");
        if (parts.size() == 2) {
            double a = parts[0].trimmed().toDouble();
            double b = parts[1].trimmed().toDouble();
            if (a != 0.0 && b != 0.0 && qAbs(a) <= 90 && qAbs(b) <= 180) {
                lat = a; lng = b; hasGPS = true;
            }
        }

        // Essai 2 : géocodage local par nom de lieu tunisien (précis)
        if (!hasGPS) {
            QString loc_lower = z.loc.toLower();
            QString nom_lower = z.nom.toLower();
            QString combined  = loc_lower + " " + nom_lower;

            struct { const char *kw; double la, lo; } places[] = {
                // ── Quartiers et lieux précis de Tunis ──
                {"tunis centre",    36.8189, 10.1658},
                {"centre ville",    36.8189, 10.1658},
                {"av habib",        36.8180, 10.1700},
                {"habib bourguiba", 36.8180, 10.1700},
                {"medina",          36.7985, 10.1658},
                {"bab souika",      36.8053, 10.1645},
                {"bab el bhar",     36.8042, 10.1811},
                {"berges du lac",   36.8433, 10.2281},
                {"lac 1",           36.8350, 10.2200},
                {"lac 2",           36.8450, 10.2500},
                {"les berges",      36.8433, 10.2281},
                {"el menzah",       36.8683, 10.1756},
                {"menzah",          36.8683, 10.1756},
                {"el manar",        36.8572, 10.1819},
                {"manar",           36.8572, 10.1819},
                {"ennasr",          36.8525, 10.2072},
                {"ennaser",         36.8525, 10.2072},
                {"el mourouj",      36.7167, 10.2167},
                {"mourouj",         36.7167, 10.2167},
                {"ben arous",       36.7481, 10.2302},
                {"bardo",           36.8103, 10.1342},
                {"la marsa",        36.8783, 10.3225},
                {"marsa",           36.8783, 10.3225},
                {"carthage",        36.8589, 10.3236},
                {"sidi bou said",   36.8694, 10.3414},
                {"hammam lif",      36.7281, 10.3314},
                {"ariana",          36.8625, 10.1956},
                {"manouba",         36.8081, 10.0956},
                {"den den",         36.8150, 10.1006},
                {"cite olympique",  36.8356, 10.1758},
                {"megrine",         36.7608, 10.2281},
                {"rades",           36.7783, 10.2733},
                {"gare centrale",   36.7980, 10.1810},
                {"gare",            36.7980, 10.1810},
                {"lycee",           36.8189, 10.1658},
                // ── Autres villes tunisiennes ──
                {"sfax",            34.7400, 10.7600},
                {"sousse",          35.8245, 10.6346},
                {"kairouan",        35.6781,  9.9941},
                {"bizerte",         37.2747,  9.8739},
                {"gabes",           33.8881, 10.0975},
                {"gafsa",           34.4250,  8.7842},
                {"monastir",        35.7643, 10.8113},
                {"djerba",          33.8075, 10.8451},
                {"nabeul",          36.4561, 10.7335},
                {"hammamet",        36.4000, 10.6167},
                {"tozeur",          33.9197,  8.1336},
                {"medenine",        33.3500, 10.5000},
                {"tataouine",       32.9214, 10.4512},
                {"kasserine",       35.1667,  8.8333},
                {"kef",             36.1747,  8.7147},
                {"siliana",         36.0850,  9.3708},
                {"mahdia",          35.5047, 11.0622},
                {nullptr, 0, 0}
            };

            for (int i = 0; places[i].kw; i++) {
                if (combined.contains(places[i].kw)) {
                    lat = places[i].la;
                    lng = places[i].lo;
                    hasGPS = true;
                    break;
                }
            }
        }

        if (!hasGPS) { lat = 33.8869; lng = 9.5375; } // centre Tunisie

        // Échapper les apostrophes pour JS
        QString nomJS  = z.nom;  nomJS.replace("'", "\\'");
        QString locJS  = z.loc;  locJS.replace("'", "\\'");
        QString prioJS = z.prio; prioJS.replace("'", "\\'");

        markersJS += QString(
            "addZoneMarker(%1, %2, '%3', '%4', '%5', '%6', '%7', '%8');\n"
        ).arg(lat, 0,'f',6).arg(lng, 0,'f',6)
         .arg(nomJS).arg(locJS).arg(prioJS)
         .arg(z.pop).arg(z.surf).arg(z.nb);
    }

    // ── 3. HTML complet ─────────────────────────────────────────────────────
    QString mapHtml = R"(<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
  <title>Carte des Zones - OurLabib</title>
  <link rel="stylesheet" href="https://unpkg.com/leaflet@1.9.4/dist/leaflet.css"/>
  <script src="https://unpkg.com/leaflet@1.9.4/dist/leaflet.js"></script>
  <style>
    * { box-sizing: border-box; margin: 0; padding: 0; }
    body { font-family: 'Segoe UI', sans-serif; background: #1a1a2e; }
    #map { height: 100vh; width: 100vw; }

    /* Panneau stats */
    #stats-panel {
      position: fixed; top: 12px; left: 50px; z-index: 1000;
      background: rgba(20,20,40,0.92); backdrop-filter: blur(8px);
      border-radius: 12px; padding: 14px 20px;
      color: #fff; font-size: 13px;
      box-shadow: 0 4px 20px rgba(0,0,0,0.5);
      border: 1px solid rgba(255,255,255,0.1);
      min-width: 200px;
    }
    #stats-panel h3 { font-size: 15px; margin-bottom: 10px; color: #a8d8ea; letter-spacing: 1px; }
    .stat-row { display: flex; justify-content: space-between; margin: 4px 0; }
    .stat-row span:last-child { font-weight: bold; color: #f8f8f8; }

    /* Légende */
    .legend {
      background: rgba(20,20,40,0.92); backdrop-filter: blur(8px);
      border-radius: 10px; padding: 12px 16px;
      color: #eee; font-size: 13px;
      box-shadow: 0 4px 20px rgba(0,0,0,0.5);
      border: 1px solid rgba(255,255,255,0.1);
      line-height: 26px;
    }
    .legend strong { display: block; margin-bottom: 6px; color: #a8d8ea; font-size: 14px; }
    .legend-dot {
      display: inline-block; width: 14px; height: 14px;
      border-radius: 50%; margin-right: 8px; vertical-align: middle;
      border: 2px solid rgba(255,255,255,0.4);
    }

    /* Popup custom */
    .zone-popup { min-width: 200px; }
    .zone-popup h4 { margin: 0 0 8px; color: #1a1a2e; font-size: 15px; }
    .zone-popup table { width: 100%; border-collapse: collapse; font-size: 12px; }
    .zone-popup td { padding: 3px 6px; border-bottom: 1px solid #eee; }
    .zone-popup td:first-child { color: #666; }
    .zone-popup td:last-child { font-weight: bold; }
    .prio-badge {
      display: inline-block; padding: 2px 8px; border-radius: 10px;
      font-size: 11px; font-weight: bold; color: #fff;
    }
  </style>
</head>
<body>
  <div id="stats-panel">
    <h3>🗺️ Zones de collecte</h3>
    <div class="stat-row"><span>Total zones</span><span id="cnt-total">0</span></div>
    <div class="stat-row"><span>🔴 Haute priorité</span><span id="cnt-haute">0</span></div>
    <div class="stat-row"><span>🟠 Moyenne</span><span id="cnt-moy">0</span></div>
    <div class="stat-row"><span>🟢 Basse</span><span id="cnt-bas">0</span></div>
    <div class="stat-row"><span>🟣 Urgente</span><span id="cnt-urg">0</span></div>
  </div>
  <div id="map"></div>
  <script>
    // Vue Tunisie complète - style CARTO comme la capture
    var map = L.map('map', { zoomControl: true }).setView([33.8869, 9.5375], 6);

    // Tuile CARTO light (fond gris/blanc - identique à la capture)
    L.tileLayer('https://{s}.basemaps.cartocdn.com/light_all/{z}/{x}/{y}{r}.png', {
      attribution: '&copy; <a href="https://www.openstreetmap.org/copyright">OpenStreetMap</a> contributors &copy; <a href="https://carto.com/attributions">CARTO</a>',
      subdomains: 'abcd',
      maxZoom: 20
    }).addTo(map);

    var counts = { total: 0, haute: 0, moy: 0, bas: 0, urg: 0 };

    function getPrioColor(p) {
      p = (p || '').toLowerCase();
      if (p.includes('haut'))   return '#e74c3c';
      if (p.includes('moyen'))  return '#f39c12';
      if (p.includes('bas'))    return '#2ecc71';
      if (p.includes('urgent')) return '#9b59b6';
      return '#95a5a6';
    }

    function makeSvgIcon(color) {
      var svg = '<svg xmlns="http://www.w3.org/2000/svg" width="36" height="46" viewBox="0 0 36 46">'
        + '<path d="M18 0 C8 0 0 8 0 18 C0 30 18 46 18 46 C18 46 36 30 36 18 C36 8 28 0 18 0Z" fill="' + color + '" stroke="white" stroke-width="2"/>'
        + '<circle cx="18" cy="18" r="8" fill="white" opacity="0.9"/>'
        + '</svg>';
      return L.divIcon({
        html: svg,
        className: '',
        iconSize: [36, 46],
        iconAnchor: [18, 46],
        popupAnchor: [0, -46]
      });
    }

    function addZoneMarker(lat, lng, nom, loc, prio, pop, surf, nb) {
      var color = getPrioColor(prio);
      var badge = '<span class="prio-badge" style="background:' + color + '">' + prio + '</span>';
      var popup = '<div class="zone-popup">'
        + '<h4>📍 ' + nom + '</h4>'
        + '<table>'
        + '<tr><td>Localisation</td><td>' + loc + '</td></tr>'
        + '<tr><td>Priorité</td><td>' + badge + '</td></tr>'
        + '<tr><td>Population</td><td>' + (pop || 'N/A') + ' hab.</td></tr>'
        + '<tr><td>Superficie</td><td>' + (surf || 'N/A') + ' km²</td></tr>'
        + '<tr><td>Poubelles</td><td>' + (nb || 'N/A') + '</td></tr>'
        + '</table></div>';

      L.marker([lat, lng], { icon: makeSvgIcon(color) })
        .addTo(map)
        .bindPopup(popup, { maxWidth: 260 });

      counts.total++;
      var p = prio.toLowerCase();
      if (p.includes('haut'))   counts.haute++;
      else if (p.includes('moyen'))  counts.moy++;
      else if (p.includes('bas'))    counts.bas++;
      else if (p.includes('urgent')) counts.urg++;
    }

)";

    mapHtml += markersJS;

    mapHtml += R"(
    // Mise à jour panneau stats
    document.getElementById('cnt-total').textContent = counts.total;
    document.getElementById('cnt-haute').textContent = counts.haute;
    document.getElementById('cnt-moy').textContent   = counts.moy;
    document.getElementById('cnt-bas').textContent   = counts.bas;
    document.getElementById('cnt-urg').textContent   = counts.urg;

    // Légende
    var legend = L.control({ position: 'bottomright' });
    legend.onAdd = function() {
      var d = L.DomUtil.create('div', 'legend');
      d.innerHTML = '<strong>🌍 Priorité des Zones</strong>'
        + '<div><span class="legend-dot" style="background:#e74c3c"></span>Haute</div>'
        + '<div><span class="legend-dot" style="background:#f39c12"></span>Moyenne</div>'
        + '<div><span class="legend-dot" style="background:#2ecc71"></span>Basse</div>'
        + '<div><span class="legend-dot" style="background:#9b59b6"></span>Urgente</div>'
        + '<div><span class="legend-dot" style="background:#95a5a6"></span>Autre</div>';
      return d;
    };
    legend.addTo(map);
  </script>
</body>
</html>
)";

    QString tempFile = QDir::tempPath() + "/map_zones.html";
    QFile file(tempFile);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << mapHtml;
        file.close();
    }

    QDesktopServices::openUrl(QUrl::fromLocalFile(tempFile));
}

// ==========================================
// CHATBOT POUBELLES
// ==========================================
void ourlabib::on_chatBtnPoubelle_clicked() {
    QString msg = ui->chatQstPoubelle->text().trimmed();
    if (msg.isEmpty()) return;

    ui->chatHistoryPoubelle->append("<br><b>Vous :</b> " + msg);
    ui->chatQstPoubelle->clear();


    QString response;
    QSqlQuery query(Connection::instance()->getDatabase());

    if (msg.contains("plein", Qt::CaseInsensitive) || msg.contains("rempli", Qt::CaseInsensitive)) {
        if (!query.exec("SELECT COUNT(*) FROM POUBELLES WHERE UPPER(ETAT) = 'PLEINE'")) {
            query.exec("SELECT COUNT(*) FROM POUBELLE WHERE UPPER(ETAT) = 'PLEINE'");
        }
        if (query.next()) {
            int nb = query.value(0).toInt();
            response = "🗑️ Il y a <b>" + QString::number(nb) + " poubelle(s) pleine(s)</b>.";
        }
    }
    else if (msg.contains("critique", Qt::CaseInsensitive) || msg.contains("urgent", Qt::CaseInsensitive)) {
        // Fallback POUBELLES / POUBELLE
        if (!query.exec("SELECT z.NOM_ZONE, ROUND(AVG(p.NIVEAU_REMPLISSAGE),1) "
                        "FROM POUBELLES p JOIN ZONES z ON p.ID_ZONE=z.ID_ZONE "
                        "GROUP BY z.NOM_ZONE ORDER BY 2 DESC")) {
            query.exec("SELECT z.NOM_ZONE, ROUND(AVG(p.NIVEAU_REMPLISSAGE),1) "
                       "FROM POUBELLE p JOIN ZONES z ON p.ID_ZONE=z.ID_ZONE "
                       "GROUP BY z.NOM_ZONE ORDER BY 2 DESC");
        }
        if (query.next()) {
            response = "🔴 Zone la plus critique : <b>" + query.value(0).toString() + "</b> avec une moyenne de remplissage de " + query.value(1).toString() + "%.";
        } else {
            response = "✅ Aucune zone critique trouvée.";
        }
    }
    else if (msg.contains("combien", Qt::CaseInsensitive) && msg.contains("poubelle", Qt::CaseInsensitive)) {
        if (!query.exec("SELECT COUNT(*) FROM POUBELLES")) {
            query.exec("SELECT COUNT(*) FROM POUBELLE");
        }
        if (query.next()) {
            response = "📊 Le nombre total de poubelles enregistrées est <b>" + query.value(0).toString() + "</b>.";
        }
    }
    else if (msg.contains("maintenance", Qt::CaseInsensitive) || msg.contains("panne", Qt::CaseInsensitive)) {
        if (!query.exec("SELECT COUNT(*) FROM POUBELLES WHERE UPPER(ETAT) IN ('EN MAINTENANCE', 'HORS SERVICE')")) {
            query.exec("SELECT COUNT(*) FROM POUBELLE WHERE UPPER(ETAT) IN ('EN MAINTENANCE', 'HORS SERVICE')");
        }
        if (query.next()) {
            response = "🔧 Il y a <b>" + query.value(0).toString() + " poubelle(s)</b> en maintenance ou hors service.";
        }
    }
    else if (msg.contains("zone", Qt::CaseInsensitive)) {
        query.exec("SELECT COUNT(*) FROM ZONES WHERE UPPER(PRIORITE) = 'HAUTE'");
        if (query.next()) {
            response = "🗺️ Il y a <b>" + query.value(0).toString() + " zone(s)</b> de haute priorité.";
        }
    }
    else if (msg.contains("bonjour", Qt::CaseInsensitive) || msg.contains("salut", Qt::CaseInsensitive)) {
        response = "👋 Bonjour ! Je suis le chatbot des poubelles. Tapez <b>aide</b> pour voir ce que je peux faire.";
    }
    else if (msg.contains("aide", Qt::CaseInsensitive) || msg.contains("help", Qt::CaseInsensitive)) {
        response = "🤖 <b>Aide - Chatbot Poubelles :</b><br>"
                   "- <i>'Plein'</i> : Connaître le nombre de poubelles pleines.<br>"
                   "- <i>'Critique'</i> : Trouver la zone la plus urgente.<br>"
                   "- <i>'Combien'</i> : Nombre total de poubelles.<br>"
                   "- <i>'Panne'</i> : Poubelles en maintenance.<br>"
                   "- <i>'Zone'</i> : Zones de haute priorité.";
    }
    else {
        response = "🤖 Je n'ai pas compris. Tapez <b>aide</b> pour voir les questions disponibles.";
    }

    ui->chatHistoryPoubelle->append("<b>Assistant :</b> " + response);
}
