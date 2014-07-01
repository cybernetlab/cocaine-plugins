#include "cocaine/service/auth/directory/base.hpp"
#include "cocaine/service/auth/directory.hpp"

using namespace cocaine::service::auth::directory;

object_t::object_t(const directory_t & directory, const Json::Value & value) :
    Json::Value(value),
    m_directory(&directory)
{
    std::cout << "creating abstract object from " << value.toStyledString();
}

/*const Json::Value &
object_t::data()
{
    return m_data;
}*/

object_t::object_t(const object_t &other) :
    Json::Value(dynamic_cast<const Json::Value &>(other)),
    m_directory(other.m_directory)
{};

object_t &
object_t::operator=(const object_t &other)
{
    if (this == &other) return *this;
    m_directory = other.m_directory;
    Json::Value::operator=(dynamic_cast<const Json::Value &>(other));
    return *this;
};
