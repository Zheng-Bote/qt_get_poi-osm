/**
 * SPDX-FileComment: Header file for OpenStreetMap POI client
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file PoiOsm.hpp
 * @brief Defines the PoiOsmClient class for querying OpenStreetMap Overpass
 * API.
 * @version 1.1.0
 * @date 2026-02-15
 *
 * @author ZHENG Robert
 * @license MIT License
 */

#pragma once

#include <QJsonObject>
#include <QList>
#include <QNetworkAccessManager>
#include <QObject>

/**
 * @brief Represents a whitelist entry for filtering POIs.
 *
 * A whitelist entry consists of a key (e.g., "amenity") and an optional value
 * (e.g., "restaurant"). If the value is empty, any POI with the key is
 * accepted.
 */
struct PoiWhitelistEntry {
  QString key;   ///< The tag key (e.g., "amenity").
  QString value; ///< The tag value (optional).
};

/**
 * @brief Result structure for a POI query.
 *
 * Contains the JSON response, a success flag, and an error message if
 * applicable.
 */
struct PoiResult {
  QJsonObject json; ///< The resulting JSON object containing POIs.
  bool ok{false};   ///< True if the query was successful, false otherwise.
  QString error;    ///< Error description if the query failed.
};

/**
 * @brief Client for querying Points of Interest (POIs) from OpenStreetMap.
 *
 * This class provides methods to query POIs either by address (using Nominatim
 * for geocoding) or by direct geographic coordinates (using Overpass API).
 */
class PoiOsmClient : public QObject {
  Q_OBJECT
public:
  /**
   * @brief Constructs a new PoiOsmClient.
   *
   * @param nam Pointer to the QNetworkAccessManager instance.
   * @param parent Pointer to the parent QObject.
   */
  explicit PoiOsmClient(QNetworkAccessManager *nam, QObject *parent = nullptr);

  /**
   * @brief Queries POIs around a specific address.
   *
   * Encodes the address using Nominatim and then queries the Overpass API
   * around the resolved coordinates.
   *
   * @param address The address to search for.
   * @param radiusMeters The search radius in meters.
   * @param whitelist List of key-value pairs to filter results.
   */
  void queryByAddress(const QString &address, int radiusMeters,
                      const QList<PoiWhitelistEntry> &whitelist = {});

  /**
   * @brief Queries POIs around specific geographic coordinates.
   *
   * Queries the Overpass API directly using the provided coordinates.
   *
   * @param lat Latitude of the center point.
   * @param lon Longitude of the center point.
   * @param radiusMeters The search radius in meters.
   * @param whitelist List of key-value pairs to filter results.
   */
  void queryByCoordinates(double lat, double lon, int radiusMeters,
                          const QList<PoiWhitelistEntry> &whitelist = {});

signals:
  /**
   * @brief Emitted when a query finishes.
   *
   * @param result The result of the query.
   */
  void finished(const PoiResult &result);

private:
  QNetworkAccessManager *m_nam; ///< Pointer to the network access manager.

  /**
   * @brief Internal helper to geocode an address.
   *
   * @param address The address to geocode.
   * @param radiusMeters The radius to search around the geocoded location.
   * @param whitelist The whitelist filter.
   */
  void geocodeAddress_(const QString &address, int radiusMeters,
                       QList<PoiWhitelistEntry> whitelist);

  /**
   * @brief Internal helper to perform the Overpass API query.
   *
   * @param lat Latitude.
   * @param lon Longitude.
   * @param radiusMeters Search radius.
   * @param whitelist Filter list.
   * @param queryInput The original query input (for result JSON construction).
   */
  void queryOverpass_(double lat, double lon, int radiusMeters,
                      QList<PoiWhitelistEntry> whitelist,
                      QJsonObject queryInput);

  /**
   * @brief Builds the Overpass QL query string.
   *
   * @param lat Latitude.
   * @param lon Longitude.
   * @param radiusMeters Search radius.
   * @param whitelist Filter list.
   * @return QString The formatted Overpass QL query.
   */
  QString buildOverpassQuery_(double lat, double lon, int radiusMeters,
                              const QList<PoiWhitelistEntry> &whitelist) const;

  /**
   * @brief Constructs the final JSON result object.
   *
   * @param centerLat Latitude of the search center.
   * @param centerLon Longitude of the search center.
   * @param radiusMeters Search radius.
   * @param whitelist Filter list used.
   * @param elements The raw elements array from Overpass.
   * @param queryInput The input parameters.
   * @return QJsonObject The structured result JSON.
   */
  QJsonObject buildResultJson_(double centerLat, double centerLon,
                               int radiusMeters,
                               const QList<PoiWhitelistEntry> &whitelist,
                               const QJsonArray &elements,
                               const QJsonObject &queryInput) const;
};
