#include <map>
#include <iostream>

class Abstract {
public:
    Abstract(std::map<int, int> ids) : ids_(ids)
    {
        std::cout << " in Abstract contructor" << std::endl;
        for (auto id: ids_)
            std::cout << "id: " << id.first << " is: " << id.second << std::endl;
    }
    virtual void do_sum(int a) = 0;
private:
   std::map<int, int> ids_;

};

class Test :public Abstract{
    std::map<int, int> ids_;
public:
    Test() : ids_{{1, 2}}, Abstract(ids_)
    {
        std::cout << " in test contructor" << std::endl;
    }
    void do_sum(int a) { }
private:
};

int main()
{
    std::map<int, int> ids_ {{1,2} , {3, 4}};

    Test tes;
}

