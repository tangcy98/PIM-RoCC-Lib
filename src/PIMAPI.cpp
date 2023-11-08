#include "PIMAPI.h"

template <typename T> static T nextPOT(T x)
{
    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return x+1;
}

template <typename T> static T lastPOT(T x)
{
    x += (++x)%2;
    x >>= 1;
    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return x+1;
}



namespace PIMAPI {
using BasicComputeType = PIMBlocks::BasicComputeType;
static void printException(PIMSTATUS e);

// std::unique_ptr<i64> PIMAPI::blockflags(new i64[
//   (PIMBasicInfo::getInstance().params._subarrnums+sizeof(i64)*8-1) / (sizeof(i64)*8)]);

PIMBlocks::_pim_space_collector PIMBlocks::_collector;

ScratchPad PIMBlocks::_sp(PIMBasicInfo::getInstance().params._scratchpad_size);

PIMBasicInfo::PIMBasicInfo()
{
    try {
        int flags = 0;
        asm volatile (
            "rocc.getinfo %[result], %[input_a]"
            : [result] "=r" (flags)
            : [input_a] "r" (&params)
            : "memory");
        if (flags!=0) {
            throw NOPIM;
        }
    }
    catch (PIMSTATUS e) {
        printException(e);
    }
}

u32 PIMBlocks::pimdest32(const u8 &bits, const u16 &offset)
{
    return ((u32)(bits) << 24) | ((u32)(offset) << 16) | (u32)(_baseaddr);
}

PIMBlocks* PIMBlocks::PIMalloc(size_t datasize, unsigned int extracoe)
{
    static const PIMBasicInfo &piminfo = PIMBasicInfo::getInstance();
    if (datasize<=0 || extracoe<=0) {
        return nullptr;
    }

    PIMBlocks *newblock = new PIMBlocks(datasize, extracoe);
    if (newblock->_baseaddr == piminfo.params._max_addrspace) {
        delete newblock;
        return nullptr;
    }
    if (newblock) {
        _collector.s.insert(newblock);
    }
    return newblock;
}

PIMSTATUS PIMBlocks::PIMfree(PIMBlocks* b)
{
    if (!b) {
        return WRONGARG;
    }
    delete b;
    _collector.s.erase(b);
    return OK;
}

PIMBlocks::PIMBlocks(size_t size, unsigned int extracoe)
{
    static const PIMBasicInfo &piminfo = PIMBasicInfo::getInstance();
    _datasize = size;
    _datablocks = (size + piminfo.params._subarr_cols - 1) / piminfo.params._subarr_cols;
    _totalblocks = _datablocks * extracoe;
    _capacity = _totalblocks * piminfo.params._subarr_cols;
    asm volatile (
        "rocc.lock %[result], %[input_a], %[input_b]"
        : [result] "=r" (_baseaddr)
        : [input_a] "r" (size), 
          [input_b] "r" (_totalblocks)
        : "memory");
}

PIMBlocks::~PIMBlocks()
{
    static const PIMBasicInfo &piminfo = PIMBasicInfo::getInstance();
    if (_baseaddr < piminfo.params._subarrnums) {
        asm volatile (
        "rocc.unlock %[input_a],%[input_b]"
        : 
        : [input_a] "r" (_baseaddr),
          [input_b] "r" (_totalblocks)
        : "memory");
    }
}

// TODO
PIMSTATUS PIMBlocks::PIMResize(size_t datasize)
{
    if (datasize > _capacity) {
        return WRONGARG;
    }
    _datasize = datasize;

    return OK;
}

PIMSTATUS PIMBlocks::PIMVload(void *addr, u8 bits, u16 rowidx, size_t len)
{
    // should we ignore len check for performance?
    if (len > _datasize) {
        return VLENCONSTRAINT;
    }
    len = (len==0 ? _datasize : len);
    static const PIMBasicInfo &info = PIMBasicInfo::getInstance();

    int flags;
    asm volatile (
        "rocc.vload %[result], %[input_a], %[input_b]"
        : [result] "=r" (flags)
        : [input_a] "r" (addr),
          [input_b] "r" (pimdest32(bits, rowidx))
        : "memory");
    if (!flags) {
        // Do not use try-catch because exception
        // exception might be a normal return
        return UNKNOWN;
    }
    return OK;
}

PIMSTATUS PIMBlocks::PIMSload(u64 num, u8 bits, size_t len)
{
    if (len > _datasize) {
        return VLENCONSTRAINT;
    }
    len = (len==0 ? _datasize : len);
    static const PIMBasicInfo &info = PIMBasicInfo::getInstance();

    int flags;
    asm volatile (
        "rocc.sload %[result], %[input_a], %[input_b]"
        : [result] "=r" (flags)
        : [input_a] "r" (num),
          [input_b] "r" (pimdest32(bits, 0))
        : "memory");
    if (!flags) {
        // Do not use try-catch because exception
        // exception might be a normal return
        return UNKNOWN;
    }
    return OK;
}

PIMSTATUS PIMBlocks::PIMVstore(void *addr, u8 bits, u16 rowidx, size_t len)
{
    // should we ignore len check for performance?
    if (len > _datasize) {
        return VLENCONSTRAINT;
    }
    len = (len==0 ? _datasize : len);
    static const PIMBasicInfo &info = PIMBasicInfo::getInstance();
    size_t addroffsetstride = bits<=8 ? 1 : (bits<=16 ? 2 : (bits<=32 ? 4 : (bits<=64 ? 8 : 16)));

    for (size_t offset=0, idx=0, addroffset=(size_t)(addr); 
      offset < len; 
      offset+=info.params._ctrl_cols_stride, ++idx, addroffset+=addroffsetstride)
    {
        int flags;
        asm volatile (
            "rocc.vstore %[result], %[input_a], %[input_b]"
            : [result] "=r" (flags)
            : [input_a] "r" ((void*)(addroffset)),
              [input_b] "r" (pimdest32(bits, rowidx))
            : "memory");
        if (!flags) {
            return UNKNOWN;
        }
    }
    return OK;
}

PIMSTATUS PIMBlocks::PIMVcopy(u8 bits, u16 srcrowidx, u16 destrowidx, size_t len)
{
    // should we ignore len check for performance?
    if (len > _datasize) {
        return VLENCONSTRAINT;
    }
    len = (len==0 ? _datasize : len);
    static const PIMBasicInfo &info = PIMBasicInfo::getInstance();

    int flags;
    asm volatile (
        "rocc.vcopy %[result], %[input_a], %[input_b]"
        : [result] "=r" (flags)
        : [input_a] "r" ((u64)(destrowidx)),
          [input_b] "r" (pimdest32(bits, srcrowidx))
        : "memory");
    if (!flags) {
        return UNKNOWN;
    }
    return OK;
}

PIMSTATUS PIMBlocks::PIMVVcompute(BasicComputeType ctype, u8 bits, u16 srcrowidx, u16 destrowidx, size_t len)
{
    // should we ignore len check for performance?
    if (len > _datasize) {
        return VLENCONSTRAINT;
    }
    len = (len==0 ? _datasize : len);
    static const PIMBasicInfo &info = PIMBasicInfo::getInstance();

    int flags;
    switch (ctype)
    {
    case PIMNOT:
        /* code */
        asm volatile (
            "rocc.vnot %[result], %[input_a], %[input_b]"
            : [result] "=r" (flags)
            : [input_a] "r" ((u64)(destrowidx)),
                [input_b] "r" (pimdest32(bits, srcrowidx))
            : "memory");
        break;

    case PIMAND:
        asm volatile (
            "rocc.vand %[result], %[input_a], %[input_b]"
            : [result] "=r" (flags)
            : [input_a] "r" ((u64)(destrowidx)),
                [input_b] "r" (pimdest32(bits, srcrowidx))
            : "memory");
        break;
    case PIMOR:
        asm volatile (
            "rocc.vor %[result], %[input_a], %[input_b]"
            : [result] "=r" (flags)
            : [input_a] "r" ((u64)(destrowidx)),
                [input_b] "r" (pimdest32(bits, srcrowidx))
            : "memory");
        break;

    case PIMNOR:
        asm volatile (
            "rocc.vnor %[result], %[input_a], %[input_b]"
            : [result] "=r" (flags)
            : [input_a] "r" ((u64)(destrowidx)),
                [input_b] "r" (pimdest32(bits, srcrowidx))
            : "memory");
        break;

    case PIMXOR:
        asm volatile (
            "rocc.vxor %[result], %[input_a], %[input_b]"
            : [result] "=r" (flags)
            : [input_a] "r" ((u64)(destrowidx)),
                [input_b] "r" (pimdest32(bits, srcrowidx))
            : "memory");
        break;

    case PIMADD:
        asm volatile (
            "rocc.vadd %[result], %[input_a], %[input_b]"
            : [result] "=r" (flags)
            : [input_a] "r" ((u64)(destrowidx)),
                [input_b] "r" (pimdest32(bits, srcrowidx))
            : "memory");
        break;

    case PIMSUB:
        asm volatile (
            "rocc.vsub %[result], %[input_a], %[input_b]"
            : [result] "=r" (flags)
            : [input_a] "r" ((u64)(destrowidx)),
                [input_b] "r" (pimdest32(bits, srcrowidx))
            : "memory");
        break;

    case PIMMUL:
        asm volatile (
            "rocc.vmul %[result], %[input_a], %[input_b]"
            : [result] "=r" (flags)
            : [input_a] "r" ((u64)(destrowidx)),
                [input_b] "r" (pimdest32(bits, srcrowidx))
            : "memory");
        break;

    case PIMCMP:
        asm volatile (
            "rocc.vcmp %[result], %[input_a], %[input_b]"
            : [result] "=r" (flags)
            : [input_a] "r" ((u64)(destrowidx)),
                [input_b] "r" (pimdest32(bits, srcrowidx))
            : "memory");
        break;
    default:
        return WRONGARG;
        break;
    }
    if (!flags) {
        return UNKNOWN;
    }

    return OK;
}

PIMSTATUS PIMBlocks::PIMloadcomputestore(BasicComputeType ctype, void **srcaddr, void *destaddr, u8 bits, size_t len)
{
    PIMSTATUS status;
    int opnum;
    // 1. load
    if ((status = PIMVload(srcaddr[0], bits, 0, len)) != OK) {
        return status;
    }
    if (ctype > PIM1OPMAX) {
        opnum=2;
        if ((status = PIMVload(srcaddr[1], bits, bits, len)) != OK) {
            return status;
        }
    }
    if (ctype > PIM2OPMAX) {
        opnum=3;
        if ((status = PIMVload(srcaddr[2], bits, bits+bits, len)) != OK) {
            return status;
        }
    }

    // 2. compute
    if ((status = PIMVVcompute(ctype, bits, 0, bits*opnum, len)) != OK) {
        return status;
    }

    // 3. store
    if ((status = PIMVstore(destaddr, bits, bits*opnum, len)) != OK) {
        return status;
    }
    return OK;
}

PIMSTATUS PIMBlocks::PIMSPexecute(std::string s, u16 srcrowidx, u16 destrowidx, size_t len)
{
    static const PIMBasicInfo &info = PIMBasicInfo::getInstance();
    s += '-' + std::to_string(_totalblocks/_datablocks);
    DLinkedNode* pnode =  _sp.get(s);
    int flags;

    // 1. If not exist, load inst.
    if (!pnode) {
        spinst_ptr _ip = SPInstPool::getSPInst(s);
        pnode = _ip ? _sp.put(s, _ip) : nullptr;
        if (!pnode) {
            return UNKNOWN;
        }
        asm volatile (
            "rocc.spload %[result], %[input_a], %[input_b]"
            : [result] "=r" (flags)
            : [input_a] "r" (pnode->instptr),
              [input_b] "r" (pnode->spidx)
            : "memory");
        if (!flags) {
            return UNKNOWN;
        }
    }

    // 2. Execute the inst
    asm volatile (
        "rocc.spexec %[result], %[input_a], %[input_b]"
        : [result] "=r" (flags)
        : [input_a] "r" ((u64)(destrowidx)),
          [input_b] "r" (((u32)(srcrowidx)+(u32)(_baseaddr)*(u32)(info.params._subarr_rows)) << 8)
        : "memory");
    if (!flags) {
        return UNKNOWN;
    }

    return OK;
}

static void  printException(PIMSTATUS e)
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

}   // end of namespace PIMAPI

