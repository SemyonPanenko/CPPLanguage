#include "deque.cpp"

int main() {

    Deque<int> dq;

    dq.push_back(3);
    dq.push_back(4);
    dq.push_front(7);

    for(auto it: dq){
        std::cout << it << " ";
    }

}