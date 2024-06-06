#include "main.h"
#include "watek_komunikacyjny.h"
#include "util.h"
#include <queue>

std::string queue_to_string(std::priority_queue<request_t> q)
{
    std::string result = "{ ";
    while (!q.empty())
    {
        result += "(" + std::to_string(q.top().timestamp) + " " + std::to_string(q.top().process_id) + "), ";
        q.pop();
    }
    result += "}";
    return result;
}

void log_queue(const std::priority_queue<request_t> &q)
{
    std::lock_guard<std::mutex> lock(globals::queue_mutex);
    auto str = queue_to_string(q);
    //println("queue: %s", str.c_str());
}

void handle_request(const packet_t &packet, int source)
{
    debug("Ktoś coś prosi. A niech ma!")

    {
        std::lock_guard<std::mutex> lock(globals::queue_mutex);
        globals::request_queue.push({packet.ts, source});
    }

    update_lamport(packet);

    send_packet(0, source, ACK);
}

void handle_ack(const packet_t &packet, int source)
{
    debug("Dostałem ACK od %d, mam już %d", source, globals::ack_count + 1);

    update_lamport(packet);
    ++globals::ack_count;
}

void handle_release(const packet_t &packet, int source)
{
    debug("Dostałem RELEASE od %d", source);

    update_lamport(packet);

    std::lock_guard<std::mutex> lock(globals::queue_mutex);

    std::priority_queue<request_t> temp_q;
    while (!globals::request_queue.empty())
    {
        if (globals::request_queue.top().process_id != source)
        {
            temp_q.push(globals::request_queue.top());
        }
        globals::request_queue.pop();
    }

    globals::request_queue.swap(temp_q);
}

/* wątek komunikacyjny; zajmuje się odbiorem i reakcją na komunikaty */
void startKomWatek()
{
    MPI_Status status{};

    packet_t packet{};
    /* Obrazuje pętlę odbierającą pakiety o różnych typach */
    while (stan != InFinish)
    {
        debug("czekam na recv");
        MPI_Recv(&packet, 1, MPI_PAKIET_T, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        switch (status.MPI_TAG)
        {
        case REQUEST:
            handle_request(packet, status.MPI_SOURCE);
            break;

        case ACK:
            handle_ack(packet, status.MPI_SOURCE);
            break;

        case RELEASE:
            handle_release(packet, status.MPI_SOURCE);
            break;

        default:
            break;
        }

        log_queue(globals::request_queue);
    }
}
