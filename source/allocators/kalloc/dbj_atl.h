
// DBJ optimization of the Active Template Library.
// Copyright (C) dbj@dbj.org
// All rights reserved.


#ifndef __DBJ_ATLSIMPCOLL_H__
#define __DBJ_ATLSIMPCOLL_H__

#pragma once

#include <atldef.h>
#include <atlchecked.h>
#include <wchar.h>

#pragma push_macro("malloc")
#undef malloc
#pragma push_macro("calloc")
#undef calloc
#pragma push_macro("realloc")
#undef realloc
#pragma push_macro("_recalloc")
#undef _recalloc
#pragma push_macro("free")
#undef free

#pragma warning(push)
#pragma warning(disable: 4800) // forcing 'int' value to bool


#pragma pack(push,_ATL_PACKING)
namespace DBJ
{

#pragma push_macro("new")
#undef new

	/////////////////////////////////////////////////////////////////////////////
	// Collection helpers - CSimpleArray & CSimpleMap

	ATLPREFAST_SUPPRESS(6319)
		// template class helpers with functions for comparing elements
		// override if using complex types without operator==
		template <class T>
	class CSimpleArrayEqualHelper
	{
	public:
		static bool IsEqual(
			_In_ const T& t1,
			_In_ const T& t2)
		{
			return (t1 == t2);
		}
	};
	ATLPREFAST_UNSUPPRESS()

		template <class T>
	class CSimpleArrayEqualHelperFalse
	{
	public:
		static bool IsEqual(
			_In_ const T&,
			_In_ const T&)
		{
			ATLASSERT(false);
			return false;
		}
	};

	template <class TKey, class TVal>
	class CSimpleMapEqualHelper
	{
	public:
		static bool IsEqualKey(
			_In_ const TKey& k1,
			_In_ const TKey& k2)
		{
			return CSimpleArrayEqualHelper<TKey>::IsEqual(k1, k2);
		}

		static bool IsEqualValue(
			_In_ const TVal& v1,
			_In_ const TVal& v2)
		{
			return CSimpleArrayEqualHelper<TVal>::IsEqual(v1, v2);
		}
	};

	template <class TKey, class TVal>
	class CSimpleMapEqualHelperFalse
	{
	public:
		static bool IsEqualKey(
			_In_ const TKey& k1,
			_In_ const TKey& k2)
		{
			return CSimpleArrayEqualHelper<TKey>::IsEqual(k1, k2);
		}

		static bool IsEqualValue(
			_In_ const TVal&,
			_In_ const TVal&)
		{
			ATLASSERT(FALSE);
			return false;
		}
	};

#define DBJ_ATL_INITIAL_ARR_SIZE 0xFF
#define DBJ_ATL_ARR_SIZE_STEP 0xFF

	template <class T, class TEqual = CSimpleArrayEqualHelper< T > >
	struct CSimpleArray final
	{
		using type = CSimpleArray;
		typedef T _ArrayElementType;
		T* data_{};
		int array_size_{};
		int allocated_size_{};

		// Construction/destruction
		explicit CSimpleArray() noexcept :
			data_((T*)calloc(DBJ_ATL_INITIAL_ARR_SIZE, sizeof(T))),
			array_size_(0), 
			allocated_size_(DBJ_ATL_INITIAL_ARR_SIZE)
		{
			ATLASSUME(data_ != NULL);
		}


		~CSimpleArray()
		{
			RemoveAll();
		}

		// no copy
		CSimpleArray(_In_ const CSimpleArray& ) = delete;
		CSimpleArray& operator=(_In_ const CSimpleArray& ) = delete;

		// move 
		CSimpleArray(_In_ CSimpleArray && src) noexcept :
			data_(NULL), array_size_( src.array_size_ ), allocated_size_( src.allocated_size_ )
		{
			if (src.allocated_size_ > 0 )
			{
				data_ = src.data_;
				src.data_ = NULL ;
			}
			src.array_size_ = 0;
			src.allocated_size_ = 0;
		}

