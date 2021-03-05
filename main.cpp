#include "buffer.h"

#include <iostream>
#include <memory>

class A
{
public:
    virtual void print() const { std::cout << "A\n"; }
};

class B : public A
{
public:
    B() {}
    B(int m, float d) : m(m), d(d) {}

    virtual void print() const { std::cout << "B " << m << " / " << d << "\n"; }

public:
    int m = 100;
    float d = 69.0f;
};

class C : public A
{
public:
    C() {}
    C(int m, float d) : m(m), d(d) {}

    virtual void print() const { std::cout << "C " << d << " / " << m << "\n"; }

public:
    int m = 100;
    float d = 69.0f;
};

void print(A& a)
{
    a.print();
} // call of base class

using namespace std;

int main()
{
    assert(sizeof(B) == sizeof(C));

    Buffer<A> buffer(sizeof(B));
    
    // populate derived class
    buffer.push_back<B>(B{1,2});
    buffer.push_back<B>(B{3,4});

    // populate another derived class of the same size
    for (int i = 0; i < 10; ++i)
        buffer.push_back<C>(C{5,6});
    
    cout << "Buffer stride:" << buffer.stride() << "\n";
    cout << "Buffer size:" << buffer.size() << "\n";
    
    cout << "\nIterate as base class(operator[]):\n";
    for (int i = 0; i < buffer.size(); ++i)
    {
        cout << "iter..." << i << "\n";
        buffer.at(i).print();
        print(buffer.at(i));
    }

    cout << "\nIterate as base class(iterator):\n";
    for (Buffer<A>::iterator iter = buffer.begin(); iter != buffer.end(); ++iter)
    {
        cout << "iter..." << distance<A>(buffer.begin(), iter) << "\n";
        iter->print();
        print(*iter);
    }

    cout << "\nIterate as base class(foreach):\n";
    int cnt{};
    for (A& a : buffer)
    {
        cout << "iter..." << cnt++ << "\n";
        a.print();
        print(a);
    }

    return 0;
}
