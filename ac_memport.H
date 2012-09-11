/**
 * @file      ac_memport.H
 * @author    Thiago Massariolli Sigrist
 *
 *            The ArchC Team
 *            http://www.archc.org/
 *
 *            Computer Systems Laboratory (LSC)
 *            IC-UNICAMP
 *            http://www.lsc.ic.unicamp.br/
 *
 * @version   1.0
 * @date      Thu Sep 22 11:47:36 2005
 *
 * @brief     Defines a wrapper template class for memory access.
 *
 * @attention Copyright (C) 2002-2006 --- The ArchC Team
 *
 */

//////////////////////////////////////////////////////////////////////////////

#ifndef _AC_MEMPORT_H_
#define _AC_MEMPORT_H_

//////////////////////////////////////////////////////////////////////////////

// Standard includes
#include <stdint.h>
#include <list>
#include <set>
#include <fstream>

// SystemC includes

// ArchC includes
#include "ac_inout_if.H"
#include "ac_log.H"
#include "ac_arch_ref.H"
#include "ac_utils.H"

//////////////////////////////////////////////////////////////////////////////

// 'using' statements
using std::string;
using std::list;
using std::istringstream;
using std::ifstream;

//////////////////////////////////////////////////////////////////////////////

// Forward class declarations, needed to compile

//////////////////////////////////////////////////////////////////////////////

