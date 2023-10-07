#include "PIMAPI.h"

namespace PIMAPI {

// std::unique_ptr<i64> PIMAPI::blockflags(new i64[
//   (PIMBasicInfo::getInstance().params._subarrnums+sizeof(i64)*8-1) / (sizeof(i64)*8)]);

PIMBasicInfo::PIMBasicInfo()
{
    try {
        int flags = 1;
        asm volatile (
            "rocc.getinfo %[result], %[input_a]"
            : [result] "=r" (flags)
            : [input_a] "r" (&params)
            : "memory");
        if (flags==0) {
            throw NOPIM;
        }
    }
    catch (PIMSTATUS e) {
        printException(e);
    }
}

PIMBlocks* PIMBlocks::PIMalloc(size_t size)
{
    if (size<0) {
        return nullptr;
    }
    PIMBasicInfo &piminfo = PIMBasicInfo::getInstance();
    PIMBlocks *newblock = new PIMBlocks(size);
    // 1. query for idle pimblocks
    // asm volatile (
    //   "rocc.status %[input_a]"
    //   : 
    //   : [input_a] "r" (blockflags.get())
    //   : "memory");
    
    // 2. lock
    int blocks = newblock->_blocknum;
    for (u16 i = 0; i < piminfo.params._max_usable_subarr && blocks > 0; i+=piminfo.params._ctrl_subarr_per_inst) {
        int locked;
        asm volatile (
          "rocc.lock %[result], %[input_a]"
          : [result] "=r" (locked)
          : [input_a] "r" (i)
          : "memory");
        if (locked) {
            blocks -= piminfo.params._max_usable_subarr;
            newblock->blocks.push_back(i);
        }
    }

    // 3. failed -> unlock the locked blocks
    if (blocks > 0) {
        for (auto &i : newblock->blocks) {
            asm volatile (
            "rocc.unlock %[input_a]"
            : 
            : [input_a] "r" (i)
            : "memory");
        }
        delete newblock;
        return nullptr;
    }
    _collector.s.insert(newblock);
    return newblock;
}

PIMSTATUS PIMBlocks::PIMfree(PIMBlocks* b)
{
    if (b == nullptr) {
        return WRONGARG;
    }
    delete b;
    _collector.s.erase(b);
}

PIMBlocks::PIMBlocks(size_t size):_datasize(size)
{
    PIMBasicInfo &piminfo = PIMBasicInfo::getInstance();
    _blocknum = (size + piminfo.params._subarr_cols - 1) / piminfo.params._subarr_cols;
}

PIMBlocks::~PIMBlocks()
{
    for (auto &i : blocks) {
        asm volatile (
        "rocc.unlock %[input_a]"
        : 
        : [input_a] "r" (i)
        : "memory");
    }
}



}   // end of namespace PIMAPI

inline static void  PIMAPI::printException(PIMSTATUS e)
{
    switch (e)
    {
    case MEMFULL:

        break;
    case WRONGARG:

        break;
    case INVALIDBITS:

        break;
    case VLENCONSTRAINT:

        break;
    case UNKNOWN:

        break;
    default:
        break;
    }
}


// inline PIMAPI::PIMSTATUS    PIMAPI::ROCC_CONFIG (const uint32 &length, const uint32 &bits)
// {
//     uint32 size;
//     try {
//         if (bits > 64 || bits < 8 || bits%2) {
//             throw INVALIDBITS;
//         }
//         if (length % ArraySize) {
//             throw VLENCONSTRAINT;
//         }
//         LOCK_PIM();
//         size = GETMEMSIZE();
//         if (size <= 0) {
//             UNLOCK_PIM();
//             throw MEMFULL;
//         }
//         UNLOCK_PIM();
//     }
//     catch (PIMSTATUS e) {
//         printException(e);
//         return e;
//     }
//     asm volatile (
//         "rccfg %[result], %[input_a], %[input_b]"   /// no inst loading
//         : [result] "=r" (dest_addr)
//         : [input_b] "r" (scratchpad.len));
//     return OK;
// }

