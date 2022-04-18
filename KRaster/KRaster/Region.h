/// Copyright (C) 2022, Antoine Basset
// This file is part of Kast.Raster <github.com/kabasset/KRaster>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef _KRASTER_REGION_H
#define _KRASTER_REGION_H

#include "KRaster/Position.h"

namespace Kast {

/**
 * @brief A _n_-D bounding box, defined by its front and back positions (both inclusive),
 * or front position and shape.
 * @details
 * Like `Position`, this class stores no pixel values, but coordinates.
 */
template <long N = 2>
struct Region {
  /**
   * @brief Create a region from a front position and shape.
   */
  static Region<N> fromShape(Position<N> frontPosition, Position<N> shape) {
    Region<N> region {frontPosition, frontPosition};
    region.back += shape - 1;
    return region;
  }

  /**
   * @brief Create an unlimited region.
   * @details
   * Front and back bounds along each axis are respectively 0 and -1.
   */
  static Region<N> whole() {
    return {Position<N>::zero(), Position<N>::max()};
  }

  /**
   * @brief Compute the region shape.
   */
  Position<N> shape() const {
    return back - front + 1;
  }

  /**
   * @brief Get the number of dimensions.
   */
  long dimension() const {
    return front.size();
  }

  /**
   * @brief Compute the region size, i.e. number of pixels.
   */
  long size() const {
    return shapeSize(shape());
  }

  /**
   * @brief The front position in the region.
   */
  Position<N> front;

  /**
   * @brief The back position in the region.
   */
  Position<N> back;
};

/**
 * @relates Region
 * @brief Check whether two regions are equal.
 */
template <long N = 2>
bool operator==(const Region<N>& lhs, const Region<N>& rhs) {
  return lhs.front == rhs.front && lhs.back == rhs.back;
}

/**
 * @relates Region
 * @brief Check whether two regions are different.
 */
template <long N = 2>
bool operator!=(const Region<N>& lhs, const Region<N>& rhs) {
  return lhs.front != rhs.front || lhs.back != rhs.back;
}

/**
 * @relates Region
 * @brief Add a position.
 */
template <long N = 2>
Region<N>& operator+=(Region<N>& lhs, const Position<N>& rhs) {
  lhs.front += rhs;
  lhs.back += rhs;
  return lhs;
}

/**
 * @relates Region
 * @brief Subtract a position.
 */
template <long N = 2>
Region<N>& operator-=(Region<N>& lhs, const Position<N>& rhs) {
  lhs.front -= rhs;
  lhs.back -= rhs;
  return lhs;
}

/**
 * @relates Region
 * @brief Add a scalar to each coordinate.
 */
template <long N = 2>
Region<N>& operator+=(Region<N>& lhs, long rhs) {
  lhs.front += rhs;
  lhs.back += rhs;
  return lhs;
}

/**
 * @relates Region
 * @brief Subtract a scalar to each coordinate.
 */
template <long N = 2>
Region<N>& operator-=(Region<N>& lhs, long rhs) {
  lhs.front -= rhs;
  lhs.back -= rhs;
  return lhs;
}

/**
 * @relates Region
 * @brief Add 1 to each coordinate.
 */
template <long N = 2>
Region<N>& operator++(Region<N>& lhs) {
  lhs += 1;
  return lhs;
}

/**
 * @relates Region
 * @brief Subtract 1 to each coordinate.
 */
template <long N = 2>
Region<N>& operator--(Region<N>& lhs) {
  lhs -= 1;
  return lhs;
}

/**
 * @relates Region
 * @brief Return the current region and then add 1 to each coordinate.
 */
template <long N = 2>
Region<N> operator++(Region<N>& lhs, int) {
  auto res = lhs;
  ++lhs;
  return res;
}

/**
 * @relates Region
 * @brief Return the current region and then subtract 1 to each coordinate.
 */
template <long N = 2>
Region<N> operator--(Region<N>& lhs, int) {
  auto res = lhs;
  --lhs;
  return res;
}

/**
 * @relates Region
 * @brief Identity.
 */
template <long N = 2>
Region<N> operator+(const Region<N>& rhs) {
  return rhs;
}

/**
 * @relates Region
 * @brief Change the sign of each coordinate.
 */
template <long N = 2>
Region<N> operator-(const Region<N>& rhs) {
  return {-rhs.front, -rhs.back};
}

/**
 * @relates Region
 * @brief Add a region and a position.
 */
template <long N = 2>
Region<N> operator+(const Region<N>& lhs, const Position<N>& rhs) {
  auto res = lhs;
  res += rhs;
  return res;
}

/**
 * @relates Region
 * @brief Subtract a region and a position.
 */
template <long N = 2>
Region<N> operator-(const Region<N>& lhs, const Position<N>& rhs) {
  auto res = lhs;
  res -= rhs;
  return res;
}

/**
 * @relates Region
 * @brief Add a region and a scalar.
 */
template <long N = 2>
Region<N> operator+(const Region<N>& lhs, long rhs) {
  auto res = lhs;
  res += rhs;
  return res;
}

/**
 * @relates Region
 * @brief Subtract a region and a scalar.
 */
template <long N = 2>
Region<N> operator-(const Region<N>& lhs, long rhs) {
  auto res = lhs;
  res -= rhs;
  return res;
}

} // namespace Kast

#endif // _KRASTER_REGION_H
