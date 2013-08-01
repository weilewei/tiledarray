/*
 *  This file is a part of TiledArray.
 *  Copyright (C) 2013  Virginia Tech
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Justus Calvin
 *  Department of Chemistry, Virginia Tech
 *
 *  hash_pmap.h
 *  May 4, 2012
 *
 */

#ifndef TILEDARRAY_PMAP_HASH_PMAP_H__INCLUDED
#define TILEDARRAY_PMAP_HASH_PMAP_H__INCLUDED

#include <TiledArray/pmap/pmap.h>

namespace TiledArray {
  namespace detail {

    /// Hashed process map
    class HashPmap : public Pmap {
    protected:
      // Import Pmap protected variables
      using Pmap::local_;
      using Pmap::rank_;
      using Pmap::procs_;
      using Pmap::size_;

    private:

      const madness::hashT seed_; ///< Hashing seed value

    public:
      typedef Pmap::size_type size_type; ///< Size type

      /// Construct a hashed process map

      /// \param world The world where the tiles are mapped
      /// \param size The number of tiles to be mapped
      /// \param seed The hash seed used to generate different maps
      HashPmap(madness::World& world, const size_type size, madness::hashT seed = 0ul) :
          Pmap(world, size), seed_(seed)
      {
        // Construct a map of all local processes
        for(size_type i = 0ul; i < size_; ++i)
          if(HashPmap::owner(i) == rank_)
            local_.push_back(i);
      }

      virtual ~HashPmap() { }

      /// Maps \c tile to the processor that owns it

      /// \param tile The tile to be queried
      /// \return Processor that logically owns \c tile
      virtual ProcessID owner(const size_type tile) const {
        madness::hashT seed = seed_;
        madness::hash_combine(seed, tile);
        return seed % procs_;
      }

    }; // class HashPmap

  } // namespace detail
}  // namespace TiledArray


#endif // TILEDARRAY_PMAP_HASH_PMAP_H__INCLUDED
