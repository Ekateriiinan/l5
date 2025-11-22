#pragma once
#include <memory_resource>

template<typename T>
class pmr_stack {
    struct Node {
        T value;
        Node* next;
        template<typename... Args>
        Node(Args&&... args) : value(std::forward<Args>(args)...), next(nullptr) {}
    };
    
    Node* top_ = nullptr;
    std::pmr::polymorphic_allocator<Node> alloc_;
    size_t count_ = 0;

public:
    class iterator {
        Node* current_;
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;
        
        iterator(Node* node = nullptr) : current_(node) {}
        reference operator*() const { return current_->value; }
        pointer operator->() const { return &current_->value; }
        iterator& operator++() { current_ = current_->next; return *this; }
        iterator operator++(int) { iterator tmp = *this; ++(*this); return tmp; }
        bool operator==(const iterator& other) const { return current_ == other.current_; }
        bool operator!=(const iterator& other) const { return current_ != other.current_; }
    };
    
    pmr_stack(std::pmr::memory_resource* mr = std::pmr::get_default_resource()) : alloc_(mr) {}
    
    ~pmr_stack() { while (!empty()) pop(); }
    
    pmr_stack(const pmr_stack&) = delete;
    pmr_stack& operator=(const pmr_stack&) = delete;
    
    pmr_stack(pmr_stack&& other) noexcept : top_(other.top_), alloc_(other.alloc_), count_(other.count_) {
        other.top_ = nullptr;
        other.count_ = 0;
    }
    
    pmr_stack& operator=(pmr_stack&& other) noexcept {
        if (this != &other) {
            while (!empty()) pop();
            top_ = other.top_;
            alloc_ = other.alloc_;
            count_ = other.count_;
            other.top_ = nullptr;
            other.count_ = 0;
        }
        return *this;
    }
    
    void push(const T& val) {
        Node* new_node = alloc_.allocate(1);
        try {
            alloc_.construct(new_node, val);
        } catch (...) {
            alloc_.deallocate(new_node, 1);
            throw;
        }
        new_node->next = top_;
        top_ = new_node;
        ++count_;
    }
    
    void push(T&& val) {
        Node* new_node = alloc_.allocate(1);
        try {
            alloc_.construct(new_node, std::move(val));
        } catch (...) {
            alloc_.deallocate(new_node, 1);
            throw;
        }
        new_node->next = top_;
        top_ = new_node;
        ++count_;
    }
    
    template<typename... Args>
    void emplace(Args&&... args) {
        Node* new_node = alloc_.allocate(1);
        try {
            alloc_.construct(new_node, std::forward<Args>(args)...);
        } catch (...) {
            alloc_.deallocate(new_node, 1);
            throw;
        }
        new_node->next = top_;
        top_ = new_node;
        ++count_;
    }
    
    void pop() {
        if (!top_) return;
        Node* old_top = top_;
        top_ = top_->next;
        alloc_.destroy(old_top);
        alloc_.deallocate(old_top, 1);
        --count_;
    }
    
    T& top() { return top_->value; }
    const T& top() const { return top_->value; }
    bool empty() const { return top_ == nullptr; }
    size_t size() const { return count_; }
    
    iterator begin() { return iterator(top_); }
    iterator end() { return iterator(nullptr); }
};