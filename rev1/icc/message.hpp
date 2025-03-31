#include <any>

namespace Communication
{

using ContainerType = std::any;

template<typename T>
struct Message 
{
    using DataType = T;
    ContainerType _container;
};

} // end of Communication namespace