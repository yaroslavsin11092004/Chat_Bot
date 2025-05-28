#pragma once
#include <nlohmann/json.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast.hpp>
#include <string>
#include <Parser.h>
#include <sstream>
#include <codecvt>
using namespace std;
using namespace boost;
using namespace boost::beast;
using namespace nlohmann;
using namespace boost::asio;
using tcp = boost::asio::ip::tcp;
namespace http = boost::beast::http;
inline vector<string> prepare_resp(string& resp_body)
{
	resp_body.erase(resp_body.begin(), resp_body.begin() + 1);
	resp_body.pop_back();
	string::iterator cur = resp_body.begin();
	vector<string> res;
	while (*cur != '\0')
	{
		string cur_str;
		while (*cur != '}') cur_str.push_back(*cur++);
		cur_str.push_back(*cur++);
		res.push_back(cur_str);
		cur++;
	}
	return res;
}

inline wstring make_request_rasa(const std::wstring& text)
{
	try
	{
		io_context ioc;
		tcp::resolver resolver(ioc);
		tcp_stream stream(ioc);
		auto const results = resolver.resolve("127.0.0.1", "5005");
		stream.connect(results);
		nlohmann::json payload;
		payload["message"] = text.c_str();
		payload["sender"] = L"user";
		http::request<http::string_body> req{ http::verb::post, "http://localhost:5005/webhooks/rest/webhook", 11 };
		req.set(http::field::host, "127.0.0.1");
		req.set(http::field::content_type, "application/json");
		req.body() = payload.dump();
		req.prepare_payload();
		write(stream, req);
		flat_buffer buffer;
		http::response<http::string_body> res;
		http::read(stream, buffer, res);
		string result_str = res.body();
		auto prb = prepare_resp(res.body());
		ostringstream stream_s;
		wstring ans;
		wstring_convert<codecvt_utf8_utf16<wchar_t>>converter;
		for (auto& i : prb)
		{
			json ji = json::parse(i);
			stream_s << ji["text"];
			wstring wansi = converter.from_bytes(stream_s.str());
			wansi.pop_back();
			wansi.erase(wansi.begin(), wansi.begin() + 1);
			stream_s.str("");
			ans += wansi + L" ";
		}
		return ans;
	}
	catch (std::exception& e)
	{
		throw std::runtime_error(std::string("Request failed: ") + e.what());
	}
}

inline wstring answer_rasa(const wstring& input)
{
	wstring cat = get_category(input);
	if (cat == L"")
	{
		wstring rasa_ans = make_request_rasa(input);
		if (rasa_ans[0] == L'f')
		{
			rasa_ans.erase(rasa_ans.begin(), rasa_ans.begin() + 1);
			return get_wether(rasa_ans);
		}
		if (rasa_ans[0] == L't')
			return get_data();
		return rasa_ans;
	}
	else if (cat == L"arifm")
	{
		wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
		string arifm = converter.to_bytes(input);
		Parser parser(arifm.c_str());
		try
		{
			parser.parse();
			wstringstream stream;
			stream << parser.eval(valarray<double>(14));
			wstring answer = L"Ответ: ";
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
		return get_data_from_db(cat, key);
	}
}