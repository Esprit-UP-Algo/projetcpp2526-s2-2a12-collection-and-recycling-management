#include "mission.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GestionMission w;
    w.show();
    return a.exec();
}
