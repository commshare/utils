#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <sstream>

using ResolveResult = boost::asio::ip::tcp::resolver::results_type;
using Endpoint = boost::asio::ip::tcp::endpoint;

class Request {
    private:
        boost::asio::ip::tcp::resolver resolver;
        boost::asio::ip::tcp::socket   socket;
        std::string                    request;
        std::string                    response;
        const std::string              hostname;
    public:
    explicit Request( boost::asio::io_context& io_ctx, std::string hostname )
        : resolver { io_ctx },
          socket   { io_ctx },
          hostname { std::move( hostname ) } {
        std::stringstream request_stream;
        request_stream << "GET / HTTP/1.1\r\n"
                          "Host: " << this->hostname << "\r\n"
                          "Accept: text/plain\n\r"
                          "Accept-Language: en-us\r\n"
                          "Accept-Encoding: identity\r\n"
                          "Connection: close"
                          "User-Agent: Asio HTTP Client";
        request = request_stream.str();
        resolver.async_resolve( this->hostname, "http",
                [ this ] ( boost::system::error_code ec, const ResolveResult& result ) {
                    resolution_handler(ec, result);
                } );

    }

    void resolution_handler( boost::system::error_code ec,
                             const ResolveResult& result ) {
        std::cout << "Resolution" << std::endl;
        if ( ec ) {
            std::cerr << "Error while resolving host \"" << hostname << "\": " << ec.message() << "\n";
            return;
        }
        boost::asio::async_connect( socket, result,
                [ this ] ( boost::system::error_code ec, const Endpoint& endpoint ) {
                    connection_handler(ec, endpoint);
                } );

    }

    void connection_handler( boost::system::error_code ec,
                             const Endpoint& endpoint ) {
        std::cout << "Connection" << std::endl;
        if ( ec ) {
            std::cerr << "Error while connecting to \"" << hostname << "\": " << ec.message() << "\n";
            return;
        }
        boost::asio::async_write( socket, boost::asio::buffer(request),
                [ this ] ( boost::system::error_code ec, size_t transferred ) {
                    write_handler(ec, transferred);
                } );

    }

    void write_handler( boost::system::error_code ec,
                        const size_t transferred ) {
        std::cout << "Write" << std::endl;
        if ( ec ) {
            std::cerr << "Error writing to \"" << hostname << "\": " << ec.message() << "\n";
            return;
        } else if ( request.size() != transferred ) {
            request.erase(0, transferred);
            boost::asio::async_write( socket, boost::asio::buffer(request),
                [ this ] ( boost::system::error_code ec, size_t transferred ) {
                    write_handler(ec, transferred);
                });
        } else {
            boost::asio::async_read( socket, boost::asio::dynamic_buffer(response),
                [ this ] ( boost::system::error_code ec, size_t transferred ) {
                    read_handler(ec, transferred);
                });
        }

    }

    void read_handler( boost::system::error_code ec,
                       const size_t transferred ) {
        std::cout << "Read" << std::endl;
        if (ec && ec.value() != 2) {
            std::cerr << "Error reading from host \"" << hostname << "\": " << ec.message() << "\n";
        }
    }

    const std::string& get_response() const noexcept {
        return response;
    }

};


int main() {
    boost::asio::io_context io_ctx;
    Request req { io_ctx, "www.example.com" };
    io_ctx.run();
    std::cout << req.get_response();
    return 0;
}
