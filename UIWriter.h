#pragma once
#include <windows.h>
#include <stdio.h>
#include <string>

using namespace std;

enum UIPart {
    SPACE,
    BKSP,
    ESC
};

class UIWriter
{
private:
    //settings
    short width = 120;
    short height = 48;
    short numSegmentsGraph = 23;
    unsigned char N = 13;

    HANDLE tScr = GetStdHandle(STD_OUTPUT_HANDLE);
    CHAR_INFO* scrBuff = new _CHAR_INFO[width*height];
    CHAR_INFO* bordBuff = new _CHAR_INFO[width*height];

    SMALL_RECT srctReadRect;
    SMALL_RECT srctWriteRect;
    COORD coordBufSize;
    COORD coordBufCoord;

    string IntToHexStr(short i)
    {
        char pt1, pt2, pt3, pt4;
        pt1 = i / 16;
        pt3 = i % 16;
        string rez = "";// = to_string(i / 16) + to_string(i % 16);
        if (pt1 > 9)
        {
            pt2 = 'A' + pt1 - 10;
            rez += pt2;
        }
        else
        {
            rez += to_string(pt1);
        }
        if (pt3 > 9)
        {
            pt4 = 'A' + pt3 - 10;
            rez += pt4;
        }
        else
        {
            rez += to_string(pt3);
        }
        return rez;
    }

    void addHorizontalLine(int targX, int startY, CHAR_INFO ch, int len, CHAR_INFO* buff) 
    {
        int shift = targX * 120;
        for (short i = 0; i < len; i++)
        {
            *(buff + shift + i + startY) = ch;
        }
    }  

    void addVerticalLine(int targY, int startX, CHAR_INFO ch, int len, CHAR_INFO* buff) 
    {
        for (short i = 0; i < len; i++)
        {
            *(buff + (i + startX) * width + targY) = ch;
        }
    }

    void addText(int Y, int X, string str, CHAR_INFO* buff) 
    {
        int shift = X * 120;
        for (short i = 0; i < str.length(); i++)
        {
            CHAR_INFO ch;
            ch.Char.UnicodeChar = str[i];
            ch.Attributes = 0x0007;
            *(buff + shift + Y + i) = ch;
        }
    }

    void addHighlightedText(int Y, int X, string str, CHAR_INFO* buff) 
    {
        int shift = X * 120;
        for (short i = 0; i < str.length(); i++)
        {
            CHAR_INFO ch;
            ch.Char.UnicodeChar = str[i];
            ch.Attributes = 0x0010 + 0x0020 + 0x0040;
            *(buff + shift + Y + i) = ch;
        }
    }

    void drowBorder(CHAR_INFO* buff)
    {
        CHAR_INFO bordSym;
        bordSym.Attributes = 0x0010 + 0x0040;
        bordSym.Char.UnicodeChar = ' ';
        addHorizontalLine(0, 0, bordSym, width, buff);
        addVerticalLine(0, 0, bordSym, height, buff);
        addVerticalLine(1, 0, bordSym, height, buff);
        addHorizontalLine(height - 1, 0, bordSym, width, buff);
        addVerticalLine(width - 1, 0, bordSym, height, buff);
        addVerticalLine(width - 2, 0, bordSym, height, buff);
        addHorizontalLine(height - 3, 0, bordSym, width, buff);
        for (size_t i = 0; i < 256 / N + 1; i++)
        {
            addHorizontalLine(2 + i*2, 0, bordSym, width, buff);
        }
    }

    void defineEmpty(CHAR_INFO* buff) {
        srctReadRect.Top = 0;
        srctReadRect.Left = 0;
        srctReadRect.Bottom = height - 1;
        srctReadRect.Right = width - 1;

        coordBufSize.Y = height;
        coordBufSize.X = width;

        coordBufCoord.X = 0;
        coordBufCoord.Y = 0;
        ReadConsoleOutput(
            tScr,
            buff,
            coordBufSize,
            coordBufCoord,
            &srctReadRect);
    }
    /**/
    void drowNamesBytes(char offset)
    {
        CHAR_INFO bordSym;
        bordSym.Attributes = 0x0010 + 0x0040;
        bordSym.Char.UnicodeChar = ' ';

        for (short i = 0; i < N; i++)
        {
            for (short j = 0; j < 256 / N + 1; j++)
            {
                if (i + N * j < 256)
                    addText(offset + width / N * i, 1 + j * 2, IntToHexStr(i + N * j), scrBuff);
                
            }
            if (i != N - 1)
                addVerticalLine(offset + width / N * i + width / N - 1, 0, bordSym, (256 / N + 1) * 2, scrBuff);
        }
    }