// PIMAPI::PIMSTATUS   PIMAPI::ROCC_NOT(void *src_addr, void *dest_addr, const int32 &length, const int32 &bits=32)
// {
//     uint32 size;
//     try {
//         if (bits > 64 || bits < 8 || bits%2) {
//             throw INVALIDBITS;
//         }
//         if (length % ArraySize) {
//             throw VLENCONSTRAINT;
//         }
//         LOCK_PIM();
//         size = GETMEMSIZE();
//         if (size <= 0) {
//             UNLOCK_PIM();
//             throw MEMFULL;
//         }
//         UNLOCK_PIM();
//     }
//     catch (PIMSTATUS e) {
//         printException(e);
//         return e;
//     };

//     if (scratchpad.len != length) {
//         scratchpad.len = length;
//         asm volatile (
//             "rcld7 %[result], %%x0, %[input_b]"   /// no inst loading
//             : [result] "=r" (dest_addr)
//             : [input_b] "r" (scratchpad.len));
//     }

//     asm volatile (
//           "rcnot %[result], %[input_a], %%x0"   /// need only one reg
//           : [result] "=r" (dest_addr)
//           : [input_a] "r" (src_addr)
//           : "memory");     /// mem changed
//     return OK;
// }

// PIMAPI::PIMSTATUS   PIMAPI::ROCC_SQUAREROOT_INT(void *src_addr, void *dest_addr, const int32 &length)
// {
//     uint32 size;
//     try {
//         if (length % ArraySize) {
//             throw VLENCONSTRAINT;
//         }
//         LOCK_PIM();
//         size = GETMEMSIZE();
//         if (size <= 0) {
//             UNLOCK_PIM();
//             throw MEMFULL;
//         }
//         UNLOCK_PIM();
//     }
//     catch (PIMSTATUS e) {
//         printException(e);
//         return e;
//     };

//     if (scratchpad.len != length) {
//         scratchpad.len = length;
//     }


//     asm volatile (
//         "rcld7 %[result], %%x0, %[input_b]"   /// no inst loading
//         : [result] "=r" (dest_addr)
//         : [input_b] "r" (scratchpad.len));


//     asm volatile (
//           "rcmul %[result], %[input_a], %%x0"   /// need only one reg
//           : [result] "=r" (dest_addr)
//           : [input_a] "r" (src_addr)
//           : "memory");     /// mem changed
//     return OK;
// }

// PIMAPI::PIMSTATUS   PIMAPI::ROCC_AND(void *src_addr, void *dest_addr, const int32 &length, const int32 &bits=32)
// {
//     uint32 size;
//     try {
//         if (bits > 64 || bits < 8 || bits%2) {
//             throw INVALIDBITS;
//         }
//         if (length % ArraySize) {
//             throw VLENCONSTRAINT;
//         }
//         LOCK_PIM();
//         size = GETMEMSIZE();
//         if (size <= 0) {
//             UNLOCK_PIM();
//             throw MEMFULL;
//         }
//         UNLOCK_PIM();
//     }
//     catch (PIMSTATUS e) {
//         printException(e);
//         return e;
//     };

//     if (scratchpad.len != length) {
//         scratchpad.len = length;
//         asm volatile (
//             "rcld7 %[result], %%x0, %[input_b]"   /// no inst loading
//             : [result] "=r" (dest_addr)
//             : [input_b] "r" (scratchpad.len));
//     }

//     asm volatile (
//           "rcnot %[result], %[input_a], %%x0"   /// need only one reg
//           : [result] "=r" (dest_addr)
//           : [input_a] "r" (src_addr)
//           : "memory");     /// mem changed
//     return OK;
// }

// PIMAPI::PIMSTATUS   PIMAPI::ROCC_OR(void *src_addr, void *dest_addr, const int32 &length, const int32 &bits=32)
// {
//     uint32 size;
//     try {
//         if (bits > 64 || bits < 8 || bits%2) {
//             throw INVALIDBITS;
//         }
//         if (length % ArraySize) {
//             throw VLENCONSTRAINT;
//         }
//         LOCK_PIM();
//         size = GETMEMSIZE();
//         if (size <= 0) {
//             UNLOCK_PIM();
//             throw MEMFULL;
//         }
//         UNLOCK_PIM();
//     }
//     catch (PIMSTATUS e) {
//         printException(e);
//         return e;
//     };

