#include "TiledArray/array.h"
#include "TiledArray/utility.h"
#include "unit_test_config.h"
#include "range_fixture.h"
#include "shape_fixtures.h"

using namespace TiledArray;

struct ArrayFixture : public TiledRangeFixture, public ShapeFixture {
  typedef Array<int, GlobalFixture::coordinate_system> ArrayN;
  typedef ArrayN::index index;
  typedef ArrayN::value_type tile_type;

  ArrayFixture() : world(*GlobalFixture::world), a(world, tr) {
    for(ArrayN::range_type::const_iterator it = a.tiles().begin(); it != a.tiles().end(); ++it) {
      if(a.is_local(*it)) {
        a.set(*it, world.rank()); // Fill the tile at *it (the index)
      }
    }
  }

  madness::World& world;
  ArrayN a;
}; // struct ArrayFixture

BOOST_FIXTURE_TEST_SUITE( array_suite , ArrayFixture )

BOOST_AUTO_TEST_CASE( constructors )
{
  // Construct a dense array
  BOOST_REQUIRE_NO_THROW(ArrayN ad(world, tr));
  ArrayN ad(world, tr);

  // Check that none of the tiles have been set.
  for(ArrayN::const_iterator it = ad.begin(); it != ad.end(); ++it)
    BOOST_CHECK(! it->probe());

  // Construct a sparse array
  BOOST_REQUIRE_NO_THROW(ArrayN as(world, tr, list.begin(), list.end()));
  ArrayN as(world, tr, list.begin(), list.end());

  // Check that none of the tiles have been set.
  for(ArrayN::const_iterator it = as.begin(); it != as.end(); ++it)
    BOOST_CHECK(! it->probe());


  // Construct a predicated array
  BOOST_REQUIRE_NO_THROW(ArrayN ap(world, tr, p));
  ArrayN ap(world, tr, p);

  // Check that none of the tiles have been set.
  for(ArrayN::const_iterator it = ap.begin(); it != ap.end(); ++it)
    BOOST_CHECK(! it->probe());

}

BOOST_AUTO_TEST_CASE( find_tiles )
{
  for(ArrayN::range_type::const_iterator it = a.tiles().begin(); it != a.tiles().end(); ++it) {
    const int owner = a.owner(*it);

    madness::Future<ArrayN::value_type> tile = a.find(*it);

    for(ArrayN::value_type::iterator it = tile.get().begin(); it != tile.get().end(); ++it)
      BOOST_CHECK_EQUAL(*it, owner);
  }
}

BOOST_AUTO_TEST_CASE( fill_tiles )
{
  ArrayN a(world, tr);

  for(ArrayN::range_type::const_iterator it = a.tiles().begin(); it != a.tiles().end(); ++it) {
    if(a.is_local(*it)) {
      a.set(*it, 0); // Fill the tile at *it (the index) with 0

      madness::Future<ArrayN::value_type> tile = a.find(*it);
      BOOST_CHECK(tile.probe());

      for(ArrayN::value_type::iterator it = tile.get().begin(); it != tile.get().end(); ++it)
        BOOST_CHECK_EQUAL(*it, 0);
    }
  }
}

BOOST_AUTO_TEST_CASE( assign_tiles )
{
  std::vector<int> data;
  ArrayN a(world, tr);

  for(ArrayN::range_type::const_iterator it = a.tiles().begin(); it != a.tiles().end(); ++it) {
    ArrayN::tiled_range_type::tile_range_type range = a.tiling().make_tile_range(*it);
    if(a.is_local(*it)) {
      if(data.size() < range.volume())
        data.resize(range.volume(), 1);
      a.set(*it, data.begin(), data.begin() + range.volume());

      madness::Future<ArrayN::value_type> tile = a.find(*it);
      BOOST_CHECK(tile.probe());

      for(ArrayN::value_type::iterator it = tile.get().begin(); it != tile.get().end(); ++it)
        BOOST_CHECK_EQUAL(*it, 1);
    }
  }
}

BOOST_AUTO_TEST_SUITE_END()
