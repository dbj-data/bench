#pragma once

#include <dbj/dbj_common.h>


#define _CRT_SECURE_NO_WARNINGS 1
// #define HASH_BLOOM 32
#include <uthash/uthash.h>

DBJ_NSPACE_BEGIN
/*
* using uthash (C macros API) in this simple way is surprisingly slow?
* string_pointers is much faster
*/
namespace string_hash_table
{

	namespace detail {

		//#pragma region 	Paul Hsieh Fast Hash
		//
		//	// Jenkins is here: http://www.burtleburtle.net/bob/hash/spooky.html
		//
		//	// https://gist.github.com/CedricGuillemet/4978020
		//
		//#undef get16bits
		//#define get16bits(d) ((((uint32_t)(((const uint8_t *)(d))[1])) << 8)\
		//+(uint32_t)(((const uint8_t *)(d))[0]) )
		////
		//// super hash function by Paul Hsieh
		////
		//	inline uint32_t paul_hsieh_hash(const char* data, int len) {
		//		uint32_t hash = len, tmp;
		//		int rem;
		//
		//		if (len <= 0 || data == NULL) return 0;
		//
		//		rem = len & 3;
		//		len >>= 2;
		//
		//		/* Main loop */
		//		for (; len > 0; len--) {
		//			hash += get16bits(data);
		//			tmp = (get16bits(data + 2) << 11) ^ hash;
		//			hash = (hash << 16) ^ tmp;
		//			data += 2 * sizeof(uint16_t);
		//			hash += hash >> 11;
		//		}
		//
		//		/* Handle end cases */
		//		switch (rem) {
		//		case 3: hash += get16bits(data);
		//			hash ^= hash << 16;
		//			hash ^= data[sizeof(uint16_t)] << 18;
		//			hash += hash >> 11;
		//			break;
		//		case 2: hash += get16bits(data);
		//			hash ^= hash << 11;
		//			hash += hash >> 17;
		//			break;
		//		case 1: hash += *data;
		//			hash ^= hash << 10;
		//			hash += hash >> 1;
		//		}
		//
		//		/* Force "avalanching" of final 127 bits */
		//		hash ^= hash << 3;
		//		hash += hash >> 5;
		//		hash ^= hash << 4;
		//		hash += hash >> 17;
		//		hash ^= hash << 25;
		//		hash += hash >> 6;
		//
		//		return hash;
		//	} // paul_hsieh_hash
		//
		//#undef get16bits
		//#pragma endregion 	

		constexpr auto	string_ht_node_data_max_len = 0xFF;

		struct string_ht_node_ final {
			int uid;         /* 'uid' is the key */
			char data[string_ht_node_data_max_len];
			UT_hash_handle hh; /* makes this structure hashable      */
		};

#define string_ht_node_clean( node_ ) \
 ( node_->data[0] = '\0', node_->uid = 0, node_ )

#define string_ht_node_assign( UID_, STR_, node_ )  \
( node_->uid = UID_ ,  (void)strncpy( node_->data, STR_, string_ht_node_data_max_len - 1 ), node_ )

		/* return the found string_ht_node_ or null if not found */
		inline struct string_ht_node_* string_ht_node_find(string_ht_node_* nodes_, int needle_uid_)
		{
			assert(nodes_);
			struct string_ht_node_* htn_ = NULL;

			// try to find the node 
			HASH_FIND_INT(nodes_, &needle_uid_, htn_);
			return htn_;
		}

		/* return the new string_ht_node_ made or old if found */
		inline struct  string_ht_node_*
			string_ht_node_add(struct string_ht_node_* nodes_, const char* next_str_)
		{
			assert(next_str_);
			struct string_ht_node_* htn_ = NULL;

			// try to find the node first
			int hash_ = 0; //  = (int)paul_hsieh_hash(next_str_, strlen(next_str_));

			/* The Paul Hsieh hash function */
			HASH_SFH(next_str_, strlen(next_str_), hash_);
			hash_ = abs(hash_);
			assert(hash_ > 0);

			// Jenkins Hash Function
			// #define HASH_JEN(key,keylen,hashv) 

			HASH_FIND_INT(nodes_, &hash_, htn_);
			if (htn_) return htn_; // found, do not add

			htn_ = (string_ht_node_*)calloc(1, sizeof(string_ht_node_));
			assert(htn_);

			if (htn_) {
				// second arg is struct member name
				// not variable name 
				HASH_ADD_INT(nodes_, uid, string_ht_node_assign(hash_, next_str_, htn_) );
			}
			return nodes_;
		}

		/* remove one node */
		inline string_ht_node_* string_ht_node_rmv(string_ht_node_* nodes_, int uid_)
		{
			assert(nodes_ && (uid_ > 0));

			string_ht_node_* tmp_ = NULL;
			// try to find the node 
			HASH_FIND_INT(nodes_, &uid_, tmp_);

			if (tmp_) {
				HASH_DEL(nodes_, tmp_);
				free(tmp_);
			}
			return nodes_;
		}

		/* remove all nodes and free the entire HT */
		inline string_ht_node_* string_ht_nodes_destruct(string_ht_node_* nodes_)
		{
			HASH_CLEAR(hh, nodes_);
			return nodes_;
		}

	} // nspace

	using type = struct detail::string_ht_node_;

	// struct string_ht_node_* strings_{};

	inline type* clear(type* strings_) noexcept {
		return (strings_ = detail::string_ht_nodes_destruct(strings_));
	}

	// assign if not found
	// return the hash uid of the string stored
	DBJ_PURE_FUNCTION
		inline type* assign(type* htable_, const char* text) noexcept
	{
		assert(text);
		htable_ = detail::string_ht_node_add(htable_, text);
		assert(htable_);
		return htable_;
	}

	// remove if found and return true
	// otherwise return false
	DBJ_PURE_FUNCTION
		inline type* remove(type* htable_, size_t needle_uid_) noexcept
	{
		return string_ht_node_rmv(htable_, (int)needle_uid_);
	}
}; // string_hash_table

DBJ_NSPACE_END