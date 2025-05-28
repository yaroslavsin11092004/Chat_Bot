#define BOOST_PYTHON_STATIC_LIB 
#include "TelegramBot.h"
#include <iostream>
#include <shellapi.h>
#include "Answer.h"
#include <fstream>
#include "Spacy.h"
#include "Rasa.h"
using namespace std;
using namespace boost::python;
wchar_t szDSN[] = L"Driver={Microsoft Access Driver (*.mdb, *.accdb)};DSN='';DBQ=C:\\Users\\yaros\\source\\repos\\TelegrammBot\\TelegrammBot\\TelegramBotDB.accdb";
wchar_t szConnStrOut[256];
bool exit_program = false;
TelegramBot* bot;
DWORD WINAPI console(LPVOID lpParam)
{
    string input;
    std::system("cls");
    std::system("color 0F");
    while (true)
    {
        cout << "Your command -> ";
        cin >> input;
        if (input == "exit")
            bot->exit();
        if (input == "get-log")
        {
            cout << "Введите user_name -> ";
            wstring user_name;
            wcin >> user_name;
            auto user_data = get_user_data(user_name);
            print_user_data(user_data);
        }
        if (input == "clear")
            std::system("cls");
    }
    return 0;
}
vector<string> parse_config(char* path)
{
    ifstream file(path);
    if (!file.is_open())
    {
        file.close();
        printf("Ошибка открытия файла конфигурации бота!");
        exit(EXIT_SUCCESS);
    }
    else
    {
        vector<string> res;
        string buffer;
        while (!file.eof())
        {
            string cur_row;
            getline(file, cur_row);
            buffer += cur_row;
        }
        file.close();
        ostringstream stream;
        string buffer_conf;
        json jconf = json::parse(buffer);
        stream << jconf["prefix_path"];
        buffer_conf = stream.str();
        buffer_conf.pop_back();
        buffer_conf.erase(buffer_conf.begin(), buffer_conf.begin() + 1);
        res.push_back(buffer_conf);
        stream.str("");
        stream << jconf["python_home"];
        buffer_conf = stream.str();
        buffer_conf.pop_back();
        buffer_conf.erase(buffer_conf.begin(), buffer_conf.begin() + 1);
        res.push_back(buffer_conf);
        stream.str("");
        stream << jconf["telegramm_api_token"];
        buffer_conf = stream.str();
        buffer_conf.pop_back();
        buffer_conf.erase(buffer_conf.begin(), buffer_conf.begin() + 1);
        res.push_back(buffer_conf);
        stream.str("");
        stream << jconf["open_wether_api_token"];
        buffer_conf = stream.str();
        buffer_conf.pop_back();
        buffer_conf.erase(buffer_conf.begin(), buffer_conf.begin() + 1);
        res.push_back(buffer_conf);
        return res;
    }
}
int main(int args, char* argv[])
{
    setlocale(LC_ALL, "ru");
    if (args != 2)
    {
        printf("Не найден путь к файлу конфигурации!");
        exit(EXIT_SUCCESS);
    }
    else
    {
        vector<string> conf_data = parse_config(argv[1]);
        wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
        wstring pref = converter.from_bytes(conf_data[0]);
        wstring python_home = converter.from_bytes(conf_data[1]);
        string token = conf_data[2];
        wether_api = conf_data[3];
        wstring exec = pref + L"\\telegrammbot\\telegrammbot\\pyvenv\\scripts\\python.exe";
        int arch;
        std::system("color 0B");
        cout << "Веберите фреймворки для работа бота (1 - TextBlob и Spacy, 2 - Rasa) -> ";
        cin >> arch;
        if (((arch == 1) | (arch == 2)) == 0)
        {
            cout << "Некорректный выбор!";
            exit(EXIT_FAILURE);
        }
        else
        {
            if (arch == 1)
            {
                init_python_and_module(python_home.c_str(), exec.c_str());
                connect_data_base(szDSN, szConnStrOut);
                bot = new TelegramBot(token, answer);
                CreateThread(0, 0, console, nullptr, 0, 0);
                bot->start();
                disconnect_data_base();
                delete bot;
                exit(EXIT_SUCCESS);
            }
            else
            {
                init_python_rasa(python_home.c_str(), exec.c_str());
                connect_data_base(szDSN, szConnStrOut);
                bot = new TelegramBot(token, answer_rasa);
                CreateThread(0, 0, console, nullptr, 0, 0);
                bot->start();
                disconnect_data_base();
                delete bot;
                exit(EXIT_SUCCESS);
            }
        }
    }
    return 0;
}