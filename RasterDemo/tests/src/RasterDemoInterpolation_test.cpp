// Copyright (C) 2022, CNES
// This file is part of Raster <github.com/kabasset/Raster>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "RasterGeometry/Interpolation.h"

#include <boost/test/unit_test.hpp>

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(RasterDemoInterpolation_test)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(extrapolate_interpolate_test) {

  //! [Unit raster]
  Cnes::Raster<int> raster({2, 2});
  raster.arange(1);
  //! [Unit raster]

  //! [Extra]
  Cnes::Position<2> outside {-1, -1};
  const auto dirichlet = Cnes::extrapolate(raster, 0);
  const auto neumann = Cnes::extrapolate<Cnes::NearestNeighbor>(raster);
  BOOST_TEST(dirichlet[outside] == 0);
  BOOST_TEST(neumann[outside] == raster[0]);
  //! [Extra]

  //! [Inter]
  Cnes::Vector<double, 2> inbetween {.5, .5};
  const auto nn = Cnes::interpolate<Cnes::NearestNeighbor>(raster);
  BOOST_TEST(nn[inbetween] == raster.at(-1));
  //! [Inter]

  //! [Inter-extra]
  const auto linearDirichlet = Cnes::interpolate<Cnes::Linear>(dirichlet);
  BOOST_TEST(linearDirichlet[outside] == 0);
  BOOST_TEST(linearDirichlet[inbetween] == 2.5);
  //! [Inter-extra]
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()