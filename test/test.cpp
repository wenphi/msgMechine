#include <iostream>

class test_base
{
  public:
    test_base(int num_ = 10)
    {
        num = num_;
    }
    virtual ~test_base() {}
    void printNum()
    {
        std::cout << "test_base:num:" << num << std::endl;
    }

  public:
    //   private:
    int num;
};

class test0 : public test_base
{
  public:
    test0(int num_) : test_base(num_)
    {
        num = 5;
    }
    void printNum1()
    {
        std::cout << "test_base:num:" << num << std::endl;
    }

  private:
    // int num;
};

int main()
{
    // test_base *tt;
    test0 *tt = new test0(15);
    tt->printNum();
}