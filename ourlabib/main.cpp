#include "connection.h"
#include "ourlabib.h"
#include <QApplication>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);

  // Charger le style QSS depuis les ressources
  QFile styleFile(":/style.qss");
  if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
    QTextStream stream(&styleFile);
    a.setStyleSheet(stream.readAll());
    styleFile.close();
  }

  ourlabib w;
  w.show();

  // Base de données non requise pour l'interface mockée
  // Connection *c = Connection::instance();
  // c->createConnect();

  return a.exec();
}