		CSimpleArray & operator=(_In_ CSimpleArray && src) noexcept 
		{
			if (this == &src) return *this;

			this->RemoveAll();
			array_size_ = src.array_size_;
			allocated_size_ = src.allocated_size_;

			if (src.allocated_size_ > 0)
			{
				data_ = src.data_;
				src.data_ = NULL;
			}
			src.array_size_ = 0;
			src.allocated_size_ = 0;

			return *this;
		}

		// Operations
		int GetSize() const
		{
			return array_size_;
		}

		// DBJ: compute it only once
		constexpr static int DBJ_ATL_ARR_MAX_COUNT = INT_MAX / sizeof(T);


		_Success_(return != FALSE)
			BOOL Add(_In_ const T& t)
		{
			
			if (array_size_ == allocated_size_)
			{
				// Make sure newElement is not a reference to an element in the array.
				// Or else, it will be invalidated by the reallocation.
				ATLENSURE((&t < data_) ||
					(&t >= (data_ + allocated_size_)));

				// DBJ: we set it in ctor to default starting size
				ATLASSUME(allocated_size_ > 0);
				
				int nNewAllocSize = /*(allocated_size_ == 0) ? 1 :*/ (array_size_ + DBJ_ATL_ARR_SIZE_STEP);

				if (
					/* DBJ: this can never happen -> ( nNewAllocSize < 0) || */ 
					(nNewAllocSize > DBJ_ATL_ARR_MAX_COUNT)
					)
				{
					return FALSE;
				}

				void* old_data_ = data_;
				T * aT = (T*)realloc(data_, nNewAllocSize );
				if (aT == NULL) 
				{
// as required here
// https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/realloc?view=vs-2019
					free(old_data_);
					array_size_ = 0 ;
					allocated_size_ = 0 ;
					return FALSE;
				}
				allocated_size_ = nNewAllocSize;
				data_ = aT;
			}
			InternalSetAtIndex(array_size_, t);
			array_size_++;
			return TRUE;
		}

		_Success_(return != FALSE)
			BOOL Remove(_In_ const T& t)
		{
			int nIndex = Find(t);
			if (nIndex == -1)
				return FALSE;
			return RemoveAt(nIndex);
		}

		_Success_(return != FALSE)
			BOOL RemoveAt(_In_ int nIndex)
		{
			ATLASSERT(nIndex >= 0 && nIndex < array_size_);
			if (nIndex < 0 || nIndex >= array_size_)
				return FALSE;
			data_[nIndex].~T();
			if (nIndex != (array_size_ - 1))
				Checked::memmove_s((void*)(data_ + nIndex), (array_size_ - nIndex) * sizeof(T), (void*)(data_ + nIndex + 1), (array_size_ - (nIndex + 1)) * sizeof(T));
			array_size_--;
			return TRUE;
		}
		void RemoveAll()
		{
			if (data_ != NULL)
			{
				for (int i = 0; i < array_size_; i++)
					data_[i].~T();
				free(data_);
				data_ = NULL;
			}
			array_size_ = 0;
			allocated_size_ = 0;
		}
		const T& operator[] (_In_ int nIndex) const
		{
			ATLASSUME(nIndex >= 0 && nIndex < array_size_);
			if (nIndex < 0 || nIndex >= array_size_)
			{
				_AtlRaiseException((DWORD)EXCEPTION_ARRAY_BOUNDS_EXCEEDED);
			}
			return data_[nIndex];
		}
		T& operator[] (_In_ int nIndex)
		{
			ATLASSUME (nIndex >= 0 && nIndex < array_size_);
			if (nIndex < 0 || nIndex >= array_size_)
			{
				_AtlRaiseException((DWORD)EXCEPTION_ARRAY_BOUNDS_EXCEEDED);
			}
			return data_[nIndex];
		}
		T* GetData() const
		{
			return data_;
		}

