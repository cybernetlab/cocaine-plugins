#include "cocaine/service/auth/directory.hpp"

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

using namespace cocaine::service::auth;

directory_t::directory_t(std::shared_ptr<logging::log_t> log,
                         std::shared_ptr<storage_t> storage,
                         const std::string & path):
    m_log(log),
    m_storage(storage),
    m_path(std::string(path))
{
    std::cout << "strating directory. path is: '" << m_path << "'\n";
    type_iterator iterator(this);
    boost::mpl::for_each<directory::types, wrap<boost::mpl::placeholders::_1> >(iterator);
    std::cout << "classes loaded\n";
    //m_permissions = m_factories["permissions"]->create("permissions");
}


resolve_result
directory_t::resolve(const std::string & name)
{
    std::cout << "roslving " << name << "\n";
    size_t at_pos = name.find('@');
    std::cout << "  @ pos: " << at_pos << "\n";
    if (at_pos == std::string::npos) {
        return resolve_result(std::string(name), *this);
    }
    std::string path_str = name.substr(at_pos + 1);
    std::cout << "  path: " << path_str << "\n";
    std::vector<std::string> path;
    boost::split(path, path_str, boost::is_any_of("."), boost::token_compress_on);
    std::string name_ = name.substr(0, at_pos);
    std::cout << "  name_: " << name_ << "\n";
    return resolve(path.begin(), path.end(), name_);
}

resolve_result
directory_t::resolve(string_iter begin, string_iter end, const std::string & name)
{
    if (m_children.find(*begin) == m_children.end()) {
        boost::filesystem::path path(m_path);
        path /= *begin;
        m_children[*begin] = std::make_shared<directory_t>(m_log, m_storage, path.string());
    }
    auto child = m_children[*begin];
    begin++;
    if (begin == end) {
        return resolve_result(std::string(name), *child);
    }
    return child->resolve(begin, end, name);
}