//     if (scratchpad.len != length) {
//         scratchpad.len = length;
//         asm volatile (
//             "rcld7 %[result], %%x0, %[input_b]"   /// no inst loading
//             : [result] "=r" (dest_addr)
//             : [input_b] "r" (scratchpad.len));
//     }

//     asm volatile (
//           "rcnot %[result], %[input_a], %%x0"   /// need only one reg
//           : [result] "=r" (dest_addr)
//           : [input_a] "r" (src_addr)
//           : "memory");     /// mem changed
//     return OK;
// }

// PIMAPI::PIMSTATUS   PIMAPI::ROCC_XOR(void *src_addr, void *dest_addr, const int32 &length, const int32 &bits=32)
// {
//     uint32 size;
//     try {
//         if (bits > 64 || bits < 8 || bits%2) {
//             throw INVALIDBITS;
//         }
//         if (length % ArraySize) {
//             throw VLENCONSTRAINT;
//         }
//         LOCK_PIM();
//         size = GETMEMSIZE();
//         if (size <= 0) {
//             UNLOCK_PIM();
//             throw MEMFULL;
//         }
//         UNLOCK_PIM();
//     }
//     catch (PIMSTATUS e) {
//         printException(e);
//         return e;
//     };

//     if (scratchpad.len != length) {
//         scratchpad.len = length;
//         asm volatile (
//             "rcld7 %[result], %%x0, %[input_b]"   /// no inst loading
//             : [result] "=r" (dest_addr)
//             : [input_b] "r" (scratchpad.len));
//     }

//     asm volatile (
//           "rcnot %[result], %[input_a], %%x0"   /// need only one reg
//           : [result] "=r" (dest_addr)
//           : [input_a] "r" (src_addr)
//           : "memory");     /// mem changed
//     return OK;
// }

// PIMAPI::PIMSTATUS   PIMAPI::ROCC_NOT(void *src_addr, void *dest_addr, const int32 &length, const int32 &bits=32)
// {
//     uint32 size;
//     try {
//         if (bits > 64 || bits < 8 || bits%2) {
//             throw INVALIDBITS;
//         }
//         if (length % ArraySize) {
//             throw VLENCONSTRAINT;
//         }
//         LOCK_PIM();
//         size = GETMEMSIZE();
//         if (size <= 0) {
//             UNLOCK_PIM();
//             throw MEMFULL;
//         }
//         UNLOCK_PIM();
//     }
//     catch (PIMSTATUS e) {
//         printException(e);
//         return e;
//     };

//     if (scratchpad.len != length) {
//         scratchpad.len = length;
//         asm volatile (
//             "rcld7 %[result], %%x0, %[input_b]"   /// no inst loading
//             : [result] "=r" (dest_addr)
//             : [input_b] "r" (scratchpad.len));
//     }

//     asm volatile (
//           "rcnot %[result], %[input_a], %%x0"   /// need only one reg
//           : [result] "=r" (dest_addr)
//           : [input_a] "r" (src_addr)
//           : "memory");     /// mem changed
//     return OK;
// }

// PIMAPI::PIMSTATUS   PIMAPI::ROCC_NOT(void *src_addr, void *dest_addr, const int32 &length, const int32 &bits=32)
// {
//     uint32 size;
//     try {
//         if (bits > 64 || bits < 8 || bits%2) {
//             throw INVALIDBITS;
//         }
//         if (length % ArraySize) {
//             throw VLENCONSTRAINT;
//         }
//         LOCK_PIM();
//         size = GETMEMSIZE();
//         if (size <= 0) {
//             UNLOCK_PIM();
//             throw MEMFULL;
//         }
//         UNLOCK_PIM();
//     }
//     catch (PIMSTATUS e) {
//         printException(e);
//         return e;
//     };

//     if (scratchpad.len != length) {
//         scratchpad.len = length;
//         asm volatile (
//             "rcld7 %[result], %%x0, %[input_b]"   /// no inst loading
//             : [result] "=r" (dest_addr)
//             : [input_b] "r" (scratchpad.len));
//     }

