#include "ScratchPad.h"

namespace PIMAPI{
// TODO
spinst_ptr SPInstPool::readSPInst(const std::string &s)
{
    return nullptr;
}

spinst_ptr SPInstPool::getSPInst(const std::string &s)
{
    static SPInstPool instance;
    if (instance.pool.count(s)) {
        return instance.pool[s];
    }
    spinst_ptr _p = instance.readSPInst(s);
    if (_p) instance.pool[s] = _p;
    return _p;
}


ScratchPad::ScratchPad(int _capacity): capacity(_capacity), size(0) {
    // use dummy head and tail
    head = new DLinkedNode();
    tail = new DLinkedNode();
    head->next = tail;
    tail->prev = head;
}

ScratchPad::~ScratchPad()
{
    delete head;
    delete tail;
    for (auto &[_, v] : instcache) {
        delete v;
    }
}

DLinkedNode* ScratchPad::get(const std::string &key) {
    if (!instcache.count(key)) {
        return nullptr;
    }
    DLinkedNode* node = instcache[key];
    moveToHead(node);
    return node;
}

DLinkedNode* ScratchPad::put(const std::string &key, spinst_ptr instptr) {
    DLinkedNode* node = nullptr;
    if (!instcache.count(key)) {
        if (size == capacity) {
            // delete tail node
            DLinkedNode* removed = removeTail();
            node = new DLinkedNode(key, instptr, removed->spidx);
            instcache.erase(removed->key);
            delete removed;
        }
        else {
            node = new DLinkedNode(key, instptr, size);
            ++size;
        }
        instcache[key] = node;
        addToHead(node);
    }
    else {
        // move to head
        node = instcache[key];
        node->instptr = instptr;
        moveToHead(node);
    }
    return node;
}

void ScratchPad::addToHead(DLinkedNode* node) {
    node->prev = head;
    node->next = head->next;
    head->next->prev = node;
    head->next = node;
}

void ScratchPad::removeNode(DLinkedNode* node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
}

void ScratchPad::moveToHead(DLinkedNode* node) {
    removeNode(node);
    addToHead(node);
}

DLinkedNode* ScratchPad::removeTail() {
    DLinkedNode* node = tail->prev;
    removeNode(node);
    return node;
}
}
