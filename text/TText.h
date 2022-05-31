#pragma once
#include "TStack.h"
#include <fstream>

using namespace std;

struct TNode;
class TText;

struct TMem
{
	TNode* pFirst;
	TNode* pFree;
	TNode* pLast;
};

struct TNode
{
	char str[100];
	TNode* pNext;
	TNode* pDown;

	bool Garbage;

	static TMem mem;

	TNode(char* _str = nullptr, TNode* _pNext = nullptr, TNode* _pDown = nullptr);

	void* operator new(size_t size);

	void operator delete(void* ptr);

	static void InitMem(size_t size);
	static void PrintFreeNodes();

	static void CleanMem(TText& t);
};

class TText
{
private:
	TNode *pFirst;
	TNode *pCurr;	
	TStack<TNode*> st;

	TNode* ReadRec(ifstream& fin);
	
	int textLevel = 0;

	void PrintRec(TNode* p);
	void WriteRec(TNode* p, ostream& out);

	TNode* CopyNode(TNode* p);
public:
	TText();
	TText(TNode* p);

	TText* GetCopy();
	void GoNextNode();
	void GoDownNode();
	void GoUp();
	void GoFirstNode();

	void InsNextLine(char* _str);
	void InsNextSection(char* _str);
	void InsDownLine(char* _str);
	void InsDownSection(char* _str);

	void Reset();
	void GoNext();
	bool IsEnd();
	
	void DelNext();
	void DelDown();
	
	char* GetCurrentLine();

	void Load(string fn);
	void Print();
	void Save(string fn);
	void NotGarbage();
};

TNode::TNode(char* _str, TNode* _pNext, TNode* _pDown)
{
	if (_str == nullptr)
		str[0] = '\0';
	else
		strcpy(str, _str);
	pNext = _pNext;
	pDown = _pDown;
}

void* TNode::operator new(size_t size)
{
	TNode* tmp = mem.pFree;

	if (tmp == nullptr)
		throw "Exception";

	mem.pFree = mem.pFree->pNext;

	return tmp;
}

void TNode::operator delete(void* ptr)
{
	TNode* tmp = mem.pFree;
	TNode* p1 = (TNode*)ptr;
	p1->pNext = tmp;
	mem.pFree = p1;
}

void TNode::InitMem(size_t size)
{
	mem.pFirst = (TNode*) new char[size * sizeof(TNode)];
	mem.pFree = mem.pFirst;
	mem.pLast = mem.pFirst + (size - 1);

	TNode* p = mem.pFirst;
	for (int i = 0; i < size - 1; i++)
	{
		p->pNext = p + 1;
		p->str[0] = 0;
		p->Garbage = true;

		p += 1;
	}
	mem.pLast->pNext = nullptr;
	mem.pLast->str[0] = 0;
}

void TNode::PrintFreeNodes()
{
	TNode* p = mem.pFree;
	while (p != nullptr)
	{
		if (p->str[0] != '\0')
			cout << p->str << '\n';
		p = p->pNext;
	}
}

void TNode::CleanMem(TText& t)
{
	for (t.Reset(); !t.IsEnd(); t.GoNext())
	{
		t.NotGarbage();
	}
	TNode* p = TNode::mem.pFree;
	while (p != nullptr)
	{
		p->Garbage = false;
		p = p->pNext;
	}
	p = TNode::mem.pFirst;
	for (p = TNode::mem.pFirst; p <= TNode::mem.pLast; p++)
	{
		if (p->Garbage)
		{
			delete p;
			p->Garbage = false;
		}
	}
}

TNode* TText::ReadRec(ifstream& fin)
{
	TNode* pTemp = nullptr, * pHead = nullptr;
	char str[81];

	while (!fin.eof())
	{
		fin.getline(str, 81, '\n');
		if (str[0] == '{')
			pTemp->pDown = ReadRec(fin);
		else if (str[0] == '}')
			break;
		else if (strcmp(str, "") == 0)
			continue;
		else
		{
			TNode* newNode = new TNode(str);
			if (pHead == nullptr)
				pTemp = pHead = newNode;
			else
				pTemp->pNext = newNode;
			pTemp = newNode;
		}
	}
	return pHead;
}

void TText::PrintRec(TNode* p)
{
	if (p != nullptr)
	{
		for (int i = 0; i < textLevel; i++)
			cout << "   ";

		if (p == pCurr) cout << "*";
		else cout << " ";

		cout << p->str << '\n';

		textLevel++;
		PrintRec(p->pDown);
		textLevel--;
		PrintRec(p->pNext);
	}
}

