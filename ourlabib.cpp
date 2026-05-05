#include "ourlabib.h"
#include "connection.h"
#include "equipes.h"
#include "poubelles.h"
#include "zones.h"
#include "missions.h"
#include "employes.h"
#include "ui_ourlabib.h"
#include <QDebug>
#include <QFileDialog>
#include <QIntValidator>
#include <QItemSelectionModel>
#include <QMessageBox>
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
#include <QPainter>
#include <QImage>
#include <QFileDialog>
#include <QDateTime>





















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
  QRegularExpression regex("^[A-Za-zÃ€-Ã¿\\s'-]+$");
  QRegularExpressionValidator *textValidator =
      new QRegularExpressionValidator(regex, this);
  ui->nomEQ->setValidator(textValidator);
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

  // Page de dÃ©marrage
  ui->stackedWidget->setCurrentIndex(0);
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
// GESTION DES Ã‰QUIPES (CRUD complet)
// ============================================================

Gestion_equipe equipeTmp;

void ourlabib::refreshEquipesTable() {
  ui->tableViewEQ->setModel(equipeTmp.afficher());
  ui->tableViewEQ->resizeColumnsToContents();

  // Mettre Ã  jour les combobox de zone et de chef d'Ã©quipe Ã  partir de la base de donnÃ©es
  ui->zoneCEQ->clear();
  QSqlQuery qZone("SELECT ID_ZONE, NOM_ZONE FROM ZONES ORDER BY ID_ZONE");
  while (qZone.next()) {
      ui->zoneCEQ->addItem(qZone.value(0).toString() + " - " + qZone.value(1).toString());
  }

  ui->chefEQ->clear();
  QSqlQuery qChef("SELECT NOM FROM EMPLOYE ORDER BY NOM");
  while (qChef.next()) {
      ui->chefEQ->addItem(qChef.value(0).toString());
  }
}

bool ourlabib::saisieEquipeValide() {
  if (ui->idEQ->text().isEmpty() || ui->idEQ->text().toInt() <= 0) {
    QMessageBox::warning(this, "Erreur",
                         "L'ID doit Ãªtre un nombre supÃ©rieur Ã  0");
    return false;
  }
  if (ui->nomEQ->text().isEmpty()) {
    QMessageBox::warning(this, "Erreur", "Le nom est obligatoire");
    return false;
  }
  if (ui->zoneCEQ->currentText().isEmpty()) {
    QMessageBox::warning(this, "Erreur", "La zone couverte est obligatoire");
    return false;
  }
  if (ui->chefEQ->currentText().isEmpty()) {
    QMessageBox::warning(this, "Erreur", "Le chef d'Ã©quipe est obligatoire");
    return false;
  }
  if (ui->nbMembresEQ->value() <= 0) {
    QMessageBox::warning(this, "Erreur",
                         "Le nombre de membres doit Ãªtre supÃ©rieur Ã  0");
    return false;
  }
  return true;
}

void ourlabib::on_enrgstEQ_clicked() {
  if (!saisieEquipeValide())
    return;

  int id = ui->idEQ->text().toInt();
  QString nom = ui->nomEQ->text();
  QString zone = ui->zoneCEQ->currentText().split(" - ").first();
  QString chef = ui->chefEQ->currentText();
  int nb = ui->nbMembresEQ->value();
  QDate date = ui->dateEQ->date();
  QString statut = ui->statutEQ->currentText();

  Gestion_equipe equipe(id, nom, zone, chef, nb, date, statut);
  if (equipe.ajouter()) {
    refreshEquipesTable();
    clearEquipeForm();
    QMessageBox::information(this, "SuccÃ¨s", "Ã‰quipe ajoutÃ©e avec succÃ¨s");
  } else {
    QMessageBox::critical(this, "Erreur",
                          "Ã‰chec de l'ajout (ID peut-Ãªtre dÃ©jÃ  utilisÃ©)");
  }
}

void ourlabib::on_modifEQ_clicked() {
  if (ui->idEQ->text().isEmpty()) {
    QMessageBox::warning(this, "Erreur",
                         "Veuillez sÃ©lectionner une Ã©quipe Ã  modifier");
    return;
  }

  int id = ui->idEQ->text().toInt();
  QString nom = ui->nomEQ->text();
  QString zone = ui->zoneCEQ->currentText().split(" - ").first();
  QString chef = ui->chefEQ->currentText();
  int nb = ui->nbMembresEQ->value();
  QDate date = ui->dateEQ->date();
  QString statut = ui->statutEQ->currentText();

  Gestion_equipe e(id, nom, zone, chef, nb, date, statut);
  if (e.modifier(id)) {
    refreshEquipesTable();
    clearEquipeForm();
    QMessageBox::information(this, "SuccÃ¨s", "Ã‰quipe modifiÃ©e avec succÃ¨s");
  } else {
    QMessageBox::critical(this, "Erreur", "Ã‰chec de la modification");
  }
}

