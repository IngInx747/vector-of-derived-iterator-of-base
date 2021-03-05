# Vector of Derived, Iterator of Base

A buffer type that stores derived class type while traverses as base class type. From memory sense the iterator of the buffer jumps sizeof(derived class) instead of sizeof(base class).

Usually the task can be done with a vector of pointers. But one may concern when storing data in continuous memory block is a must and implementing memory allocation algorithm is of great cost.

## Usage

```
class Base
{
public:
    virtual void foo() { /* Not implemented */ }
}

class Derived : public Base
{
public:
    virtual void foo() { /* Method using Attribute */ }
protected:
    Attribute attrib;
}

int main()
{
    Buffer<Base> buffer(sizeof(Derived));

    buffer.push_back<Derived>(Derived{...});

    for (Base& base : buffer)
        base.foo(); // call Derived::foo()

    for (auto iter = buffer.begin(); iter != buffer.end(); ++iter)
        iter->foo(); // call Derived::foo()
}
```

## Case

Consider multiple derived classes that have the same function interface but different member attributes i.e. are of different sizes.

```
class Base {...};
class Derived_A : public Base {...};
class Derived_B : public Base {...};
...
class Derived_Z : public Base {...};
```

To store different types, a common way is to store base type pointer to the derived structure.

```
class Node : public BaseNode
{
    vector<Base*> buffer;
};
```

In some cases, one of these derived structure needs to be store in continuous blocks of memory. To hold an array of a certain derived type, one can use template like this:

```
template <class T_Derived>
class Node : public BaseNode
{
    vector<T_Derived> buffer;
};
```

One con of using template is type cannot be told during runtime, so as to perform type casting. In this example, one can never cast ```BaseNode*``` to ```Node<T>*``` as computer cannot tell the type T.

With stride-iterating vector one can get rid of template to create uniform type. Class definition is like:

```
class Node : public BaseNode
{
    Buffer<Base> buffer;
};
```