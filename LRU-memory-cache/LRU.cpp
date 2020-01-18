#include "LRU.h"

#include <algorithm>

LRU::LRU()
{}

LRU::~LRU()
{}

bool LRU::get(const std::string& key, std::string& result)
{
    lru_map_type::iterator map_elem_iterator = lookup_map_.find(key);
    if(lookup_map_.cend() == map_elem_iterator)
    {
        // Key not found in cache.
        return false;
    }

	// Set node as most recently visited.
	visit_listnode(map_elem_iterator->second);

	// Update the lookup map to point to the newest node.
	map_elem_iterator->second = most_recent_listnode();

	// Set the result value to the value of the newest node.
	result = most_recent_listnode()->second;
    
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

	// Set node as most recently visited.
	visit_listnode(map_elem_iterator->second);

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
		recently_used_list_.emplace_front(key, value);
		lookup_map_.emplace(key, recently_used_list_.begin());
	}
	else
	{
		// Element has been seen before. Move corresponding list node as most recently used.
		lru_list_type::iterator list_node_iterator = map_elem_iterator->second;
		visit_listnode_new_value(list_node_iterator, value);

		// Within the lookup map, update the list node iterator for the given key.
		map_elem_iterator->second = recently_used_list_.begin();
	}
}

void LRU::visit_listnode(lru_list_type::iterator& node_iterator)
{
    // Move the data out of the nodes.
    std::string key_str = std::move(node_iterator->first);
    std::string value_str = std::move(node_iterator->second);

    // Delete the node.
    recently_used_list_.erase(node_iterator);

    // Create a new node at the front of the list, moving the key-value pair into it.
    recently_used_list_.emplace_front(
        lru_node_type(
            std::move(key_str),
            std::move(value_str)
        )
    );
}

void LRU::visit_listnode_new_value(lru_list_type::iterator& node_iterator, const std::string& value)
{
	// Move the key out of the node.
	std::string key_str = std::move(node_iterator->first);

	// Delete the node.
	recently_used_list_.erase(node_iterator);

	// Create a new node at the front of the list, moving/copying the existing key and the new value into it.
	recently_used_list_.emplace_front(
		lru_node_type(
			std::move(key_str),
			std::move(value)
		)
	);
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
