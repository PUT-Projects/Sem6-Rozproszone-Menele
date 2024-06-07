#ifndef UTILH
#define UTILH
#include "main.hpp"
#include "globals.hpp"
#include <vector>

/* packet_t ma trzy pola, więc NITEMS=3. Wykorzystane w initialize_packet_type */
#define NITEMS 3

/* Typy wiadomości */
/* TYPY PAKIETÓW */
#define ACK 1
#define REQUEST 2
#define RELEASE 3
#define APP_PKT 4
#define FINISH 5

namespace app
{
    struct packet_t
    {
        int ts = 0; /* timestamp (zegar lamporta */
        int src = 0;

        int data = 0; /* przykładowe pole z danymi; można zmienić nazwę na bardziej pasującą */
    };

    extern MPI_Datatype MPI_PAKIET_T;
    void initialize_packet_type();

    /* wysyłanie pakietu, skrót: wskaźnik do pakietu (0 oznacza stwórz pusty pakiet), do kogo, z jakim typem */
    void send_packet(packet_t* pkt, int destination, int tag);

    enum state_t
    {
        InRun,
        InMonitor,
        InWant,
        InSection,
        InFinish
    };

    inline state_t app_state = InRun;

    /* zmiana stanu, obwarowana muteksem */
    void change_state(state_t);

    void update_lamport(const packet_t& pkt);
    void increment_lamport();

    void add_group(std::vector<proccess_t>& group);
    [[nodiscard]] bool release_from_group(proccess_t proccess_id);

    void add_awaiting_release(proccess_t proccess_id);
    void try_release_the_awaiting();

    std::vector<proccess_t> get_my_group();

    int get_guides_count();
    void log_queue();

}
#endif