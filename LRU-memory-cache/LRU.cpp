#include "LRU.h"

#include <algorithm>

LRU::LRU()
{}

LRU::~LRU()
{}

bool LRU::get(const std::string& key, std::string& result)
{
	const std::string* value_ptr = get_by_pointer(key);

    if(nullptr == value_ptr)
    {
        // Key not found in cache.
        return false;
    }

	// Copy the result value into the given string reference.
	result = *value_ptr;
    
    return true;
}

const std::string* LRU::get_by_pointer(const std::string& key)
{
    lru_map_type::iterator map_elem_iterator = lookup_map_.find(key);
    if(lookup_map_.cend() == map_elem_iterator)
    {
        // Key not found in cache.
		return nullptr;
    }

	try {
		// Set node as most recently visited. Might throw as there is memory allocation involved.
		visit_listnode(map_elem_iterator->second);
	}
	catch (const std::bad_alloc&)
	{
		// In case the program runs out of memory, do a cleanup and return nullptr to indicate value is not there.
		cleanup();

		return nullptr;
	}

	// Update the lookup map to point to the newest node.
	map_elem_iterator->second = most_recent_listnode();
    
    // Return a pointer to the value of the newest node.
	return &most_recent_listnode()->second;
}

void LRU::put(const std::string& key, const std::string& value)
{
	// Check if the key is seen before. If so, put it in the front of the recently used list.
	lru_map_type::iterator map_elem_iterator = lookup_map_.find(key);
	if (lookup_map_.end() == map_elem_iterator)
	{
		// Element has not been seen before, add element as most recently used and update the lookup map.
		// Add to the recently used list first.
		try {
			recently_used_list_.emplace_front(key, value);
		}
		catch (const std::bad_alloc&)
		{
			// In case the program runs out of memory, do a cleanup and do not put the new value in (for now).
			cleanup();
			return;
		}

		// Also add element and list iterator to the lookup map.
		try {
			lookup_map_.emplace(key, recently_used_list_.begin());
		}
		catch (const std::bad_alloc&)
		{
			// In case the program runs out of memory here, do a cleanup, make sure to also delete the element from the
			// recently used list (as the element has been inserted there as well).
			recently_used_list_.erase(recently_used_list_.begin());
			cleanup();
			return;
		}
	}
	else
	{
		// Element has been seen before. Move corresponding list node as most recently used.
		lru_list_type::iterator list_node_iterator = map_elem_iterator->second;

		try {
			visit_listnode_new_value(list_node_iterator, value);
		}
		catch (const std::bad_alloc&)
		{
			// In case the program runs out of memory, do a cleanup and return.
			// The new value is not in the cache yet.
			cleanup();
			return;
		}
		
		// Within the lookup map, update the list node iterator for the given key.
		map_elem_iterator->second = recently_used_list_.begin();
	}
}

void LRU::visit_listnode(lru_list_type::iterator& node_iterator)
{
	const std::string& key_str = node_iterator->first;
	const std::string& value_str = node_iterator->second;

	// Create a new node at the front of the list, moving the key-value pair into it.
	recently_used_list_.emplace_front(
		lru_node_type(
			std::move(key_str),
			std::move(value_str)
		)
	);

	// Delete the old node.
	recently_used_list_.erase(node_iterator);
}

void LRU::visit_listnode_new_value(lru_list_type::iterator& node_iterator, const std::string& value)
{
	const std::string& key_str = node_iterator->first;

	// Create a new node at the front of the list, moving/copying the existing key and the new value into it.
	recently_used_list_.emplace_front(
		lru_node_type(
			std::move(key_str),
			std::move(value)
		)
	);

	// Delete the old node.
	recently_used_list_.erase(node_iterator);
}

void LRU::delete_last()
{
    const lru_node_type& last_node = recently_used_list_.back();

    const std::string& key = last_node.first;

    lookup_map_.erase(key);
    recently_used_list_.pop_back();
}

void LRU::cleanup()
{
    if(empty())
    {
        return;
    }

    // Remove 10% of the elements, at least 1.
    const size_t amount_to_remove = std::max(static_cast<size_t>(size() * 0.1), 1u);

    for(size_t i = 0; i < amount_to_remove; i++)
    {
        delete_last();
    }
}
