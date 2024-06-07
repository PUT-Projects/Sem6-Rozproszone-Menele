/* w main.h także makra println oraz debug -  z kolorkami! */
#include <thread>
#include "main.hpp"
#include "utils.hpp"
#include "main_thread.hpp"
#include "background_thread.hpp"
#include <map>
#include <string_view>

/*
 * W main.h extern int rank (zapowiedź) w main.c int rank (definicja)
 * Zwróćcie uwagę, że każdy proces ma osobą pamięć, ale w ramach jednego
 * procesu wątki współdzielą zmienne - więc dostęp do nich powinien
 * być obwarowany muteksami. Rank i size akurat są write-once, więc nie trzeba,
 * ale zob util.c oraz util.h - zmienną state_t state i funkcję change_state
 *
 */

/*
 * Każdy proces ma dwa wątki - główny i komunikacyjny
 * w plikach, odpowiednio, main_thread.c oraz (siurpryza) background_thread.c
 *
 *
 */
namespace
{
    void finalize(std::thread &comm_thread)
    {
        println("czekam na wątek \"komunikacyjny\"\n");
        comm_thread.join();
        MPI_Type_free(&app::MPI_PAKIET_T);
        MPI_Finalize();
    }

    void check_thread_support(int provided)
    {
        printf("THREAD SUPPORT: chcemy %d. Co otrzymamy?\n", provided);
        switch (provided)
        {
        case MPI_THREAD_SINGLE:
            printf("Brak wsparcia dla wątków, kończę\n");
            /* Nie ma co, trzeba wychodzić */
            fprintf(stderr, "Brak wystarczającego wsparcia dla wątków - wychodzę!\n");
            MPI_Finalize();
            exit(-1);
            break;
        case MPI_THREAD_FUNNELED:
            printf("tylko te wątki, ktore wykonaly mpi_init_thread mogą wykonać wołania do biblioteki mpi\n");
            break;
        case MPI_THREAD_SERIALIZED:
            /* Potrzebne zamki wokół wywołań biblioteki MPI */
            printf("tylko jeden watek naraz może wykonać wołania do biblioteki MPI\n");
            break;
        case MPI_THREAD_MULTIPLE:
            printf("Pełne wsparcie dla wątków\n"); /* tego chcemy. Wszystkie inne powodują problemy */
            break;
        default:
            printf("Nikt nic nie wie\n");
        }
    }

    bool is_number(std::string_view sv)
    {
        return !sv.empty() && sv.find_first_not_of("0123456789") == std::string::npos;
    }

    std::map<std::string_view, int> parse_args(int argc, char *argv[])
    {
        std::map<std::string_view, int> args = {
            {"-p", 1},
            {"-g", 2}
        };

        for (int i = 1; i < argc; i += 2)
        {
            if (is_number(argv[i + 1]))
            {
                args[argv[i]] = std::stoi(argv[i + 1]);
            }
            else
            {
                println("Argument %s nie jest liczbą", argv[i + 1]);
                exit(1);
            }
        }
        return args;
    }

    void print_configuration()
    {
        std::cout << "Konfiguracja:\n";
        std::cout << " - liczba przewodników: " << app::globals::guides_capacity << "\n";
        std::cout << " - rozmiar grupy: " << app::globals::group_size << "\n";
        std::cout << " - liczba procesów: " << app::globals::size << "\n";
    }

}

int main(int argc, char *argv[])
{
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    check_thread_support(provided);

    app::initialize_packet_type();
    MPI_Comm_size(MPI_COMM_WORLD, &app::globals::size);
    MPI_Comm_rank(MPI_COMM_WORLD, &app::globals::rank);
    srand(app::globals::rank);

    auto args = parse_args(argc, argv);

    app::globals::lamport_clock = 0; // app::globals::rank;
    app::globals::guides_capacity = args["-p"];
    app::globals::guides_count = app::globals::guides_capacity;

    app::globals::group_size = args["-g"];

    if (app::globals::rank == 0) print_configuration();

    std::thread comm_thread(app::background_thread);

    app::main_loop();

    finalize(comm_thread);
    return 0;
}