		int Find(_In_ const T& t) const
		{
			for (int i = 0; i < array_size_; i++)
			{
				if (TEqual::IsEqual(data_[i], t))
					return i;
			}
			return -1;  // not found
		}

		_Success_(return != FALSE)
			BOOL SetAtIndex(
				_In_ int nIndex,
				_In_ const T& t)
		{
			if (nIndex < 0 || nIndex >= array_size_)
				return FALSE;
			InternalSetAtIndex(nIndex, t);
			return TRUE;
		}

		// Implementation
		class Wrapper
		{
		public:
			Wrapper(_In_ const T& _t) : t(_t)
			{
			}
			template <class _Ty>
			void* __cdecl operator new(
				_In_ size_t,
				_In_ _Ty* p)
			{
				return p;
			}
			template <class _Ty>
			void __cdecl operator delete(
				_In_ void* /* pv */,
				_In_ _Ty* /* p */)
			{
			}
			T t;
		};

		// Implementation
		void InternalSetAtIndex(
			_In_ int nIndex,
			_In_ const T& t)
		{
			new(data_ + nIndex) Wrapper(t);
		}


	}; // CSimpleArray

#define CSimpleValArray CSimpleArray


	// intended for small number of simple types or pointers
	template <class TKey, class TVal, class TEqual = CSimpleMapEqualHelper< TKey, TVal > >
	class CSimpleMap
	{
	public:
		TKey* m_aKey;
		TVal* m_aVal;
		int array_size_;

		typedef TKey _ArrayKeyType;
		typedef TVal _ArrayElementType;

		// Construction/destruction
		CSimpleMap() :
			m_aKey(NULL), m_aVal(NULL), array_size_(0)
		{
		}

		~CSimpleMap()
		{
			RemoveAll();
		}

		// Operations
		int GetSize() const
		{
			return array_size_;
		}
		BOOL Add(
			_In_ const TKey& key,
			_In_ const TVal& val)
		{
			TKey* pKey;
			pKey = (TKey*)_recalloc(m_aKey, (array_size_ + 1), sizeof(TKey));
			if (pKey == NULL)
				return FALSE;
			m_aKey = pKey;
			TVal* pVal;
			pVal = (TVal*)_recalloc(m_aVal, (array_size_ + 1), sizeof(TVal));
			if (pVal == NULL)
				return FALSE;
			m_aVal = pVal;
			InternalSetAtIndex(array_size_, key, val);
			array_size_++;
			return TRUE;
		}
		BOOL Remove(_In_ const TKey& key)
		{
			int nIndex = FindKey(key);
			if (nIndex == -1)
				return FALSE;
			return RemoveAt(nIndex);
		}
		BOOL RemoveAt(_In_ int nIndex)
		{
			ATLASSERT(nIndex >= 0 && nIndex < array_size_);
			if (nIndex < 0 || nIndex >= array_size_)
				return FALSE;
			m_aKey[nIndex].~TKey();
			m_aVal[nIndex].~TVal();
			if (nIndex != (array_size_ - 1))
			{
				Checked::memmove_s((void*)(m_aKey + nIndex), (array_size_ - nIndex) * sizeof(TKey), (void*)(m_aKey + nIndex + 1), (array_size_ - (nIndex + 1)) * sizeof(TKey));
				Checked::memmove_s((void*)(m_aVal + nIndex), (array_size_ - nIndex) * sizeof(TVal), (void*)(m_aVal + nIndex + 1), (array_size_ - (nIndex + 1)) * sizeof(TVal));
			}
			TKey* pKey;
			pKey = (TKey*)_recalloc(m_aKey, (array_size_ - 1), sizeof(TKey));
			if (pKey != NULL || array_size_ == 1)
				m_aKey = pKey;
			TVal* pVal;
			pVal = (TVal*)_recalloc(m_aVal, (array_size_ - 1), sizeof(TVal));
			if (pVal != NULL || array_size_ == 1)
				m_aVal = pVal;
			array_size_--;
			return TRUE;
		}
		void RemoveAll()
		{
			if (m_aKey != NULL)
			{
				for (int i = 0; i < array_size_; i++)
				{
					m_aKey[i].~TKey();
					m_aVal[i].~TVal();
				}
				free(m_aKey);
				m_aKey = NULL;
			}
			if (m_aVal != NULL)
			{
				free(m_aVal);
				m_aVal = NULL;
			}

			array_size_ = 0;
		}
		BOOL SetAt(
			_In_ const TKey& key,
			_In_ const TVal& val)
		{
			int nIndex = FindKey(key);
			if (nIndex == -1)
				return FALSE;
			ATLASSERT(nIndex >= 0 && nIndex < array_size_);
			m_aKey[nIndex].~TKey();
			m_aVal[nIndex].~TVal();
			InternalSetAtIndex(nIndex, key, val);
			return TRUE;
		}
		TVal Lookup(_In_ const TKey& key) const
		{
			int nIndex = FindKey(key);
			if (nIndex == -1)
				return NULL;    // must be able to convert
			return GetValueAt(nIndex);
		}
		TKey ReverseLookup(_In_ const TVal& val) const
		{
			int nIndex = FindVal(val);
			if (nIndex == -1)
				return NULL;    // must be able to convert
			return GetKeyAt(nIndex);
		}
		TKey& GetKeyAt(_In_ int nIndex) const
		{
			ATLASSERT(nIndex >= 0 && nIndex < array_size_);
			if (nIndex < 0 || nIndex >= array_size_)
				_AtlRaiseException((DWORD)EXCEPTION_ARRAY_BOUNDS_EXCEEDED);

			return m_aKey[nIndex];
		}
		TVal& GetValueAt(_In_ int nIndex) const
		{
			ATLASSERT(nIndex >= 0 && nIndex < array_size_);
			if (nIndex < 0 || nIndex >= array_size_)
				_AtlRaiseException((DWORD)EXCEPTION_ARRAY_BOUNDS_EXCEEDED);

			return m_aVal[nIndex];
		}

