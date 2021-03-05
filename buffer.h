#pragma once
#ifndef BUFFER_H
#define BUFFER_H

#include <iterator> // std::forward_iterator_tag
#include <cstddef>  // std::ptrdiff_t
#include <cassert>
#include <cstring>

////////////////////////////////////////////////////////////////
/// Buffer
////////////////////////////////////////////////////////////////

template <class T_Base>
class Buffer
{
protected:
	typedef unsigned char Byte;
	typedef T_Base Base;
	
public:
	struct out_of_range {};
	class iterator;
	
public:
    Buffer(size_t stride): mStride(stride)
	{}

    Buffer(size_t stride, size_t size): mStride(stride), mSize(size), mCapacity(size)
	{
		mData = new Byte[stride * size];
	}

    ~Buffer()
	{
		delete[] mData;
		mData = NULL;
	}

	inline size_t capacity() const { return mCapacity; }

	inline size_t size() const { return mSize; }

	inline size_t stride() const { return mStride; }

	inline bool empty() const { return mSize == 0; }

	// Requests a change in capacity
	// reserve() will never decrase the capacity.
    void reserve(size_t newcapacity)
	{
		if (newcapacity <= mCapacity) return;

		Byte* p = new Byte[mStride * newcapacity];

		size_t n = mStride * mSize;

		for (size_t i = 0; i < n; ++i)
			p[i] = mData[i];

		delete[] mData;

		mData = p;
		mCapacity = newcapacity;
	}

	// Changes the Buffer's size.
	// If the newsize is smaller, the last elements will be lost.
    void resize(size_t newsize)
	{
		reserve(newsize);

		size_t m = mStride * mSize;
		size_t n = mStride * newsize;

		for (size_t i = m; i < n; ++i)
			mData[i] = Byte{};

		mSize = newsize;
	}

	// Reduces capcity to fit the size
	void shrink_to_fit()
	{
		// TODO
	}

	template<class T_Derived>
    void push_back(const T_Derived& elem)
	{
		assert(sizeof(T_Derived) == mStride);

		if (mCapacity == 0)
			reserve(8);
		else if (mSize == mCapacity)
			reserve(2 * mCapacity);

		// Virtual function table along with data needs to be copied
		// Some method like operator= or std::copy won't do the trick
		memcpy(&mData[mStride * mSize], &elem, sizeof(elem));
		
		++mSize;
	}

    void pop_back()
	{
		--mSize;
	}

    template<class T_Derived>
	T_Derived& at(int n)
	{
		//assert(sizeof(T_Derived) == mStride);

		if (n < 0 || mSize <= n) throw out_of_range{};

		T_Derived* p = reinterpret_cast<T_Derived*>(&mData[mStride * n]);
		return *p;
	}

    template<class T_Derived>
	const T_Derived& at(int n) const
	{
		//assert(sizeof(T_Derived) == mStride);

		if (n < 0 || mSize <= n) throw out_of_range{};
		
		T_Derived* p = reinterpret_cast<T_Derived*>(&mData[mStride * n]);
		return *p;
	}

	T_Base& at(int n)
	{
		if (n < 0 || mSize <= n) throw out_of_range{};

		T_Base* p = reinterpret_cast<T_Base*>(&mData[mStride * n]);
		return *p;
	}

	const T_Base& at(int n) const
	{
		if (n < 0 || mSize <= n) throw out_of_range{};

		T_Base* p = reinterpret_cast<T_Base*>(&mData[mStride * n]);
		return *p;
	}

	// Removes all elements from the Vector
	// Capacity is not changed.
    void clear()
	{
    	mSize = 0;
	}
	
    inline iterator begin()
	{
		return iterator(&mData[0], mStride);
	}
    
	inline const iterator begin() const
	{
		return iterator(&mData[0], mStride);
	}

    inline iterator end()
	{
		return iterator(&mData[mSize * mStride], mStride);
	}

	inline iterator end() const
	{
		return iterator(&mData[mSize * mStride], mStride);
	}
	
protected:
	// meta data
	Byte* mData = NULL;

	// buffer current size
    size_t mSize = 0;

	// buffer capacity
    size_t mCapacity = 0;

	// size of struct(usually a derived class)
	size_t mStride;
};

template<class T_Base>
class Buffer<T_Base>::iterator
{
public:
	iterator()
	{}

	iterator(Byte* p, size_t stride): mCurr(p), mStride(stride)
	{}

	inline iterator& operator++()
	{
		mCurr += mStride;
		return *this;
	}

	inline iterator& operator++(int)
	{
		mCurr += mStride;
		return *this;
	}

	inline iterator& operator--()
	{
		mCurr -= mStride;
		return *this;
	}

	inline iterator& operator--(int)
	{
		mCurr -= mStride;
		return *this;
	}

	inline iterator& operator+=(int n)
	{
		mCurr += mStride * n;
		return *this;
	}

	inline iterator& operator-=(int n)
	{
		mCurr -= mStride * n;
		return *this;
	}

	inline T_Base& operator*()
	{
		return *reinterpret_cast<T_Base*>(mCurr);
	}

	inline T_Base* operator->()
	{
		return reinterpret_cast<T_Base*>(mCurr);
	}

	inline explicit operator T_Base* ()
	{
		return reinterpret_cast<T_Base*>(mCurr);
	}
	
	inline bool operator==(const iterator& b) const
	{
		return mCurr == b.mCurr;
	}

	inline bool operator!=(const iterator& b) const
	{
		return mCurr != b.mCurr;
	}

protected:
	// meta data
	Byte* mCurr = NULL;

	// size of struct(usually a derived class)
	size_t mStride;

public:
	template<class T_Base>
	friend inline std::ptrdiff_t distance(typename Buffer<T_Base>::iterator first, typename Buffer<T_Base>::iterator last);

	template<class T_Base>
	friend inline void advance(typename Buffer<T_Base>::iterator& iter, size_t n);
};

template<class T_Base>
inline std::ptrdiff_t distance(typename Buffer<T_Base>::iterator first, typename Buffer<T_Base>::iterator last)
{
	return (last.mCurr - first.mCurr) / static_cast<std::ptrdiff_t>(first.mStride);
}

template<class T_Base>
inline void advance(typename Buffer<T_Base>::iterator& iter, size_t n)
{
	iter += n;
}

#endif
