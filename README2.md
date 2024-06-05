# Algorytm:

Algorytm polega na zakolejkowaniu wszystkich turystów w kolejkę do zasobu i udzielenie go G turystom na przodzie kolejki dla każdego przewodnika.

1. Rozsyłamy REQ o Przewodnika do wszystkich Turystów 
2. Czekamy na ACK od wszystkich Turystów
3. Jeśli ACK od wszystkich to puszczamy G turystów z kolejki do przewodnika

4. Turysta dla którego nie ma przewodnika czeka na REL od G pierwszych procesów
5. Po zakończeniu wycieczki zwracamy REL, losujemy pobicie i czekamy czas $X$. W tym czasie zwracamy ACK i info o szpitalu.