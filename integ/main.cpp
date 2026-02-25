#include "ourlabib.h"
#include "connection.h"
#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Charger le style QSS depuis les ressources
    QFile styleFile(":/style.qss");
    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream stream(&styleFile);
        a.setStyleSheet(stream.readAll());
        styleFile.close();
    }

    ourlabib w;
    Connection* c = Connection::instance();
    bool test = c->createConnect();

    if(test)
    {/*
        // Créer les tables
        c->createTables();
        
        // Insérer des données de test
        c->insertEmploye("Dupont", "Jean", "jean@email.com");
        c->insertEmploye("Martin", "Marie", "marie@email.com");
        c->insertEmploye("Durand", "Pierre", "pierre@email.com");
        
        c->insertEquipe("Équipe A", "Nettoyage secteur nord");
        c->insertEquipe("Équipe B", "Nettoyage secteur sud");
        
        c->insertZone("Zone Nord", "Bâtiment A");
        c->insertZone("Zone Sud", "Bâtiment B");
        
        c->insertPoubelle("Couloir 1", 50, 100);
        c->insertPoubelle("Couloir 2", 75, 100);
        
        c->insertMission("Nettoyage", "Nettoyer tous les poubelles");*/
        
        w.show();
        QMessageBox::information(nullptr, QObject::tr("database is open"),
                                 QObject::tr("connection successful.\n"
                                             "Click Cancel to exit."), QMessageBox::Cancel);
    }
    else
    {
        QMessageBox::critical(nullptr, QObject::tr("database is not open"),
                              QObject::tr("connection failed.\n"
                                          "Click Cancel to exit."), QMessageBox::Cancel);
    }

    return a.exec();
}
