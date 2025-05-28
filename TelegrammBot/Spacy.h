#pragma once
#include "Answer.h"
#include <sstream>
#include <Parser.h>
#include <locale>
#include <codecvt>
inline wstring answer(wstring input)
{
    if (input == L"������" || input == L"/start")return wstring(L"������������, � ���-��������.\n����:\n1)���������� ���������� ���� �� �������, ������ ��� ��� ������;\n2) ��������� ������� �������������� ���������");
    if (input == L"����� ������� �����?") return get_data();
    if (input[0] == L'r')
    {
        input.erase(input.begin(), input.begin() + 1);
        string res = extract<string>(proc(to_utf8_codec(input)));
        wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
        return converter.from_bytes(res);
    }
    wstring table = get_category(input);
    if (table == L"")
    {
        string utf8 = to_utf8_codec(input);
        double pol = extract<double>(sent(utf8));
        wstringstream stream;
        stream << rand_int(0, 9);
        wstring key = stream.str();
        if (pol == 0)
        {
            table = L"Neutrality";
            return get_data_from_db(table, key) + L" ���������, ���� ����-������ ������.";
        }
        else if (pol < 0)
        {
            table = L"Negative";
            return get_data_from_db(table, key) + L" �� ������ ��� ��������?";
        }
        else
        {
            table = L"Happy";
            return get_data_from_db(table, key) + L" ��� � ��� ���� ���� ������?";
        }
    }
    else if (table == L"arifm")
    {
        wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
        string arifm = converter.to_bytes(input);
        Parser parser(arifm.c_str());
        try
        {
            parser.parse();
            wstringstream stream;
            stream << parser.eval(valarray<double>(14));
            wstring answer = L"�����: ";
            return answer + stream.str();
        }
        catch (std::exception& e)
        {
            wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
            return converter.from_bytes(e.what());
        }
    }
    else
    {
        wstringstream stream;
        stream << rand_int(0, 9);
        wstring key = stream.str();
        return get_data_from_db(table, key);
    }
}