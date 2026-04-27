#include "connection.h"
#include "ourlabib.h"
#include "ui_ourlabib.h"
#include <QDebug>
#include <QInputDialog>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QRandomGenerator>

#ifdef USE_OPENCV
#include <opencv2/opencv.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#endif

bool ourlabib::loginUser(const QString &username, const QString &password) {
  Connection *c = Connection::instance();
  if (!c->getDatabase().isOpen())
    return false;

  QSqlQuery query(c->getDatabase());
  query.prepare("SELECT ROLE, NOM_UTILISATEUR FROM UTILISATEUR WHERE LOWER(NOM_UTILISATEUR) = "
                "LOWER(:user) "
                "AND MOT_DE_PASSE = :pass");
  query.bindValue(":user", username);
  query.bindValue(":pass", password);

  if (!query.exec()) {
    qDebug() << "Erreur login:" << query.lastError().text();
    return false;
  }

  if (query.next()) {
    currentRole = query.value(0).toString();
    currentUser = query.value(1).toString();
    isLoggedIn = true;
    return true;
  }

  // TENTATIVE DE CONNEXION PAR MOT DE PASSE TEMPORAIRE (RESET)
  QSqlQuery resetQuery(c->getDatabase());
  resetQuery.prepare("SELECT REAL_PASS, NOM_UTILISATEUR FROM DEMANDES_RESET WHERE "
                     "LOWER(NOM_UTILISATEUR) = LOWER(:user) AND TEMP_PASS = :pass");
  resetQuery.bindValue(":user", username);
  resetQuery.bindValue(":pass", password);

  if (resetQuery.exec() && resetQuery.next()) {
    QString realPwd = resetQuery.value(0).toString();
    QString dbUsername = resetQuery.value(1).toString();

    // Si l'admin n'a pas encore défini de vrai mot de passe
    if (realPwd.isEmpty()) {
       QMessageBox::information(
           this, "Connexion Temporaire",
           "Vous êtes connecté avec un mot de passe temporaire.\n\n"
           "Vérifiez le bouton 'Ma Demande MDP' dans le menu principal "
           "plus tard pour connaître votre mot de passe définitif lorsque l'admin l'aura généré.");
    } else {
       // 1. Mettre à jour le vrai mot de passe dans UTILISATEUR
       QSqlQuery updateQuery(c->getDatabase());
       updateQuery.prepare("UPDATE UTILISATEUR SET MOT_DE_PASSE = :pwd WHERE "
                           "LOWER(NOM_UTILISATEUR) = LOWER(:user)");
       updateQuery.bindValue(":pwd", realPwd);
       updateQuery.bindValue(":user", username);
       updateQuery.exec();

       // 2. Supprimer la demande traitée
       QSqlQuery delQuery(c->getDatabase());
       delQuery.prepare(
           "DELETE FROM DEMANDES_RESET WHERE LOWER(NOM_UTILISATEUR) = LOWER(:user)");
       delQuery.bindValue(":user", username);
       delQuery.exec();

       QSqlQuery commitQuery(c->getDatabase());
       commitQuery.exec("COMMIT");

       // 3. Notification Flash pour l'utilisateur
       QMessageBox::information(
           this, "Mot de passe définitif prêt",
           "BIENVENUE !\n\nL'administrateur a traité votre compte.\n"
           "Votre nouveau mot de passe RÉEL est : " + realPwd + "\n\n"
           "Veuillez l'utiliser lors de votre prochaine connexion.\n"
           "Votre demande a été clôturée avec succès.");
    }

    currentUser = dbUsername;

    // Recuperer le vrai role pour la session
    QSqlQuery roleQuery(c->getDatabase());
    roleQuery.prepare(
        "SELECT ROLE FROM UTILISATEUR WHERE LOWER(NOM_UTILISATEUR) = LOWER(:user)");
    roleQuery.bindValue(":user", username);
    if (roleQuery.exec() && roleQuery.next()) {
      currentRole = roleQuery.value(0).toString();
    } else {
      currentRole = "AGENT";
    }

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
  ui->btnGestionMDP->setVisible(currentRole == "ADMIN");

  // Afficher le bouton "Ma Demande MDP" si l'utilisateur est connecté et 
  // a une demande en cours (avec TEMP_PASS généré et possible REAL_PASS).
  bool hasPendingRequest = false;
  Connection *c = Connection::instance();
  if (c->getDatabase().isOpen()) {
    QSqlQuery chk(c->getDatabase());
    chk.prepare(
        "SELECT COUNT(*) FROM DEMANDES_RESET WHERE NOM_UTILISATEUR = :user");
    chk.bindValue(":user", currentUser);
    if (chk.exec() && chk.next()) {
      hasPendingRequest = chk.value(0).toInt() > 0;
    }
  }
  ui->btnCheckStatusMenu->setVisible(hasPendingRequest && currentRole != "ADMIN");
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

    // Verification des demandes en attente pour l'ADMIN
    if (currentRole == "ADMIN") {
      Connection *c = Connection::instance();
      QSqlQuery checkReq(c->getDatabase());
      checkReq.exec(
          "SELECT COUNT(*) FROM DEMANDES_RESET WHERE REAL_PASS IS NULL");
      if (checkReq.next()) {
        int count = checkReq.value(0).toInt();
        if (count > 0) {
          QMessageBox::warning(
              this, "Notifications Système",
              "Attention Administrateur,\n\nIl y a " + QString::number(count) +
                  " nouvelle(s) demande(s) de réinitialisation en attente.");
        }
      }
    }
  } else {
    QMessageBox::critical(this, "Erreur",
                          "Nom d'utilisateur ou mot de passe incorrect!");
    ui->lineEdit_password->clear();
  }
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

  QString user = QInputDialog::getText(
      this, "Mot de passe oublié",
      "Identifiant utilisateur :", QLineEdit::Normal, "", &ok);
  if (!ok || user.isEmpty())
    return;

  Connection *c = Connection::instance();
  if (!c->getDatabase().isOpen())
    return;

  QSqlQuery query(c->getDatabase());
  query.prepare("SELECT NOM_UTILISATEUR FROM UTILISATEUR WHERE "
                "LOWER(NOM_UTILISATEUR) = LOWER(:user)");
  query.bindValue(":user", user);

  if (query.exec() && query.next()) {
    QString actualUser = query.value(0).toString();

    // VÉRIFIER L'ÉTAT DE LA DEMANDE AVANT D'EN CRÉER UNE NOUVELLE
    QSqlQuery checkStatus(c->getDatabase());
    checkStatus.prepare("SELECT TEMP_PASS FROM DEMANDES_RESET WHERE NOM_UTILISATEUR = :user");
    checkStatus.bindValue(":user", actualUser);
    
    if (checkStatus.exec() && checkStatus.next()) {
        QString tempPass = checkStatus.value(0).toString();
        
        // Si c'est une ancienne demande bloquée avec TEMP_PASS = NULL, on en génère un
        if (tempPass.isEmpty()) {
            tempPass = "TEMP-" + QString::number(QRandomGenerator::global()->bounded(1000, 9999));
            QSqlQuery updateTemp(c->getDatabase());
            updateTemp.prepare("UPDATE DEMANDES_RESET SET TEMP_PASS = :temp WHERE NOM_UTILISATEUR = :user");
            updateTemp.bindValue(":temp", tempPass);
            updateTemp.bindValue(":user", actualUser);
            updateTemp.exec();
            
            QSqlQuery commitQuery(c->getDatabase());
            commitQuery.exec("COMMIT");
        }
        
        QMessageBox::information(
            this, "Mot de passe temporaire",
            "Votre demande de réinitialisation est bien enregistrée.\n\n"
            "Voici votre mot de passe TEMPORAIRE :\n\n    " + tempPass + "\n\n"
            "Utilisez-le pour vous connecter sans attendre. "
            "Vous pourrez voir votre vrai mot de passe définitif dans "
            "le menu de l'application une fois que l'admin l'aura changé.");
        return;
    }

    // SI AUCUNE DEMANDE, EN CRÉER UNE NOUVELLE AVEC MDP TEMPORAIRE IMMÉDIAT
    QString autoTempPass = "TEMP-" + QString::number(QRandomGenerator::global()->bounded(1000, 9999));
    QSqlQuery insertQuery(c->getDatabase());
    insertQuery.prepare(
        "INSERT INTO DEMANDES_RESET (NOM_UTILISATEUR, TEMP_PASS, DATE_DEMANDE) "
        "VALUES (:user, :temp, CURRENT_TIMESTAMP)");
    insertQuery.bindValue(":user", actualUser);
    insertQuery.bindValue(":temp", autoTempPass);

    if (insertQuery.exec()) {
      QSqlQuery commitQuery(c->getDatabase());
      commitQuery.exec("COMMIT");

      QMessageBox::information(
          this, "Mot de passe temporaire rapide",
          "Alerte envoyée à l'administrateur !\n\n"
          "Pour ne pas stopper votre travail, voici un mot de passe temporaire immédiat :\n\n"
          "    " + autoTempPass + "\n\n"
          "Connectez-vous dès maintenant avec ce mot de passe. Vous récupérerez votre "
          "vrai mot de passe définitif dans le menu de l'application (Ma Demande MDP) "
          "une fois que l'admin l'aura changé.");
    } else {
      QMessageBox::warning(this, "Information",
                           "Veuillez contacter l'administrateur pour "
                           "réinitialiser votre mot de passe.");
    }
  } else {
    QMessageBox::warning(this, "Erreur", "Identifiant inconnu.");
  }
}