    void drowMenu()
    {
        addText(width / 4 - 18, height - 2, "ESC - exit", scrBuff);
        addText((width / 4) * 2 - 18, height - 2, "BKSP - Change high value", scrBuff);
        addText((width / 4) * 3 - 12, height - 2, "SPACE - Change representation", scrBuff);
    }
    
    void drowValues(bool IsPercentage, unsigned int* DataValue, double* DataPerc, float HighLineVal, char offset)
    {
        if (!IsPercentage)
        {
            for (short i = 0; i < N; i++)
            {
                for (short j = 0; j < 256 / N + 1; j++)
                {
                    if (i + N * j < 256)
                    {
                        string temp = to_string(*(DataValue + i + N * j));
                        if (*(DataPerc + i + N * j) * 1000 > HighLineVal)
                            addHighlightedText(offset + width / N * i + 8 - temp.length(), 1 + j * 2, temp, scrBuff);
                        else
                            addText(offset + width / N * i + 8 - temp.length(), 1 + j * 2, temp, scrBuff);
                    }

                }
            }
        }
        else
        {
            for (short i = 0; i < N; i++)
            {
                for (short j = 0; j < 256 / N + 1; j++)
                {
                    if (i + N * j < 256)
                    {
                        string temp = to_string((int)round(*(DataPerc + i + N * j) * 1000)) + "%o";
                        if (*(DataPerc + i + N * j) * 1000 > HighLineVal)
                            addHighlightedText(offset + width / N * i + 8 - temp.length(), 1 + j * 2, temp, scrBuff);
                        else
                            addText(offset + width / N * i + 8 - temp.length(), 1 + j * 2, temp, scrBuff);
                    }

                }
            }
        }
    }

public:
    UIWriter()
    {
        HANDLE hWnd = GetStdHandle(STD_OUTPUT_HANDLE);
        COORD bufferSize = { width, height };
        SetConsoleScreenBufferSize(hWnd, bufferSize);
        system("mode con cols=120 lines=48");

        defineEmpty(bordBuff);
        defineEmpty(scrBuff);
        drowBorder(bordBuff);
        memcpy(scrBuff, bordBuff, width * height * 4);
    }

    ~UIWriter() 
    {
        delete[] scrBuff;
        delete[] bordBuff;
    }

    void Print(CHAR_INFO* buff) 
    {
        srctReadRect.Top = 0;
        srctReadRect.Left = 0;
        srctReadRect.Bottom = height-1;
        srctReadRect.Right = width-1;

        coordBufSize.Y = height;
        coordBufSize.X = width;

        coordBufCoord.X = 0;
        coordBufCoord.Y = 0;
        WriteConsoleOutput(
            tScr,
            buff,
            coordBufSize,
            coordBufCoord,
            &srctReadRect);
    }   

    void Print() 
    {
        srctReadRect.Top = 0;
        srctReadRect.Left = 0;
        srctReadRect.Bottom = height-1;
        srctReadRect.Right = width-1;

        coordBufSize.Y = height;
        coordBufSize.X = width;

        coordBufCoord.X = 0;
        coordBufCoord.Y = 0;
        WriteConsoleOutput(
            tScr,
            scrBuff,
            coordBufSize,
            coordBufCoord,
            &srctReadRect);
    }


    void DrowGraf(unsigned int* DataValue, double* DataPerc, float HighLineVal, bool IsPercentage)
    {  
        memcpy(scrBuff, bordBuff, width * height * 4);

        char offset = 2;
        drowNamesBytes(offset);
        drowMenu();
        drowValues(IsPercentage, DataValue, DataPerc, HighLineVal, offset);

        Print(scrBuff);
    }
    
    void HighlightUI(UIPart part) 
    {
        switch (part)
        {
        case SPACE:
            addHighlightedText((width / 4) * 3 - 12, height - 2, "SPACE - Change representation", scrBuff);
            break; 
        case BKSP:
            addHighlightedText((width / 4) * 2 - 18, height - 2, "BKSP - Change high value", scrBuff);
            break;
        case ESC:
            addHighlightedText(width / 4 - 18, height - 2, "ESC - exit", scrBuff);
            break;
        default:
            break;
        }
        Print();
    }
};