#include <stdint.h>
#include <string>
#include <map>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "cmdline.h"
#include "glog_client_impl.h"
#include "gsl.h"


using namespace std;
using namespace glog;

typedef int (*DispatchFunc)(cmdline::parser& parser);

class SimpleClient
{
public:
    SimpleClient(uint64_t svrid, const std::string& addr)
        : client_(svrid, grpc::CreateChannel(addr, grpc::InsecureCredentials()))
    {

    }

    int Propose(gsl::cstring_view<> data)
    {
        return client_.Propose(data);
    }


private:
    glog::GlogClientImpl client_;
};

int Propose(cmdline::parser& parser)
{
    auto svrid = parser.get<uint64_t>("svrid");
    auto addr = parser.get<string>("addr");
    auto data = parser.get<string>("data");

    SimpleClient client(svrid, addr);
    int ret = client.Propose({data.data(), data.size()});
    return ret;
}


std::map<std::string, DispatchFunc> create_dispatch()
{
    map<string, DispatchFunc> dispatch;
    
    dispatch["Propose"] = Propose;
    return dispatch;
}

    int
main ( int argc, char *argv[] )
{
    cmdline::parser parser;
    parser.add<string>("func", 'f', "function name");
    parser.add<uint64_t>("svrid", 'i', "svrid");
    parser.add<string>("addr", 'a', "ip:port");
    parser.add<string>("data", 'd', "data");

    parser.parse_check(argc, argv);

    auto func_name = parser.get<string>("func");
    auto dispatch = create_dispatch();

    assert(nullptr != dispatch[func_name]);
    int ret = dispatch[func_name](parser);
    cout << func_name << " execute ret " << ret << endl;

    // TODO
    return EXIT_SUCCESS;
}				/* ----------  end of function main  ---------- */


