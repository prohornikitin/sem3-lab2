#include <optional>

template <class K, class V>
class IDictionary
{
	virtual size_t GetCount() const = 0;

	virtual void Add(K key, V value) = 0;
	virtual void Remove(K key) = 0;
	virtual std::optional<V> Get(const K & key) const = 0;
	virtual V & operator[](const K & key) = 0;
	virtual V operator[](const K & key) const = 0;
	virtual bool Contains(const K & key) const = 0;
};
