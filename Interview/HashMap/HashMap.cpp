#include <iostream>
using namespace std;

struct hash 
{
	size_t operator()(const int &key) 
	{
		return key % 3;
	}
};

struct Compare
{
	bool operator()(const int& a, const int &b) const 
	{
		return a == b;
	}
};

template <class Key, class Value>
class Hash_Node 
{
public:
	int m_key;
	int m_value;
	struct Hash_Node *next;
	Hash_Node(Key key1, Value value1) :m_key(key1), m_value(value1), next(NULL) {}

};

template<class Key, class Value, class Hash, class Compare>
class Hash_Map
{
public:
	Hash_Map(int len = 6):size(len) 
	{
		cur = 0;
		bucket = size / 2;
		Hash_Map = new Hash_Node<Key, Value> *[size];
		for (int i = 0; i < bucket; i++) 
		{
			hash_map[i] = NULL;
		}
	}
	Hash_Node<Key, Value>* find(Key key)
	{
		int hash_code = hash(key);
		Hash_Node<Key, Value> *pNode = hash_map[hash_code];
		while (pNode)
		{
			if (compare(pNode->m_key, key)) 
			{
				return pNode;
			}
			else
			{
				pNode = pNode->next;
			}
		}
		return NULL;
	}

	bool set(Key key, Value value) 
	{
		if (cur > size)
		{
			return false;
		}
		Hash_Node<Key, Value> *pNode = find(key);
		if (pNode == NULL) 
		{
			int hash_code = hash(key);
			Hash_Node<Key, Value>*pData = new Hash_Node<Key, Value>(key, value);
			Hash_Node<Key, Value>*pNode = hash_map[hash_code];
			if (pNode == nullptr) 
			{
				hash_map[hash_code] = pData;
			}
			else
			{
				while (pNode->next != nullptr)
				{
					pNode = pNode->next;
				}
				pNode->next = pData;
			}
		}
		else
		{
			pNode->m_value = value;
		}
		++cur;
		return true;
	}

	Value &get(Key key) 
	{
		Hash_Node<Key, Value> *pNode = find(key);
		if (pNode)
			return pNode->m_value;
		set(key, 0);
		return find(key)->m_value;
	}

	~Hash_Map() {
		for (int i = 0; i<bucket; i++)
		{
			while (true)
			{
				Hash_Node<Key, Value> *pnode = hash_map[i];
				if (pnode) 
				{
					hash_map[i] = pnode->next;
					delete pnode;
				}
				else {
					break;
				}
			}
		}
		delete[]hash_map;
	}

private:
	Hash_Node<Key, Value> **hash_map;
	Hash hash;
	Compare compare;
	int bucket;
	int size;
	int cur;
};

