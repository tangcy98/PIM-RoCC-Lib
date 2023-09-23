#ifndef _PIM_ROCC_
#define _PIM_ROCC_

#include <unordered_map>

#define MAXARRYANUM 64

typedef int int32;
typedef short int16;
typedef unsigned short uint16;
typedef unsigned int uint32;

namespace PIMAPI {

const int ArraySize = 256;
const int ScratchpadSize = 6;

typedef enum PIMSTATUS {
    MEMFULL = -5,
    WRONGARG = -4,
    INVALIDBITS = -3,
    VLENCONSTRAINT = -2,
    UNKNOWN = 0,
    OK = 1,
    
} PIMSTATUS;

typedef enum ShiftDirection {
    ShiftRight, ShiftLeft,
} ShiftDirection;

typedef enum ShiftOption {
    LogicalShift, ArithmeticShift, CircularShift,
} ShiftDirection;


inline static void  printException(PIMSTATUS e);

typedef struct ScratchPad {
    uint32 size;
    std::unordered_map<int, int> _pad;
    ScratchPad(uint32 sz = ScratchpadSize) : size(sz){};
    inline void resize(const uint32 &sz) {size=sz;};
} ScratchPad;

extern uint32   vlen;
extern ScratchPad scratchpad;

inline void         LOCK_PIM    ();
inline void         UNLOCK_PIM  ();
int32               GETMEMSIZE  ();
void                MALLOC_PIM  ();
inline void         FREE_PIM    ();

inline PIMSTATUS    ROCC_CONFIG (const int32 &length, const uint32 &bits);
PIMSTATUS           ROCC_NOT    (void *src_addr, void *dest_addr, const int32 &length, const int32 &bits=32);
PIMSTATUS           ROCC_INT2FLOAT  (void *src_addr, void *dest_addr, const int32 &length);
PIMSTATUS           ROCC_FLOAT2INT  (void *src_addr, void *dest_addr, const int32 &length);
PIMSTATUS           ROCC_ADJUSTINTWIDTH (void *src_addr, void *dest_addr, const int32 &length, const int32 &src_bits, const int32 &dest_bits);
PIMSTATUS           ROCC_SQUAREROOT_INT (void *src_addr, void *dest_addr, const int32 &length);
PIMSTATUS           ROCC_SIN    (void *src_addr, void *dest_addr, const int32 &length);
PIMSTATUS           ROCC_LOG    (void *src_addr, void *dest_addr, const int32 &length);
PIMSTATUS           ROCC_POWER  (void *src1_addr, const int32 &src2, void *dest_addr, const int32 &length);
PIMSTATUS           ROCC_POWER  (void *src1_addr, void *src2_addr, void *dest_addr, const int32 &length);
PIMSTATUS           ROCC_SHIFT  (void *src1_addr, const int32 &src2, void *dest_addr, const int32 &length, ShiftDirection dir, ShiftOption opt, const int32 &bits);
PIMSTATUS           ROCC_SHIFT  (void *src1_addr, void *src2_addr, void *dest_addr, const int32 &length, ShiftDirection dir, ShiftOption opt, const int32 &bits);
PIMSTATUS           ROCC_AND    (void *src1_addr, void *src2_addr, void *dest_addr, const int32 &length, const int32 &bits=32);
PIMSTATUS           ROCC_OR (void *src1_addr, void *src2_addr, void *dest_addr, const int32 &length, const int32 &bits=32);
PIMSTATUS           ROCC_XOR    (void *src1_addr, void *src2_addr, void *dest_addr, const int32 &length, const int32 &bits=32);
PIMSTATUS           ROCC_MAX_INT    (void *src1_addr, void *src2_addr, void *dest_addr, const int32 &length);
PIMSTATUS           ROCC_MIN_INT    (void *src1_addr, void *src2_addr, void *dest_addr, const int32 &length);
PIMSTATUS           ROCC_COMP_INT   (void *src1_addr, void *src2_addr, void *dest_addr, const int32 &length);
PIMSTATUS           ROCC_ADD_INT    (void *src1_addr, void *src2_addr, void *dest_addr, const int32 &length);
PIMSTATUS           ROCC_SUB_INT    (void *src1_addr, void *src2_addr, void *dest_addr, const int32 &length);
PIMSTATUS           ROCC_MUL_INT    (void *src1_addr, void *src2_addr, void *dest_addr, const int32 &length);
PIMSTATUS           ROCC_DIV_INT    (void *src1_addr, void *src2_addr, void *dest_addr, const int32 &length);
PIMSTATUS           ROCC_MOD_INT    (void *src1_addr, void *src2_addr, void *dest_addr, const int32 &length);
PIMSTATUS           ROCC_COMP_FLOAT ();
PIMSTATUS           ROCC_ADD_FLOAT  ();
PIMSTATUS           ROCC_SUB_FLOAT  ();
PIMSTATUS           ROCC_MUL_FLOAT  ();
PIMSTATUS           ROCC_DIV_FLOAT  ();

}

#endif
