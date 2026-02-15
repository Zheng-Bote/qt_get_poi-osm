/**
 * SPDX-FileComment: Main entry point for the qt_get_poi-osm application
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file main.cpp
 * @brief  Implements the main entry point and CLI argument parsing.
 * @version 1.1.0
 * @date 2026-02-15
 *
 * @author ZHENG Robert
 * @license MIT License
 */

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QTextStream>

#include "PoiOsm.hpp"

/**
 * @brief Main entry point of the application.
 *
 * Sets up the QCoreApplication, parses command line arguments, and initiates
 * the POI query.
 *
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return int Exit code.
 */
int main(int argc, char *argv[]) {
  QCoreApplication app(argc, argv);
  QCoreApplication::setApplicationName("qt_get_poi-osm");
  QCoreApplication::setApplicationVersion("1.1.0");

  QCommandLineParser parser;
  parser.setApplicationDescription("OSM POI finder (JSON, 100km radius)");
  parser.addHelpOption();
  parser.addVersionOption();

  QCommandLineOption latOpt({"l", "lat"}, "Latitude", "lat");
  QCommandLineOption lonOpt({"L", "lon"}, "Longitude", "lon");
  QCommandLineOption addrOpt({"a", "address"}, "Address", "address");
  QCommandLineOption whitelistOpt(
      {"w", "whitelist"},
      "Whitelist entry key[=value], e.g. amenity=restaurant or tourism",
      "entry");

  parser.addOption(latOpt);
  parser.addOption(lonOpt);
  parser.addOption(addrOpt);
  parser.addOption(whitelistOpt);

  parser.process(app);

  const bool hasLatLon = parser.isSet(latOpt) && parser.isSet(lonOpt);
  const bool hasAddr = parser.isSet(addrOpt);

  if (!hasLatLon && !hasAddr) {
    QTextStream(stderr) << "Provide either --lat/--lon or --address\n";
    return 1;
  }

  QList<PoiWhitelistEntry> whitelist;
  for (const QString &entry : parser.values(whitelistOpt)) {
    const QStringList parts = entry.split('=', Qt::KeepEmptyParts);
    PoiWhitelistEntry w;
    w.key = parts.value(0).trimmed();
    if (parts.size() > 1)
      w.value = parts.value(1).trimmed();
    if (!w.key.isEmpty())
      whitelist.push_back(w);
  }

  auto *nam = new QNetworkAccessManager(&app);
  auto *client = new PoiOsmClient(nam, &app);

  QObject::connect(
      client, &PoiOsmClient::finished, &app, [](const PoiResult &res) {
        QTextStream out(stdout);
        if (!res.ok) {
          QJsonObject err;
          err["schema_version"] = 1;
          err["error"] = res.error;
          out << QJsonDocument(err).toJson(QJsonDocument::Compact) << "\n";
        } else {
          out << QJsonDocument(res.json).toJson(QJsonDocument::Compact) << "\n";
        }
        out.flush();
        QCoreApplication::quit();
      });

  const int radius = 100000;

  if (hasLatLon) {
    bool okLat = false, okLon = false;
    const double lat = parser.value(latOpt).toDouble(&okLat);
    const double lon = parser.value(lonOpt).toDouble(&okLon);
    if (!okLat || !okLon) {
      QTextStream(stderr) << "Invalid lat/lon\n";
      return 1;
    }
    client->queryByCoordinates(lat, lon, radius, whitelist);
  } else {
    client->queryByAddress(parser.value(addrOpt), radius, whitelist);
  }

  return app.exec();
}
