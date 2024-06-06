#ifndef MAINH
#define MAINH
#include <mpi.h>
#include <cstdio>
#include <cstdlib>
#include <cstddef>
#include <unistd.h>
#include <cstring>
#include <pthread.h>
#include <mutex>
#include <queue>

#include "util.h"
/* boolean */
#define TRUE 1
#define FALSE 0
#define SEC_IN_STATE 1
#define STATE_CHANGE_PROB 10

#define ROOT 0

/* tutaj TYLKO zapowiedzi - definicje w main.c */
extern int rank;
extern int size;
extern int ackCount;
extern pthread_t threadKom;

extern int lamportClock;

struct Request
{
    int timestamp;
    int process_id;

    friend bool operator<(const Request &a, const Request &b) {
        return a.timestamp == b.timestamp ? a.process_id >= b.process_id : a.timestamp >= b.timestamp;
    }
};

inline std::mutex lamport_mutex{};
inline std::mutex queue_mutex{};
inline std::priority_queue<Request> requests{};

#ifndef DEBUG
//#define DEBUG
#endif



/* macro debug - działa jak printf, kiedy zdefiniowano
   DEBUG, kiedy DEBUG niezdefiniowane działa jak instrukcja pusta 
   
   używa się dokładnie jak printfa, tyle, że dodaje kolorków i automatycznie
   wyświetla rank

   w związku z tym, zmienna "rank" musi istnieć.

   w printfie: definicja znaku specjalnego "%c[%d;%dm [%d]" escape[styl bold/normal;kolor [RANK]
                                           FORMAT:argumenty doklejone z wywołania debug poprzez __VA_ARGS__
					   "%c[%d;%dm"       wyczyszczenie atrybutów    27,0,37
                                            UWAGA:
                                                27 == kod ascii escape. 
                                                Pierwsze %c[%d;%dm ( np 27[1;10m ) definiuje styl i kolor literek
                                                Drugie   %c[%d;%dm czyli 27[0;37m przywraca domyślne kolory i brak pogrubienia (bolda)
                                                ...  w definicji makra oznacza, że ma zmienną liczbę parametrów
                                            
*/
#ifdef DEBUG
#define debug(FORMAT, ...) printf("%c[%d;%dm [P:%d;L:%d]: " FORMAT "%c[%d;%dm\n",  27, (1+(rank/7))%2, 31+(6+rank)%7, rank, lamportClock, ##__VA_ARGS__, 27,0,37);
#else
#define debug(...);
#endif

// makro println - to samo co debug, ale wyświetla się zawsze
//#define println(FORMAT,...) printf("%c[%d;%dm [%d]: " FORMAT "%c[%d;%dm\n",  27, (1+(rank/7))%2, 31+(6+rank)%7, rank, ##__VA_ARGS__, 27,0,37);
#define println(FORMAT, ...) printf("%c[%d;%dm [P:%d;L:%d]: " FORMAT "%c[%d;%dm\n",  27, (1+(rank/7))%2, 31+(6+rank)%7, rank, lamportClock, ##__VA_ARGS__, 27,0,37);


#endif
