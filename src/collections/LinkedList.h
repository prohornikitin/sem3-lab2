#pragma once

#include <debug.h>

#include <stdexcept>

template <class T>
class LinkedList
{
public:
	LinkedList() = default;
	LinkedList(std::initializer_list<T> init)
	{
		size_t i = 0;
		for(auto k : init)
		{
			Append(k);
			i++;
		}
	}

	LinkedList(T * items, size_t count)
	{
		// TODO: Optimize
		for(size_t i = 0; i < count; ++i)
		{
			Append(items[i]);
		}
	}

	LinkedList(const LinkedList & list)
	{
		// TODO: Optimize
		for(size_t i = 0; i < list.length; ++i)
		{
			Append(list.Get(i));
		}
	}

	T GetFirst() const
	{
		return head->data;
	}

	T GetLast() const
	{
		return tail->data;
	}

	T Get(size_t index) const
	{
		return GetNode(index)->data;
	}

	T & GetRef(size_t index)
	{
		return GetNode(index)->data;
	}

	LinkedList<T> * GetSubList(size_t startIndex, size_t endIndex) const
	{
		LinkedList<T> * subList = new LinkedList<T>();
		auto endNode = GetNode(endIndex);
		for(Node * i = GetNode(startIndex); i != endNode->next; i = i->next)
		{
			subList->Append(i->data);
		}
		return subList;
	}

	size_t GetLength() const
	{
		return length;
	}

	void Append(T item)
	{
		InsertAt(item, length);
	}

	void Prepend(T item)
	{
		InsertAt(item, 0);
	}

	void InsertAt(T item, size_t index)
	{
		if(index > length)
		{
			while(index >= length)
			{
				Node * node = new Node(item, tail, nullptr);
				tail->next = node;
				tail = node;
				length++;
			}
			return;
		}
		Node * prev = nullptr;
		Node * next = nullptr;
		if(index == 0)
		{
			prev = nullptr;
			next = head;
		}
		else if(index == length)
		{
			prev = tail;
			next = nullptr;
		}
		else
		{
			prev = GetNode(index - 1);
			next = prev->next;
		}

		Node * node = new Node(item, prev, next);
		if(prev != nullptr)
		{
			prev->next = node;
		}
		if(next != nullptr)
		{
			next->prev = node;
		}

		if(index == 0)
		{
			head = node;
		}
		if(index == length)
		{
			tail = node;
		}
		length++;
	}

	void RemoveAt(size_t index)
	{
		Node * toBeRemoved = GetNode(index);
		if(toBeRemoved->prev != nullptr)
		{
			toBeRemoved->prev->next = toBeRemoved->next;
		}
		if(toBeRemoved->next != nullptr)
		{
			toBeRemoved->next->prev = toBeRemoved->prev;
		}

		if(toBeRemoved == tail)
		{
			tail = toBeRemoved->prev;
		}
		if(toBeRemoved == head)
		{
			head = toBeRemoved->next;
		}

		delete toBeRemoved;
		length--;
	}

	LinkedList<T> * Concat(LinkedList<T> * list) const
	{
		LinkedList<T> * copy = new LinkedList(*this);
		for(Node * i = list->head; i != nullptr; i = i->next)
		{
			copy->Append(i->data);
		}
	}

	~LinkedList()
	{
		if(head != nullptr)
		{
			for(Node * i = head->next; i != nullptr; i = i->next)
			{
				delete i->prev;
			}
		}
		delete tail;
	}

private:
	class Node
	{
	public:
		Node(T data, Node * prev = nullptr, Node * next = nullptr) :
			data(data),
			prev(prev),
			next(next)
		{}

		Node(Node * prev = nullptr, Node * next = nullptr) :
			prev(prev),
			next(next)
		{}

		T data;
		Node * prev;
		Node * next;
	};

	Node * GetNode(size_t index) const
	{
		if(index >= length)
		{
			throw std::out_of_range("Invalid index " + std::to_string(index));
		}

		if(index < length - index)
		{
			Node * node = head;
			for(size_t i = 0; i < index; ++i)
			{
				node = node->next;
			}
			//~ DPRINTF("node=%p, value=%d\n", node, node->data);
			return node;
		}
		else
		{
			Node * node = tail;
			for(size_t i = length - 1; i > index; --i)
			{
				node = node->prev;
			}
			//~ DPRINTF("node=%p, value=%d\n", node, node->data);
			return node;
		}
	}

	Node * head = nullptr;
	Node * tail = nullptr;
	size_t length = 0;
};
