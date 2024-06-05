# Algorytm:

Algorytm polega na zakolejkowaniu wszystkich turystów w kolejkę do zasobu i udzielenie go G turystom na przodzie kolejki dla każdego przewodnika.

1. Rozsyłamy REQ o Przewodnika do wszystkich Turystów 
2. Czekamy na ACK od wszystkich Turystów
3. Jeśli ACK od wszystkich to puszczamy G turystów z kolejki do przewodnika

4. Turysta dla którego nie ma przewodnika czeka na REL od G pierwszych procesów
5. Po zakończeniu wycieczki zwracamy REL, losujemy pobicie i czekamy czas $X$. W tym czasie zwracamy ACK i info o szpitalu.

# Sprawozdanie
Sprawozdanie musi zawierać opis problemu oraz algorytmu w postaci słownej oraz w
postaci pseudokodu (najlepiej jako maszyna stanów). Algorytm powinien pokazywać
możliwe stany procesów, wiadomości wysyłane w każdym stanie, reakcje na odbiór
wiadomości danych typów w zależności od stanu procesów, oraz warunki przejścia między
stanami. Sprawozdanie musi również zawierać opis złożoności czasowej oraz
komunikacyjnej.
Opis podany w sprawozdaniu powinien być kompletny, szczegółowy i nie powinien wymagać
dodatkowych wyjaśnień podczas obrony projektu. Należy przyjąć, że dowolna osoba
powinna być w stanie zaimplementować algorytm na podstawie jego opisu w sprawozdaniu.
Chociaż zmniejsza to czytelność, dla wygody odnoszenia się do algorytmu podczas dyskusji
proszę o numerowanie linii. Poniżej przykład niedoskonały (bo googledocs numeruje linie dla
całego dokumentu, więc zamiast tego włączyłem listę numerowaną)
Przykład:
Algorytm Ricarta-Agrawali
Struktury i zmienne:
a. WaitQueue - Kolejka procesów oczekujących na ACK,
początkowo pusta
b. AckNum - liczba otrzymanych potwierdzeń ACK,
początkowo 0
c. n - liczba procesów
Wiadomości:
2. Wszystkie wiadomości są podbite znacznikiem czasowym
(timestampem), modyfikowanym zgodnie z zasadami skalarnego
zegara logicznego Lamporta.
a. REQ - żądanie o dostęp do sekcji krytycznej. Zawiera
priorytet żądania
b. ACK - potwierdzenie dostępu do sekcji krytycznej
Stany:
3. Początkowym stanem procesu jest REST
a. REST - nie ubiega się o dostęp.
b. WAIT - czeka na dostęp do sekcji krytycznej
c. INSECTION - w sekcji krytycznej
ciąg dalszy na następnej stronie
Szkic algorytmu:
4. Proces i ubiegający się o wejście do sekcji krytycznej wysyła
do wszystkich pozostałych prośby REQ o dostęp. Pozostałe
procesy odsyłają ACK do procesu i, o ile same się nie
ubiegają o dostęp albo jeżeli priorytet ich żądania jest
mniejszy od priorytetu procesu i. W przeciwnym wypadku
zapamiętują REQ w kolejce WaitQueue i odsyłają ACK po wyjściu
z sekcji krytycznej. Proces wchodzi do sekcji po zebraniu
ACKów od wszystkich.
5. Uwaga: Im większy zegar Lamporta, tym mniejszy priorytet.
Opis szczegółowy algorytmu dla procesu i:
6. REST: stan początkowy.
7. Proces i przebywa w stanie REST do czasu, aż podejmie decyzję
o ubieganie się o sekcję krytyczną. Ze stanu REST następuje
przejście do stanu WAIT po uprzednim wysłaniu wiadomości REQ
do wszystkich innych procesów oraz ustawieniu AckCounter na
zero. Wszystkie wiadomości REQ są opisane tym samym
priorytetem, równym zegarowi Lamporta w chwili wysłania
pierwszej wiadomości REQ
8. Reakcje na wiadomości:
a. REQ: odsyła ACK
b. ACK: ignoruje (sytuacja niemożliwa)
9. WAIT: ubieganie się o sekcję krytyczną.
10. Ze stanu WAIT następuje przejście do stanu INSECTION pod
warunkiem, że proces otrzyma ACK od wszystkich innych
procesów (AckCounter == n - 1).
a. REQ: od procesu j: jeżeli priorytet zawarty w REQ jest
większy od priorytetu i (pamiętamy: większe wartości
oznaczają mniejszy priorytet), odsyła ACK. W przeciwnym
wypadku REQ zapamiętywany jest w kolejce WaitQueue
b. ACK: zwiększa licznik otrzymanych ACK (AckCounter++).
Tak, jak opisano to wyżej, gdy otrzymano ACK od
wszystkich pozostałych procesów, proces i przechodzi do
stanu INSECTION
11. INSECTION: przebywanie w sekcji krytycznej.
12. Proces przebywa w sekcji krytycznej do czasu podjęcia
decyzji o jej opuszczeniu. Po podjęciu decyzji o opuszczeniu
sekcji, proces wysyła ACK w reakcji na wszystkie REQ
znajdujące się w WaitQueue, a następnie przechodzi do stanu
REST
a. REQ: dodaje żądanie do kolejki WaitQueue
b. ACK: Niemożliwe. Ignorowane.
-- Koniec --