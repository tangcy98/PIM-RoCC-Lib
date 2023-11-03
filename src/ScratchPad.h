#ifndef _PIM_SCRATCHPAD_
#define _PIM_SCRATCHPAD_

#include <unordered_map>
#include <string>
#include "Basic.h"


namespace PIMAPI{
struct DLinkedNode {
    std::string key;
    spinst_ptr instptr;
    spaddr_t spidx;
    DLinkedNode* prev;
    DLinkedNode* next;
    DLinkedNode(): key(""), instptr(nullptr), spidx(0), prev(nullptr), next(nullptr) {}
    DLinkedNode(const std::string &_key, spinst_ptr _inst, int _spidx): key(_key), instptr(_inst), spidx(_spidx), prev(nullptr), next(nullptr) {}
};

/// @brief A pool storing SPInsts already read
/// @todo do we need also set a max size and use LRU here?
class SPInstPool {
private:
    std::unordered_map<std::string, spinst_ptr> pool;
    spinst_ptr readSPInst(const std::string &s);
    SPInstPool() = default;
    ~SPInstPool() {for (auto &[k,v]:pool) delete v;}
public:
    SPInstPool(const SPInstPool&) = delete;
    SPInstPool(SPInstPool&&) = delete;
    SPInstPool& operator=(const SPInstPool&) = delete;
    SPInstPool& operator=(SPInstPool&&) = delete;
    static spinst_ptr getSPInst(const std::string &s);
};

/** 
 * @brief ScratchPad using LRU
 * @todo this is a real-time image of scratchpad in software level.
 *  Need to make it a firmware in hardware level in the future
 *  and we cannot guarantee the data consistency here
**/   
class ScratchPad {
private:
    std::unordered_map<std::string, DLinkedNode*> instcache;
    DLinkedNode* head;  ///< dummy head
    DLinkedNode* tail;  ///< dummy tail
    unsigned int size;
    unsigned int capacity;
private:
    void addToHead(DLinkedNode* node);
    void removeNode(DLinkedNode* node);
    void moveToHead(DLinkedNode* node);
    DLinkedNode* removeTail();
public:
    ScratchPad(int _capacity);
    ~ScratchPad();
    DLinkedNode* get(const std::string &key);   ///< O(1)
    DLinkedNode* put(const std::string &key, spinst_ptr instptr);      ///< O(1)
};
}   // end of namespace PIMAPI

#endif
