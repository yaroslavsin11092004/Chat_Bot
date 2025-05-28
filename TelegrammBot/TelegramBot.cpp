#include "TelegramBot.h"
#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast.hpp>
#include <sstream>
#include <codecvt>
#include <locale>
#pragma warning(disable:4996);
using namespace std;
using tcp = boost::asio::ip::tcp;
namespace http = boost::beast::http;
TelegramBot::TelegramBot(const std::string& token, const std::function<std::wstring(wstring)>& func_answer) :
	bot_token(token), api_url("https://api.telegram.org/bot" + token), func_answer(func_answer) {}
void TelegramBot::start()
{
	while (!is_exit)
		try
		{
			nlohmann::json payload; 
			if (!last_update_id.empty())
				payload["offset"] = std::stoi(last_update_id) + 1;
			auto response = make_requests("getUpdates", payload);
			handle_updates(response["result"]);
		}
		catch (const std::exception& e)
		{
			std::cerr << "Error: " << e.what() << std::endl;
		}
}
void TelegramBot::handle_updates(const nlohmann::json& updates)
{
	for (const auto& update : updates)
	{
		if (update.contains("update_id"))
			last_update_id = std::to_string(update["update_id"].get<int>());
		if (update.contains("message") && update["message"].contains("text"))
		{
			ostringstream stream;
			stream << update["message"]["chat"]["id"];
			std::string chat_id = stream.str();
			std::string text = update["message"]["text"].get<std::string>();
			wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
			wstring wstr = converter.from_bytes(text.c_str());
			std::wstring resp_text(func_answer(wstr).c_str());
			wstring first_name = converter.from_bytes(update["message"]["from"]["first_name"].get<string>());
			wstring user_name = converter.from_bytes(update["message"]["from"]["username"].get<std::string>());
			vector<wstring> args{ first_name, user_name, converter.from_bytes(chat_id), wstr, resp_text };
			CreateThread(0, 0, thread_log, &args, 0, 0);
			send_message(chat_id, resp_text);
		}
	}
}
void TelegramBot::send_message(const std::string& chat_id, const std::wstring& text)
{
	nlohmann::json payload;
	payload["chat_id"] = chat_id;
	payload["text"] = text.c_str();
	make_requests("sendMessage", payload);
}
nlohmann::json TelegramBot::make_requests(const std::string& method, const nlohmann::json& payload)
{
	try
	{
		boost::asio::io_context ioc;
		boost::asio::ssl::context ssl_ctx(boost::asio::ssl::context::tlsv12_client);
		ssl_ctx.set_default_verify_paths();
		tcp::resolver resolver(ioc);
		boost::beast::ssl_stream<boost::beast::tcp_stream> stream(ioc, ssl_ctx);
		auto const results = resolver.resolve("api.telegram.org", "443");
		boost::beast::get_lowest_layer(stream).connect(results);
		stream.handshake(boost::asio::ssl::stream_base::client);
		http::request<http::string_body> req{ http::verb::post, "/bot" + bot_token + "/" + method, 11 };
		req.set(http::field::host, "api.telegram.org");
		req.set(http::field::content_type, "application/json");
		req.body() = payload.dump();
		req.prepare_payload();
		http::write(stream, req);
		boost::beast::flat_buffer buffer;
		http::response<http::string_body> res;
		http::read(stream, buffer, res);
		boost::system::error_code ec;
		if (ec == boost::asio::ssl::error::stream_truncated)
			ec.assign(0, ec.category());
		else if (ec)
			throw boost::system::system_error(ec);
		return nlohmann::json::parse(res.body());
	}
	catch (std::exception& e)
	{
		throw std::runtime_error(std::string("Request failed: ") + e.what());
	}
}