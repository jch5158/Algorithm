#pragma once

template <typename T>

class CList
{
private:
	struct Node
	{
		// 생성자입니다.
		Node(T data = NULL):data(data),next(nullptr),prev(nullptr),bDeleteFlag(false)
		{
		}

		T data;
		Node* prev;
		Node* next;
		bool bDeleteFlag;
	};

public:

	class Iterator
	{
	public:

		friend class CList;
			
		Iterator(Node* node = nullptr)
		{
			mpNode = node;
		}

		Iterator(const Iterator& iter)
		{
			mpNode = iter.mpNode;
		}

		void SetDeleteFlag(void)
		{
			mpNode->bDeleteFlag = true;
		}

		bool GetDeleteFlag(void) const
		{
			return mpNode->bDeleteFlag;
		}

		Iterator GetNextIter()
		{
			Iterator iter(mpNode->next);

			return iter;
		}


		// 연산자 오버로딩
		const Iterator operator++(int)
		{	
			const Iterator iterator(mpNode);
			
			mpNode = mpNode->next;

			return iterator;
		}

		Iterator& operator++()
		{	
			mpNode = mpNode->next;
		
			return *this;
		}

		const Iterator operator--(int)
		{
			mpNode = mpNode->prev;
				
			const Iterator iterator(mpNode);

			return iterator;
		}

		Iterator& operator--()
		{			
			mpNode = mpNode->prev;
			
			return *this;
		}

		T& operator*() const
		{
			return mpNode->data;
		}

		T& operator->() const
		{
			return mpNode->data;
		}

		bool operator==(const Iterator& iter) const
		{
			return mpNode->data == iter.mpNode->data;
		}

		bool operator!=(const Iterator& iter) const
		{
			return !(mpNode->data == iter.mpNode->data);
		}

	private:

		Node* mpNode;
	};
	
	CList()
	{
		mpHead = new Node();
		mpTail = new Node();

		mpHead->next = mpTail;
		mpTail->prev = mpHead;

		mListSize = 0;
	}

	~CList()
	{
		Node* prev = nullptr;

		while (mpHead != nullptr)
		{
			prev = mpHead;
			mpHead = mpHead->next;
			delete prev;
		}
	}

	int GetUseSize(void) const
	{
		return mListSize;
	}

	void PushBack(T data)
	{
		Node* pNode = new Node(data);

		// mpTailPrev에 추가
		InputNode(mpTail, pNode);

		mListSize += 1;
	}

	void PushFront(T data)
	{
		Node* pNode = new Node(data);

		InputNode(mpHead->next, pNode);

		mListSize += 1;
	}

	// iter 노드 뒤에 연결
	void insert(Iterator iter, T data)
	{		
		Node* pNode = new Node(data);

		InputNode(iter.mpNode, pNode);

		mListSize += 1;
	}

	Iterator begin() const
	{
		return Iterator(mpHead->next);
	}

	Iterator end() const
	{
		return Iterator(mpTail);
	}

	Iterator erase(Iterator& iter)
	{
		mListSize -= 1;

		Iterator iterBuffer = iter;

		iterBuffer.mpNode->prev->next = iterBuffer.mpNode->next;

		iterBuffer.mpNode->next->prev = iterBuffer.mpNode->prev;

		Iterator iterator(iterBuffer.mpNode->next);

		delete iterBuffer.mpNode;

		return iterator;
	}

	bool Remove(T data)
	{
		Iterator iterE = end();

		for (Iterator iter = begin(); iter != iterE; ++iter)
		{
			if (iter.node->data == data)
			{
				erase(iter);

				return true;
			}
		}
		return false;
	}


	void DataSwap(Iterator iterF, Iterator iterE) 
	{
		T bufferData = iterF.mpNode->data;

		iterF.mpNode->data = iterE.mpNode->data;

		iterE.mpNode->data = bufferData;
	}

private:

	// node의 prev에 newNode를 연결
	void InputNode(Node* node, Node* newNode)
	{
		newNode->prev = node->prev;
		newNode->next = node;

		node->prev->next = newNode;
		node->prev = newNode;
	}


	Node* mpHead;
	Node* mpTail;

	int mListSize;

};