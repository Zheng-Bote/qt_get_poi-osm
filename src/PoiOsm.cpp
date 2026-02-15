/**
 * SPDX-FileComment: Implementation of OpenStreetMap POI client
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file PoiOsm.cpp
 * @brief  Implements the PoiOsmClient class.
 * @version 1.1.0
 * @date 2026-02-15
 *
 * @author ZHENG Robert
 * @license MIT License
 */

#include "PoiOsm.hpp"

#include <QDateTime>
#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QUrlQuery>
#include <memory>

/**
 * @brief Returns the User-Agent string for network requests.
 * @return QString The User-Agent string.
 */
static QString userAgent() {
  return QStringLiteral("qt_get_poi-osm/1.1.0 (contact: example@example.com)");
}

PoiOsmClient::PoiOsmClient(QNetworkAccessManager *nam, QObject *parent)
    : QObject(parent), m_nam(nam) {}

void PoiOsmClient::queryByAddress(const QString &address, int radiusMeters,
                                  const QList<PoiWhitelistEntry> &whitelist) {
  geocodeAddress_(address, radiusMeters, whitelist);
}

void PoiOsmClient::queryByCoordinates(
    double lat, double lon, int radiusMeters,
    const QList<PoiWhitelistEntry> &whitelist) {
  QJsonObject input;
  input["address"] = QJsonValue::Null;
  input["lat"] = lat;
  input["lon"] = lon;

  queryOverpass_(lat, lon, radiusMeters, whitelist, input);
}

void PoiOsmClient::geocodeAddress_(const QString &address, int radiusMeters,
                                   QList<PoiWhitelistEntry> whitelist) {
  QUrl url("https://nominatim.openstreetmap.org/search");
  QUrlQuery q;
  q.addQueryItem("q", address);
  q.addQueryItem("format", "json");
  q.addQueryItem("limit", "1");
  url.setQuery(q);

  QNetworkRequest req(url);
  req.setHeader(QNetworkRequest::UserAgentHeader, userAgent());

  QNetworkReply *reply = m_nam->get(req);

  QObject::connect(
      reply, &QNetworkReply::finished, this,
      [this, reply, address, radiusMeters, whitelist]() mutable {
        PoiResult result;

        if (reply->error() != QNetworkReply::NoError) {
          result.ok = false;
          result.error =
              QStringLiteral("Geocoding error: %1").arg(reply->errorString());
          emit finished(result);
          return;
        }

        const QByteArray data = reply->readAll();
        QJsonParseError err{};
        const QJsonDocument doc = QJsonDocument::fromJson(data, &err);
        if (err.error != QJsonParseError::NoError || !doc.isArray()) {
          result.ok = false;
          result.error = QStringLiteral("Invalid geocoding JSON: %1")
                             .arg(err.errorString());
          emit finished(result);
          return;
        }

        const QJsonArray arr = doc.array();
        if (arr.isEmpty()) {
          result.ok = false;
          result.error = QStringLiteral("No geocoding result for address");
          emit finished(result);
          return;
        }

        const QJsonObject obj = arr.first().toObject();
        bool okLat = false, okLon = false;
        const double lat = obj.value("lat").toString().toDouble(&okLat);
        const double lon = obj.value("lon").toString().toDouble(&okLon);
        if (!okLat || !okLon) {
          result.ok = false;
          result.error =
              QStringLiteral("Invalid coordinates in geocoding response");
          emit finished(result);
          return;
        }

        QJsonObject input;
        input["address"] = address;
        input["lat"] = QJsonValue::Null;
        input["lon"] = QJsonValue::Null;

        queryOverpass_(lat, lon, radiusMeters, whitelist, input);
      });
}

QString PoiOsmClient::buildOverpassQuery_(
    double lat, double lon, int radiusMeters,
    const QList<PoiWhitelistEntry> &whitelist) const {
  QString query = "[out:json][timeout:25];(";

  if (whitelist.isEmpty()) {
    query += QStringLiteral("node(around:%1,%2,%3);")
                 .arg(radiusMeters)
                 .arg(lat, 0, 'f', 6)
                 .arg(lon, 0, 'f', 6);
  } else {
    for (const auto &w : whitelist) {
      if (w.value.isEmpty()) {
        query += QStringLiteral("node(around:%1,%2,%3)[\"%4\"];")
                     .arg(radiusMeters)
                     .arg(lat, 0, 'f', 6)
                     .arg(lon, 0, 'f', 6)
                     .arg(w.key);
      } else {
        query += QStringLiteral("node(around:%1,%2,%3)[\"%4\"=\"%5\"];")
                     .arg(radiusMeters)
                     .arg(lat, 0, 'f', 6)
                     .arg(lon, 0, 'f', 6)
                     .arg(w.key, w.value);
      }
    }
  }

  query += ");out center;";
  return query;
}