void ourlabib::on_btnCheckStatusMenu_clicked() {
  Connection *c = Connection::instance();
  if (!c->getDatabase().isOpen())
    return;

  QSqlQuery query(c->getDatabase());
  query.prepare(
      "SELECT REAL_PASS FROM DEMANDES_RESET WHERE NOM_UTILISATEUR = :user");
  query.bindValue(":user", currentUser);

  if (query.exec() && query.next()) {
    QString realPass = query.value(0).toString();

    if (realPass.isEmpty()) {
      QMessageBox::information(
          this, "Statut de votre demande",
          "Votre mot de passe définitif est EN ATTENTE.\n\n"
          "L'administrateur n'a pas encore traité votre demande pour définir votre "
          "vrai mot de passe.\nContinuez à utiliser le mot de passe temporaire pour l'instant.");
    } else {
      QMessageBox::information(
          this, "Mot de passe récupéré !",
          "L'administrateur a traité votre demande.\n\n"
          "Votre NOUVEAU mot de passe RÉEL est :\n\n"
          "    " + realPass + "\n\n"
          "La demande est clôturée. Utilisez ce vrai mot de passe à votre prochaine connexion.");
      
      // Mise à jour de la table UTILISATEUR
      QSqlQuery updateQuery(c->getDatabase());
      updateQuery.prepare("UPDATE UTILISATEUR SET MOT_DE_PASSE = :pwd WHERE LOWER(NOM_UTILISATEUR) = LOWER(:user)");
      updateQuery.bindValue(":pwd", realPass);
      updateQuery.bindValue(":user", currentUser);
      updateQuery.exec();

      // Suppression de la demande
      QSqlQuery delQuery(c->getDatabase());
      delQuery.prepare("DELETE FROM DEMANDES_RESET WHERE LOWER(NOM_UTILISATEUR) = LOWER(:user)");
      delQuery.bindValue(":user", currentUser);
      delQuery.exec();

      QSqlQuery commitQuery(c->getDatabase());
      commitQuery.exec("COMMIT");

      ui->btnCheckStatusMenu->setVisible(false);
    }
  } else {
    QMessageBox::information(
        this, "Aucune demande",
        "Vous n'avez pas de demande de réinitialisation en cours.");
    ui->btnCheckStatusMenu->setVisible(false);
  }
}

