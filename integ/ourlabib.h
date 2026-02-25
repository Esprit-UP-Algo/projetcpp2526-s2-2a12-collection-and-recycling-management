#ifndef OURLABIB_H
#define OURLABIB_H
#include "gestion_equipe.h"
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class ourlabib;
}
QT_END_NAMESPACE

class ourlabib : public QMainWindow
{
    Q_OBJECT

public:
    ourlabib(QWidget *parent = nullptr);
    ~ourlabib();

private slots:

    // Navigation Menu Principal
    void on_pushButton_login_clicked();
    void on_btnLogout_clicked();
    void on_btnEmployes_clicked();
    void on_btnZones_clicked();
    void on_btnPoubelles_clicked();
    void on_btnEquipes_clicked();
    void on_btnMissions_clicked();

    // Boutons Retour
    void on_btnRetourEmployes_clicked();
    void on_btnRetourZones_clicked();
    void on_btnRetourPoubelles_clicked();
    void on_btnRetourEquipes_clicked();
    void on_btnRetourMissions_clicked();

    // Map et Chatbot
    void on_btnMap_clicked();
    void on_btnChatbot_clicked();
    void on_btnRetourMap_clicked();
    void on_btnRetourChatbot_clicked();
    void on_btnSendMessage_clicked();

    // Calendrier et Mail
    void on_btnCalendrier_clicked();
    void on_btnMail_clicked();
    void on_btnRetourCalendrier_clicked();
    void on_btnRetourMail_clicked();
    void on_btnEnvoyerMail_clicked();

    // Equipes Chatbot et SMS
    void on_btnChatbotEquipes_clicked();
    void on_btnSMS_clicked();
    void on_btnRetourSMS_clicked();
    void on_btnEnvoyerSMS_clicked();

    // Poubelles Chatbot et QR Code
    void on_btnChatbotPoubelles_clicked();
    void on_btnQRCode_clicked();
    void on_btnRetourQRCode_clicked();

    // Quitter l'application
    void on_btnQuitApp_clicked();

    // Actions Management (Sort, Stats, Export) - Nouveaux Boutons Header
    void on_btnTrierEmployeHeader_clicked();
    void on_btnStatsEmployeHeader_clicked();
    void on_btnExportEmployeHeader_clicked();

    // Nouveaux boutons de la page Employés
    void on_btnStatistiquesEmployes_clicked();
    void on_btnExportEmploye_clicked();
    void on_btnTrierEmploye_clicked();

    // Boutons Retour des nouvelles pages Employé
    void on_btnRetourStatsEmploye_clicked();
    void on_btnRetourExportEmploye_clicked();
    void on_btnRetourTrierEmploye_clicked();

    void on_btnTrierZonesHeader_clicked();
    void on_btnStatsZonesHeader_clicked();
    void on_btnExportZonesHeader_clicked();

    void on_btnTrierEquipeHeader_clicked();
    void on_btnStatsEquipeHeader_clicked();
    void on_btnExportEquipeHeader_clicked();

    void on_btnTrierPoubellesHeader_clicked();
    void on_btnStatsPoubellesHeader_clicked();
    void on_btnExportPoubellesHeader_clicked();

    void on_btnTrierMissionsHeader_clicked();
    void on_btnStatsMissionsHeader_clicked();
    void on_btnExportMissionsHeader_clicked();

    // Boutons Retour Missions
    void on_btnRetourStatsMission_clicked();
    void on_btnRetourExportMission_clicked();
    void on_btnRetourTrierMission_clicked();

    // Boutons Retour Equipes
    void on_btnRetourStatsEquipe_clicked();
    void on_btnRetourExportEquipe_clicked();
    void on_btnRetourTrierEquipe_clicked();

    // Boutons Retour Zones
    void on_btnRetourStatsZone_clicked();
    void on_btnRetourExportZone_clicked();
    void on_btnRetourTrierZone_clicked();

    // Boutons Retour Poubelles
    void on_btnRetourStatsPoubelle_clicked();
    void on_btnRetourExportPoubelle_clicked();
    void on_btnRetourTrierPoubelle_clicked();

    // Boutons Action Equipes (Table)
    void on_statEQ_clicked();
    void on_exportEQ_clicked();
    void on_trieEQ_clicked();

    // Boutons Action Zones (Table)
    void on_statzone_clicked();
    void on_exportzone_clicked();
    void on_triezone_clicked();

    // Boutons Action Poubelles (Table)
    void on_btnStatistiquesPoubelles_clicked();
    void on_btnExportPoubelle_clicked();
    void on_btnTrierPoubelle_clicked();

    // Boutons Action Missions (Table)
    void on_btnStatistiquesMissions_clicked();
    void on_btnExportMission_clicked();
    void on_btnTrierMission_clicked();

    //crud Equipe
    void showEvent(QShowEvent *event) override;
    void on_enrgstEQ_clicked();
    void on_supEQ_clicked();
    void on_modifEQ_clicked();
    void on_anulEQ_clicked();
    void on_tableViewEQ_clicked(const QModelIndex &index);

    void on_refreshEQ_clicked();


    void on_rechercheEQ_clicked();

private:
    Ui::ourlabib *ui;
    Gestion_equipe equipe;
    int lastPageIndex = 1; // Stocke la page précédente pour le retour du chatbot
};
#endif // OURLABIB_H
