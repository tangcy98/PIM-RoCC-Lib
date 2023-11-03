#ifndef _PIM_ROCC_
#define _PIM_ROCC_

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <algorithm>
#include "ScratchPad.h"
#include "Basic.h"



namespace PIMAPI {

typedef enum PIMSTATUS {
    MEMFULL = -5,
    WRONGARG = -4,
    INVALIDBITS = -3,
    VLENCONSTRAINT = -2,
    NOPIM = -1,
    UNKNOWN = 0,
    OK = 1,
} PIMSTATUS;


/// @brief A Singleton class that record the PIM basic info
class PIMBasicInfo {
    // friend class PIMBlocks;
public:
    struct {
      u8 _scratchpad_size;    ///< size of scratchpad
      u8 _subarr_per_mat;
      u8 _mat_per_bank;
      u8 _banknums;   ///< total bank nums
      u8 _inst_slots;   ///< count of inst slots
      u16 _subarrnums;   ///< total sub-arrays nums
      u16 _usable_subarr;   ///< total sub-arrays nums
      u16 _ctrl_subarr_per_inst;   ///< count of sub-arrays a inst can control
      u16 _subarr_rows;///< count of rows in a sub-array
      u16 _subarr_cols;///< count of columns in a sub-array
      u16 _ctrl_cols_stride;    ///< _ctrl_subarr_per_inst *  _subarr_cols
    } params;
public:
    // singleton getInstance
    static const PIMBasicInfo& getInstance() {
        static PIMBasicInfo instance;
        return instance;
    }
public:
    PIMBasicInfo(const PIMBasicInfo&) = delete;
    PIMBasicInfo(PIMBasicInfo&&) = delete;
    PIMBasicInfo& operator=(const PIMBasicInfo&) = delete;
    PIMBasicInfo& operator=(PIMBasicInfo&&) = delete;
private:
    PIMBasicInfo(); // get basic info
    ~PIMBasicInfo() = default;
};



/// @brief A user allocated PIM blocks
class PIMBlocks {
public:
    enum BasicComputeType {
        PIMNOT=0, PIM1OPMAX,
        PIMAND, PIMOR, PIMNOR, PIMXOR,
        PIMADD, PIMSUB, PIMMUL, PIMCMP,
        PIM2OPMAX,
    };
    static PIMBlocks* PIMalloc(size_t size, unsigned int extracoe = 1);
    static PIMSTATUS PIMfree(PIMBlocks* b);
    // static std::unique_ptr<i64> blockflags;
private:
    static struct _pim_space_collector{
        std::unordered_set<PIMBlocks*> s;
        ~_pim_space_collector() { std::for_each(s.begin(),s.end(),[](PIMBlocks *const &p){delete p;}); }
    } _collector;   ///< this is used to release unreleased blocks
    static ScratchPad _sp;
    size_t  _datasize;
    size_t  _capacity;
    pimaddr_t _baseaddr;
    u16  _datablocks;
    u16  _totalblocks;

public:
    size_t  getDataSize() const {return _datasize;}
    size_t  getCapacity() const {return _capacity;}
    pimaddr_t getBaseAddr() const {return _baseaddr;}
    size_t  getDataBlocks() const {return _datablocks;}
    size_t  getTotalBlocks() const {return _totalblocks;}
public:
    PIMBlocks(const PIMBlocks&) = delete;
    PIMBlocks(PIMBlocks&&) = delete;
    PIMBlocks& operator=(const PIMBlocks&) = delete;
    PIMBlocks& operator=(PIMBlocks&&) = delete;
private:
    PIMBlocks(size_t size, unsigned int extracoe);
    ~PIMBlocks();
public:
    PIMSTATUS PIMVload(void *addr, u8 bits, u16 rowidx, size_t len=0);
    PIMSTATUS PIMVstore(void *addr, u8 bits, u16 rowidx, size_t len=0);
    PIMSTATUS PIMSload(u64 num, u8 bits, u16 rowidx, size_t len=0);
    PIMSTATUS PIMVcopy(u8 bits, u16 srcrowidx, u16 destrowidx, size_t len=0);

    ///< use existing data in PIM and just compute (need to know specific row index)
    PIMSTATUS PIMcompute(BasicComputeType ctype, u8 bits, u16 srcrowidx, u16 destrowidx, size_t len=0); ///< just compute

    ///< load data into PIM and store after compute (do not care detailed row information)
    PIMSTATUS PIMloadcomputestore(BasicComputeType ctype, void **srcaddr, void *destaddr, u8 bits, size_t len=0); ///< load and compute and store

    ///< use existing data in PIM and just execute scratchpad instruction
    PIMSTATUS PIMSPexecute(std::string s, u16 srcrowidx, u16 destrowidx, size_t len=0);
};


typedef enum ShiftDirection {
    ShiftRight, ShiftLeft,
} ShiftDirection;

typedef enum ShiftOption {
    LogicalShift, ArithmeticShift, CircularShift,
} ShiftOption;


}   // end of namespace PIMAPI

#endif
