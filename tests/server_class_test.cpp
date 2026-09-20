#include "async_msg/server.hpp"
#include<cassert>

int main()
{
    async_msg::server server(9090);

    assert(server.port() == 9090);

    return server.run();
}
