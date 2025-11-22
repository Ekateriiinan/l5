#include "fixed_block_memory_resource.hpp"
#include <stdexcept>

fixed_block_memory_resource::fixed_block_memory_resource(void* buffer, size_t size)
    : block_(buffer), size_(size), used_(0) {}

fixed_block_memory_resource::~fixed_block_memory_resource() {
    allocations_.clear();
}

void* fixed_block_memory_resource::do_allocate(size_t bytes, size_t alignment) {
    if (used_ + bytes > size_) {
        throw std::bad_alloc();
    }
    
    void* ptr = static_cast<char*>(block_) + used_;
    used_ += bytes;
    allocations_.push_back(ptr);
    
    return ptr;
}

void fixed_block_memory_resource::do_deallocate(void* p, size_t bytes, size_t alignment) {
    auto it = std::find(allocations_.begin(), allocations_.end(), p);
    if (it != allocations_.end()) {
        allocations_.erase(it);
    }
}

bool fixed_block_memory_resource::do_is_equal(const std::pmr::memory_resource& other) const noexcept {
    return this == &other;
}