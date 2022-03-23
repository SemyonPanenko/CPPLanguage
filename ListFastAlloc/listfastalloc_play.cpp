#include "listfastalloc.cpp"
#include <iostream>

int main() {

    List<int> lst;

    for (size_t i = 0; i < 10; i++)
    {
        lst.push_back(i + i);
    }

    int current_back = *lst.rbegin();
    int current_front = *lst.begin();

    std::cout << current_back << " " << current_front << "\n";

    for (size_t i = 0; i < 5; i++)
    {
        lst.pop_back();
    }

    for (size_t i = 0; i < 3; i++)
    {
        lst.pop_front();
    }

    current_back = *lst.rbegin();
    current_front = *lst.begin();

    std::cout << current_back << " " << current_front << "\n";

}