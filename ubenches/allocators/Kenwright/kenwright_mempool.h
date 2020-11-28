#pragma once
/*

Fast Efficient Fixed-Size Memory Pool, No Loops and No Overhead

Ben Kenwright
School of Computer Science
Newcastle University
Newcastle, United Kingdom,
b.kenwright@ncl.ac.uk

Copyright (c) IARIA, 2012. ISBN: 978-1-61208-222-6

2020-11-25		dbj@dbj.org		adapted for use 
*/

#include <new.h>

namespace kenwright {

	class pool final
	{ // Basic type define
		typedef unsigned int uint;
		typedef unsigned char uchar;

		uint m_numOfBlocks; // Num of blocks
		uint m_sizeOfEachBlock; // Size of each block
		uint m_numFreeBlocks; // Num of remaining blocks
		uint m_numInitialized; // Num of initialized blocks
		uchar* m_memStart; // Beginning of memory pool
		uchar* m_next; // Num of next free block
	
	public:
		
		pool() = delete;
		//{
		//	m_numOfBlocks = 0;
		//	m_sizeOfEachBlock = 0;
		//	m_numFreeBlocks = 0;
		//	m_numInitialized = 0;
		//	m_memStart = NULL;
		//	m_next = 0;
		//}
		
		~pool() { DestroyPool(); }

		pool(size_t sizeOfEachBlock,
			uint numOfBlocks)
		{
			m_numOfBlocks = numOfBlocks;
			m_sizeOfEachBlock = sizeOfEachBlock;
			m_memStart = new uchar[m_sizeOfEachBlock *
				m_numOfBlocks];
			m_numFreeBlocks = numOfBlocks;
			m_next = m_memStart;
		}
	
		void DestroyPool()
		{
			delete[] m_memStart;
			m_memStart = NULL;
		}
		
		uchar* AddrFromIndex(uint i) const
		{
			return m_memStart + (i * m_sizeOfEachBlock);
		}
		
		uint IndexFromAddr(const uchar* p) const
		{
			return (((uint)(p - m_memStart)) / m_sizeOfEachBlock);
		}
		
		// allocate one
		void* Allocate()
		{
			if (m_numInitialized < m_numOfBlocks)
			{
				uint* p = (uint*)AddrFromIndex(m_numInitialized);
				*p = m_numInitialized + 1;
				m_numInitialized++;
			}
			void* ret = NULL;
			if (m_numFreeBlocks > 0)
			{
				ret = (void*)m_next;
				--m_numFreeBlocks;
				if (m_numFreeBlocks != 0)
				{
					m_next = AddrFromIndex(*((uint*)m_next));
				}
				else
				{
					m_next = NULL;
				}
			}
			return ret;
		}
		
		void DeAllocate(void* p)
		{
			if (m_next != NULL)
			{
				(*(uint*)p) = IndexFromAddr(m_next);
				m_next = (uchar*)p;
			}
			else
			{
				*((uint*)p) = m_numOfBlocks;
				m_next = (uchar*)p;
			}
			++m_numFreeBlocks;
		}

	}; // pool

} // namespace kenwright 
