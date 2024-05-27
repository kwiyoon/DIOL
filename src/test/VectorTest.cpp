#include <vector>
#include <iostream>

class VectorTest {
public:
    std::vector<int *> a;

    void f(std::vector<int *> &v) {
        v.push_back(new int(42));
    }

    void erase(std::vector<int *> &v, int *memtable) {
        for (auto it = v.begin(); it != v.end();) {
            auto element = *it;
            if (element == memtable) {
                it = v.erase(it);
            } else it++;
        }
    }

    void test() {

        a.push_back(new int(1));
        a.push_back(new int(2));
        a.push_back(new int(3));

        for (auto i: a)
            std::cout << i <<","<< *i  << std::endl;
        std::cout << "-----\n";
        erase(a, a.front());
        for (auto i: a)
            std::cout << i <<","<< *i << std::endl;
    }
};