void ourlabib::on_btnFaceID_clicked() {
#ifdef USE_OPENCV
    QString username = ui->lineEdit_username->text().trimmed();
    if (username.isEmpty()) {
        QMessageBox::warning(this, "Face ID", "Veuillez d'abord saisir votre nom d'utilisateur pour vérification faciale.");
        return;
    }

    // 1. Récupérer les données faciales depuis la DB
    Connection *c = Connection::instance();
    QSqlQuery query(c->getDatabase());
    query.prepare("SELECT FACE_DATA FROM UTILISATEUR WHERE LOWER(NOM_UTILISATEUR) = LOWER(:user)");
    query.bindValue(":user", username);

    if (!query.exec() || !query.next()) {
        QMessageBox::warning(this, "Face ID", "Utilisateur inconnu ou Face ID non configuré.");
        return;
    }

    QByteArray dbFaceData = query.value(0).toByteArray();
    if (dbFaceData.isEmpty()) {
        QMessageBox::warning(this, "Face ID", "Vous n'avez pas encore configuré votre Face ID. Connectez-vous avec votre mot de passe et allez dans le menu principal.");
        return;
    }

    // Convertir QByteArray en cv::Mat
    std::vector<uchar> buf(dbFaceData.begin(), dbFaceData.end());
    cv::Mat refFace = cv::imdecode(buf, cv::IMREAD_GRAYSCALE);
    if (refFace.empty()) {
        QMessageBox::critical(this, "Erreur", "Impossible de charger les données faciales de référence.");
        return;
    }

    // 2. Ouvrir la caméra pour capture live
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        QMessageBox::critical(this, "Erreur Caméra", "Impossible d'accéder à la webcam.");
        return;
    }

    cv::CascadeClassifier faceCascade;
    faceCascade.load("C:/Users/LENOVO/Desktop/qt/ourlabib/OpenCV-MinGW-Build-OpenCV-4.5.5-x64/OpenCV-MinGW-Build-OpenCV-4.5.5-x64/etc/haarcascades/haarcascade_frontalface_default.xml");

    cv::Mat frame, faceROI;
    bool loggedIn = false;

    while (true) {
        cap >> frame;
        if (frame.empty()) break;

        cv::Mat gray;
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
        std::vector<cv::Rect> faces;
        faceCascade.detectMultiScale(gray, faces, 1.1, 4, 0, cv::Size(100, 100));

        for (const auto& r : faces) {
            cv::rectangle(frame, r, cv::Scalar(255, 0, 0), 2);
            faceROI = gray(r);
            
            // --- COMPARAISON PROFESSIONNELLE ---
            cv::resize(faceROI, faceROI, refFace.size());
            
            // Calcul de l'histogramme pour la comparaison
            cv::Mat hist_ref, hist_current;
            int histSize = 256;
            float range[] = { 0, 256 };
            const float* histRange = { range };
            cv::calcHist(&refFace, 1, 0, cv::Mat(), hist_ref, 1, &histSize, &histRange);
            cv::calcHist(&faceROI, 1, 0, cv::Mat(), hist_current, 1, &histSize, &histRange);
            cv::normalize(hist_ref, hist_ref, 0, 1, cv::NORM_MINMAX);
            cv::normalize(hist_current, hist_current, 0, 1, cv::NORM_MINMAX);

            double score = cv::compareHist(hist_ref, hist_current, cv::HISTCMP_CORREL);
            
            if (score > 0.75) { // Seuil de confiance (0.75 = 75% match)
                cv::rectangle(frame, r, cv::Scalar(0, 255, 0), 2);
                cv::putText(frame, "MATCH: " + std::to_string(int(score*100)) + "%", cv::Point(r.x, r.y-10), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 255, 0), 2);
                loggedIn = true;
            } else {
                cv::putText(frame, "SCANNING...", cv::Point(r.x, r.y-10), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(255,0,0), 2);
            }
        }

        cv::imshow("Authentification Faciale - OurLabib", frame);
        char key = (char)cv::waitKey(30);
        if (key == 27 || loggedIn) break; 
    }

    cv::destroyAllWindows();
    cap.release();

    if (loggedIn) {
        // Simuler une connexion réussie
        QSqlQuery loginQ(c->getDatabase());
        loginQ.prepare("SELECT ROLE, NOM_UTILISATEUR FROM UTILISATEUR WHERE LOWER(NOM_UTILISATEUR) = LOWER(:user)");
        loginQ.bindValue(":user", username);
        if (loginQ.exec() && loginQ.next()) {
            currentRole = loginQ.value(0).toString();
            currentUser = loginQ.value(1).toString();
            isLoggedIn = true;
            QMessageBox::information(this, "Face ID Succès", "Identité confirmée. Bienvenue " + currentUser + " !");
            updateMenuVisibility();
            ui->stackedWidget->setCurrentIndex(1);
        }
    } else {
        QMessageBox::warning(this, "Échec Face ID", "Reconnaissance impossible. Veuillez réessayer ou utiliser votre mot de passe.");
    }
#else
    QMessageBox::warning(this, "Face ID Désactivé", "OpenCV n'est pas configuré.");
#endif
}

void ourlabib::on_btnLogout_clicked() {
  logoutUser();
  ui->stackedWidget->setCurrentIndex(0);
}