void ourlabib::on_supEQ_clicked() {
  int id = 0;
  QModelIndex index = ui->tableViewEQ->selectionModel()->currentIndex();
  if (index.isValid()) {
    id = ui->tableViewEQ->model()
             ->data(ui->tableViewEQ->model()->index(index.row(), 0))
             .toInt();
  } else if (!ui->idEQ->text().isEmpty()) {
    id = ui->idEQ->text().toInt();
  }

  if (id == 0) {
    QMessageBox::warning(this, "Erreur",
                         "Veuillez sÃ©lectionner une Ã©quipe Ã  supprimer");
    return;
  }

  if (QMessageBox::question(
          this, "Confirmation",
          "Supprimer l'Ã©quipe ID " + QString::number(id) + " ?",
          QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
    if (equipeTmp.supprimer(id)) {
      refreshEquipesTable();
      clearEquipeForm();
      QMessageBox::information(this, "SuccÃ¨s", "Ã‰quipe supprimÃ©e avec succÃ¨s");
    } else {
      QMessageBox::critical(this, "Erreur", "Ã‰chec de la suppression");
    }
  }
}

void ourlabib::on_tableViewEQ_clicked(const QModelIndex &index) {
  if (!index.isValid())
    return;

  QAbstractItemModel *model = ui->tableViewEQ->model();
  int row = index.row();

  ui->idEQ->setText(model->data(model->index(row, 0)).toString());
  ui->nomEQ->setText(model->data(model->index(row, 1)).toString());
  QString idZoneStr = model->data(model->index(row, 2)).toString();
  for(int i = 0; i < ui->zoneCEQ->count(); i++) {
      if(ui->zoneCEQ->itemText(i).startsWith(idZoneStr + " - ") || ui->zoneCEQ->itemText(i) == idZoneStr) {
          ui->zoneCEQ->setCurrentIndex(i);
          break;
      }
  }
  ui->chefEQ->setCurrentText(model->data(model->index(row, 3)).toString());
  ui->nbMembresEQ->setValue(model->data(model->index(row, 4)).toInt());

  QDate date = model->data(model->index(row, 5)).toDate();
  if (date.isValid())
    ui->dateEQ->setDate(date);

  QString statut = model->data(model->index(row, 6)).toString();
  int idx = ui->statutEQ->findText(statut);
  if (idx != -1)
    ui->statutEQ->setCurrentIndex(idx);
}

void ourlabib::on_anulEQ_clicked() { clearEquipeForm(); }

void ourlabib::clearEquipeForm() {
  ui->idEQ->clear();
  ui->nomEQ->clear();
  ui->zoneCEQ->setCurrentIndex(0);
  ui->chefEQ->setCurrentIndex(0);
  ui->nbMembresEQ->setValue(5);
  ui->dateEQ->setDate(QDate::currentDate());
  ui->statutEQ->setCurrentIndex(0);
}

void ourlabib::on_rechercheEQ_clicked() {
  if (ui->rechEQ->text().isEmpty()) {
    refreshEquipesTable();
    return;
  }

  int id = ui->rechEQ->text().toInt();
  QSqlQueryModel *model = equipeTmp.rechercher(id);
  if (model && model->rowCount() > 0) {
    ui->tableViewEQ->setModel(model);
    QMessageBox::information(this, "Recherche", "Ã‰quipe trouvÃ©e !");
  } else {
    QMessageBox::warning(this, "Recherche",
                         "Aucune Ã©quipe trouvÃ©e avec cet ID");
    refreshEquipesTable();
  }
  ui->rechEQ->clear();
}

void ourlabib::on_refreshEQ_clicked() {
  refreshEquipesTable();
  clearEquipeForm();
}

void ourlabib::on_trieEQ_clicked() {
  ui->tableViewEQ->setModel(equipeTmp.trier());
}

void ourlabib::on_statEQ_clicked() {
  QBarSet *set = new QBarSet("Membres");
  set->setColor(QColor("#9BE15D"));

  QStringList noms;
  QSqlQuery query("SELECT NOM_EQUIPE, NOMBRE_MEMBRES FROM EQUIPE ORDER BY "
                  "NOMBRE_MEMBRES DESC");
  while (query.next()) {
    noms << query.value(0).toString();
    *set << query.value(1).toInt();
  }

  if (noms.isEmpty()) {
    QMessageBox::information(this, "Information", "Aucune Ã©quipe trouvÃ©e!");
    return;
  }

  QBarSeries *series = new QBarSeries();
  series->append(set);

  QChart *chart = new QChart();
  chart->addSeries(series);
  chart->setTitle("Statistiques des Ã‰quipes");
  chart->setAnimationOptions(QChart::SeriesAnimations);

  QBarCategoryAxis *axisX = new QBarCategoryAxis();
  axisX->append(noms);
  axisX->setTitleText("Ã‰quipes");
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
  window->setWindowTitle("Statistiques des Ã‰quipes");
  window->show();
}

void ourlabib::on_exportEQ_clicked() {
  QAbstractItemModel *model = ui->tableViewEQ->model();
  if (!model || model->rowCount() == 0) {
    QMessageBox::warning(this, "Attention", "Le tableau est vide !");
    return;
  }

  QString fileName = QFileDialog::getSaveFileName(this, "Exporter en PDF", "",
                                                  "Fichiers PDF (*.pdf)");
  if (fileName.isEmpty())
    return;
  if (!fileName.endsWith(".pdf", Qt::CaseInsensitive))
    fileName += ".pdf";

  QTextDocument doc;
  QTextCursor cursor(&doc);
  cursor.insertHtml(
      "<h2 style='text-align:center;'>Liste des Ã‰quipes</h2><br>");

  int rows = model->rowCount();
  int cols = model->columnCount();

  QTextTableFormat tableFormat;
  tableFormat.setBorder(1);
  tableFormat.setCellPadding(5);
  tableFormat.setCellSpacing(0);
  tableFormat.setAlignment(Qt::AlignCenter);
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
// GESTION DES ZONES (CRUD complet)
// ============================================================

zones zoneTmp;

void ourlabib::refreshZonesTable() {
  ui->tableViewZones->setModel(zoneTmp.afficher());
  ui->tableViewZones->resizeColumnsToContents();
}

bool ourlabib::saisieZoneValide() {
  if (ui->idzone->text().isEmpty() || ui->idzone->text().toInt() <= 0) {
    QMessageBox::warning(this, "Erreur",
                         "L'ID doit Ãªtre un nombre supÃ©rieur Ã  0");
    return false;
  }
  if (ui->nbpoubelle->text().isEmpty()) {
    QMessageBox::warning(this, "Erreur",
                         "Le nombre de poubelles est obligatoire");
    return false;
  }
  if (ui->nomzone->text().isEmpty()) {
    QMessageBox::warning(this, "Erreur", "Le nom de la zone est obligatoire");
    return false;
  }
  if (ui->localisationzone->text().isEmpty()) {
    QMessageBox::warning(this, "Erreur", "La localisation est obligatoire");
    return false;
  }
  if (ui->population->text().isEmpty()) {
    QMessageBox::warning(this, "Erreur", "La population est obligatoire");
    return false;
  }
  if (ui->superfice->text().isEmpty()) {
    QMessageBox::warning(this, "Erreur", "La superficie est obligatoire");
    return false;
  }
  return true;
}

void ourlabib::on_enregistrerzone_clicked() {
  if (!saisieZoneValide())
    return;

  int id = ui->idzone->text().toInt();
  QString nom = ui->nomzone->text();
  QString local = ui->localisationzone->text();
  QString prior = ui->prioritzone->currentText();
  int pop = ui->population->text().toInt();
  double super = ui->superfice->text().replace(",", ".").toDouble();
  int nb = ui->nbpoubelle->text().toInt();

  zones z(id, nom, local, prior, pop, super, nb);
  if (z.ajouter()) {
    refreshZonesTable();
    clearZoneForm();
    QMessageBox::information(this, "SuccÃ¨s", "Zone ajoutÃ©e avec succÃ¨s");
  } else {
    QMessageBox::critical(this, "Erreur",
                          "Ã‰chec de l'ajout (ID peut-Ãªtre dÃ©jÃ  utilisÃ©)");
  }
}

void ourlabib::on_modifzone_clicked() {
  if (ui->idzone->text().isEmpty()) {
    QMessageBox::warning(this, "Erreur",
                         "Veuillez sÃ©lectionner une zone Ã  modifier");
    return;
  }

  int id = ui->idzone->text().toInt();
  QString nom = ui->nomzone->text();
  QString local = ui->localisationzone->text();
  QString prior = ui->prioritzone->currentText();
  int pop = ui->population->text().toInt();
  double super = ui->superfice->text().replace(",", ".").toDouble();
  int nb = ui->nbpoubelle->text().toInt();

  zones z(id, nom, local, prior, pop, super, nb);
  if (z.modifier(id)) {
    refreshZonesTable();
    clearZoneForm();
    QMessageBox::information(this, "SuccÃ¨s", "Zone modifiÃ©e avec succÃ¨s");
  } else {
    QMessageBox::critical(this, "Erreur", "Ã‰chec de la modification");
  }
}

void ourlabib::on_supzone_clicked() {
  int id = 0;
  QModelIndex index = ui->tableViewZones->selectionModel()->currentIndex();
  if (index.isValid()) {
    id = ui->tableViewZones->model()
             ->data(ui->tableViewZones->model()->index(index.row(), 0))
             .toInt();
  } else if (!ui->idzone->text().isEmpty()) {
    id = ui->idzone->text().toInt();
  }

  if (id == 0) {
    QMessageBox::warning(this, "Erreur",
                         "Veuillez sÃ©lectionner une zone Ã  supprimer");
    return;
  }

  if (QMessageBox::question(
          this, "Confirmation",
          "Supprimer la zone ID " + QString::number(id) + " ?",
          QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
    if (zoneTmp.supprimer(id)) {
      refreshZonesTable();
      clearZoneForm();
      QMessageBox::information(this, "SuccÃ¨s", "Zone supprimÃ©e avec succÃ¨s");
    } else {
      QMessageBox::critical(this, "Erreur", "Ã‰chec de la suppression");
    }
  }
}

void ourlabib::on_tableViewZones_activated(const QModelIndex &index) {
  on_tableViewZones_clicked(index);
}

void ourlabib::on_tableViewZones_clicked(const QModelIndex &index) {
  QAbstractItemModel *model = ui->tableViewZones->model();
  if (!model)
    return;
  int row = index.row();

  ui->idzone->setText(model->data(model->index(row, 0)).toString());
  ui->nomzone->setText(model->data(model->index(row, 1)).toString());
  ui->localisationzone->setText(model->data(model->index(row, 2)).toString());
  ui->superfice->setText(model->data(model->index(row, 3)).toString());
  ui->population->setText(model->data(model->index(row, 4)).toString());
  ui->prioritzone->setCurrentText(model->data(model->index(row, 5)).toString());
  ui->nbpoubelle->setText(model->data(model->index(row, 6)).toString());
}

void ourlabib::on_annlzone_clicked() { clearZoneForm(); }

void ourlabib::clearZoneForm() {
  ui->idzone->clear();
  ui->nomzone->clear();
  ui->localisationzone->clear();
  ui->prioritzone->setCurrentIndex(0);
  ui->population->clear();
  ui->superfice->clear();
  ui->nbpoubelle->clear();
}

void ourlabib::on_rechzone_clicked() {
  int id = ui->recherchezone->text().toInt();
  if (ui->recherchezone->text().isEmpty()) {
    refreshZonesTable();
    ui->recherchezone->clear();
    return;
  }

  QSqlQueryModel *model = zoneTmp.rechercher(id);
  if (model && model->rowCount() > 0) {
    ui->tableViewZones->setModel(model);
    QMessageBox::information(this, "Recherche", "Zone trouvÃ©e !");
  } else {
    QMessageBox::warning(this, "Recherche", "Aucune zone trouvÃ©e avec cet ID");
    refreshZonesTable();
  }
  ui->recherchezone->clear();
}

void ourlabib::on_refreshzone_clicked() {
  refreshZonesTable();
  clearZoneForm();
}

void ourlabib::on_triezone_clicked() {
  ui->tableViewZones->setModel(zoneTmp.trier());
}

void ourlabib::on_statzone_clicked() {
  QBarSet *set = new QBarSet("Population");
  set->setColor(QColor("#4CAF50"));

  QStringList noms;
  QSqlQuery query(
      "SELECT NOM_ZONE, POPULATION FROM ZONES ORDER BY POPULATION DESC");
  while (query.next()) {
    noms << query.value(0).toString();
    *set << query.value(1).toInt();
  }

  if (noms.isEmpty()) {
    QMessageBox::information(this, "Information", "Aucune zone trouvÃ©e!");
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
  cursor.insertHtml("<h2 style='text-align:center;'>Liste des Zones</h2><br>");

  int rows = model->rowCount();
  int cols = model->columnCount();

  QTextTableFormat tableFormat;
  tableFormat.setBorder(1);
  tableFormat.setCellPadding(3);
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

  QSqlQuery checkQuery(Connection::instance()->getDatabase());
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
                         "La localisation ne peut pas etre vide.");
    return;
  }

  Poubelle p(id, loc, niv, cap, type, zone, etat, date);
  if (p.modifier(id)) {
    refreshPoubellesTable();
    clearPoubelleForm();
    QMessageBox::information(this, "Succes", "Poubelle modifier avec succes !");
  } else {
    QMessageBox::critical(this, "Erreur", "echec de la modification.");
  }
}

void ourlabib::on_btnSupprimerPoubelle_clicked() {
  QModelIndexList selection =
      ui->tableViewPoubelles->selectionModel()->selectedRows();

  if (selection.isEmpty()) {
    QMessageBox::warning(this, "Attention",
                         "Veuillez selectionner au moins une poubelle.");
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

void ourlabib::on_btnMap_clicked() { ui->stackedWidget->setCurrentIndex(7); }
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
void ourlabib::on_btnQRCode_clicked() {
    QModelIndex idx = ui->tableViewPoubelles->currentIndex();
    if (!idx.isValid()) {
        QMessageBox::warning(this, "Attention",
                             "Veuillez d'abord sélectionner une poubelle.");
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

    // URL Google Maps avec le nom de la localisation (sans GPS)
    QString locEncoded = QUrl::toPercentEncoding(localisation);
    QString mapUrl = "https://www.google.com/maps/search/" + locEncoded;

    // Générer le QR code
    auto matrix = QRCodeGen::generate(mapUrl);
    int size = matrix.size();

    const int moduleSize = 8;
    const int border = 4;
    int imgSize = (size + 2 * border) * moduleSize;

    QImage qrImage(imgSize, imgSize, QImage::Format_RGB32);
    qrImage.fill(Qt::white);
    QPainter painter(&qrImage);
    painter.setBrush(Qt::black);
    painter.setPen(Qt::NoPen);

    for (int r = 0; r < size; r++)
        for (int c = 0; c < size; c++)
            if (matrix[r][c])
                painter.drawRect(
                    (border + c) * moduleSize,
                    (border + r) * moduleSize,
                    moduleSize, moduleSize);
    painter.end();

    // --- Fenêtre d'affichage ---
    QWidget *win = new QWidget;
    win->setWindowTitle(QString("QR Code — Poubelle #%1").arg(idPoubelle));
    win->setFixedSize(imgSize + 280, imgSize + 40);
    win->setStyleSheet("background-color: #f5f5f5;");

    QHBoxLayout *mainLayout = new QHBoxLayout(win);

    QLabel *qrLabel = new QLabel;
    qrLabel->setPixmap(QPixmap::fromImage(qrImage));
    qrLabel->setFixedSize(imgSize, imgSize);
    qrLabel->setStyleSheet("border: 2px solid #ccc; background: white; padding: 4px;");
    mainLayout->addWidget(qrLabel);

    QVBoxLayout *infoLayout = new QVBoxLayout;
    infoLayout->setSpacing(8);

    auto addInfo = [&](QString lbl, QString val, QString color = "#2c3e50") {
        QLabel *l = new QLabel(QString("<b>%1</b><br><span style='color:%3'>%2</span>")
                                   .arg(lbl, val, color));
        l->setStyleSheet("background:white; border-radius:6px; padding:6px; font-size:12px;");
        infoLayout->addWidget(l);
    };

    addInfo("🗑️ ID",          idPoubelle);
    addInfo("♻️ Type",        type);
    addInfo("📦 Capacité",    capacite + " L");
    addInfo("📊 Remplissage", remplissage + "%",
            remplissage.toInt() >= 80 ? "#e74c3c" :
                remplissage.toInt() >= 50 ? "#f39c12" : "#27ae60");
    addInfo("📍 Localisation", localisation);
    addInfo("🔧 État",         etat,
            etat=="Plein"||etat=="Hors service" ? "#e74c3c" : "#27ae60");
    addInfo("📅 Vidange",      dateVidange);
    addInfo("🗺️ Maps",        "<small>" + mapUrl + "</small>", "#2980b9");

    QPushButton *btnSave = new QPushButton("💾 Sauvegarder le QR Code");
    btnSave->setStyleSheet(
        "background:#2ecc71; color:white; font-weight:bold;"
        "padding:8px; border-radius:6px; font-size:13px;");
    QObject::connect(btnSave, &QPushButton::clicked, [=]() {
        QString fileName = QFileDialog::getSaveFileName(
            win, "Sauvegarder",
            QString("QRCode_Poubelle_%1.png").arg(idPoubelle),
            "Images PNG (*.png)");
        if (!fileName.isEmpty()) {
            qrImage.save(fileName, "PNG");
            QMessageBox::information(win, "Succès", "Sauvegardé !\n" + fileName);
        }
    });

    infoLayout->addStretch();
    infoLayout->addWidget(btnSave);
    mainLayout->addLayout(infoLayout);
    win->show();
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

void ourlabib::on_btnSendMessage_clicked() {
    QString message = ui->chatInput->text().trimmed();
    if (message.isEmpty())
        return;

    ui->chatHistory->append("<b style='color:#2980b9'>Vous :</b> " + message);
    ui->chatInput->clear();

    QString response;
    Connection *c = Connection::instance();
    QSqlQuery query(c->getDatabase());

    if (message.contains("plein", Qt::CaseInsensitive) ||
        message.contains("rempli", Qt::CaseInsensitive)) {
        query.exec("SELECT COUNT(*) FROM POUBELLE WHERE ETAT = 'Plein'");
        if (query.next()) {
            int nb = query.value(0).toInt();
            response = QString("🗑️ Il y a <b>%1 poubelle(s) pleine(s)</b>.").arg(nb);
            QSqlQuery q2(c->getDatabase());
            q2.exec("SELECT z.NOM_ZONE, COUNT(*) FROM POUBELLE p "
                    "JOIN ZONES z ON p.ID_ZONE = z.ID_ZONE "
                    "WHERE p.ETAT = 'Plein' GROUP BY z.NOM_ZONE");
            while (q2.next())
                response += QString("<br>• %1 : %2 poubelle(s)").arg(q2.value(0).toString()).arg(q2.value(1).toInt());
        }
    }
    else if (message.contains("critique", Qt::CaseInsensitive) ||
             message.contains("urgent", Qt::CaseInsensitive)) {
        query.exec("SELECT z.NOM_ZONE, ROUND(AVG(p.NIVEAU_REMPLISSAGE),1) "
                   "FROM POUBELLE p JOIN ZONES z ON p.ID_ZONE = z.ID_ZONE "
                   "GROUP BY z.NOM_ZONE ORDER BY 2 DESC");
        if (query.next())
            response = QString("🔴 Zone la plus critique : <b>%1</b> — remplissage moyen <b>%2%</b>.")
                           .arg(query.value(0).toString()).arg(query.value(1).toDouble());
    }
    else if (message.contains("combien", Qt::CaseInsensitive) &&
             message.contains("poubelle", Qt::CaseInsensitive)) {
        query.exec("SELECT COUNT(*) FROM POUBELLE");
        if (query.next())
            response = QString("🗑️ <b>%1 poubelles</b> enregistrées.").arg(query.value(0).toInt());
    }
    else if (message.contains("maintenance", Qt::CaseInsensitive) ||
             message.contains("panne", Qt::CaseInsensitive)) {
        query.exec("SELECT COUNT(*) FROM POUBELLE WHERE ETAT = 'En maintenance' OR ETAT = 'Hors service'");
        if (query.next())
            response = QString("🔧 <b>%1 poubelle(s)</b> en maintenance ou hors service.").arg(query.value(0).toInt());
    }
    else if (message.contains("zone", Qt::CaseInsensitive)) {
        query.exec("SELECT COUNT(*) FROM ZONES");
        if (query.next())
            response = QString("🗺️ <b>%1 zones</b> dans le système.").arg(query.value(0).toInt());
        QSqlQuery q2(c->getDatabase());
        q2.exec("SELECT NOM_ZONE FROM ZONES WHERE PRIORITE = 'Haute'");
        while (q2.next())
            response += "<br>• " + q2.value(0).toString();
    }
    else if (message.contains("equipe", Qt::CaseInsensitive)) {
        query.exec("SELECT COUNT(*) FROM EQUIPE WHERE STATUT = 'Active'");
        if (query.next())
            response = QString("👥 <b>%1 équipe(s) active(s)</b>.").arg(query.value(0).toInt());
        QSqlQuery q2(c->getDatabase());
        q2.exec("SELECT COUNT(*) FROM EQUIPE WHERE STATUT = 'En mission'");
        if (q2.next() && q2.value(0).toInt() > 0)
            response += QString("<br>🚛 <b>%1 équipe(s)</b> en mission.").arg(q2.value(0).toInt());
    }
    else if (message.contains("employ", Qt::CaseInsensitive)) {
        query.exec("SELECT COUNT(*) FROM EMPLOYE WHERE DISPONIBILITE = 'Disponible'");
        if (query.next())
            response = QString("👤 <b>%1 employé(s)</b> disponibles.").arg(query.value(0).toInt());
    }
    else if (message.contains("mission", Qt::CaseInsensitive)) {
        query.exec("SELECT COUNT(*) FROM MISSION WHERE ETAT = 'En cours'");
        if (query.next())
            response = QString("🚀 <b>%1 mission(s)</b> en cours.").arg(query.value(0).toInt());
        QSqlQuery q2(c->getDatabase());
        q2.exec("SELECT COUNT(*) FROM MISSION WHERE ETAT = 'Planifiee'");
        if (q2.next())
            response += QString("<br>📅 <b>%1 mission(s)</b> planifiée(s).").arg(q2.value(0).toInt());
    }
    else if (message.contains("bonjour", Qt::CaseInsensitive) ||
             message.contains("salut", Qt::CaseInsensitive)) {
        response = "👋 Bonjour ! Tapez <b>aide</b> pour voir ce que je peux faire.";
    }
    else if (message.contains("aide", Qt::CaseInsensitive) ||
             message.contains("help", Qt::CaseInsensitive)) {
        response = "ℹ️ <b>Vous pouvez me demander :</b><br>"
                   "• poubelles pleines / en maintenance<br>"
                   "• zone critique / urgente<br>"
                   "• combien de poubelles ?<br>"
                   "• missions en cours<br>"
                   "• équipes actives<br>"
                   "• employés disponibles";
    }
    else {
        response = "🤖 Je n'ai pas compris. Tapez <b>aide</b> pour voir les questions disponibles.";
    }

    ui->chatHistory->append("<b style='color:#27ae60'>Assistant :</b> " + response + "<br>");
}

void ourlabib::on_btnSMS_clicked() { ui->stackedWidget->setCurrentIndex(11); }
void ourlabib::on_btnRetourSMS_clicked() {
  ui->stackedWidget->setCurrentIndex(3);
}

void ourlabib::on_btnEnvoyerSMS_clicked() {
  QString num = ui->smsNumero->text().trimmed();
  QString msg = ui->smsCorps->toPlainText().trimmed();
  if (num.isEmpty() || msg.isEmpty()) {
    QMessageBox::warning(this, "Erreur",
                         "Veuillez saisir un numÃ©ro et un message.");
    return;
  }
  QMessageBox::information(this, "SMS EnvoyÃ©",
                           "SMS envoyÃ© avec succÃ¨s au : " + num);
  ui->smsNumero->clear();
  ui->smsCorps->clear();
  ui->stackedWidget->setCurrentIndex(3);
}

void ourlabib::on_btnCalendrier_clicked() {
  ui->stackedWidget->setCurrentIndex(9);
}
void ourlabib::on_btnRetourCalendrier_clicked() {
  ui->stackedWidget->setCurrentIndex(6);
}

void ourlabib::on_btnMail_clicked() { ui->stackedWidget->setCurrentIndex(10); }
void ourlabib::on_btnRetourMail_clicked() {
  ui->stackedWidget->setCurrentIndex(6);
}

void ourlabib::on_btnEnvoyerMail_clicked() {
  QString dest = ui->mailDestinataire->text().trimmed();
  QString sujet = ui->mailObjet->text().trimmed();
  QString corps = ui->mailCorps->toPlainText().trimmed();
  if (dest.isEmpty() || sujet.isEmpty() || corps.isEmpty()) {
    QMessageBox::warning(this, "Erreur", "Veuillez remplir tous les champs.");
    return;
  }
  QMessageBox::information(this, "Mail EnvoyÃ©",
                           "Mail envoyÃ© avec succÃ¨s Ã  : " + dest);
  ui->mailDestinataire->clear();
  ui->mailObjet->clear();
  ui->mailCorps->clear();
  ui->stackedWidget->setCurrentIndex(6);
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
    ui->equipeMission->setCurrentText(model->data(model->index(row, 4)).toString());
    ui->zoneMission->setCurrentText(model->data(model->index(row, 5)).toString());
    ui->etatMission->setCurrentText(model->data(model->index(row, 6)).toString());
    ui->prioriteMission->setCurrentText(model->data(model->index(row, 7)).toString());
    ui->descriptionMission->setPlainText(model->data(model->index(row, 8)).toString());
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
        qDebug() << "Base de donnÃ©es non ouverte!";
        return;
    }

    QSqlQuery query(c->getDatabase());
    query.prepare("SELECT ID_EQUIPE, NOM_EQUIPE FROM EQUIPE ORDER BY NOM_EQUIPE");

    if (!query.exec()) {
        qDebug() << "Erreur chargement Ã©quipes:" << query.lastError().text();
        return;
    }

    ui->comboBoxEquipeEmploye->clear();
    while (query.next()) {
        int id = query.value(0).toInt();
        QString nom = query.value(1).toString();
        ui->comboBoxEquipeEmploye->addItem(nom, id);
    }

    if (ui->comboBoxEquipeEmploye->count() == 0) {
        ui->comboBoxEquipeEmploye->addItem("Aucune Ã©quipe", 0);
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
    ui->sexeEmploye->setCurrentText("FÃ©minin");
  else
    ui->sexeEmploye->setCurrentText("Non spÃ©cifiÃ©");

  QString nomEquipe = model->data(model->index(row, 8)).toString();
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
  } else if (ui->sexeEmploye->currentText() == "FÃ©minin") {
    sexe = "Femme";
  } else {
    sexe = "Homme";
  }

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
                         "Le salaire doit Ãªtre supÃ©rieur Ã  0.");
    return;
  }

  if (idEquipe == 0) {
    QMessageBox::warning(this, "Ã‰quipe invalide",
                         "Veuillez sÃ©lectionner une Ã©quipe valide.");
    return;
  }

  QSqlQuery checkEmail(Connection::instance()->getDatabase());
  checkEmail.prepare("SELECT COUNT(*) FROM EMPLOYE WHERE EMAIL = :email");
  checkEmail.bindValue(":email", email);
  checkEmail.exec();
  if (checkEmail.next() && checkEmail.value(0).toInt() > 0) {
    QMessageBox::warning(this, "Email existant",
                         "Cet email est dÃ©jÃ  utilisÃ© par un autre employÃ©!");
    return;
  }

  Employe emp(0, nom, email, poste, dispo, heures, sal, sexe, idEquipe);
  bool ok = emp.ajouter();

  if (ok) {
    QMessageBox::information(this, "SuccÃ¨s", "EmployÃ© ajoutÃ© avec succÃ¨s !");
    rafraichirTableEmployes(ui);
    ui->idEmploye->clear();
    ui->nomEmploye->clear();
    ui->emailEmploye->clear();
    ui->heuresTravailEmploye->setValue(35);
    ui->salaireEmploye->setValue(2500.0);
  } else {
    QMessageBox::critical(this, "Erreur",
                          "Ã‰chec de l'ajout. VÃ©rifiez les donnÃ©es saisies.\n"
                          "(Email dupliquÃ© ou contrainte BD non respectÃ©e)");
  }
}

void ourlabib::on_btnSupprimerEmploye_clicked() {
  if (selectedEmployeId == -1) {
    QMessageBox::warning(this, "Aucune sÃ©lection",
                         "Veuillez sÃ©lectionner un employÃ© dans le tableau.");
    return;
  }

  QMessageBox::StandardButton rep = QMessageBox::question(
      this, "Confirmation",
      "Voulez-vous vraiment supprimer cet employÃ© (ID = " +
          QString::number(selectedEmployeId) + ") ?",
      QMessageBox::Yes | QMessageBox::No);
  if (rep != QMessageBox::Yes)
    return;

  bool ok = empTmp.supprimer(selectedEmployeId);

  if (ok) {
    QMessageBox::information(this, "SuccÃ¨s", "EmployÃ© supprimÃ© avec succÃ¨s !");
    selectedEmployeId = -1;
    rafraichirTableEmployes(ui);
    ui->idEmploye->clear();
    ui->nomEmploye->clear();
    ui->emailEmploye->clear();
    ui->heuresTravailEmploye->setValue(35);
    ui->salaireEmploye->setValue(2500.0);
  } else {
    QMessageBox::critical(this, "Erreur",
                          "Ã‰chec de la suppression.\n"
                          "(L'employÃ© est peut-Ãªtre liÃ© Ã  une Ã©quipe)");
  }
}

void ourlabib::on_btnModifierEmploye_clicked() {
  if (selectedEmployeId == -1) {
    QMessageBox::warning(
        this, "Aucune sÃ©lection",
        "Veuillez d'abord cliquer sur un employÃ© dans le tableau.");
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
  } else if (ui->sexeEmploye->currentText() == "FÃ©minin") {
    sexe = "Femme";
  } else {
    sexe = "Homme";
  }

  int idEquipe = ui->comboBoxEquipeEmploye->currentData().toInt();

  if (nom.isEmpty() || email.isEmpty() || poste.isEmpty()) {
    QMessageBox::warning(this, "Champs manquants",
                         "Nom, Email et Poste sont obligatoires !");
    return;
  }

  Employe emp(selectedEmployeId, nom, email, poste, dispo, heures, sal, sexe,
              idEquipe);
  bool ok = emp.modifier(selectedEmployeId);

  if (ok) {
    QMessageBox::information(this, "SuccÃ¨s", "EmployÃ© modifiÃ© avec succÃ¨s !");
    rafraichirTableEmployes(ui);
  } else {
    QMessageBox::critical(this, "Erreur",
                          "Ã‰chec de la modification.\n"
                          "VÃ©rifiez les donnÃ©es saisies.");
  }
}

void ourlabib::on_btnRechercherEmploye_clicked() {
  QString valeur = ui->rechercheEmploye->text().trimmed();

  if (valeur.isEmpty()) {
    rafraichirTableEmployes(ui);
    return;
  }

  QSqlQueryModel *model = empTmp.rechercher(valeur, "TOUT");
  ui->tableEmployes->setModel(model);

  int nbResultats = model->rowCount();
  if (nbResultats == 0)
    QMessageBox::information(this, "Recherche",
                             "Aucun employÃ© trouvÃ© pour : " + valeur);
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
  painter.drawText(3000, 1000, "Liste des EmployÃ©s");
  painter.setFont(QFont("Arial", 10));

  QSqlQueryModel *model = empTmp.afficher();
  int y = 2000;
  painter.drawText(500, y, "Nom");
  painter.drawText(2500, y, "Email");
  painter.drawText(5500, y, "Poste");
  painter.drawText(7000, y, "DisponibilitÃ©");
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
  QMessageBox::information(this, "Export PDF", "L'exportation PDF a rÃ©ussi !");
  QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
}

void ourlabib::on_btnTrierEmploye_clicked() {
  QSqlQueryModel *model = empTmp.trier("HEURES_TRAVAIL", "ASC");
  ui->tableEmployes->setModel(model);
  QMessageBox::information(
      this, "Tri rÃ©ussi",
      "Les employÃ©s ont Ã©tÃ© triÃ©s selon leurs heures de travail.");
}

void ourlabib::afficherStatsMissions() {
    QSqlQuery query;
    query.exec("SELECT ETAT, COUNT(*) FROM MISSION GROUP BY ETAT");

    QPieSeries *series = new QPieSeries();
    while (query.next()) {
        series->append(query.value(0).toString(), query.value(1).toInt());
    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Repartition des Missions par Etat");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    if (ui->layoutStatsMissionContent->count() > 0) {
        QLayoutItem* item;
        while ((item = ui->layoutStatsMissionContent->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
    }
    ui->layoutStatsMissionContent->addWidget(chartView);
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

void ourlabib::on_calendarMissions_clicked(const QDate &date) {
    ui->tableViewMission->setModel(missionTmp.rechercher(date.toString("dd/MM/yyyy")));
}

void ourlabib::afficherStatsEmployes() {
    QSqlQuery query;
    query.exec("SELECT POSTE, COUNT(*) FROM EMPLOYE GROUP BY POSTE");

    QPieSeries *series = new QPieSeries();
    while (query.next()) {
        QString label = query.value(0).toString().isEmpty() ? "Non defini" : query.value(0).toString();
        series->append(label, query.value(1).toInt());
    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Repartition des Employes par Poste");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    // Recherche de layoutStatsContent dans pageStatsEmployes
    if (ui->layoutStatsContent->count() > 0) {
        QLayoutItem* item;
        while ((item = ui->layoutStatsContent->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
    }
    ui->layoutStatsContent->addWidget(chartView);
}

void ourlabib::loadMissionsCombos() {
    Connection *c = Connection::instance();
    if (!c->getDatabase().isOpen()) return;

    QSqlQuery qEquipe(c->getDatabase());
    ui->equipeMission->clear();
    qEquipe.exec("SELECT ID_EQUIPE, NOM_EQUIPE FROM EQUIPE ORDER BY NOM_EQUIPE");
    while (qEquipe.next()) {
        ui->equipeMission->addItem(qEquipe.value(1).toString(), qEquipe.value(0).toInt());
    }

    QSqlQuery qZone(c->getDatabase());
    ui->zoneMission->clear();
    qZone.exec("SELECT ID_ZONE, NOM_ZONE FROM ZONES ORDER BY NOM_ZONE");
    while (qZone.next()) {
        ui->zoneMission->addItem(qZone.value(1).toString(), qZone.value(0).toInt());
    }
}
