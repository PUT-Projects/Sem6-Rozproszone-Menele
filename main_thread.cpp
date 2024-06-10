#include "main.hpp"
#include "utils.hpp"
#include "main_thread.hpp"
#include "request_t.hpp"
#include "globals.hpp"
#include <thread>
#include <chrono>
#include <queue>
#include <mutex>
#include <vector>
#include <numeric>

namespace
{
    std::vector<proccess_t> my_group{};

    bool good_position_in_queue(int max)
    {
        std::priority_queue<app::request_t> temp_q;
        {
            std::lock_guard<std::mutex> lock(app::globals::queue_mutex);
            temp_q = app::globals::request_queue;
        }

        for (int i = 0; i < max && !temp_q.empty(); ++i)
        {
            if (temp_q.top().process_id == app::globals::rank)
            {
                return true;
            }
            temp_q.pop();
        }

        return false;
    }

    int get_queue_size()
    {
        std::lock_guard<std::mutex> lock(app::globals::queue_mutex);
        return app::globals::request_queue.size();
    }

    std::string vector_to_string(const std::vector<proccess_t> &v)
    {
        std::string result = "{ ";
        for (const auto &el : v)
        {
            result += std::to_string(el) + ", ";
        }
        result += "}";
        return result;
    }

    void start_new_group()
    {
        std::vector<proccess_t> group{};

        std::lock_guard<std::mutex> lock(app::globals::queue_mutex);
        for (int i = 0; i < app::globals::group_size; ++i)
        {
            group.push_back(app::globals::request_queue.top().process_id);
            app::globals::request_queue.pop();
        }

        app::add_group(group);
    }

    enum class tour_result
    {
        success,
        hospital
    };

    tour_result critical_section()
    {
        auto group_str = vector_to_string(my_group);

        int val = std::accumulate(my_group.begin(), my_group.end(), 0) % 2;
        const char *place = val == 0 ? "Dębcu" : "Safari";

        println("Jestem (w sekcji krytycznej) z grupą: %s, na %s", group_str.c_str(), place);

        if (val == 0)
        {
            int hospital_chance = rand() % 100;
            if (hospital_chance < 40)
                return tour_result::hospital;
        }

        sleep(1);
        return tour_result::success;
    }

    void send_releases(const tour_result &result)
    {
        if (result != tour_result::hospital)
        {
            println("Wracamy chłopaki...  (opuszczam sekcję krytyczną)") debug("Zmieniam stan na wysyłanie");
        }
        else
        {
            println("AAAAAaaaa... Pobili mnie! To ci z Dębca!  (opuszczam sekcję krytyczną)") debug("Zmieniam stan na wysyłanie");
        }

        app::packet_t empty_pack{};
        std::lock_guard<std::mutex> lock(app::globals::send_mutex);
        for (int i = 0; i <= app::globals::size - 1; ++i)
        {
            app::send_packet(&empty_pack, i, RELEASE);
        }
    }

    bool verify_guides_and_queue_size()
    {
        std::scoped_lock lock{app::globals::queue_mutex, app::globals::guides_mutex};

        return app::globals::request_queue.size() >= app::globals::group_size && app::globals::guides_count > 0;
    }
}

namespace app
{
    void main_loop()
    {
        srandom(globals::rank);
        int perc;

        while (app_state != InFinish)
        {
            switch (app_state)
            {
            case InRun:
                perc = random() % 100;
                if (perc < 95)
                {
                    debug("Perc: %d", perc);
                    // println("Ubiegam się o sekcję krytyczną")debug("Zmieniam stan na wysyłanie");
                    auto pkt = new packet_t{};
                    globals::ack_count = 0;
                    std::lock_guard<std::mutex> lock(app::globals::send_mutex);
                    for (int i = 0; i <= globals::size - 1; ++i)
                    {
                        send_packet(pkt, i, REQUEST);
                    }
                    change_state(InWant);

                    delete pkt;
                }
                debug("Skończyłem myśleć");
                break;
            case InWant:
                println("Czekam na potwierdzenia");

                if (globals::ack_count != globals::size)
                    break;

                while (true)
                {
                    if (verify_guides_and_queue_size())
                    {
                       // log_queue();
                        bool am_i_in_group = good_position_in_queue(globals::group_size);

                        start_new_group();

                        if (am_i_in_group) // lecimy na wycieczke
                        {
                            my_group = get_my_group();
                            auto group_str = vector_to_string(my_group);
                            println("Ekipa gotowa! %s %s", group_str.c_str(), "Lecimy!");
                            std::this_thread::sleep_for(std::chrono::seconds(SEC_IN_STATE));
                            break;
                        }
                        else 
                        {
                        }
                      //   log_queue();
                    }
                            try_release_the_awaiting();
                }

                change_state(InSection);

                break;
            case InSection:

                tour_result result = critical_section();

                send_releases(result);

                if (result == tour_result::hospital)
                    std::this_thread::sleep_for(std::chrono::seconds(5));

                change_state(InRun);

                break;
            }
            sleep(SEC_IN_STATE);
        }
    }

}
