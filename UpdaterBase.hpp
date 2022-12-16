#pragma once

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <string>
#include <fstream>
#include <ctime>
#include "easylogging++.h"

INITIALIZE_EASYLOGGINGPP

namespace MoneyConv 
{
	using json = nlohmann::json;

	static const std::string API_SITE = "https://openexchangerates.org/api/latest.json?app_id=";
	static const std::string API_KEY = "api_key";
	extern const std::string FILEPATH_BASE = "ExchangeRate.json";
	static const std::string FILEPATH_ERROR_INFO = "error.json";

	class UpdaterBase 
	{
	public:
		UpdaterBase() = delete;
		UpdaterBase(const UpdaterBase&) = delete;
		UpdaterBase& operator=(const UpdaterBase&) = delete;

		static void checkRelevanceDatabase() noexcept;
		static void downloadExchangeRateJSON() noexcept;
	};

	inline void UpdaterBase::checkRelevanceDatabase() noexcept
	{
		std::ifstream baseFile(FILEPATH_BASE);
		if (!baseFile.is_open()) downloadExchangeRateJSON();
		else if (baseFile.peek() == std::ifstream::traits_type::eof()) downloadExchangeRateJSON();
		else {
			json data = json::parse(baseFile);
			if (std::time(nullptr) - data["timestamp"] > 43200) downloadExchangeRateJSON(); // ѕрошло 43200 секунд (12 часов) с последнего получени€ данных
		}
	}

	inline void UpdaterBase::downloadExchangeRateJSON() noexcept
	{
		cpr::Response r = cpr::Get(cpr::Url{ (API_SITE + API_KEY) });
		if (r.status_code == 200) {
			std::ofstream outFile(FILEPATH_BASE);
			outFile << r.text;
			outFile.close();
		}
		else {
			LOG(ERROR) << "Error receiving data from the API server. status_code: " << r.status_code << ". For more information, open the file \"error.json\"";
			std::ofstream errorFile(FILEPATH_ERROR_INFO);
			errorFile << r.text;
			errorFile.close();
		}
	}
}