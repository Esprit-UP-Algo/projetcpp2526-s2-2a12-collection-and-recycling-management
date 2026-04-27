#ifndef OURLABIB_H
#define OURLABIB_H

#include <QMainWindow>
#include <QModelIndex>
#include "missions.h"
#include "employes.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

QT_BEGIN_NAMESPACE
namespace Ui { class ourlabib; }
QT_END_NAMESPACE

class ourlabib : public QMainWindow {
    Q_OBJECT

public:
    ourlabib(QWidget *parent = nullptr);
    ~ourlabib();

private slots:
    // Navigation principale
    void on_pushButton_login_clicked();
    void on_label_forgot_linkActivated(const QString &link);
    void on_btnLogout_clicked();
    void on_btnFaceID_clicked();

    // AccÃ¨s aux pages
    void on_btnEmployes_clicked();
    void on_btnEquipes_clicked();
    void on_btnZones_clicked();
    void on_btnPoubelles_clicked();
    void on_btnMissions_clicked();

    // Retour
    void on_btnRetourEmployes_clicked();
    void on_btnRetourEquipes_clicked();
    void on_btnRetourZones_clicked();
    void on_btnRetourPoubelles_clicked();
    void on_btnRetourMissions_clicked();

    // ========== GESTION Ã‰QUIPES ==========
    void on_enrgstEQ_clicked();
    void on_modifEQ_clicked();
    void on_supEQ_clicked();
    void on_tableViewEQ_clicked(const QModelIndex &index);
    void on_anulEQ_clicked();
    void on_rechercheEQ_clicked();
    void on_refreshEQ_clicked();
    void on_trieEQ_clicked();
    void on_statEQ_clicked();
    void on_exportEQ_clicked();

    // ========== GESTION ZONES ==========
    void on_enregistrerzone_clicked();
    void on_modifzone_clicked();
    void on_supzone_clicked();
    void on_tableViewZones_activated(const QModelIndex &index);
    void on_tableViewZones_clicked(const QModelIndex &index);
    void on_annlzone_clicked();
    void on_rechzone_clicked();
    void on_refreshzone_clicked();
    void on_triezone_clicked();
    void on_statzone_clicked();
    void on_exportzone_clicked();

    // ========== GESTION POUBELLES ==========
    void on_btnEnregistrerPoubelle_clicked();
    void on_btnModifierPoubelle_clicked();
    void on_btnSupprimerPoubelle_clicked();
    void on_tableViewPoubelles_clicked(const QModelIndex &index);
    void on_btnAnnulerPoubelle_clicked();
    void on_btnRechercherPoubelle_clicked();
    void on_btnRefreshPoubelle_clicked();
    void on_btnTrierPoubelle_clicked();
    void on_btnStatistiquesPoubelles_clicked();
    void on_btnExportPoubelle_clicked();

    // Fonctions supplÃ©mentaires
    void on_btnMap_clicked();
    void on_btnRetourMap_clicked();
    void on_btnChatbot_clicked();
    void on_btnRetourChatbot_clicked();
    void on_btnSendMessage_clicked();
    void on_btnChatbotEquipes_clicked();
    void on_btnChatbotPoubelles_clicked();
    void on_btnSMS_clicked();
    void on_btnRetourSMS_clicked();
    void on_btnEnvoyerSMS_clicked();
    void on_btnCalendrier_clicked();
    void on_btnRetourCalendrier_clicked();
    void highlightCalendarDates();
    void on_calendarMissions_clicked(const QDate &date);
    void on_btnMail_clicked();
    void on_btnRetourMail_clicked();
    void on_btnEnvoyerMail_clicked();
    void on_btnQRCode_clicked();
    void on_btnRetourQRCode_clicked();
    void on_btnConfigFaceID_clicked();

    // Boutons action EmployÃ©s
    void on_btnStatistiquesEmployes_clicked();
    void on_btnExportEmploye_clicked();
    void on_btnTrierEmploye_clicked();
    void on_btnTrierEmployeHeader_clicked();
    void on_btnStatsEmployeHeader_clicked();
    void on_btnExportEmployeHeader_clicked();
    void on_btnRetourStatsEmploye_clicked();
    void on_btnRetourExportEmploye_clicked();
    void on_btnRetourTrierEmploye_clicked();