QJsonObject PoiOsmClient::buildResultJson_(
    double centerLat, double centerLon, int radiusMeters,
    const QList<PoiWhitelistEntry> &whitelist, const QJsonArray &elements,
    const QJsonObject &queryInput) const {
  QJsonObject root;
  root["schema_version"] = 1;

  QJsonObject source;
  source["provider"] = "OpenStreetMap";
  source["geocoder"] = "Nominatim";
  source["overpass_endpoint"] = "https://overpass-api.de/api/interpreter";
  root["source"] = source;

  QJsonObject query;
  query["input"] = queryInput;

  QJsonObject resolvedCenter;
  resolvedCenter["lat"] = centerLat;
  resolvedCenter["lon"] = centerLon;
  query["resolved_center"] = resolvedCenter;

  query["radius_m"] = radiusMeters;

  QJsonArray wlArray;
  for (const auto &w : whitelist) {
    QJsonObject o;
    o["key"] = w.key;
    o["value"] = w.value;
    wlArray.append(o);
  }
  query["whitelist"] = wlArray;

  query["timestamp_utc"] =
      QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
  root["query"] = query;

  QJsonArray poisArray;
  for (const auto &v : elements) {
    const QJsonObject obj = v.toObject();
    if (obj.value("type").toString() != "node")
      continue;

    const double lat = obj.value("lat").toDouble();
    const double lon = obj.value("lon").toDouble();
    const QJsonObject tags = obj.value("tags").toObject();

    bool accepted = whitelist.isEmpty();
    if (!accepted) {
      for (const auto &w : whitelist) {
        const QString val = tags.value(w.key).toString();
        if (!val.isEmpty() && (w.value.isEmpty() || val == w.value)) {
          accepted = true;
          break;
        }
      }
    }
    if (!accepted)
      continue;

    QJsonObject poi;
    poi["lat"] = lat;
    poi["lon"] = lon;

    if (tags.contains("name"))
      poi["name"] = tags.value("name").toString();
    else
      poi["name"] = QJsonValue::Null;

    poi["tags"] = tags;

    poisArray.append(poi);
  }

  QJsonObject results;
  results["count"] = poisArray.size();
  results["pois"] = poisArray;

  root["results"] = results;

  return root;
}

void PoiOsmClient::queryOverpass_(double lat, double lon, int radiusMeters,
                                  QList<PoiWhitelistEntry> whitelist,
                                  QJsonObject queryInput) {
  QUrl url("https://overpass-api.de/api/interpreter");
  QNetworkRequest req(url);
  req.setHeader(QNetworkRequest::ContentTypeHeader,
                "application/x-www-form-urlencoded");
  req.setHeader(QNetworkRequest::UserAgentHeader, userAgent());

  const QString query = buildOverpassQuery_(lat, lon, radiusMeters, whitelist);
  const QByteArray body = "data=" + QUrl::toPercentEncoding(query);

  QNetworkReply *reply = m_nam->post(req, body);

  QObject::connect(
      reply, &QNetworkReply::finished, this,
      [this, reply, lat, lon, radiusMeters, whitelist, queryInput]() mutable {
        reply->deleteLater();

        PoiResult result;

        if (reply->error() != QNetworkReply::NoError) {
          result.ok = false;
          result.error =
              QStringLiteral("Overpass error: %1").arg(reply->errorString());
          emit finished(result);
          return;
        }

        const QByteArray data = reply->readAll();
        QJsonParseError err{};
        const QJsonDocument doc = QJsonDocument::fromJson(data, &err);
        if (err.error != QJsonParseError::NoError || !doc.isObject()) {
          result.ok = false;
          result.error = QStringLiteral("Invalid Overpass JSON: %1")
                             .arg(err.errorString());
          emit finished(result);
          return;
        }

        const QJsonObject rootObj = doc.object();
        const QJsonArray elements = rootObj.value("elements").toArray();

        result.ok = true;
        result.json = buildResultJson_(lat, lon, radiusMeters, whitelist,
                                       elements, queryInput);
        emit finished(result);
      });
}
