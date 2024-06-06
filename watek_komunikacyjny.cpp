#include "main.h"
#include "watek_komunikacyjny.h"
#include "util.h"
#include <queue>

std::string queue_to_string(std::priority_queue<Request> q)
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

void log_queue(const std::priority_queue<Request> &q)
{
    std::lock_guard<std::mutex> lock(queue_mutex);
    auto str = queue_to_string(q);
    debug("queue: %s", str.c_str());
}

void handle_request(const packet_t &packet, int source)
{
    debug("Ktoś coś prosi. A niech ma!")

    {
        std::lock_guard<std::mutex> lock(queue_mutex);
        requests.push({packet.ts, source});
    }

    // mutex
    update_lamport(packet);

    // wysyłamy ACK
    sendPacket(0, source, ACK);
}

void handle_ack(const packet_t &packet, int source)
{
    debug("Dostałem ACK od %d, mam już %d", source, ackCount + 1);
    // mutex
    update_lamport(packet);

    ++ackCount;
}

void handle_release(const packet_t &packet, int source)
{
    debug("Dostałem RELEASE od %d", source);

    // mutex
    update_lamport(packet);

    // usuwamy request z kolejki
    std::lock_guard<std::mutex> lock(queue_mutex);

    std::priority_queue<Request> temp_q;
    while (!requests.empty())
    {
        if (requests.top().process_id != source)
        {
            temp_q.push(requests.top());
        }
        requests.pop();
    }

    requests.swap(temp_q);
}

/* wątek komunikacyjny; zajmuje się odbiorem i reakcją na komunikaty */
void *startKomWatek(void *ptr)
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

        log_queue(requests);
    }

    return nullptr;
}