		int FindKey(_In_ const TKey& key) const
		{
			for (int i = 0; i < array_size_; i++)
			{
				if (TEqual::IsEqualKey(m_aKey[i], key))
					return i;
			}
			return -1;  // not found
		}
		int FindVal(_In_ const TVal& val) const
		{
			for (int i = 0; i < array_size_; i++)
			{
				if (TEqual::IsEqualValue(m_aVal[i], val))
					return i;
			}
			return -1;  // not found
		}

		BOOL SetAtIndex(
			_In_ int nIndex,
			_In_ const TKey& key,
			_In_ const TVal& val)
		{
			if (nIndex < 0 || nIndex >= array_size_)
				return FALSE;
			InternalSetAtIndex(nIndex, key, val);
			return TRUE;
		}


		// Implementation

		template <typename T>
		class Wrapper
		{
		public:
			Wrapper(_In_ const T& _t) : t(_t)
			{
			}
			template <class _Ty>
			void* operator new(
				_In_ size_t,
				_In_ _Ty* p)
			{
				return p;
			}
			template <class _Ty>
			void operator delete(
				_In_ void* /* pv */,
				_In_ _Ty* /* p */)
			{
			}
			T t;
		};
		void InternalSetAtIndex(
			_In_ int nIndex,
			_In_ const TKey& key,
			_In_ const TVal& val)
		{
			new(m_aKey + nIndex) Wrapper<TKey>(key);
			new(m_aVal + nIndex) Wrapper<TVal>(val);
		}
	};

#pragma pop_macro("new")

};  // namespace DBJ
#pragma pack(pop)

#pragma warning(pop)

#pragma pop_macro("free")
#pragma pop_macro("realloc")
#pragma pop_macro("_recalloc")
#pragma pop_macro("malloc")
#pragma pop_macro("calloc")


#endif  // __DBJ_ATLSIMPCOLL_H__
