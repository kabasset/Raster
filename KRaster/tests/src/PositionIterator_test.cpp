// Copyright (C) 2022, CNES
// This file is part of KRaster <github.com/kabasset/KRaster>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "KRaster/PositionIterator.h"
#include "KRaster/Raster.h"

#include <boost/test/unit_test.hpp>

using namespace Cnes;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(PositionIterator_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(shape_is_screened_in_order_test) {
  Position<5> shape {2, 3, 4, 5, 6};
  VecRaster<Index, 5> raster(shape);
  for (std::size_t i = 0; i < raster.size(); ++i) {
    raster[i] = i;
  }
  Index i = 0;
  for (const auto& p : raster.domain()) {
    BOOST_TEST(raster.index(p) == i);
    ++i;
  }
}

BOOST_AUTO_TEST_CASE(region_is_screened_in_order_test) {
  Position<4> shape {3, 4, 5, 6};
  VecRaster<Index, 4> raster(shape);
  Region<4> region {Position<4>::zero() + 1, shape - 2};
  for (std::size_t i = 0; i < raster.size(); ++i) {
    raster[i] = i;
  }
  Index current = 0;
  Index count = 0;
  for (const auto& p : region) {
    BOOST_TEST(raster.index(p) > current);
    current = raster.index(p);
    ++count;
  }
  BOOST_TEST(count == region.size());
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
