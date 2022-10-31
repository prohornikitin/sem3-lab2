#pragma once

#include "IDictionary.h"
#include <memory>
#include <iostream>
#include <algorithm>
#include <exception>
#include <stack>
#include <sstream>
#include <functional>
#include <optional>
#include <tuple>


template <class K, class V>
class TreeDictionary : public IDictionary<K, V>{
private:
	class Node {
	public:
		Node(K key, V value) :
			height(1), key(key), value(value),
			left(nullptr), right(nullptr) {}
		Node(K key, V value, Node* left, Node* right) :
			height(1), key(key), value(value), 
			left(left), right(right) {}


		void FixHeight()
		{
			height = std::max(
				_GetHeight(left),
				_GetHeight(right)
			) + 1;
		}

		friend int _GetHeight(Node* n)
		{
			if(n == nullptr)
			{
				return 0;
			}
			return n->height;
		}

		friend int BalanceFactor(Node* n)
		{
			if(n == nullptr)
			{
				return 0;
			}
			return _GetHeight(n->right) - _GetHeight(n->left);
		}

		int height;
		K key;
		V value;
		Node* left;
		Node* right;
	};

	Node* root = nullptr;
	std::function<bool(const K &, const K &)> cmpLess;
	size_t size = 0;


	void RotateRightAround(Node* & p)
	{
		Node* q = p->left;
		p->left = q->right;
		q->right = p;
		p->FixHeight();
		q->FixHeight();
		p = q;
	}

	void RotateLeftAround(Node* & q)
	{
		Node* p = q->right;
		q->right = p->left;
		p->left = q;
		q->FixHeight();
		p->FixHeight();
		q = p;
	}

	Node* FindMin(Node* n)
	{
		while(n->left != nullptr)
		{
			n = n->left;
		}
		return n;
	}

	Node* RemoveMin(Node* p)
	{
		if(p->left == nullptr)
		{
			return p->right;
		}
		p->left = RemoveMin(p->left);
		Balance(p);
		return p;
	}

	void Balance(Node* & p)
	{
		p->FixHeight();
		if(BalanceFactor(p) == 2)
		{
			if(BalanceFactor(p->right) < 0)
			{
				RotateRightAround(p->right);
			}
			RotateLeftAround(p);
		}
		if(BalanceFactor(p) == -2)
		{
			if(BalanceFactor(p->left) > 0)
			{
				RotateLeftAround(p->left);
			}
			RotateRightAround(p);
		}
	}

	Node* InsertInternal(Node* r, const K & key, const V & value)
	{
		
		if(r == nullptr)
		{
			return new Node(key, value);
		}
		
		if(key == r->key)
		{
			throw std::invalid_argument("such a key already exists");
		}
		
		
		if(cmpLess(key, r->key))
		{
			r->left = InsertInternal(r->left, key, value);
		}
		else
		{
			r->right = InsertInternal(r->right, key, value);
		}

		Balance(r);
		return r;
	}

	Node* RemoveInternal(Node* n, K key)
	{
		if(n == nullptr)
		{
			return 0;
		}
		
		if(key == n->key)
		{
			size--;
			Node* l = n->left;
			Node* r = n->right;
			delete n;
			if(r == nullptr)
			{
				 return l;
			}
			Node* min = FindMin(r);
			min->right = RemoveMin(r);
			min->left = l;
			Balance(min);
			return min;
		}
		
		if(cmpLess(key, n->key))
		{
			n->left = RemoveInternal(n->left, key);
		}
		else
		{
			n->right = RemoveInternal(n->right, key);
		}
		Balance(n);
		return n;
	}

	void DeleteAll(Node* r)
	{
		if(r == nullptr)
		{
			return;
		}
		DeleteAll(r->right);
		DeleteAll(r->left);
		delete r;
	}

