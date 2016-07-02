// 'tlb.h' - Translation look-aside buffer model
//
// Copyright (C) 2013 The ArchC team.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// ----------------------------------------------------------------------
// Author : Gabriel Krisman Bertazi, 21/11/2013
//
// Please report bugs to <krisman.gabriel@gmail.com>
// ----------------------------------------------------------------------

#ifndef TLB_H
#define TLB_H

#include <ac_instr.H>
#include "arm_parms.H"

template <unsigned int CACHE_MAGNITUDE>
class tlb
{
  struct cache_item
  {
    bool valid;
    uint32_t va;
    uint32_t pa;
  };

  struct cache_item *vector;

 public:

  tlb()
    {
      vector = new cache_item[(1<<CACHE_MAGNITUDE)];
    }

  ~tlb()
    {
      delete vector;
    }

  bool fetch_item(const uint32_t va, uint32_t *pa)
  {

    uint32_t h = hash(va);
    cache_item *cell = &(vector[h]);

    if(cell->valid == false || cell->va != va)
      return false;

    *pa = cell->pa;
    return true;
  }

  void insert_item(const uint32_t va, const uint32_t pa)
  {
    uint32_t h = hash(va);

    vector[h].valid = true;
    vector[h].va = va;
    vector[h].pa = pa;
  }

  void flush_item(const uint32_t va)
  {
    uint32_t h = hash(va);
    vector[h].valid = false;
  }

  void flush_all(const uint32_t va)
  {
    uint32_t h = hash(va);
    vector[h].valid = false;
  }

private:
  inline uint32_t hash(const uint32_t address)
  {
    return ((address) & ((1<<CACHE_MAGNITUDE)-1));
  }

};

#endif // !TLB_H