//     asm volatile (
//           "rcnot %[result], %[input_a], %%x0"   /// need only one reg
//           : [result] "=r" (dest_addr)
//           : [input_a] "r" (src_addr)
//           : "memory");     /// mem changed
//     return OK;
// }

// PIMAPI::PIMSTATUS   PIMAPI::ROCC_NOT(void *src_addr, void *dest_addr, const int32 &length, const int32 &bits=32)
// {
//     uint32 size;
//     try {
//         if (bits > 64 || bits < 8 || bits%2) {
//             throw INVALIDBITS;
//         }
//         if (length % ArraySize) {
//             throw VLENCONSTRAINT;
//         }
//         LOCK_PIM();
//         size = GETMEMSIZE();
//         if (size <= 0) {
//             UNLOCK_PIM();
//             throw MEMFULL;
//         }
//         UNLOCK_PIM();
//     }
//     catch (PIMSTATUS e) {
//         printException(e);
//         return e;
//     };

//     if (scratchpad.len != length) {
//         scratchpad.len = length;
//         asm volatile (
//             "rcld7 %[result], %%x0, %[input_b]"   /// no inst loading
//             : [result] "=r" (dest_addr)
//             : [input_b] "r" (scratchpad.len));
//     }

//     asm volatile (
//           "rcnot %[result], %[input_a], %%x0"   /// need only one reg
//           : [result] "=r" (dest_addr)
//           : [input_a] "r" (src_addr)
//           : "memory");     /// mem changed
//     return OK;
// }

// PIMAPI::PIMSTATUS   PIMAPI::ROCC_NOT(void *src_addr, void *dest_addr, const int32 &length, const int32 &bits=32)
// {
//     uint32 size;
//     try {
//         if (bits > 64 || bits < 8 || bits%2) {
//             throw INVALIDBITS;
//         }
//         if (length % ArraySize) {
//             throw VLENCONSTRAINT;
//         }
//         LOCK_PIM();
//         size = GETMEMSIZE();
//         if (size <= 0) {
//             UNLOCK_PIM();
//             throw MEMFULL;
//         }
//         UNLOCK_PIM();
//     }
//     catch (PIMSTATUS e) {
//         printException(e);
//         return e;
//     };

//     if (scratchpad.len != length) {
//         scratchpad.len = length;
//         asm volatile (
//             "rcld7 %[result], %%x0, %[input_b]"   /// no inst loading
//             : [result] "=r" (dest_addr)
//             : [input_b] "r" (scratchpad.len));
//     }

//     asm volatile (
//           "rcnot %[result], %[input_a], %%x0"   /// need only one reg
//           : [result] "=r" (dest_addr)
//           : [input_a] "r" (src_addr)
//           : "memory");     /// mem changed
//     return OK;
// }

// PIMAPI::PIMSTATUS   PIMAPI::ROCC_NOT(void *src_addr, void *dest_addr, const int32 &length, const int32 &bits=32)
// {
//     uint32 size;
//     try {
//         if (bits > 64 || bits < 8 || bits%2) {
//             throw INVALIDBITS;
//         }
//         if (length % ArraySize) {
//             throw VLENCONSTRAINT;
//         }
//         LOCK_PIM();
//         size = GETMEMSIZE();
//         if (size <= 0) {
//             UNLOCK_PIM();
//             throw MEMFULL;
//         }
//         UNLOCK_PIM();
//     }
//     catch (PIMSTATUS e) {
//         printException(e);
//         return e;
//     };

//     if (scratchpad.len != length) {
//         scratchpad.len = length;
//         asm volatile (
//             "rcld7 %[result], %%x0, %[input_b]"   /// no inst loading
//             : [result] "=r" (dest_addr)
//             : [input_b] "r" (scratchpad.len));
//     }

//     asm volatile (
//           "rcnot %[result], %[input_a], %%x0"   /// need only one reg
//           : [result] "=r" (dest_addr)
//           : [input_a] "r" (src_addr)
//           : "memory");     /// mem changed
//     return OK;
// }
