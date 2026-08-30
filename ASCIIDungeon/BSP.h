#pragma once
#include <vector>

template<typename T>
class BSPNode
{
	friend class BSPTree<T>;
public:
	const T& GetData() const { return _data; }
	BSPNode<T>* GetParent() const { return _parent; }
	BSPNode<T>* GetLeftChild() const { return _left; }
	BSPNode<T>* GetRightChild() const { return _right; }

private:
	BSPNode(const T& data)
		: _data(data)
	{
	}

	~BSPNode()
	{
		delete _left;
		_left = nullptr;

		delete _right;
		_right = nullptr;
	}

	BSPNode(const BSPNode& other) = delete;
	BSPNode& operator=(const BSPNode& other) = delete;

private:
	T _data;

	BSPNode<T>* _left = nullptr;
	BSPNode<T>* _right = nullptr;
	BSPNode<T>* _parent = nullptr;
};

template<typename T>
class BSPTree
{
public:
	BSPTree(const T& data)
		: _root(new BSPNode<T>(data))
	{
	}

	~BSPTree()
	{
		delete _root;
		_root = nullptr;
	}

	BSPNode<T>* AddLeftChild(BSPNode<T>* parent, T& data)
	{
		if (!parent)
			return nullptr;

		if (parent->_left)
			return nullptr;

		BSPNode<T>* child = new BSPNode<T>(data);
		child->_parent = parent;
		parent->_left = child;

		return child;
	}

	BSPNode<T>* AddRightChild(BSPNode<T>* parent, T& data)
	{
		if (!parent)
			return nullptr;

		if (parent->_right)
			return nullptr;

		BSPNode<T>* child = new BSPNode<T>(data);
		child->_parent = parent;
		parent->_right = child;

		return child;
	}

	void AddLeaf(BSPNode<T>& leaf)
	{
		_leaves.emplace_back(&leaf);
	}

	BSPNode<T>* GetRoot() const { return _root; }
	std::vector<BSPNode<T>*>& GetLeaves() { return _leaves; }

private:
	BSPNode<T>* _root;
	std::vector<BSPNode<T>*> _leaves;
};