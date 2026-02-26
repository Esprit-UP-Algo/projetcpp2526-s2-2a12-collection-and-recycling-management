#include "connection.h"
#include "ourlabib.h"
#include "ui_ourlabib.h"
#include <QDebug>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlTableModel>

// --- Navigation Logique (Login / Main Menu) ---

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

    // Vérification basique (sans base de données)
    if (username == "admin" && password == "admin") {
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
        QMessageBox::critical(
            this, "Erreur",
            "Nom d'utilisateur ou mot de passe incorrect! (essayez admin/admin)");
        ui->lineEdit_password->clear();
    }
}

void ourlabib::on_btnLogout_clicked() {
    ui->stackedWidget->setCurrentIndex(0); // Page Login
}

void ourlabib::on_btnQuitApp_clicked() {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Quitter",
                                  "Voulez-vous vraiment quitter l'application ?",
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        QApplication::quit();
    }
}
