#pragma once

#include <nlohmann/json.hpp>
#include "easylogging++.h"
#include <iostream>
#include <string>

namespace MoneyConv {

	using json = nlohmann::json;

	class Converter {
	public:
		Converter() = delete;
		Converter(const Converter&) = delete;
		Converter& operator=(const Converter&) = delete;

		static const std::string typesCurrencies; // Только типы
		static std::string getTypesCurrencies() noexcept; // Получение данных из файла (типы и расшифровки)

		static double getCoef(const char* from, const char* to);
		static double getCoef(const std::string& from, const std::string& to);

		static void initialize();

	private:
		static auto read_file(std::string_view path) noexcept -> std::string;

		static const char* FILEPATH_typesCurrencies;
		inline static json data;
		static bool isInitialize;
	};

	class Currency
	{
	public:
		Currency() = delete;
		~Currency() = default;

		Currency(const Currency&) = default;
		Currency(Currency&&) = default;
		Currency& operator=(const Currency&) = default;

		Currency(double amount, const char* name);
		Currency(double amount, const std::string& name);

		friend std::ostream& operator<<(std::ostream& out, const Currency& curr);

		Currency convertTo(const char* name) const;
		Currency convertTo(const std::string& name) const;

		Currency operator+(const Currency& other) const;
		Currency operator-(const Currency& other) const;
		void operator+=(const Currency& other);
		void operator-=(const Currency& other);

	private:
		double _amount;
		std::string _name;
	};
}
