#include "mapzones.h"
#include <QDesktopServices>
#include <QUrl>

MapZones::MapZones(QObject *parent) : QObject(parent) {}

void MapZones::openMap() {
    // Ouvre la carte des zones dans le navigateur par défaut
    // (Implémentation simplifiée qui redirige vers une recherche Google Maps ou un fichier local)
    QDesktopServices::openUrl(QUrl("https://www.google.com/maps/search/Zones+Tunisie"));
}
