#ifndef _PIM_ROCC_
#define _PIM_ROCC_

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <bitset>
#include <memory>

#define MAXARRYANUM 64

using u64 = unsigned long long;
using i64 = long long;
using u32 = unsigned int;
using i32 = int;
using u16 = unsigned short;
using i16 = short;
using u8 = unsigned char;
using i8 = char;

namespace PIMAPI {

const int ArraySize = 256;
const int ScratchpadSize = 6;

typedef enum PIMSTATUS {
    MEMFULL = -5,
    WRONGARG = -4,
    INVALIDBITS = -3,
    VLENCONSTRAINT = -2,
    NOPIM = -1,
    UNKNOWN = 0,
    OK = 1,
    
} PIMSTATUS;

inline static void  printException(PIMSTATUS e);



/// @brief A Singleton class that record the PIM basic info
class PIMBasicInfo {
    // friend class PIMBlocks;
public:
    struct {
      u8 _scratchpad_size;    ///< size of scratchpad
      u8 _subarr_per_mat;
      u8 _mat_per_bank;
      u8 _banknums;   ///< total bank nums
      u16 _subarrnums;   ///< total sub-arrays nums
      u16 _max_usable_subarr;   ///< total sub-arrays nums
      u16 _ctrl_subarr_per_inst;   ///< count of sub-arrays a inst can control
      u16 _subarr_rows;///< count of rows in a sub-array
      u16 _subarr_cols;///< count of columns in a sub-array
    } params;

public:
    // singleton getInstance
    static PIMBasicInfo& getInstance() {
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
    static PIMBlocks* PIMalloc(size_t size);
    static PIMSTATUS PIMfree(PIMBlocks* b);
    // static std::unique_ptr<i64> blockflags;
private:
    static class _pim_space_collector{
    public:
        std::unordered_set<PIMBlocks*> s;
        ~_pim_space_collector() {
            for (auto &p : s) {
                delete p;
            }
        }
    } _collector;   ///< this is used to release unreleased blocks
    size_t  _datasize;
    std::vector<u16> blocks;
    u16  _blocknum;

public:
    size_t  getDataSize() const {return _datasize;}
    std::vector<u16> getBlocks() const {return blocks;}
    size_t  getBlockNum() const {return _blocknum;}
public:
    PIMBlocks(const PIMBlocks&) = delete;
    PIMBlocks(PIMBlocks&&) = delete;
    PIMBlocks& operator=(const PIMBlocks&) = delete;
    PIMBlocks& operator=(PIMBlocks&&) = delete;
private:
    PIMBlocks(size_t size);
    ~PIMBlocks();
public:
    PIMSTATUS PIMload(void *addr, size_t bits, u16 rowidx, size_t len=0, size_t col_offset=0);
    PIMSTATUS PIMstore(void *addr, size_t bits, u16 rowidx, size_t len=0, size_t col_offset=0);
    // PIMSTATUS 
};

typedef enum ShiftDirection {
    ShiftRight, ShiftLeft,
} ShiftDirection;

typedef enum ShiftOption {
    LogicalShift, ArithmeticShift, CircularShift,
} ShiftDirection;



// typedef struct ScratchPad {
//     uint32 size;
//     std::unordered_map<int, int> _pad;
//     ScratchPad(uint32 sz = ScratchpadSize) : size(sz){};
//     inline void resize(const uint32 &sz) {size=sz;};
// } ScratchPad;



// int32               GETMEMSIZE  ();

// inline PIMSTATUS    ROCC_CONFIG (const int32 &length, const uint32 &bits);
// PIMSTATUS           ROCC_NOT    (void *src_addr, void *dest_addr, const int32 &length, const int32 &bits=32);
// PIMSTATUS           ROCC_INT2FLOAT  (void *src_addr, void *dest_addr, const int32 &length);
// PIMSTATUS           ROCC_FLOAT2INT  (void *src_addr, void *dest_addr, const int32 &length);
// PIMSTATUS           ROCC_ADJUSTINTWIDTH (void *src_addr, void *dest_addr, const int32 &length, const int32 &src_bits, const int32 &dest_bits);
// PIMSTATUS           ROCC_SQUAREROOT_INT (void *src_addr, void *dest_addr, const int32 &length);
// PIMSTATUS           ROCC_SIN    (void *src_addr, void *dest_addr, const int32 &length);
// PIMSTATUS           ROCC_LOG    (void *src_addr, void *dest_addr, const int32 &length);
// PIMSTATUS           ROCC_POWER  (void *src1_addr, const int32 &src2, void *dest_addr, const int32 &length);
// PIMSTATUS           ROCC_POWER  (void *src1_addr, void *src2_addr, void *dest_addr, const int32 &length);
// PIMSTATUS           ROCC_SHIFT  (void *src1_addr, const int32 &src2, void *dest_addr, const int32 &length, ShiftDirection dir, ShiftOption opt, const int32 &bits);
// PIMSTATUS           ROCC_SHIFT  (void *src1_addr, void *src2_addr, void *dest_addr, const int32 &length, ShiftDirection dir, ShiftOption opt, const int32 &bits);
// PIMSTATUS           ROCC_AND    (void *src1_addr, void *src2_addr, void *dest_addr, const int32 &length, const int32 &bits=32);
// PIMSTATUS           ROCC_OR (void *src1_addr, void *src2_addr, void *dest_addr, const int32 &length, const int32 &bits=32);
// PIMSTATUS           ROCC_XOR    (void *src1_addr, void *src2_addr, void *dest_addr, const int32 &length, const int32 &bits=32);
// PIMSTATUS           ROCC_MAX_INT    (void *src1_addr, void *src2_addr, void *dest_addr, const int32 &length);
// PIMSTATUS           ROCC_MIN_INT    (void *src1_addr, void *src2_addr, void *dest_addr, const int32 &length);
// PIMSTATUS           ROCC_COMP_INT   (void *src1_addr, void *src2_addr, void *dest_addr, const int32 &length);
// PIMSTATUS           ROCC_ADD_INT    (void *src1_addr, void *src2_addr, void *dest_addr, const int32 &length);
// PIMSTATUS           ROCC_SUB_INT    (void *src1_addr, void *src2_addr, void *dest_addr, const int32 &length);
// PIMSTATUS           ROCC_MUL_INT    (void *src1_addr, void *src2_addr, void *dest_addr, const int32 &length);
// PIMSTATUS           ROCC_DIV_INT    (void *src1_addr, void *src2_addr, void *dest_addr, const int32 &length);
// PIMSTATUS           ROCC_MOD_INT    (void *src1_addr, void *src2_addr, void *dest_addr, const int32 &length);
// PIMSTATUS           ROCC_COMP_FLOAT ();
// PIMSTATUS           ROCC_ADD_FLOAT  ();
// PIMSTATUS           ROCC_SUB_FLOAT  ();
// PIMSTATUS           ROCC_MUL_FLOAT  ();
// PIMSTATUS           ROCC_DIV_FLOAT  ();

}   // end of namespace PIMAPI

#endif
