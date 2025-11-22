#include <iostream>
#include "fixed_block_memory_resource.hpp"
#include "pmr_stack.hpp"

int main() {
    char buffer[2048];
    fixed_block_memory_resource res(buffer, 2048);
    
    pmr_stack<int> int_stack(&res);
    for (int i = 0; i < 5; ++i) {
        int_stack.push(i * 10);
    }
    
    std::cout << "Stack: ";
    for (auto it = int_stack.begin(); it != int_stack.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << "\n";
    
    pmr_stack<std::string> str_stack(&res);
    str_stack.emplace("hello");
    str_stack.emplace("world");
    
    while (!str_stack.empty()) {
        std::cout << str_stack.top() << " ";
        str_stack.pop();
    }
    std::cout << "\n";
    
    return 0;
}