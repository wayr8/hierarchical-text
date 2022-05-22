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
	//В стеке сохраняются все указатели до текущего (текущий не хранится)
	//При перемещениях стек модифицируем
	TStack<TNode*> st;

	//Рекурсивное чтение из файла
	TNode* ReadRec(ifstream& fin);

	//Печать на экран текста
	//печатает саму строку, потом всё по pDown, потом всё по pNext
	//(рекурсивно)
	int textLevel = 0;

	void PrintRec(TNode* p);
	void WriteRec(TNode* p, ostream& out);

	TNode* CopyNode(TNode* p);

public:
	TText();
	TText(TNode* p);
	//Функция копирования; вызывает рекурсивную функцию копирования
	TText* GetCopy();
	//Перемещение указателя pCurr на следующее звено
	void GoNextNode();
	//Перемещение указателя pCurr на звено подчинённой ему части
	void GoDownNode();
	//Переход назад (вверх) по вложенности
	void GoUp();
	//Возврат в корневую (самую первую) строку всего текста
	void GoFirstNode();

	//Вставка новой строки за текущей
	void InsNextLine(char* _str);
	//Вставка нового заголовка за текущей строкой и
	//переподчинение всей нижней части списка ему
	void InsNextSection(char* _str);
	//Вставка новой строки в начало подчинённой части
	void InsDownLine(char* _str);
	//Вставка нового заголовка в подчинённую часть
	void InsDownSection(char* _str);

	//С утечкой памяти (удаляется только pDel, но не его внутренности)!!!
	//Удалить следующее звено
	void DelNext();
	void DelDown();

	//Возврат pCurr на первое звено
	void Reset();
	//Переход pCurr далее
	void GoNext();
	//Проверка окончания обхода
	bool IsEnd();

	//Получение строки текущего звена
	char* GetCurrentLine();

	//Загрузка текста из файла
	void Load(string fn);
	//Печать текста на экран
	void Print();
	//Сохранение текста в файл
	void Save(string fn);
	//Пометка текущего звена как "не мусор"
	void NotGarbage();
};

//................................................................

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
		throw "Out of memory";

	mem.pFree = mem.pFree->pNext;

	//ВОЗМОЖНО, ПРИДЁТСЯ ЧТО-ТО ДОПИСАТЬ
	return tmp;
}

void TNode::operator delete(void* ptr)
{
	TNode* tmp = mem.pFree;
	//Запомнили освобождённый
	TNode* p1 = (TNode*)ptr;
	//Новый освобождённый теперь показывает на следующий свободный
	p1->pNext = tmp;
	//Обновим первый свободный
	mem.pFree = p1;

	//Оптимальнее (проверить):
	//TNode* newpFree = (TNode*) ptr;
	//newpFree->pNext = mem.pFree;
	//mem.pFree = newpFree;

	//ВОЗМОЖНО, ПРИДЁТСЯ ЧТО-ТО ДОПИСАТЬ
}

void TNode::InitMem(size_t size)
{
	//Выделение памяти под size элементов TNode (через костыль)
	mem.pFirst = (TNode*) new char[size * sizeof(TNode)];

	//Установка pFree
	mem.pFree = mem.pFirst;
	//Вычисление pLast через pFree
	mem.pLast = mem.pFirst + (size - 1);

	TNode* p = mem.pFirst;
	//Расстановка pNext'ов ([ ] -> [ ] -> ... [ ] -> [ ])
	for (int i = 0; i < size - 1; i++)
	{
		p->pNext = p + 1;
		p->str[0] = 0;
		p->Garbage = true;

		p += 1;
	}
	//Установка null для последнего
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
	//Проход по списку свободных, отметка всех свободных как "не мусор"
	for (t.Reset(); !t.IsEnd(); t.GoNext())
	{
		//(pCurr->Garbage = false)
		t.NotGarbage();
	}

	//Проход по списку занятых, отметка всех занятых как "не мусор"
	TNode* p = TNode::mem.pFree;
	while (p != nullptr)
	{
		p->Garbage = false;
		p = p->pNext;
	}

	//Остальное - мусор, его нужно вернуть в список свободных
	p = TNode::mem.pFirst;
	for (p = TNode::mem.pFirst; p <= TNode::mem.pLast; p++)
	{
		if (p->Garbage)
		{
			//Это наш самописный delete
			delete p;

			//Сам дописал
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

	//Если не дошли до "фиктивной строки снизу стека"
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

	if (!out.is_open())	throw "Export exception!";

	WriteRec(pFirst, out);
}

void TText::NotGarbage()
{
	pCurr->Garbage = false;
}