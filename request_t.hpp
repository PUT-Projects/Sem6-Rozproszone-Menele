#ifndef REQUEST_T_HPP
#define REQUEST_T_HPP

namespace app
{
    struct request_t
    {
        int timestamp;
        int process_id;

        friend bool operator<(const request_t &a, const request_t &b)
        {
            return a.timestamp == b.timestamp ? a.process_id >= b.process_id : a.timestamp >= b.timestamp;
        }
    };
}
#endif