#include "main.h"
#include "watek_glowny.h"

bool good_position_in_queue(int max) {
    std::priority_queue<Request> temp_q;
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        temp_q = requests;
    }

    for (int i = 0; i < max && !temp_q.empty(); i++) {
        if (temp_q.top().process_id == rank) {
            return true;
        }
        temp_q.pop();
    }

    return false;
}

void mainLoop() {
    srandom(rank);
    int tag;
    int perc;

    while (stan != InFinish) {
        switch (stan) {
            case InRun:
                perc = random() % 100;
                if (perc < 25) { debug("Perc: %d", perc);
                    println("Ubiegam się o sekcję krytyczną")debug("Zmieniam stan na wysyłanie");
                    packet_t *pkt = static_cast<packet_t *>(malloc(sizeof(packet_t)));
                    pkt->data = perc;
                    ackCount = 0;
                    for (int i = 0; i <= size - 1; i++) {
                        //if (i != rank)
                            sendPacket(pkt, i, REQUEST);
                    }
                    changeState(InWant); 

                    free(pkt);
                }
                debug("Skończyłem myśleć");
                break;
            case InWant:
                println("Czekam na wejście do sekcji krytycznej")

                if (ackCount == size && good_position_in_queue(2)) {
                    changeState(InSection);
                }
                break;
            case InSection:
                // tutaj zapewne jakiś muteks albo zmienna warunkowa
                debug("size: %d, ackCount: %d", size, ackCount);
                println("Jestem w sekcji krytycznej")
                sleep(1);
                //if ( perc < 25 ) {
                debug("Perc: %d", perc);
                println("Wychodzę z sekcji krytycznej")debug("Zmieniam stan na wysyłanie");
                packet_t *pkt = static_cast<packet_t *>(malloc(sizeof(packet_t)));
                pkt->data = perc;
                for (int i = 0; i <= size - 1; i++)
                    if (i != rank)
                        sendPacket(pkt, (i + 1) % size, RELEASE);
                changeState(InRun);
                free(pkt);
                //}
                break;

        }
        sleep(SEC_IN_STATE);
    }
}
