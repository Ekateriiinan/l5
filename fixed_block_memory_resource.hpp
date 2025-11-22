#pragma once
#include <memory_resource>
#include <vector>

class fixed_block_memory_resource : public std::pmr::memory_resource {
    void* block_;
    size_t size_;
    size_t used_;
    std::vector<void*> allocations_;

    void* do_allocate(size_t bytes, size_t alignment) override;
    void do_deallocate(void* p, size_t bytes, size_t alignment) override;
    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override;

public:
    fixed_block_memory_resource(void* buffer, size_t size);
    ~fixed_block_memory_resource();
    
    fixed_block_memory_resource(const fixed_block_memory_resource&) = delete;
    fixed_block_memory_resource& operator=(const fixed_block_memory_resource&) = delete;
};