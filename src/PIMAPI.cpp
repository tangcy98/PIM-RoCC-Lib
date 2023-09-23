#include "PIMAPI.h"

namespace PIMAPI {
    uint32   vlen;
    ScratchPad scratchpad;
}

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

inline void         PIMAPI::LOCK_PIM    ()
{

}

inline void         PIMAPI::UNLOCK_PIM  ()
{

}

int32               PIMAPI::GETMEMSIZE  ()
{
    return MAXARRYANUM;
}


inline PIMAPI::PIMSTATUS    PIMAPI::ROCC_CONFIG (const uint32 &length, const uint32 &bits)
{
    uint32 size;
    try {
        if (bits > 64 || bits < 8 || bits%2) {
            throw INVALIDBITS;
        }
        if (length % ArraySize) {
            throw VLENCONSTRAINT;
        }
        LOCK_PIM();
        size = GETMEMSIZE();
        if (size <= 0) {
            UNLOCK_PIM();
            throw MEMFULL;
        }
        UNLOCK_PIM();
    }
    catch (PIMSTATUS e) {
        printException(e);
        return e;
    }
    asm volatile (
        "rccfg %[result], %[input_a], %[input_b]"   /// no inst loading
        : [result] "=r" (dest_addr)
        : [input_b] "r" (scratchpad.len));
    return OK;
}

PIMAPI::PIMSTATUS   PIMAPI::ROCC_NOT(void *src_addr, void *dest_addr, const int32 &length, const int32 &bits=32)
{
    uint32 size;
    try {
        if (bits > 64 || bits < 8 || bits%2) {
            throw INVALIDBITS;
        }
        if (length % ArraySize) {
            throw VLENCONSTRAINT;
        }
        LOCK_PIM();
        size = GETMEMSIZE();
        if (size <= 0) {
            UNLOCK_PIM();
            throw MEMFULL;
        }
        UNLOCK_PIM();
    }
    catch (PIMSTATUS e) {
        printException(e);
        return e;
    };

    if (scratchpad.len != length) {
        scratchpad.len = length;
        asm volatile (
            "rcld7 %[result], %%x0, %[input_b]"   /// no inst loading
            : [result] "=r" (dest_addr)
            : [input_b] "r" (scratchpad.len));
    }

    asm volatile (
          "rcnot %[result], %[input_a], %%x0"   /// need only one reg
          : [result] "=r" (dest_addr)
          : [input_a] "r" (src_addr)
          : "memory");     /// mem changed
    return OK;
}

PIMAPI::PIMSTATUS   PIMAPI::ROCC_SQUAREROOT_INT(void *src_addr, void *dest_addr, const int32 &length)
{
    uint32 size;
    try {
        if (length % ArraySize) {
            throw VLENCONSTRAINT;
        }
        LOCK_PIM();
        size = GETMEMSIZE();
        if (size <= 0) {
            UNLOCK_PIM();
            throw MEMFULL;
        }
        UNLOCK_PIM();
    }
    catch (PIMSTATUS e) {
        printException(e);
        return e;
    };

    if (scratchpad.len != length) {
        scratchpad.len = length;
    }


    asm volatile (
        "rcld7 %[result], %%x0, %[input_b]"   /// no inst loading
        : [result] "=r" (dest_addr)
        : [input_b] "r" (scratchpad.len));


    asm volatile (
          "rcmul %[result], %[input_a], %%x0"   /// need only one reg
          : [result] "=r" (dest_addr)
          : [input_a] "r" (src_addr)
          : "memory");     /// mem changed
    return OK;
}

PIMAPI::PIMSTATUS   PIMAPI::ROCC_AND(void *src_addr, void *dest_addr, const int32 &length, const int32 &bits=32)
{
    uint32 size;
    try {
        if (bits > 64 || bits < 8 || bits%2) {
            throw INVALIDBITS;
        }
        if (length % ArraySize) {
            throw VLENCONSTRAINT;
        }
        LOCK_PIM();
        size = GETMEMSIZE();
        if (size <= 0) {
            UNLOCK_PIM();
            throw MEMFULL;
        }
        UNLOCK_PIM();
    }
    catch (PIMSTATUS e) {
        printException(e);
        return e;
    };

    if (scratchpad.len != length) {
        scratchpad.len = length;
        asm volatile (
            "rcld7 %[result], %%x0, %[input_b]"   /// no inst loading
            : [result] "=r" (dest_addr)
            : [input_b] "r" (scratchpad.len));
    }

    asm volatile (
          "rcnot %[result], %[input_a], %%x0"   /// need only one reg
          : [result] "=r" (dest_addr)
          : [input_a] "r" (src_addr)
          : "memory");     /// mem changed
    return OK;
}

