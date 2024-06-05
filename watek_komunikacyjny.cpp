#include "main.h"
#include "watek_komunikacyjny.h"
#include "util.h"

#define max(a,b) a > b ? a : b

/* wątek komunikacyjny; zajmuje się odbiorem i reakcją na komunikaty */
void *startKomWatek(void *ptr)
{
    MPI_Status status;
    int is_message = FALSE;
    packet_t pakiet;
    /* Obrazuje pętlę odbierającą pakiety o różnych typach */
    while ( stan!=InFinish ) {
	debug("czekam na recv");
        MPI_Recv( &pakiet, 1, MPI_PAKIET_T, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        //przy odbieraniu (wątek komunikacyjny) max( ts, lokalny zegar ) +1
        switch ( status.MPI_TAG ) {
	    case REQUEST: 
                debug("Ktoś coś prosi. A niech ma!")
		sendPacket( 0, status.MPI_SOURCE, ACK );
	    break;
	    case ACK: 
            debug("Dostałem ACK od %d, mam już %d", status.MPI_SOURCE, ackCount + 1);
            //mutex
            pthread_mutex_lock(&lamportMutex);
            lamportClock = max( pakiet.ts, lamportClock) +1;
            pthread_mutex_unlock(&lamportMutex);

            ++ackCount;

	    break;
        case RELEASE:
            debug("Dostałem RELEASE od %d", status.MPI_SOURCE);

	    default:
	    break;
        }
    }
    return nullptr;
}
