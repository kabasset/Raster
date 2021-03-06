// @copyright 2022, Antoine Basset (CNES)
// This file is part of Raster <github.com/kabasset/Raster>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "LitlRaster/Box.h"

#include <boost/test/unit_test.hpp>
#include <set>

using namespace Litl;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(Box_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(ctors_test) {
  Position<7> front;
  ++front;
  Position<7> back {2, 8, 6, 1, 9, 8, 9};
  Box<7> fromTo {front, back};
  BOOST_TEST(fromTo.shape() == back);
  Box<7> fromShape {front, fromTo.shape()};
  BOOST_TEST(fromShape.back() == back);
}

BOOST_AUTO_TEST_CASE(shift_test) {
  const Position<3> front {1, 2, 3};
  const Position<3> back {4, 5, 6};
  Box<3> region {front, back};
  const auto shape = region.shape();
  region += shape - 1;
  BOOST_TEST(region.shape() == shape);
  BOOST_TEST(region.front() == back);
}

BOOST_AUTO_TEST_CASE(surrounding_test) {
  const auto central = Box<2>::fromShape({1, 1}, {4, 3});
  const Box<2> margin {{-3, -2}, {2, 1}};
  const auto chunks = central.surround(margin);
  const auto box = central + margin;
  std::set<Indices<2>> all;
  std::set<Indices<2>> chunked;
  for (const auto& p : box) {
    all.insert(p.container());
  }
  BOOST_TEST(all.size() == box.size());

  for (const auto& p : central) {
    chunked.insert(p.container());
  }
  for (const auto& c : chunks) {
    for (const auto& p : c) {
      chunked.insert(p.container());
    }
  }
  BOOST_TEST(chunked == all);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
