#include "unordered_map.cpp"

int main() {

    UnorderedMap<int, int> test_map;

    test_map.insert({1, 2});
    test_map.insert({4, 5});


    std::cout << test_map[1] << test_map[4];

}