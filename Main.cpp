#include "UIWriter.h"
#include "FileReader.h"
#include <conio.h>

float GetNewHighValue();

using namespace std;

int main(int argc, char* argv[])
{
	string path;
	if (argc == 2)
		path = argv[1];
	FileReader FRead;
	bool Success = false;
	UIWriter UI;

	while (!Success)
	{
		try
		{
			FRead = FileReader(path);
		}
		catch (const char* e)
		{
			cout << e << endl;
			cout << "Input correct path: ";
			char temp[140];
			cin.getline(temp, 140);
			path = temp;
			path = path.substr(path.find_first_not_of('"'), path.find_last_not_of('"') - path.find_first_not_of('"') + 1);
			continue;
		}
		Success = true;
	}


	bool IsExit = false;
	bool percentage = true;
	float HighLineVal = 0;//input outside

	while (!IsExit) {
		UI.DrowGraf(FRead.NumBytes, FRead.PercBytes, HighLineVal, percentage);
		char inpSym = _getch();
		if (inpSym == 32)
		{
			UI.HighlightUI(SPACE);
			percentage = !percentage;
			Sleep(25);
		}
		if (inpSym == '\b')
		{
			UI.HighlightUI(BKSP);
			Sleep(25);
			HighLineVal = GetNewHighValue();
			system("cls");
		}
		if (inpSym == 27)
		{
			UI.HighlightUI(ESC);
			Sleep(25);
			IsExit = true;
		}
	}
	system("cls");
}

float GetNewHighValue()
{
	system("cls");
	float rez;
	cout << "Input new high value(in parts per thousand): ";
	cin >> rez;
	return rez;
}