    // Boutons header Ã‰quipes
    void on_btnTrierEquipeHeader_clicked();
    void on_btnStatsEquipeHeader_clicked();
    void on_btnExportEquipeHeader_clicked();
    void on_btnRetourStatsEquipe_clicked();
    void on_btnRetourExportEquipe_clicked();
    void on_btnRetourTrierEquipe_clicked();

    // Boutons header Zones
    void on_btnTrierZonesHeader_clicked();
    void on_btnStatsZonesHeader_clicked();
    void on_btnExportZonesHeader_clicked();
    void on_btnRetourStatsZone_clicked();
    void on_btnRetourExportZone_clicked();
    void on_btnRetourTrierZone_clicked();

    // Boutons header Poubelles
    void on_btnTrierPoubellesHeader_clicked();
    void on_btnStatsPoubellesHeader_clicked();
    void on_btnExportPoubellesHeader_clicked();
    void on_btnRetourStatsPoubelle_clicked();
    void on_btnRetourExportPoubelle_clicked();
    void on_btnRetourTrierPoubelle_clicked();

    // Boutons header Missions
    void on_btnTrierMissionsHeader_clicked();
    void on_btnStatsMissionsHeader_clicked();
    void on_btnExportMissionsHeader_clicked();
    void on_btnRetourStatsMission_clicked();
    void on_btnRetourExportMission_clicked();
    void on_btnRetourTrierMission_clicked();
    void on_btnStatistiquesMissions_clicked();
    void on_btnExportMission_clicked();
    void on_btnTrierMission_clicked();

    // Login / Quit
    void on_btnQuitApp_clicked();
    void on_btnGestionMDP_clicked();
    void on_btnCheckStatusMenu_clicked();
    void on_btnRetourMDP_clicked();
    void on_btnValiderReset_clicked();
    void on_btnSupprimerDemande_clicked();
    void on_tableViewDemandes_clicked(const QModelIndex &index);

    // ========== GESTION MISSIONS ==========
    void on_btnEnregistrerMission_clicked();
    void on_btnModifierMission_clicked();
    void on_btnSupprimerMission_clicked();
    void on_tableViewMission_clicked(const QModelIndex &index);
    void on_btnAnnulerMission_clicked();
    void on_btnRechercherMission_clicked();
    void on_btnRefreshMission_clicked();


    // ========== GESTION EMPLOYÃ‰S ==========
    void loadEquipes();
    void loadMissionsCombos();
    void afficherStatsEmployes();
    void on_tableEmployes_clicked(const QModelIndex &index);
    void on_btnEnregistrerEmploye_clicked();
    void on_btnSupprimerEmploye_clicked();
    void on_btnModifierEmploye_clicked();
    void on_btnRechercherEmploye_clicked();
    void on_btnRefreshEmploye_clicked();
    void on_btnAnnulerEmploye_clicked();

private:
    Ui::ourlabib *ui;
    int lastPageIndex = 1;

    // Fonctions de rafraÃ®chissement
    void refreshEquipesTable();
    void refreshZonesTable();
    void refreshPoubellesTable();
    void refreshDemandesTable();

    // Fonctions de validation
    bool saisieEquipeValide();
    bool saisieZoneValide();

    // Fonctions de nettoyage des formulaires
    void clearEquipeForm();
    void clearZoneForm();
    void clearPoubelleForm();

    // Variables pour la session
    QString currentUser;
    QString currentRole;
    bool isLoggedIn;

    // MÃ©thodes de session
    bool loginUser(const QString& username, const QString& password);
    void logoutUser();
    bool hasAccess(const QString& module);
    void updateMenuVisibility();

    // Utilitaires Missions
    void refreshMissionsTable();
    void clearMissionForm();
    bool saisieMissionValide();
    void afficherStatsMissions();
    void exporterMissionsPDF();
    Missions missionTmp;
    void envoyerEmailResend(QString dest, QString sujet, QString corpsHtml);
};

#endif // OURLABIB_H
