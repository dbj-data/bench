#pragma once

#include <dbj/dbj_common.h>


#define _CRT_SECURE_NO_WARNINGS 1
#include <uthash/uthash.h>

DBJ_NSPACE_BEGIN
/*
* using uthash (C macros API) in this simple way is surprisingly slow?
* string_pointers is much faster
*/
namespace string_hash_table
{

	namespace detail {

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