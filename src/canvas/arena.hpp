#pragma once

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <new>

namespace plexy::ui {


class FrameArena {
public:
    FrameArena() = default;
    ~FrameArena() { shutdown(); }

    void init(size_t capacity = 256 * 1024) {
        buffer_ = (uint8_t*)std::malloc(capacity);
        capacity_ = capacity;
        offset_ = 0;
    }

    void shutdown() {
        if (buffer_) { std::free(buffer_); buffer_ = nullptr; }
        capacity_ = 0;
        offset_ = 0;
    }

    void* alloc(size_t size, size_t align = 8) {
        offset_ = (offset_ + align - 1) & ~(align - 1);
        assert(offset_ + size <= capacity_ && "FrameArena out of memory");
        void* ptr = buffer_ + offset_;
        offset_ += size;
        return ptr;
    }

    template<typename T, typename... Args>
    T* alloc_obj(Args&&... args) {
        void* mem = alloc(sizeof(T), alignof(T));
        return new (mem) T(static_cast<Args&&>(args)...);
    }

    template<typename T>
    T* alloc_array(size_t count) {
        void* mem = alloc(sizeof(T) * count, alignof(T));
        std::memset(mem, 0, sizeof(T) * count);
        return (T*)mem;
    }

    void reset() { offset_ = 0; }
    size_t used() const { return offset_; }
    size_t capacity() const { return capacity_; }

private:
    uint8_t* buffer_ = nullptr;
    size_t capacity_ = 0;
    size_t offset_ = 0;
};


template<typename T, size_t BlockSize = 64>
class PoolAllocator {
    struct Block {
        alignas(T) uint8_t storage[sizeof(T) * BlockSize];
        Block* next;
    };

    union FreeNode {
        FreeNode* next;
        alignas(T) uint8_t storage[sizeof(T)];
    };

public:
    PoolAllocator() = default;
    ~PoolAllocator() { shutdown(); }

    void init() {
        grow();
    }

    void shutdown() {
        Block* b = first_block_;
        while (b) {
            Block* next = b->next;
            std::free(b);
            b = next;
        }
        first_block_ = nullptr;
        free_list_ = nullptr;
        count_ = 0;
    }

    T* alloc() {
        if (!free_list_) grow();
        FreeNode* node = free_list_;
        free_list_ = node->next;
        count_++;
        std::memset(node, 0, sizeof(T));
        return reinterpret_cast<T*>(node);
    }

    void free(T* ptr) {
        FreeNode* node = reinterpret_cast<FreeNode*>(ptr);
        node->next = free_list_;
        free_list_ = node;
        count_--;
    }

    size_t count() const { return count_; }

private:
    void grow() {
        Block* block = (Block*)std::malloc(sizeof(Block));
        block->next = first_block_;
        first_block_ = block;

        uint8_t* mem = block->storage;
        for (size_t i = 0; i < BlockSize; i++) {
            FreeNode* node = reinterpret_cast<FreeNode*>(mem + i * sizeof(T));
            node->next = free_list_;
            free_list_ = node;
        }
    }

    Block* first_block_ = nullptr;
    FreeNode* free_list_ = nullptr;
    size_t count_ = 0;
};


template<typename T, size_t N>
class SmallVec {
public:
    SmallVec() : size_(0), capacity_(N), data_(inline_) {}

    ~SmallVec() {
        if (data_ != inline_) std::free(data_);
    }

    SmallVec(const SmallVec&) = delete;
    SmallVec& operator=(const SmallVec&) = delete;

    void push(const T& val) {
        if (size_ == capacity_) grow();
        data_[size_++] = val;
    }

    void swap_remove(size_t i) {
        assert(i < size_);
        data_[i] = data_[size_ - 1];
        size_--;
    }

    void clear() { size_ = 0; }

    T& operator[](size_t i) { return data_[i]; }
    const T& operator[](size_t i) const { return data_[i]; }
    size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }
    T* begin() { return data_; }
    T* end() { return data_ + size_; }
    const T* begin() const { return data_; }
    const T* end() const { return data_ + size_; }

private:
    void grow() {
        size_t new_cap = capacity_ * 2;
        T* new_data;
        if (data_ == inline_) {
            new_data = (T*)std::malloc(sizeof(T) * new_cap);
            std::memcpy(new_data, inline_, sizeof(T) * size_);
        } else {
            new_data = (T*)std::realloc(data_, sizeof(T) * new_cap);
        }
        data_ = new_data;
        capacity_ = new_cap;
    }

    size_t size_;
    size_t capacity_;
    T* data_;
    T inline_[N];
};

} 