PIMAPI::PIMSTATUS   PIMAPI::ROCC_OR(void *src_addr, void *dest_addr, const int32 &length, const int32 &bits=32)
{
    uint32 size;
    try {
        if (bits > 64 || bits < 8 || bits%2) {
            throw INVALIDBITS;
        }
        if (length % ArraySize) {
            throw VLENCONSTRAINT;
        }
        LOCK_PIM();
        size = GETMEMSIZE();
        if (size <= 0) {
            UNLOCK_PIM();
            throw MEMFULL;
        }
        UNLOCK_PIM();
    }
    catch (PIMSTATUS e) {
        printException(e);
        return e;
    };

    if (scratchpad.len != length) {
        scratchpad.len = length;
        asm volatile (
            "rcld7 %[result], %%x0, %[input_b]"   /// no inst loading
            : [result] "=r" (dest_addr)
            : [input_b] "r" (scratchpad.len));
    }

    asm volatile (
          "rcnot %[result], %[input_a], %%x0"   /// need only one reg
          : [result] "=r" (dest_addr)
          : [input_a] "r" (src_addr)
          : "memory");     /// mem changed
    return OK;
}

PIMAPI::PIMSTATUS   PIMAPI::ROCC_XOR(void *src_addr, void *dest_addr, const int32 &length, const int32 &bits=32)
{
    uint32 size;
    try {
        if (bits > 64 || bits < 8 || bits%2) {
            throw INVALIDBITS;
        }
        if (length % ArraySize) {
            throw VLENCONSTRAINT;
        }
        LOCK_PIM();
        size = GETMEMSIZE();
        if (size <= 0) {
            UNLOCK_PIM();
            throw MEMFULL;
        }
        UNLOCK_PIM();
    }
    catch (PIMSTATUS e) {
        printException(e);
        return e;
    };

    if (scratchpad.len != length) {
        scratchpad.len = length;
        asm volatile (
            "rcld7 %[result], %%x0, %[input_b]"   /// no inst loading
            : [result] "=r" (dest_addr)
            : [input_b] "r" (scratchpad.len));
    }

    asm volatile (
          "rcnot %[result], %[input_a], %%x0"   /// need only one reg
          : [result] "=r" (dest_addr)
          : [input_a] "r" (src_addr)
          : "memory");     /// mem changed
    return OK;
}

PIMAPI::PIMSTATUS   PIMAPI::ROCC_NOT(void *src_addr, void *dest_addr, const int32 &length, const int32 &bits=32)
{
    uint32 size;
    try {
        if (bits > 64 || bits < 8 || bits%2) {
            throw INVALIDBITS;
        }
        if (length % ArraySize) {
            throw VLENCONSTRAINT;
        }
        LOCK_PIM();
        size = GETMEMSIZE();
        if (size <= 0) {
            UNLOCK_PIM();
            throw MEMFULL;
        }
        UNLOCK_PIM();
    }
    catch (PIMSTATUS e) {
        printException(e);
        return e;
    };

    if (scratchpad.len != length) {
        scratchpad.len = length;
        asm volatile (
            "rcld7 %[result], %%x0, %[input_b]"   /// no inst loading
            : [result] "=r" (dest_addr)
            : [input_b] "r" (scratchpad.len));
    }

    asm volatile (
          "rcnot %[result], %[input_a], %%x0"   /// need only one reg
          : [result] "=r" (dest_addr)
          : [input_a] "r" (src_addr)
          : "memory");     /// mem changed
    return OK;
}

