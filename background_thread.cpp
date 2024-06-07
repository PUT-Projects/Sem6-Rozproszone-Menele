#include "main.hpp"
#include "background_thread.hpp"
#include "utils.hpp"
#include <queue>

namespace
{
    void handle_request(const app::packet_t& packet, int source)
    {
        debug("Ktoś coś prosi. A niech ma!")

        {
            std::lock_guard<std::mutex> lock(app::globals::queue_mutex);
            app::globals::request_queue.push({packet.ts, source});
        }

        app::update_lamport(packet);

        app::send_packet(0, source, ACK);
    }

    void handle_ack(const app::packet_t& packet, int source)
    {
        debug("Dostałem ACK od %d, mam już %d", source, app::globals::ack_count + 1);

        app::update_lamport(packet);
        ++app::globals::ack_count;
    }

    void handle_release(const app::packet_t& packet, int source)
    {
        debug("Dostałem RELEASE od %d", source);

        app::update_lamport(packet);

        if (!app::release_from_group(source))
        {
            app::add_awaiting_release(source);
        }
    }
}

namespace app
{
    /* wątek komunikacyjny; zajmuje się odbiorem i reakcją na komunikaty */
    void background_thread()
    {
        MPI_Status status{};

        packet_t packet{};
        /* Obrazuje pętlę odbierającą pakiety o różnych typach */
        while (app_state != InFinish)
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
        }
    }

}