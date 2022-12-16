#include <iostream>

#include "UpdaterBase.hpp"
#include "Currency.hpp"

using MoneyConv::UpdaterBase;
using MoneyConv::Converter;
using MoneyConv::Currency;

int main(int argc, char** argv) 
{
    UpdaterBase::checkRelevanceDatabase();
    Converter::initialize();
    Currency rubBalance(100, "RUB");
    std::cout << rubBalance << std::endl << rubBalance.convertTo("USD");

    return 0;
}