PIMAPI::PIMSTATUS   PIMAPI::ROCC_NOT(void *src_addr, void *dest_addr, const int32 &length, const int32 &bits=32)
{
    uint32 size;
    try {
        if (bits > 64 || bits < 8 || bits%2) {
            throw INVALIDBITS;
        }
        if (length % ArraySize) {
            throw VLENCONSTRAINT;
        }
        LOCK_PIM();
        size = GETMEMSIZE();
        if (size <= 0) {
            UNLOCK_PIM();
            throw MEMFULL;
        }
        UNLOCK_PIM();
    }
    catch (PIMSTATUS e) {
        printException(e);
        return e;
    };

    if (scratchpad.len != length) {
        scratchpad.len = length;
        asm volatile (
            "rcld7 %[result], %%x0, %[input_b]"   /// no inst loading
            : [result] "=r" (dest_addr)
            : [input_b] "r" (scratchpad.len));
    }

    asm volatile (
          "rcnot %[result], %[input_a], %%x0"   /// need only one reg
          : [result] "=r" (dest_addr)
          : [input_a] "r" (src_addr)
          : "memory");     /// mem changed
    return OK;
}

PIMAPI::PIMSTATUS   PIMAPI::ROCC_NOT(void *src_addr, void *dest_addr, const int32 &length, const int32 &bits=32)
{
    uint32 size;
    try {
        if (bits > 64 || bits < 8 || bits%2) {
            throw INVALIDBITS;
        }
        if (length % ArraySize) {
            throw VLENCONSTRAINT;
        }
        LOCK_PIM();
        size = GETMEMSIZE();
        if (size <= 0) {
            UNLOCK_PIM();
            throw MEMFULL;
        }
        UNLOCK_PIM();
    }
    catch (PIMSTATUS e) {
        printException(e);
        return e;
    };

    if (scratchpad.len != length) {
        scratchpad.len = length;
        asm volatile (
            "rcld7 %[result], %%x0, %[input_b]"   /// no inst loading
            : [result] "=r" (dest_addr)
            : [input_b] "r" (scratchpad.len));
    }

    asm volatile (
          "rcnot %[result], %[input_a], %%x0"   /// need only one reg
          : [result] "=r" (dest_addr)
          : [input_a] "r" (src_addr)
          : "memory");     /// mem changed
    return OK;
}

PIMAPI::PIMSTATUS   PIMAPI::ROCC_NOT(void *src_addr, void *dest_addr, const int32 &length, const int32 &bits=32)
{
    uint32 size;
    try {
        if (bits > 64 || bits < 8 || bits%2) {
            throw INVALIDBITS;
        }
        if (length % ArraySize) {
            throw VLENCONSTRAINT;
        }
        LOCK_PIM();
        size = GETMEMSIZE();
        if (size <= 0) {
            UNLOCK_PIM();
            throw MEMFULL;
        }
        UNLOCK_PIM();
    }
    catch (PIMSTATUS e) {
        printException(e);
        return e;
    };

    if (scratchpad.len != length) {
        scratchpad.len = length;
        asm volatile (
            "rcld7 %[result], %%x0, %[input_b]"   /// no inst loading
            : [result] "=r" (dest_addr)
            : [input_b] "r" (scratchpad.len));
    }

    asm volatile (
          "rcnot %[result], %[input_a], %%x0"   /// need only one reg
          : [result] "=r" (dest_addr)
          : [input_a] "r" (src_addr)
          : "memory");     /// mem changed
    return OK;
}

PIMAPI::PIMSTATUS   PIMAPI::ROCC_NOT(void *src_addr, void *dest_addr, const int32 &length, const int32 &bits=32)
{
    uint32 size;
    try {
        if (bits > 64 || bits < 8 || bits%2) {
            throw INVALIDBITS;
        }
        if (length % ArraySize) {
            throw VLENCONSTRAINT;
        }
        LOCK_PIM();
        size = GETMEMSIZE();
        if (size <= 0) {
            UNLOCK_PIM();
            throw MEMFULL;
        }
        UNLOCK_PIM();
    }
    catch (PIMSTATUS e) {
        printException(e);
        return e;
    };

    if (scratchpad.len != length) {
        scratchpad.len = length;
        asm volatile (
            "rcld7 %[result], %%x0, %[input_b]"   /// no inst loading
            : [result] "=r" (dest_addr)
            : [input_b] "r" (scratchpad.len));
    }

    asm volatile (
          "rcnot %[result], %[input_a], %%x0"   /// need only one reg
          : [result] "=r" (dest_addr)
          : [input_a] "r" (src_addr)
          : "memory");     /// mem changed
    return OK;
}
