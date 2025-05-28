#pragma once
#define BOOST_PYTHON_STATIC_LIB 
#include <string>
#include <string.h>
#include <regex>
#include <Windows.h>
#include <sqlext.h>
#include <chrono>
#include <ctime> 
#include <codecvt>
#include <boost/python.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast.hpp>
#include <thread>
#pragma warning(disable:4996)
using namespace std;
using namespace boost::python;
inline HENV hEnv;
inline HDBC hDbc;
inline object sent;
inline object proc;
inline object trans;
inline string wether_api;
inline int rand_int(int min, int max) { return rand() % (max - min + 1); }
inline void connect_data_base(const wchar_t* szDSN, const wchar_t* szConnStrOut)
{
	SQLAllocEnv(&hEnv);
	SQLAllocConnect(hEnv, &hDbc);
	int iConnStrLen;
	SQLDriverConnect(hDbc, NULL, (wchar_t*)szDSN, SQL_NTS, (wchar_t*)szConnStrOut, 255, (SQLSMALLINT*)&iConnStrLen, SQL_DRIVER_NOPROMPT);
}
inline void disconnect_data_base()
{
	SQLDisconnect(hDbc);
	SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
	SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
}
inline wstring get_data_from_db(wstring table_name, wstring key)
{
	wstring sql_request = L"SELECT * FROM " + table_name + L" WHERE answer_id = " + key;
	HSTMT hstmt;
	SQLAllocStmt(hDbc, &hstmt);
	SQLPrepare(hstmt, (SQLWCHAR*)sql_request.c_str(), SQL_NTS);
	SQLExecute(hstmt);
	wchar_t buffer[2048];
	SQLLEN len;
	SQLFetch(hstmt);
	SQLGetData(hstmt, 2, SQL_C_WCHAR, (wchar_t*)buffer, sizeof(buffer), &len);
	buffer[2047] = L'\0';
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	return wstring(buffer);
}
inline wstring get_category(const wstring& input)
{
	wregex rx1(L"порт");
	wregex rx2(L"стори");
	wregex rx3(L"осмос");
	wregex rx4(L"(\\d+(\\.\\d+)?\\s*[\\+\\-\\*\\^\\/]\\s*\\d+(\\.\\d+)?)");
	wsmatch sm;
	bool is_rx1 = regex_search(input, sm, rx1);
	bool is_rx2 = regex_search(input, sm, rx2);
	bool is_rx3 = regex_search(input, sm, rx3);
	bool is_rx4 = regex_search(input, sm, rx4);
	if ((is_rx1 | is_rx2 | is_rx3 | is_rx4) == 0)
		return wstring(L"");
	else if (is_rx1) return wstring(L"Sport");
	else if (is_rx2) return wstring(L"History");
	else if (is_rx3) return wstring(L"Space");
	else if (is_rx4) return wstring(L"arifm");
}
inline wstring get_data()
{
	auto start = std::chrono::system_clock::now();
	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - start;
	time_t end_time = std::chrono::system_clock::to_time_t(end);
	wstringstream stream;
	stream << ctime(&end_time);
	wstring res = L"Дата и время: ";
	res += stream.str();
	wstringstream str1;
	str1 << elapsed_seconds.count();
	res += L"\n";
	res += L"Истекшее время: ";
	res += str1.str();
	return res;
}
inline string to_utf8_codec(const wstring& input)
{
	int utf8_size = WideCharToMultiByte(CP_UTF8, 0, input.c_str(), input.length(), nullptr, 0, nullptr, nullptr);
	string utf8_str(utf8_size, '\0');
	WideCharToMultiByte(CP_UTF8, 0, input.c_str(), input.length(), &utf8_str[0], utf8_size, nullptr, nullptr);
	return utf8_str;
}
inline bool is_stop = false;
inline int loadingBar()
{
	std::system("color 0A");
	char a = 177, b = (int)'*';
	printf("\n\n\n\n");
	printf("\n\n\n\n\t\t\t\t\tLoading...\n\n");
	printf("\t\t\t\t\t");
	printf("\r");
	printf("\t\t\t\t\t");
	while(!is_stop) {
		printf("%c", b);
		Sleep(1000);
	}
	return 0;
}
inline void init_python_and_module(const wchar_t* home, const wchar_t* exec)
{
	thread th1(loadingBar);
	th1.detach();
	PyConfig conf;
	PyConfig_InitIsolatedConfig(&conf);
	PyConfig_SetString(&conf, &conf.home, home);
	PyConfig_SetString(&conf, &conf.executable, exec);
	Py_InitializeFromConfig(&conf);
	PyConfig_Clear(&conf);
	sent = import("spacy_sc").attr("sent_analyze");
	sent("test");
	proc = import("spacy_sc").attr("proc_text");
	wstring ans = L"Я";
	string to = to_utf8_codec(ans);
	string r = extract<string>(proc(to));
	is_stop = true;
}
inline void init_python_rasa(const wchar_t* home, const wchar_t* exec)
{

	thread th1(loadingBar);
	th1.detach();
	PyConfig conf;
	PyConfig_InitIsolatedConfig(&conf);
	PyConfig_SetString(&conf, &conf.home, home);
	PyConfig_SetString(&conf, &conf.executable, exec);
	Py_InitializeFromConfig(&conf);
	PyConfig_Clear(&conf);
	trans = import("translate_script").attr("translate");
	wstring ans = L"Я";
	string to = to_utf8_codec(ans);
	trans(to);
	is_stop = true;
}
inline wstring get_wether(wstring& input)
{
	auto to_utf = to_utf8_codec(input);
	string city = extract<string>(trans(to_utf));
	boost::asio::io_context ioc;
	boost::asio::ip::tcp::resolver resolve(ioc);
	boost::asio::ssl::context ssl_ctx(boost::asio::ssl::context::tlsv12_client);
	ssl_ctx.set_default_verify_paths();
	boost::beast::ssl_stream<boost::beast::tcp_stream> stream(ioc, ssl_ctx);
	auto const results = resolve.resolve("api.openweathermap.org", "443");
	get_lowest_layer(stream).connect(results);
	stream.handshake(boost::asio::ssl::stream_base::client);
	string str_req = "https://api.openweathermap.org/data/2.5/weather?q=" + city + "&appid=" + wether_api;
	boost::beast::http::request<boost::beast::http::string_body> req{ boost::beast::http::verb::post, str_req, 11 };
	req.set(boost::beast::http::field::host, "api.openweathermap.org");
	req.set(boost::beast::http::field::content_type, "application/json");
	req.prepare_payload();
	write(stream, req);
	boost::beast::flat_buffer buffer;
	boost::beast::http::response<boost::beast::http::string_body> res;
	boost::beast::http::read(stream, buffer, res);
	nlohmann::json resp = nlohmann::json::parse(res.body());
	wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
	ostringstream str_stream;
	str_stream << resp["main"]["feels_like"];
	wstring wans = L"Температура (по ощущению): " + converter.from_bytes(str_stream.str()) + L"K";
	str_stream.str("");
	str_stream << resp["main"]["temp"];
	wans += L"\nТемпература (текущая): " + converter.from_bytes(str_stream.str()) + L"K";
	str_stream.str("");
	str_stream << resp["main"]["temp_max"];
	wans += L"\nТемпература (максимальная): " + converter.from_bytes(str_stream.str()) + L"K";
	str_stream.str("");
	str_stream << resp["main"]["temp_min"];
	wans += L"\nТемпература (минимальная): " + converter.from_bytes(str_stream.str()) + L"K";
	str_stream.str("");
	str_stream << resp["weather"];
	string cl = str_stream.str();
	str_stream.str("");
	cl.pop_back();
	cl.erase(cl.begin(), cl.begin() + 1);
	nlohmann::json cloud_js = nlohmann::json::parse(cl);
	str_stream << cloud_js["description"];
	wans += L"\nОблачность: " + converter.from_bytes(str_stream.str());
	str_stream.str("");
	str_stream << cloud_js["main"];
	wans += L", " + converter.from_bytes(str_stream.str());
	str_stream.str("");
	str_stream << resp["wind"]["speed"];
	wans += L"\nСкорость ветра: " + converter.from_bytes(str_stream.str()) + L"м / c";
	str_stream.str("");
	str_stream << resp["name"];
	wans += L"\nГород: " + converter.from_bytes(str_stream.str());
	return wans;
}
inline void log_dialog(const wstring& first_name, const wstring& user_name, const wstring& chat_id, const wstring& message)
{
	wstring sql_exist = L"SELECT COUNT(*) FROM Logging WHERE first_name = '" + first_name + L"'";
	HSTMT stmt1;
	SQLAllocStmt(hDbc, &stmt1);
	SQLPrepare(stmt1, (SQLWCHAR*)sql_exist.c_str(), SQL_NTS);
	SQLExecute(stmt1);
	wchar_t buffer[2048];
	SQLLEN len;
	SQLFetch(stmt1);
	SQLGetData(stmt1, 1, SQL_C_WCHAR, (wchar_t*)buffer, sizeof(buffer), &len);
	SQLFreeHandle(SQL_HANDLE_STMT, stmt1);
	buffer[2047] = L'\0';
	double count = wcstod(buffer, nullptr);
	HSTMT stmt2;
	SQLAllocStmt(hDbc, &stmt2);
	if (count == 1)
	{
		wstring sql_update = L"UPDATE Logging SET log_dialog = log_dialog + '&" + message + L"' WHERE first_name = '" + first_name + L"'";
		SQLPrepare(stmt2, (SQLWCHAR*)sql_update.c_str(), SQL_NTS);
		SQLExecute(stmt2);
		SQLFreeHandle(SQL_HANDLE_STMT, stmt2);
	}
	else
	{
		wstring sql_insert = L"INSERT INTO Logging (first_name, user_name, chat_id, log_dialog) VALUES ('" + first_name + L"', '" + user_name + L"', '" + chat_id + L"', '" + message + L"')";
		SQLPrepare(stmt2, (SQLWCHAR*)sql_insert.c_str(), SQL_NTS);
		SQLExecute(stmt2);
		SQLFreeHandle(SQL_HANDLE_STMT, stmt2);
	}
}
inline DWORD WINAPI thread_log(LPVOID lpParam)
{
	vector<wstring> param = *(vector<wstring>*)lpParam;
	wstring message_user = L"Вы: " + param[3];
	wstring message_bot = L"Бот: " + param[4];
	log_dialog(param[0], param[1], param[2], message_user);
	log_dialog(param[0], param[1], param[2], message_bot);
}
inline vector<wstring> get_user_data(const wstring& user_name)
{
	wstring sql_req = L"SELECT * FROM Logging WHERE user_name = '" + user_name + L"'";
	HSTMT hstmt;
	SQLAllocStmt(hDbc, &hstmt);
	SQLPrepare(hstmt, (SQLWCHAR*)sql_req.c_str(), SQL_NTS);
	SQLExecute(hstmt);
	wchar_t first_name_buf[256];
	wchar_t chat_id_buf[256];
	wchar_t dialog_buf[98304];
	SQLLEN len;
	SQLFetch(hstmt);
	SQLGetData(hstmt, 1, SQL_C_WCHAR, (wchar_t*)first_name_buf, sizeof(first_name_buf), &len);
	first_name_buf[len] = L'\0';
	len = 0;
	SQLGetData(hstmt, 3, SQL_C_WCHAR, (wchar_t*)chat_id_buf, sizeof(chat_id_buf), &len);
	chat_id_buf[len] = L'\0';
	len = 0;
	SQLGetData(hstmt, 4, SQL_C_WCHAR, (wchar_t*)dialog_buf, sizeof(dialog_buf), &len);
	dialog_buf[len] = L'\0';
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	wstring first_name(first_name_buf);
	wstring chat_id(chat_id_buf);
	wstring dialog(dialog_buf);
	vector<wstring> res{ first_name, user_name, chat_id, dialog };
	return res;
}
inline void print_user_data(vector<wstring>& data)
{
	wprintf(L"Первое имя пользователя: ");
	wprintf(data[0].c_str());
	wprintf(L"\n");
	wprintf(L"Второе имя пользователя: ");
	wprintf(data[1].c_str());
	wprintf(L"\n");
	wprintf(L"id чата: ");
	wprintf(data[2].c_str());
	wprintf(L"\n");
	wprintf(L"История диалога:\n");
	wstring::iterator cur = data[3].begin();
	while (*cur != L'\0')
	{
		wstring cur_str;
		while (*cur != L'&' && *(cur + 1) != L'\0') cur_str.push_back(*cur++);
		cur_str += L'\n';
		wprintf(cur_str.c_str());
		if (*cur != L'\0') cur++;
	}
}