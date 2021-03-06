/**
 * @file      ac_stats.H
 * @author    Thiago Massariolli Sigrist   
 *
 * @author    The ArchC Team
 *            http://www.archc.org/
 *
 *            Computer Systems Laboratory (LSC)
 *            IC-UNICAMP
 *            http://www.lsc.ic.unicamp.br/
 *
 * @version   2.0beta2
 * @date      Thu, 04 May 2006 14:57:53 -0300
 *
 * @brief     Convenience header file for ArchC statistics.
 *
 * @attention Copyright (C) 2002-2005 --- The ArchC Team
 *
 */

//////////////////////////////////////////////////////////////////////////////

#ifndef AC_STATS_H
#define AC_STATS_H

//////////////////////////////////////////////////////////////////////////////

// Standard includes

// SystemC includes

// ArchC includes
#include "ac_stats_base.H"
#include "ac_basic_stats.H"
#include "ac_processor_stats.H"
#include "ac_instruction_stats.H"

//////////////////////////////////////////////////////////////////////////////

// using statements

//////////////////////////////////////////////////////////////////////////////

// Forward class declarations, needed to compile

//////////////////////////////////////////////////////////////////////////////

// Macro definitions

/// Macro defining a struct with an enum for all processor global stats names,
/// and a corresponding string with all names separated by commas.
#define AC_SET_STAT_LIST(proc, stats...) struct proc##_stat_list {\
  enum { stats, END_OF_STATS };\
  static const char* statnames;\
};\
namespace proc##_stat_ids {\
  enum { stats, END_OF_STATS };\
};

/// Macro defining the value for the string containing a list of the stats'
/// names.
#define AC_CONF_STAT_LIST(proc, stats...) const char* proc##_stat_list::statnames = #stats

/// Macro defining a struct with an enum for all processor instruction stats
/// names, and a corresponding string with all names separated by commas.
#define AC_SET_INSTR_STAT_LIST(proc, stats...) struct proc##_instr_stat_list {\
  enum { stats, END_OF_STATS };\
  static const char* statnames;\
};\
namespace proc##_instr_stat_ids {\
  enum { stats, END_OF_STATS };\
};

/// Macro defining the value for the string containing a list of the stats'
/// names.
#define AC_CONF_INSTR_STAT_LIST(proc, stats...) const char* proc##_instr_stat_list::statnames = #stats

/// Macro defining a processor-specific instance of global stats.
#define AC_SET_STATS(proc, stats...) AC_SET_STAT_LIST(proc, stats);\
  typedef ac_processor_stats<proc##_stat_list> proc##_stats

/// Macro defining a processor-specific instance of instruction stats.
#define AC_SET_INSTR_STATS(proc, stats...) AC_SET_INSTR_STAT_LIST(proc, stats);\
  typedef ac_instruction_stats<proc##_instr_stat_list> proc##_instr_stats

//////////////////////////////////////////////////////////////////////////////

#endif // AC_STATS_H

