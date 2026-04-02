#include "connection.h"
#include "ourlabib.h"
#include "ui_ourlabib.h"
#include <QDebug>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QInputDialog>
#include <QSqlError>

bool ourlabib::loginUser(const QString &username, const QString &password) {
    Connection *c = Connection::instance();
    if (!c->getDatabase().isOpen())
        return false;

    QSqlQuery query(c->getDatabase());
    query.prepare("SELECT ROLE FROM UTILISATEUR WHERE LOWER(NOM_UTILISATEUR) = LOWER(:user) "
                  "AND MOT_DE_PASSE = :pass");
    query.bindValue(":user", username);
    query.bindValue(":pass", password);

    if (!query.exec()) {
        qDebug() << "Erreur login:" << query.lastError().text();
        return false;
    }

    if (query.next()) {
        currentUser = username;
        currentRole = query.value(0).toString();
        isLoggedIn = true;
        return true;
    }
    return false;
}

void ourlabib::logoutUser() {
    currentUser = "";
    currentRole = "";
    isLoggedIn = false;
}

bool ourlabib::hasAccess(const QString &module) {
    if (currentRole == "ADMIN")
        return true;

    if (currentRole == "CHEF_EQUIPE") {
        if (module == "EMPLOYES")
            return false;
        if (module == "ZONES")
            return true;
        if (module == "EQUIPES")
            return true;
        if (module == "MISSIONS")
            return true;
        if (module == "POUBELLES")
            return false;
        return false;
    }

    if (currentRole == "RH") {
        if (module == "EMPLOYES")
            return true;
        return false;
    }

    if (currentRole == "AGENT") {
        if (module == "POUBELLES")
            return true;
        return false;
    }

    return false;
}

void ourlabib::updateMenuVisibility() {
    ui->btnEmployes->setVisible(hasAccess("EMPLOYES"));
    ui->btnZones->setVisible(hasAccess("ZONES"));
    ui->btnEquipes->setVisible(hasAccess("EQUIPES"));
    ui->btnMissions->setVisible(hasAccess("MISSIONS"));
    ui->btnPoubelles->setVisible(hasAccess("POUBELLES"));
}

void ourlabib::on_pushButton_login_clicked() {
    QString username = ui->lineEdit_username->text().trimmed();
    QString password = ui->lineEdit_password->text().trimmed();
    bool isNotRobot = ui->checkBox->isChecked();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez remplir tous les champs!");
        return;
    }

    if (!isNotRobot) {
        QMessageBox::warning(this, "Erreur", "Veuillez cocher 'I am not a robot'!");
        return;
    }

    if (loginUser(username, password)) {
        QMessageBox::information(this, "Succes", "Connexion reussie!");
        ui->lineEdit_username->clear();
        ui->lineEdit_password->clear();
        ui->checkBox->setChecked(false);
        updateMenuVisibility();
        ui->stackedWidget->setCurrentIndex(1);
    } else {
        QMessageBox::critical(this, "Erreur", "Nom d'utilisateur ou mot de passe incorrect!");
        ui->lineEdit_password->clear();
    }
}

void ourlabib::on_btnLogout_clicked() {
    logoutUser();
    ui->stackedWidget->setCurrentIndex(0);
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

void ourlabib::on_label_forgot_linkActivated(const QString &link) {
    Q_UNUSED(link);
    bool ok;
    
    QString user = QInputDialog::getText(this, "Recuperation - Etape 1", 
                                       "Identifiant utilisateur :", 
                                       QLineEdit::Normal, "", &ok);
    if (!ok || user.isEmpty()) return;

    Connection *c = Connection::instance();
    if (!c->getDatabase().isOpen()) return;

    QSqlQuery query(c->getDatabase());
    query.prepare("SELECT NOM_UTILISATEUR FROM UTILISATEUR WHERE LOWER(NOM_UTILISATEUR) = LOWER(:user)");
    query.bindValue(":user", user);

    if (query.exec() && query.next()) {
        QString actualUser = query.value(0).toString(); // Recuperer le vrai nom avec sa casse
        
        QString newPwd = QInputDialog::getText(this, "Recuperation - Etape 2", 
                                             "Nouveau mot de passe :", 
                                             QLineEdit::Password, "", &ok);
        if (!ok || newPwd.isEmpty()) return;

        QString confirmPwd = QInputDialog::getText(this, "Recuperation - Etape 3", 
                                                 "Confirmer le nouveau mot de passe :", 
                                                 QLineEdit::Password, "", &ok);
        if (!ok || confirmPwd != newPwd) {
            QMessageBox::warning(this, "Erreur", "Les mots de passe ne correspondent pas.");
            return;
        }

        QSqlQuery updateQuery(c->getDatabase());
        updateQuery.prepare("UPDATE UTILISATEUR SET MOT_DE_PASSE = :pwd WHERE NOM_UTILISATEUR = :user");
        updateQuery.bindValue(":pwd", newPwd);
        updateQuery.bindValue(":user", actualUser);

        if (updateQuery.exec()) {
            // Pour Oracle via ODBC, il faut forcer le commit si ne s'est pas fait
            QSqlQuery commitQuery(c->getDatabase());
            commitQuery.exec("COMMIT");
            
            QMessageBox::information(this, "Succes", "Votre mot de passe a ete reinitialise avec succes.");
        } else {
            QMessageBox::critical(this, "Erreur", "Une erreur est survenue : " + updateQuery.lastError().text());
        }
    } else {
        QMessageBox::warning(this, "Erreur", "Identifiant utilisateur introuvable.");
    }
}
