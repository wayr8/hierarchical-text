#define _CRT_SECURE_NO_WARNINGS
#define HOME  71
#define DOWN  80
#define NEXT  77
#define UP	  72
#define ESC	  27
#define INS	  82
#define DEL	  83
#define ENTER 13

#include <iostream>
#include <conio.h>
#include "TText.h"
#include <Windows.h>

void ClearScreen()
{
	HANDLE Console = GetStdHandle(STD_OUTPUT_HANDLE);
	if (!Console) return;

	CONSOLE_SCREEN_BUFFER_INFO buf;
	GetConsoleScreenBufferInfo(Console, &buf);

	DWORD Count;

	COORD zpos;
	zpos.X = 0;
	zpos.Y = 0;

	FillConsoleOutputCharacter(Console, ' ', buf.dwSize.X * buf.dwSize.Y, zpos, &Count);
	SetConsoleCursorPosition(Console, zpos);
}

TMem TNode::mem;

int main()
{
	TNode::InitMem(100);

	TText t;

	t.Load("..\\text.txt");
	t.GoFirstNode();

	char ch;

	do {
		cout << ">,v,^, Home, Ins, Del, Enter, Esc\n\n";

		t.Print();
		cout << '\n';

		ch = _getch();

		if (ch == 0xE0)
			ch = _getch();
		if (ch == ESC)
		{
			cout << "\nCompletion of the program\n\n";
			t.Print();
			cout << '\n';

			t.Save("..\\out.txt");
			break;
		}

		if (ch != ENTER)
			ch = _getch();

		switch (ch)
		{
		case ENTER: break;
		case DOWN:
			t.GoDownNode();
			break;
		case NEXT:
			t.GoNextNode();
			break;
		case UP:
			t.GoUp();
			break;
		case HOME:
			t.GoFirstNode();
			break;
		case INS:
			cout << "Enter new text item: ";
			char item[256];
			if (fgets(item, sizeof(item), stdin))
			{
				item[strcspn(item, "\r\n")] = 0;

				cout << "Insert: 1 - Next line, 2 - Next section, 3 - Down line, 4 - Down section\n";
				int key1;
				cin >> key1;
				if (key1 == 1) t.InsNextLine(item);
				else if (key1 == 2) t.InsNextSection(item);
				else if (key1 == 3) t.InsDownLine(item);
				else if (key1 == 4) t.InsDownSection(item);
			}
			cin.ignore(1);
			break;
		case DEL:
			int key2;
			cout << "1 - Delete down, 2 - Delete next\n";
			cin >> key2;
			if (key2 == 1) t.DelDown();
			else if (key2 == 2) t.DelNext();
			cin.ignore(1);
			break;
		}

		ClearScreen();
	} while (ch != ESC);

	cout << "\nFree nodes:\n";
	TNode::PrintFreeNodes();

	TNode::CleanMem(t);
	cout << "\nFree nodes after clean memory:\n";
	TNode::PrintFreeNodes();

	cout << "\nOutput in file:\n";
	t.Print();

	cout << "\nCopy test:\n";

	TText* t2 = t.GetCopy();
	t2->Print();
}
