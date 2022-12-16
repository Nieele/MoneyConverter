#include "Currency.hpp"

namespace MoneyConv {

	const std::string Converter::typesCurrencies = "AED|AFN|ALL|AMD|ANG|AOA|ARS|AUD|AWG|AZN|BAM|BBD|BDT|BGN|BHD|BIF|BMD|BND|BOB|BRL|BSD|BTC|BTN|BWP|BYN|BZD|CAD|CDF|CHF|CLF|CLP|CNH|CNY|COP|CRC|CUC|CUP|CVE|CZK|DJF|DKK|DOP|DZD|EGP|ERN|ETB|EUR|FJD|FKP|GBP|GEL|GGP|GHS|GIP|GMD|GNF|GTQ|GYD|HKD|HNL|HRK|HTG|HUF|IDR|ILS|IMP|INR|IQD|IRR|ISK|JEP|JMD|JOD|JPY|KES|KGS|KHR|KMF|KPW|KRW|KWD|KYD|KZT|LAK|LBP|LKR|LRD|LSL|LYD|MAD|MDL|MGA|MKD|MMK|MNT|MOP|MRU|MUR|MVR|MWK|MXN|MYR|MZN|NAD|NGN|NIO|NOK|NPR|NZD|OMR|PAB|PEN|PGK|PHP|PKR|PLN|PYG|QAR|RON|RSD|RUB|RWF|SAR|SBD|SCR|SDG|SEK|SGD|SHP|SLL|SOS|SRD|SSP|STD|STN|SVC|SYP|SZL|THB|TJS|TMT|TND|TOP|TRY|TTD|TWD|TZS|UAH|UGX|USD|UYU|UZS|VES|VND|VUV|WST|XAF|XAG|XAU|XCD|XDR|XOF|XPD|XPF|XPT|YER|ZAR|ZMW|ZWL";
	const char* Converter::FILEPATH_typesCurrencies = "typesCurrencies.txt";
	bool Converter::isInitialize = false;

	std::string Converter::getTypesCurrencies() noexcept
	{
		std::ifstream inFile(FILEPATH_typesCurrencies);
		if (!inFile.is_open()) {
			LOG(ERROR) << "typesCurrencies.txt not found";
			return std::string();
		}
		return read_file(FILEPATH_typesCurrencies);
	}

	double Converter::getCoef(const char* from, const char* to)
	{
		return getCoef(std::string(from), std::string(to));
	}

	double Converter::getCoef(const std::string& from, const std::string& to)
	{
		if (!isInitialize) {
			LOG(ERROR) << "The json data is not initialized";
			throw std::exception("The json data is not initialized");
		}

		if (data["rates"][from] != "null" && data["rates"][to] != "null")
			return (double)data["rates"][from] / (double)data["rates"][to];

		if (data["rates"][from].is_null()) LOG(ERROR) << "Incorrect currency (" << from << ")";
		if (data["rates"][to].is_null()) LOG(ERROR) << "Incorrect currency (" << to << ")";
		throw std::exception("Incorrect currency");
	}

	void Converter::initialize()
	{
		std::ifstream inFile("ExchangeRate.json");
		if (!inFile.is_open()) {
			LOG(ERROR) << "[Converter::initialize] - file not found";
			throw std::exception("[Converter::initialize] - file not found");
		}
		else if (inFile.peek() == std::ifstream::traits_type::eof()) {
			LOG(ERROR) << "[Converter::initialize] - file is empty";
			throw std::exception("[Converter::initialize] - file is empty");
		}

		data = json::parse(inFile);
		inFile.close();
		isInitialize = true;
	}

	auto Converter::read_file(std::string_view path) noexcept -> std::string
	{
		constexpr auto read_size = std::size_t(4096);
		auto stream = std::ifstream(path.data());
		stream.exceptions(std::ios_base::badbit);

		auto out = std::string();
		auto buf = std::string(read_size, '\0');
		while (stream.read(&buf[0], read_size)) {
			out.append(buf, 0, stream.gcount());
		}
		out.append(buf, 0, stream.gcount());
		return out;
	}

	Currency::Currency(double amount, const char* name) : Currency(amount, std::string(name)) {}
	Currency::Currency(double amount, const std::string& name) : _amount(amount), _name(name)
	{
		if (!Converter::typesCurrencies.find(_name)) {
			LOG(ERROR) << "Not found currency with name: " << _name;
			_amount = 0.0;
			_name = "USD";
		}
	}

	Currency Currency::convertTo(const char* name) const
	{
		return convertTo(std::string(name));
	}

	Currency Currency::convertTo(const std::string& name) const
	{
		double coef = Converter::getCoef(_name, name);
		return Currency(_amount / coef, name);
	}

	Currency Currency::operator+(const Currency& other) const
	{
		if (_name == other._name) return Currency(_amount + other._amount, _name);
		return Currency(_amount + other.convertTo(_name)._amount, _name);
	}

	Currency Currency::operator-(const Currency& other) const
	{
		if (_name == other._name) return Currency(_amount - other._amount, _name);
		return Currency(_amount - other.convertTo(_name)._amount, _name);
	}

	void Currency::operator+=(const Currency& other)
	{
		if (_name == other._name) _amount += other._amount;
		else {
			_amount += other.convertTo(_name)._amount;
		}
	}

	void Currency::operator-=(const Currency& other)
	{
		if (_name == other._name) _amount -= other._amount;
		else {
			_amount -= other.convertTo(_name)._amount;
		}
	}

	std::ostream& operator<<(std::ostream& out, const Currency& curr)
	{
		out << curr._amount << " " << curr._name;
		return out;
	}
}