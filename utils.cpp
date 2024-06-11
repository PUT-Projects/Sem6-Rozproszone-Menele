#include "main.hpp"
#include "utils.hpp"
#include <mpi.h>
#include <vector>
#include <algorithm>
#include <mutex>

namespace
{
    std::mutex state_mutex{};

    struct tagNames_t
    {
        const char *name;
        int tag;
    } tagNames[] = {{"pakiet aplikacyjny", APP_PKT}, {"finish", FINISH}, {"potwierdzenie", ACK}, {"prośbę o sekcję krytyczną", REQUEST}, {"zwolnienie sekcji krytycznej", RELEASE}};

    const char* tag2string(int tag)
    {
        for (int i = 0; i < (int)(sizeof(tagNames) / sizeof(struct tagNames_t)); ++i)
        {
            if (tagNames[i].tag == tag)
                return tagNames[i].name;
        }
        return "<unknown>";
    }
}

namespace app
{

    MPI_Datatype MPI_PAKIET_T;

    /* tworzy typ MPI_PAKIET_T
     */
    void initialize_packet_type()
    {
        int blocklengths[NITEMS] = {1, 1, 1};
        MPI_Datatype typy[NITEMS] = {MPI_INT, MPI_INT, MPI_INT};

        MPI_Aint offsets[NITEMS];
        offsets[0] = offsetof(packet_t, ts);
        offsets[1] = offsetof(packet_t, src);
        offsets[2] = offsetof(packet_t, data);

        MPI_Type_create_struct(NITEMS, blocklengths, offsets, typy, &MPI_PAKIET_T);

        MPI_Type_commit(&MPI_PAKIET_T);
    }

    /* opis patrz util.h */
    void send_packet(packet_t *pkt, int destination, int tag)
    {
        bool freepkt = false;
        if (pkt == nullptr)
        {
            pkt = new packet_t;
            freepkt = true;
        }

        pkt->src = globals::rank;
        pkt->ts = ++globals::lamport_clock;
        MPI_Send(pkt, 1, MPI_PAKIET_T, destination, tag, MPI_COMM_WORLD);
        debug("Wysyłam %s do %d\n", tag2string(tag), destination);

        if (freepkt)
        {
            delete pkt;
        }
    }

    void change_state(state_t newState)
    {
        std::lock_guard<std::mutex> lock(state_mutex);
        if (app_state == InFinish)
            return;

        app_state = newState;
    }

    void update_lamport(const packet_t &pkt)
    {
        std::lock_guard<std::mutex> lock(globals::lamport_mutex);

        globals::lamport_clock = std::max(pkt.ts, globals::lamport_clock) + 1;
    }

    void increment_lamport()
    {
        std::lock_guard<std::mutex> lock(globals::lamport_mutex);
        ++globals::lamport_clock;
    }

    void add_group(std::vector<proccess_t> &group)
    {
        std::lock_guard<std::mutex> lock(globals::guides_mutex);
        globals::groups.push_back(std::move(group));
        globals::guides_count--;
    }

    [[nodiscard]] bool release_from_group(proccess_t proccess_id)
    {
        std::lock_guard<std::mutex> lock(globals::guides_mutex);
        for (int i = 0; i < (int)globals::groups.size(); ++i)
        {
            auto it = std::find(globals::groups[i].begin(), globals::groups[i].end(), proccess_id);
            if (it != globals::groups[i].end())
            {
                globals::groups[i].erase(it);

                if (globals::groups[i].empty())
                {
                    globals::groups.erase(globals::groups.begin() + i);
                    globals::guides_count++;
                }

                return true;
            }
        }
        // releasy ktore przyszly wczesniej niz ten proces stwiedzil ze wycieczka ruszyla
        return false;
    }

    void add_awaiting_release(proccess_t proccess_id)
    {
        std::lock_guard<std::mutex> lock(globals::awaiting_releases_mutex);
        globals::awaiting_releases.push_back(proccess_id);
    }

    void try_release_the_awaiting()
    {
        std::lock_guard<std::mutex> lock(globals::awaiting_releases_mutex);
        for (int i = 0; i < (int)globals::awaiting_releases.size(); ++i)
        {
            bool success = release_from_group(globals::awaiting_releases[i]);
            if (success)
            {
                globals::awaiting_releases.erase(globals::awaiting_releases.begin() + i);
                --i;
            }
        }
    }

    int get_guides_count()
    {
        std::lock_guard<std::mutex> lock(globals::guides_mutex);
        return globals::guides_count;
    }
}

namespace
{
    std::string queue_to_string(std::priority_queue<app::request_t> q)
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
}

namespace app
{
    void log_queue()
    {
        std::lock_guard<std::mutex> lock(globals::queue_mutex);
        auto str = queue_to_string(globals::request_queue);
        println("queue: %s", str.c_str());
    }

    std::vector<proccess_t> get_my_group()
    {
        std::lock_guard<std::mutex> lock(globals::guides_mutex);

        for (int i = 0; i < (int)globals::groups.size(); ++i)
        {
            auto it = std::find(globals::groups[i].begin(), globals::groups[i].end(), globals::rank);
            if (it != globals::groups[i].end())
            {
                return globals::groups[i];
            }
        }

        return {};
    }
}