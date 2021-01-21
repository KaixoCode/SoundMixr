#pragma once
#include "pch.hpp"
// : public moodycamel::ConcurrentQueue<double>

class Channel 
{
public:

	void Push(double* v, int size)
	{
		for (int i = 0; i < size; i++)
		{
			write = (write + 1) % SIZE;
			arr[write] = v[i];
		}
	}

	void StartRead(int size)
	{
		read = (write + SIZE - size) % SIZE;
	}

	double Pop()
	{
		double v = arr[read];
		read = (read + 1) % SIZE;
		return v;
	}

private:
	static const int SIZE = 8192 * 4;
	double arr[SIZE];
	int write = 0, read = 0;

	std::queue<double> m_Queue;
	int m_Size = 0, m_MaxSize = 256 * 2;


	int id;

};