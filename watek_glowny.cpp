#include "main.h"
#include "watek_glowny.h"
#include "request_t.hpp"
#include "globals.hpp"
#include <thread>
#include <chrono>
#include <queue>
#include <mutex>

bool good_position_in_queue(int max)
{
    std::priority_queue<request_t> temp_q;
    {
        std::lock_guard<std::mutex> lock(globals::queue_mutex);
        temp_q = globals::request_queue;
    }

    for (int i = 0; i < max && !temp_q.empty(); ++i)
    {
        if (temp_q.top().process_id == globals::rank)
        {
            return true;
        }
        temp_q.pop();
    }

    return false;
}

int get_queue_size()
{
    std::lock_guard<std::mutex> lock(globals::queue_mutex);
    return globals::request_queue.size();
}

void mainLoop()
{
    srandom(globals::rank);
    int tag;
    int perc;

    while (stan != InFinish)
    {
        switch (stan)
        {
        case InRun:
            perc = random() % 100;
            if (perc < 95)
            {
                debug("Perc: %d", perc);
                // println("Ubiegam się o sekcję krytyczną")debug("Zmieniam stan na wysyłanie");
                auto pkt = new packet_t{.data = perc};
                globals::ack_count = 0;
                for (int i = 0; i <= globals::size - 1; ++i)
                {
                    // if (i != globals::rank)
                    send_packet(pkt, (globals::rank + i) % globals::size, REQUEST);
                }
                change_state(InWant);

                delete pkt;
            }
            debug("Skończyłem myśleć");
            break;
        case InWant:
            println("Czekam na potwierdzenia");

            if (globals::ack_count == globals::size && good_position_in_queue(globals::group_size))
            {   
                println("Czekam na pozostalych uczestników wycieczki...");
                while (get_queue_size() < globals::group_size) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(200));
                }
                println("Wszyscy są gotowi, wchodzę do sekcji krytycznej!");
                change_state(InSection);
            }
            break;
        case InSection:
            // tutaj zapewne jakiś muteks albo zmienna warunkowa
            debug("size: %d, ack_count: %d", globals::size, globals::ack_count);
            println("Jestem w sekcji krytycznej")
                sleep(1);
            // if ( perc < 25 ) {
            debug("Perc: %d", perc);
            println("Wychodzę z sekcji krytycznej") debug("Zmieniam stan na wysyłanie");
            auto pkt = new packet_t{.data = perc};

            for (int i = 0; i <= globals::size - 1; ++i)
            {
                send_packet(pkt, (globals::rank + i) % globals::size, RELEASE);
            }

            change_state(InRun);
            delete pkt;
            //}
            break;
        }
        sleep(SEC_IN_STATE);
    }
}
