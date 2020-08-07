#ifndef _BUFFER_H
#define _BUFFER_H

#include <array>

template<typename T, size_t size>
class Buffer
{
private:
	std::array<T, size> arr;
	size_t index = 0;
	bool full;
public:
	void push(T d)
	{
		arr[index++] = d;
		if (index == size)
		{
			index = 0;
			full = true;
		}
	}

	void flush()
	{
		index = 0;
		full = false;
	}

	const T& operator[](size_t i) const { return arr[i]; }
	T& operator[](size_t i) { return arr[i]; }

	const T& get(size_t i) const { return arr[i]; }
	T& get(size_t i) { return arr[i]; }

	size_t getSize() { return size; }

	bool isFull() { return full; }
};

#endif