	Node* SubtreeInternal(Node* n) const
	{
		if(n == nullptr)
		{
			return nullptr;
		}
		Node* newN = new Node(n->key, n->value);
		newN->left = SubtreeInternal(n->left);
		newN->right = SubtreeInternal(n->right);
		newN->FixHeight();
		return newN;
	}

	bool TreeEqualInternal(Node* a, Node* b) const
	{
		if(a == nullptr || b == nullptr)
		{
			return a == b;
		}
		return a->key == b->key &&
			a->value == b->value &&
			TreeEqualInternal(a->right, b->right) &&
			TreeEqualInternal(a->left, b->left);
	}

public:
	using Cmp = std::function<bool(const K & a, const K & b)>;
	
	TreeDictionary(std::initializer_list<std::pair<K, V>> list)
		: TreeDictionary()
	{
		for(auto i : list)
		{
			Add(i.first, i.second);
		}
	}
	
	TreeDictionary()
		: cmpLess(([](const K & a, const K & b){return a < b;}))
	{
		
	}
	
	TreeDictionary(Cmp cmpLess)
		: cmpLess(cmpLess)
	{
		
	}

	TreeDictionary(const TreeDictionary<K,V> & other)
		: cmpLess(other.cmpLess)
	{
		Node* new_root = other.SubtreeInternal(other.root);
		DeleteAll(root);
		root = new_root;
	}

	TreeDictionary<K,V> & operator=(const TreeDictionary<K,V> & other)
	{
		if(this == &other)
		{
			return *this;
		}

		Node* new_root = other.SubtreeInternal(other.root);
		DeleteAll(root);
		root = new_root;
		cmpLess = other.cmpLess;
		return *this;
	}
	
	TreeDictionary<K,V> & operator=(std::initializer_list<std::pair<K, V>> list)
	{
		DeleteAll(root);
		root = nullptr;
		
		for(auto i : list)
		{
			Add(i.first, i.second);
		}
		
		return *this;
	}

	friend bool operator==(const TreeDictionary<K,V> & a, const TreeDictionary<K,V> & b)
	{
		return a.TreeEqualInternal(a.root, b.root);
	}

	friend bool operator!=(const TreeDictionary<K,V> & a, const TreeDictionary<K,V> & b)
	{
		return !(a == b);
	}

	virtual size_t GetCount() const override
	{
		return size;
	}
	
	virtual void Add(K key, V value) override
	{
		root = InsertInternal(root, key, value);
		size++;
	}
	
	virtual void Remove(K key) override
	{
		size_t sizeBuff = size;
		root = RemoveInternal(root, key);
		if(size == sizeBuff)
		{
			throw std::invalid_argument("there is no key to Remove");
		}
	}
	
	virtual std::optional<V> Get(const K & key) const override
	{
		Node* n = root;
		while(n != nullptr)
		{
			if(key == n->key)
			{
				return n->value;
			}

			if(cmpLess(key, n->key))
			{
				n = n->left;
			}
			else
			{
				n = n->right;
			}
		}
		return {};
	}
	
	virtual V& operator[](const K & key) override
	{
		Node* n = root;
		while(n != nullptr)
		{
			if(key == n->key)
			{
				return n->value;
			}

			if(cmpLess(key, n->key))
			{
				n = n->left;
			}
			else
			{
				n = n->right;
			}
		}
		throw std::invalid_argument("No such key");
	}
	
	virtual V operator[](const K & key) const override
	{
		auto opt = Get(key);
		if(opt.has_value()) {
			return opt.value();
		}
		throw std::invalid_argument("No such key");
	}
	
	virtual bool Contains(const K & key) const override
	{
		Node* n = root;
		while(n != nullptr)
		{
			if(key == n->key)
			{
				return true;
			}

			if(cmpLess(key, n->key))
			{
				n = n->left;
			}
			else
			{
				n = n->right;
			}
		}
		return false;
	}

	~TreeDictionary() {
		DeleteAll(root);
	}
};

