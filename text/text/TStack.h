#pragma once
#include <iostream>

using namespace std;

template <class T>
class TStack
{
private:
	T* arr;
	int size;
	int pos;

public:
	TStack(int _size = 300);
	TStack(const TStack& other);
	~TStack();

	TStack& operator=(const TStack& other);

	bool operator==(const TStack& other) const;
	bool operator!=(const TStack& other) const;

	bool IsEmpty() const;
	bool IsNotEmpty() const;
	bool IsFull() const;
	bool IsNotFull() const;

	void Push(T item);
	T Pop();

	void Clear();

	T Top() const;

	friend std::ostream& operator<<(ostream& out, const TStack& s)
	{
		for (int i = 0; i <= s.pos; i++)
			out << s.arr[i] << ' ';
		return out;
	}
};

template <class T>
TStack<T>::TStack(int _size)
{
	if (_size <= 0)
		throw _size;

	size = _size;
	arr = new T[size];
	pos = -1;
}

template <class T>
TStack<T>::~TStack()
{
	delete[] arr;
}

template <class T>
TStack<T>::TStack(const TStack& other)
{
	size = other.size;
	pos = other.pos;

	arr = new T[size];

	for (int i = 0; i <= pos; i++)
		arr[i] = other.arr[i];
}

template <class T>
TStack<T>& TStack<T>::operator=(const TStack& other)
{
	if (size != other.size)
	{
		delete[] arr;
		size = other.size;
		arr = new T[size];
	}

	pos = other.pos;

	for (int i = 0; i <= pos; i++)
		arr[i] = other.arr[i];

	return *this;
}

template <class T>
bool TStack<T>::operator==(const TStack& other) const
{
	if (size != other.size)
		return false;

	for (int i = 0; i < size; i++)
		if (arr[i] != other.arr[i])
			return false;

	return true;
}

template <class T>
bool TStack<T>::operator!=(const TStack& other) const
{
	return !operator==(other);
}

template <class T>
bool TStack<T>::IsEmpty() const
{
	return (pos == -1);
}

template <class T>
bool TStack<T>::IsNotEmpty() const
{
	return (pos != -1);
}

template <class T>
bool TStack<T>::IsFull() const
{
	return (pos == (size - 1));
}

template <class T>
bool TStack<T>::IsNotFull() const
{
	return (pos < (size - 1));
}

template <class T>
void TStack<T>::Push(T item)
{
	if (IsFull())
		throw "Can't push";

	pos++;
	arr[pos] = item;
}

template <class T>
T TStack<T>::Pop()
{
	if (IsEmpty())
		throw "Can't pop";

	T item = arr[pos];
	pos--;

	return item;
}

template <class T>
void TStack<T>::Clear()
{
	pos = -1;
}

template <class T>
T TStack<T>::Top() const
{
	if (IsEmpty())
		throw "Can't top";

	return arr[pos];
}