#pragma once
#ifndef TELEGRAMBOT_H
#define TELEGRAMBOT_H
#include <string>
#include <functional>
#include <nlohmann/json.hpp>
#include <boost/beast.hpp>
#include "Answer.h"
class TelegramBot
{
private:
	std::string api_url;
	std::string bot_token;
	std::string last_update_id;
	void handle_updates(const nlohmann::json& updates);
	void send_message(const std::string& chat_id, const std::wstring& text);
	nlohmann::json make_requests(const std::string& method, const nlohmann::json& pay_load = {});
	std::function<std::wstring(std::wstring)> func_answer;
	bool is_exit = false;
public:
	TelegramBot(const std::string& token, const std::function<std::wstring(std::wstring)>&func_answer);
	void start();
	void exit() { is_exit = true; }
};
#endif 