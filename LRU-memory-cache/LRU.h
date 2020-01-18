#ifndef LRU_H
#define LRU_H

#include <list>
#include <unordered_map>

/** Main class for the Least-Recently Used (LRU) cache data structure.
*/
class LRU
{
public:

	using lru_node_type = std::pair<std::string, std::string>;
	using lru_list_type = std::list<lru_node_type>;
	using lru_map_type = std::unordered_map<std::string, lru_list_type::iterator>;

	LRU();
	
	// Delete the copy constructors as they will cause (unexpected) large memory copies.
	LRU(const LRU&) = delete;
	LRU& operator=(LRU&) = delete;

	// Move constructors are fine, cache is moved to another LRU object and not copied.
	LRU(LRU&&) = default;
	LRU& operator=(LRU&&) = default;

	~LRU();
	
	/** Query for an element in the cache by key. If the key is found, return
	 * true and copy the string into the result field.
	 */
	bool get(const std::string& key, std::string& result);
	/** Query for an element in the cache by key. If the key is round, return
	 *  the pointer to the value string. If not found, return nullptr.
	 */
	const std::string* get_by_pointer(const std::string& key);
	
	/** Insert a key and its associated value pair into the cache. Replacing
	 * any previous value.
	 */
	void put(const std::string& key, const std::string& value);

	bool empty() const
	{
		return lookup_map_.empty();
	}

	size_t size() const
	{
		return lookup_map_.size();
	}

	const lru_list_type& recently_used_list() const
	{
		return recently_used_list_;
	}

	const lru_map_type& lookup_map() const
	{
		return lookup_map_;
	}
	
private:

	/** Set a node as most recently used.
	 */
	void visit_listnode(lru_list_type::iterator& node_iterator);

	/** Set a node as most recently used and assign a new value to it.
	 */
	void visit_listnode_new_value(lru_list_type::iterator& node_iterator, const std::string& value);

	/** Delete least recently used element. Cache must not be empty.
	 */
	void delete_last();

	/** Remove the 10% least recently used elements, freeing up memory.
	 */
	void cleanup();

	lru_list_type::iterator most_recent_listnode()
	{
		return recently_used_list_.begin();
	}

	lru_list_type recently_used_list_;
	lru_map_type lookup_map_;
};

#endif //LRU_H
