#include <iostream>
#include <string>
#include <format>
#include <vector>
#include <any>
#include <exception>

#include "socket.hpp"

#ifdef DEBUG_COMMUNICATION
using namespace Communication;

template<typename T>
void foo(Message<T> const& msg) 
try
{
    using Type = Message<T>::DataType;
    std::cout << std::format("Printing message in {}: {}", __func__, std::any_cast<Type>(msg._container)) << std::endl;
}
catch(std::exception& e)
{
    std::cout << std::format("Exception in {}: {}", __func__, e.what()) << std::endl;
}
#endif

int main()
{

#ifdef DEBUG_COMMUNICATION
    auto msg = create_str_message();
    foo(msg);
#endif

    std::cout << "Communication main.cpp. Hello :s" << std::endl;
    return 0;
    
}