#pragma once

namespace async_msg
{
    class server{
        public:
        explicit server(int port);

        int run();

       int port()const;
        private:
        int port_;
    };
}//namespace async_msg