/// Template wrapper class for memory access.
template<typename ac_word, typename ac_Hword> class ac_memport :
  public ac_arch_ref<ac_word, ac_Hword> {

private:
  ac_inout_if* storage;

  ac_word aux_word;
  ac_Hword aux_Hword;
  uint8_t aux_byte;

protected:
  typedef list<change_log<ac_word> > log_list;
#ifdef AC_UPDATE_LOG
  log_list changes;                 //!< Update log.
  fstream update_file;              //!< Update log binary file.
#endif

#ifdef AC_DELAY
  log_list delays;                  //!< Delayed update queue.
#endif

public:

  ///Default constructor
  explicit ac_memport(ac_arch<ac_word, ac_Hword>& ref) : ac_arch_ref<ac_word, ac_Hword>(ref) {}

  ///Default constructor with initialization
  explicit ac_memport(ac_arch<ac_word, ac_Hword>& ref, ac_inout_if& stg) : ac_arch_ref<ac_word, ac_Hword>(ref), storage(&stg) {}

  virtual ~ac_memport() {}

  ///Reads a word
  inline ac_word read(uint32_t address) {
    storage->read(&aux_word, address, sizeof(ac_word) * 8);
    if (!this->ac_mt_endian) {
      aux_word = byte_swap(aux_word);
    }
    return aux_word;
  }

  ///Reads a byte
  inline uint8_t read_byte(uint32_t address) {
    storage->read(&aux_byte, address, 8);
    return aux_byte;
  }

  ///Reads half word
  inline ac_Hword read_half(uint32_t address) {

    if (!this->ac_mt_endian) {
      storage->read(&aux_Hword, address, sizeof(ac_Hword) * 8);
      aux_Hword = convert_endian(sizeof(ac_Hword), aux_Hword, 0);
      return aux_Hword;
    }
    else {
      storage->read(&aux_Hword, address, sizeof(ac_Hword) * 8);
      return aux_Hword;
    } 

  }
  
  //!Writing a word
  inline void write(uint32_t address, ac_word datum) {
    aux_word = datum;
    if (!this->ac_mt_endian) {
      aux_word = byte_swap(datum);
    }
    storage->write(&aux_word, address, sizeof(ac_word) * 8);
  }

  //!Writing a byte 
  inline void write_byte(uint32_t address, uint8_t datum) {
    storage->write(&datum, address, 8);
  }

  //!Writing a short int 
  inline void write_half(uint32_t address, ac_Hword datum) {
    if (!this->ac_mt_endian) {
      aux_Hword = convert_endian(sizeof(ac_Hword), datum, 0);
      storage->write(&aux_Hword, address, sizeof(ac_Hword) * 8);
    }
    else {
      storage->write(&datum, address, sizeof(ac_Hword) * 8);
    }
  }

#ifdef AC_DELAY
  //!Writing a word
  inline void write(uint32_t address, ac_word datum, uint32_t time) {
    if (!this->ac_mt_endian)
      delays.push_back(change_log<ac_word>(address, byte_swap(datum), time));
    else
      delays.push_back(change_log<ac_word>(address, datum, time));
  }

  //!Writing a byte 
  inline void write_byte(uint32_t address, uint8_t datum,
			 uint32_t time) {
    uint32_t oset_addr = address % sizeof(ac_word);
    uint32_t base_addr = address - oset_addr;

    storage->read(&aux_word, base_addr, sizeof(ac_word) * 8);

    ((uint8_t*)(&aux_word))[oset_addr] = datum;
    
    delays.push_back(change_log<ac_word>(base_addr, aux_word, time));
    
  }

  //!Writing a short int 
  inline void write_half(uint32_t address, ac_Hword datum, uint32_t time) {
    uint32_t oset_addr = address % sizeof(ac_word);
    uint32_t base_addr = address - oset_addr;

    storage->read(&aux_word, base_addr, sizeof(ac_word) * 8);

    aux_Hword = datum;
    if (!this->ac_mt_endian) {
      aux_Hword = convert_endian(sizeof(ac_Hword), datum, 0);
    }
    ((ac_Hword*)(&aux_word))[oset_addr] = aux_Hword;
    
    delays.push_back(change_log<ac_word>(base_addr, aux_word, time));
    
  }

#endif

  /** 
   * Locks the device.
   * 
   */
  inline void lock() {
    storage->lock();
  } 

  /** 
   * Unlocks the device.
   * 
   */
  inline void unlock() {
    storage->unlock();
  }

#ifdef AC_UPDATE_LOG
  //! Reset log lists.
  void reset_log() { changes.clear(); }
  
  //!Dump storage device log.
  int change_dump(ostream& output) {
    log_list::iterator itor;
    
    if (changes.size()) {
      output << endl << endl;
      output << "**************** ArchC Change log *****************\n";
      output << "* Device: "<< name << "\t\t" << "PC: " << hex << ios_ac_pc << dec << endl;
      output << "***************************************************\n";
      output << "*        Address         Value          Time      *\n";
      output << "***************************************************\n";
      
      for (itor = changes.begin(); itor != changes.end(); itor++)
	output << "*  " << *itor << "     *" << endl;
      
      output << "***************************************************\n";
    }
    return 0;
  }
  
  //!Save storage device log.
  void change_save() {
    log_list::iterator itor;
    
    if (changes.size()) {

    for (itor = changes.begin(); itor != changes.end(); itor++)
      itor->save(this->update_file);

  }
}
#endif

  //!Method to provide the name of the device.
  inline char* get_name() {
    return "";
  }

  //!Method to provide the size of the device.
  uint32_t get_size() {
    return storage->get_size();
  }

#ifdef AC_UPDATE_LOG
  //!Method to provide the change list.
  log_list* get_changes() {
    return &changes;
  }
#endif

  /// ***** EDIT *****
  ///Method to load device content from a file.
  /// Use local Data array
  inline void load(char* file) {

    ifstream input;
    string read;
    string word;
    istringstream line;
    unsigned text_size=0;
    bool is_addr, is_text=0, first_addr=1;
    long long data;
    unsigned int  addr=0;
    unsigned char* Data;

    Data = new unsigned char[storage->get_size()];
    if (this->ac_heap_ptr >0) {
      storage->read(Data, 0, 32, this->ac_heap_ptr);
    }

    //Try to read as ELF first
    if (ac_load_elf<ac_word, ac_Hword>(*this, file, Data, storage->get_size(), this->ac_heap_ptr, this->ac_start_addr, this->ac_mt_endian) == EXIT_SUCCESS) {
      //init decode cache and return
      if(!this->dec_cache_size)
        this->dec_cache_size = this->ac_heap_ptr;
      storage->write(Data, 0, 32, (this->ac_heap_ptr)/4);
      delete[] Data;
      return;
    }

    // Looking for initialization file.
    input.open(file);
    if(!input){
      AC_ERROR("Could not open input file:" << file);
      AC_ERROR("Simulation aborted.");
      exit(1);
    }
    else{
      union {
        unsigned t32;
        unsigned char b8[5];
      } buf;
      buf.t32 = 0;
      is_text = 1;
      this->ac_start_addr = 0;
      unsigned index = 0;
      std::set<unsigned> loadedset;
      while( !input.eof() ){

        line.clear();
        getline(input, read);
        line.str(read);

        is_addr = 1;

        //Processing line
        while(line >> word){
          if (word[0] == '#')
            break;

          //Processing word
          if( is_addr ){
            if (strncmp(word.c_str(), ".data:", 6) == 0) {
              is_text = 0;
              continue;
            }
            if (strncmp(word.c_str(), ".text:", 6) == 0) {
              is_text = 1;
              continue;
            }
            addr = strtol(word.c_str(), NULL, 16);
            addr = addr << 1;
            if (addr != 0 
                && loadedset.find(addr) != loadedset.end()) {
              fprintf(stderr, "Loader error: Adddress 0x%X is written",
                      addr >> 1);
              fprintf(stderr, " multiple times.\n");
              abort();
            }
            loadedset.insert(addr);
            is_addr = 0;
            buf.t32 = 0;
            index = 0;
            if( is_text && first_addr ){
              first_addr = 0;
              //              this->ac_start_addr = addr;
            }
            if ( is_text && (addr > text_size)) {
              text_size = addr;              
            }
          }
          else{
            for (int i = 0; i < word.length(); ++i) {
              char c  = word[i];
              //data = strtoll( word.c_str(), NULL, 16);
              if (c >= '0' && c <= '9')
                buf.b8[index++] = (unsigned char) c - '0';
              else if (c >= 'a' && c <= 'f') 
                buf.b8[index++] = (unsigned char) c - 'a' + 10;
              else if (c >= 'A' && c <= 'F') 
                buf.b8[index++] = (unsigned char) c - 'A' + 10;
              else
                continue;
              if (index == 5) {
                unsigned datum = 0;
                for (int j = 0; j < 5; ++j) {
                  datum = datum << 4;
                  datum += buf.b8[j];
                }
                datum = datum << 12;
                write(addr, datum);
                index = 0;
                buf.t32 = 0;
                addr+= 1;
                if ( is_text && (addr > text_size)) {                                                                                
                  text_size = addr;                                                                                                 
                }               
              }            
            } //endfor
          }
        }
      }
    }
    if(!this->dec_cache_size)
      this->dec_cache_size = text_size;

    input.close();
  }

  /// ***** EDIT *****
  //!Method to load device content from an array.
  void load_array(const uint8_t* d, const uint32_t s) {
    unsigned char* Data;
    if (storage->get_size() < s) {
      fprintf(stderr, "Storage %s: trying to load an array bigger then storage size.\n", storage->get_name().c_str());
      exit(EXIT_FAILURE);
    }
    storage->write(d, 0, 8, s);
  }


#ifdef AC_DELAY

  //!Commiting delayed updates
  virtual void commit_delays(double time) {
    typename log_list::iterator itor;

    itor = delays.begin();

    // Sometimes, when a memory hierarchy is present and the processor spends
    // some cycles in a wait status, we may have to commit changes for every
    // cycle <= current time.
    while (delays.size() && (itor->time <= time)) {
      storage->write(&(itor->value), itor->addr, sizeof(ac_word) * 8);
      itor = delays.erase(itor);
    }
  }

#endif

  //!Dump the entire contents of a storage device
  virtual void dump() {}

  ///Binding operator
  inline void operator ()(ac_inout_if& stg) {
    storage = &stg;
  }

};

//////////////////////////////////////////////////////////////////////////////

#endif // _AC_MEMPORT_H_
