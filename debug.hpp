#include <libserv/libserv.hpp>
#include <smart_ptr/smart_ptr.hpp>

#include <iomanip>
#include <sstream>

class byte_buffer_dump_logger_handler : public ft::serv::event_handler_base
{
private:
    static std::string _dump(const ft::serv::byte_buffer& buf)
    {
        std::ostringstream oss;
        const ft::serv::byte_t* arr = buf.get();
        for (ft::serv::byte_buffer::size_type i = ft::serv::byte_buffer::size_type(); i < buf.raw_length(); i++)
        {
            (i ? oss << " " : oss) << std::setfill('0') << std::setw(2) << std::hex << std::uppercase << static_cast<int>(arr[i]); //%02X
        }
        return oss.str();
    }

public:
    void on_read(ft::serv::event_layer& layer, ft::shared_ptr<void> arg)
    {
        ft::serv::logger::trace("[#%d, R] %s", layer.get_ident(), _dump(*ft::static_pointer_cast<ft::serv::byte_buffer>(arg)).c_str());
        layer.notify_read(arg);
    }

    void on_write(ft::serv::event_layer& layer, ft::shared_ptr<const void> arg)
    {
        ft::serv::logger::trace("[#%d, S] %s", layer.get_ident(), _dump(*ft::static_pointer_cast<const ft::serv::byte_buffer>(arg)).c_str());
        layer.post_write(arg);
    }
};
