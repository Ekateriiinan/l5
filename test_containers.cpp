#include <gtest/gtest.h>
#include "fixed_block_memory_resource.hpp"
#include "pmr_stack.hpp"

struct TestStruct {
    int x;
    double y;
    TestStruct(int a, double b) : x(a), y(b) {}
    bool operator==(const TestStruct& other) const { return x == other.x && y == other.y; }
};

class MemoryTest : public ::testing::Test {
protected:
    char buffer[1024];
    fixed_block_memory_resource* res;
    
    void SetUp() override { res = new fixed_block_memory_resource(buffer, 1024); }
    void TearDown() override { delete res; }
};

TEST_F(MemoryTest, AllocDealloc) {
    void* p1 = res->allocate(100, 1);
    void* p2 = res->allocate(200, 1);
    EXPECT_NE(p1, nullptr);
    EXPECT_NE(p2, nullptr);
    res->deallocate(p1, 100, 1);
    res->deallocate(p2, 200, 1);
}

TEST_F(MemoryTest, OutOfMemory) {
    void* p1 = res->allocate(900, 1);
    EXPECT_THROW(res->allocate(200, 1), std::bad_alloc);
    res->deallocate(p1, 900, 1);
}

TEST_F(MemoryTest, StackInt) {
    pmr_stack<int> stack(res);
    stack.push(1);
    stack.push(2);
    EXPECT_EQ(stack.top(), 2);
    stack.pop();
    EXPECT_EQ(stack.top(), 1);
    EXPECT_EQ(stack.size(), 1);
}

TEST_F(MemoryTest, StackComplex) {
    pmr_stack<TestStruct> stack(res);
    stack.emplace(1, 2.5);
    stack.emplace(3, 4.5);
    EXPECT_EQ(stack.top(), TestStruct(3, 4.5));
    stack.pop();
    EXPECT_EQ(stack.top(), TestStruct(1, 2.5));
}

TEST_F(MemoryTest, Iterator) {
    pmr_stack<int> stack(res);
    stack.push(1);
    stack.push(2);
    stack.push(3);
    
    auto it = stack.begin();
    EXPECT_EQ(*it, 3);
    ++it;
    EXPECT_EQ(*it, 2);
    ++it;
    EXPECT_EQ(*it, 1);
    ++it;
    EXPECT_EQ(it, stack.end());
}

TEST_F(MemoryTest, MoveStack) {
    pmr_stack<int> stack1(res);
    stack1.push(10);
    stack1.push(20);
    
    pmr_stack<int> stack2 = std::move(stack1);
    EXPECT_TRUE(stack1.empty());
    EXPECT_EQ(stack2.top(), 20);
    EXPECT_EQ(stack2.size(), 2);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}