void TText::WriteRec(TNode* p, ostream& out)
{
	if (p != nullptr)
	{
		out << p->str << '\n';
		if (p->pDown != nullptr)
		{
			out << "{\n";
			WriteRec(p->pDown, out);

			out << "}\n";
		}
		WriteRec(p->pNext, out);
	}
}

TNode* TText::CopyNode(TNode* p)
{
	TNode* pd, * pn, * pCopy;

	if (p->pDown != nullptr)
		pd = CopyNode(p->pDown);
	else pd = nullptr;

	if (p->pNext != nullptr)
		pn = CopyNode(p->pNext);
	else pn = nullptr;

	pCopy = new TNode(p->str, pn, pd);
	return pCopy;
}

TText::TText() {	}

TText::TText(TNode* p)
{
	pFirst = p;
	pCurr = nullptr;
}

TText* TText::GetCopy()
{
	TText* res;
	res = new TText(CopyNode(pFirst));
	return res;
}

void TText::GoNextNode()
{
	if (pCurr != nullptr && pCurr->pNext != nullptr)
	{
		st.Push(pCurr);
		pCurr = pCurr->pNext;
	}
}

void TText::GoDownNode()
{
	if (pCurr != nullptr && pCurr->pDown != nullptr)
	{
		st.Push(pCurr);
		pCurr = pCurr->pDown;
	}
}

void TText::GoUp()
{
	if (!st.IsEmpty())
	{
		TNode* prevNode = st.Pop();
		pCurr = prevNode;
	}
}

void TText::GoFirstNode()
{
	st.Clear();
	pCurr = pFirst;
}

void TText::InsNextLine(char* _str)
{
	if (pCurr != nullptr)
	{
		TNode* newNode = new TNode(_str);
		newNode->pNext = pCurr->pNext;
		pCurr->pNext = newNode;
	}
}

void TText::InsNextSection(char* _str)
{
	if (pCurr != nullptr)
	{
		TNode* newNode = new TNode(_str);
		newNode->pDown = pCurr->pNext;
		pCurr->pNext = newNode;
	}
}

void TText::InsDownLine(char* _str)
{
	if (pCurr != nullptr)
	{
		TNode* newNode = new TNode(_str);
		newNode->pNext = pCurr->pDown;
		pCurr->pDown = newNode;
	}
}

void TText::InsDownSection(char* _str)
{
	if (pCurr != nullptr)
	{
		TNode* newNode = new TNode(_str);
		newNode->pDown = pCurr->pDown;
		pCurr->pDown = newNode;
	}
}

void TText::DelNext()
{
	if (pCurr != nullptr)
	{
		TNode* pDel = pCurr->pNext;
		if (pDel != nullptr)
		{
			pCurr->pNext = pDel->pNext;
			delete pDel;
		}
	}
}

void TText::DelDown()
{
	if (pCurr != nullptr)
	{
		TNode* pDel = pCurr->pDown;
		if (pDel != nullptr)
		{
			pCurr->pDown = pDel->pNext;
			delete pDel;
		}
	}

}

void TText::Reset()
{
	st.Clear();

	if (pFirst != nullptr)
	{
		pCurr = pFirst;

		st.Push(pCurr);

		if (pCurr->pNext != nullptr)
			st.Push(pCurr->pNext);
		if (pCurr->pDown != nullptr)
			st.Push(pCurr->pDown);
	}
}

void TText::GoNext()
{
	pCurr = st.Pop();
	if (pCurr != pFirst)
	{
		if (pCurr->pNext != nullptr)
			st.Push(pCurr->pNext);
		if (pCurr->pDown != nullptr)
			st.Push(pCurr->pDown);
	}
}

bool TText::IsEnd()
{
	return st.IsEmpty();
}

char* TText::GetCurrentLine()
{
	return pCurr->str;
}

void TText::Load(string fn)
{
	ifstream s(fn);
	pFirst = ReadRec(s);
}

void TText::Print()
{
	PrintRec(pFirst);
}

void TText::Save(string fn)
{
	ofstream out;
	out.open(fn);

	if (!out.is_open())	throw "Exception";

	WriteRec(pFirst, out);
}

void TText::NotGarbage()
{
	pCurr->Garbage = false;
}
