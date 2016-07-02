//
// @file      ac_decoded_cache.h
// @author    Gabriel krisman Bertazi <gabriel@krisman.be>
//
//            The ArchC Team
//            http://www.archc.org/
//
//            IC - UNICAMP
//
// @version
// @date      Thu Dec 05 2013
//
// @brief     The ArchC Decoded Cache class. Header file.
//
// @attention Copyright (C) 2013 --- The ArchC Team
//

#ifndef _AC_DECODED_CACHE_H_
#define _AC_DECODED_CACHE_H_

// This is a Decoded instructions cache with support to self modifying
// code. For each cache line we keep the decoded instruction, and a
// buffer containing the instructions bits. For every cache access, host
// must provide the current encoded bits of the instruction it intends
// to access, so the cache can verify whether it holds the correct
// instruction or not.

// WARNING: Architectures with variable instruction size.
//
// This algorithm provides an speculative approach to determine how many
// bits must be fetched from memory in order to match the cache's tag,
// since it uses the number of bits for the current instruction in the
// cache line. This is a good speculation, and if the cache line is no
// match, the decoder itself is responsible for fetching additional
// bits. Although, for architectures with variable instruction size,
// this method may force the system to fetch more bits than necessary,
// which might cause unexpected page faults if we cross a page boundary.
//
// This problem is expected for models that have variable instruction
// size *and* also use Memory Management Unit to provide Virtual
// Memory. For these models, the best thing to do is to either
// deactivate the support for self-modifying code in this cache or to
// fully disable this cache (The last is done by using the -ndc
// parameter with acsim.)

#include <ac_instr.H>
#include "string.h"

template <typename ac_word,
          typename instr_dec,
          unsigned int DEC_CACHE_MAGNITUDE>

class ac_decoded_cache {

  // Struct used for decoded instructions cache.
  struct cache_item {

    // Whether this is a valid cache line or not.
    bool valid;

    // This Holds the decodified instruction. It is a generic template
    // for the instr_dec structure declared in <model>_decode_unit.H
    instr_dec instr_p;

    // This holds the raw codification for the instruction, as fetched
    // from memory. It allows fast verification if the instruction in a
    // given address was modified since last fetch, which would require it
    // to be redecoded. This allow us to use the Decoder Cache feature
    // along with self-modifying code.
    ac_word instr_bits[4];

    // The number of ac_word in instr_bits.
    unsigned quant;
  };

  cache_item *vector;

public:

  ac_decoded_cache () {
    vector = new cache_item[(1<<DEC_CACHE_MAGNITUDE)];
  }

  ~ac_decoded_cache () {
    for (int i = 0; i < (1<<DEC_CACHE_MAGNITUDE); i++) {
      if (vector[i].valid)
        delete vector[i].instr_p;
    }
    delete vector;
  }

 #define likely(x)      __builtin_expect(!!(x), 1)
 #define unlikely(x)    __builtin_expect(!!(x), 0)

  inline __attribute__((always_inline))
  instr_dec *fetch_item (const ac_word address,
                         const ac_word *instr_bits,
                         const unsigned quant) {

    ac_word h = hash (address);
    cache_item *cell = &(vector[h]);

    if (unlikely(cell->valid == false))// || quant != cell->quant)
      return NULL;

    // Verify instruction bits.
    for (int i = 0; i < quant; i++) {
      if (unlikely (cell->instr_bits[i] != instr_bits[i])) {
        return NULL;
      }
    }
    return &(cell->instr_p);
  }

  inline __attribute__((always_inline))
  instr_dec *insert_item (const ac_word address,
                          ac_word *instr_bits,
                          unsigned quant) {

  ac_word h = hash (address);

    // Insert new element.
    vector[h].valid = true;
    vector[h].quant = quant;

    memcpy (reinterpret_cast<void *> (vector[h].instr_bits),
            reinterpret_cast<const void *> (instr_bits),
            (quant*sizeof (ac_word)));

    return &(vector[h].instr_p);
  }

  bool flush_item (const ac_word address) {

    ac_word h = hash (address);

    if (!vector[h].valid)
      return false;

    vector[h].valid = false;
    return true;
  }

  unsigned fetch_size (const ac_word address) {
    ac_word h = hash (address);
    return vector[h].valid? (vector[h].quant) : 0;
  }

private:
  inline __attribute__((always_inline))
  ac_word hash (const ac_word address) {

    return ((address>>2) & ((1<<DEC_CACHE_MAGNITUDE)-1));
  }
};
#endif // !_AC_DECODEDCACHE_H_

