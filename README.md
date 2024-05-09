# Sem6-Rozproszone-Menele

Poszukujący mocnych wrażeń turyści pokochali nowy sposób spędzania wolnego czasu: safari w Poznaniu, nocą na Dębcu. Po założeniu koszulek "kochamy Legia Warszawa" turyści, pod opieką przewodnika, ruszają na Dębiec.

Turyści najpierw rezerwują przewodnika. Jest P nierozróżnialnych przewodników, każdy opiekuje się grupą turystów o rozmiarze G. Turystów musi być co najmniej 2*G.
Wycieczka rusza, gdy grupa osiąga rozmiar G.
W czasie wycieczki turysta może zostać pobity. W takim wypadku trafia do szpitala i przez pewien czas nie bierze udziału w wycieczce.
Zaimplementować procesy T turystów. T >> P

Wykorzystać Zegar Lamporta i algorytm Ricarta-Agrawali do synchronizacji procesów.

## 1
Turyści najpierw rezerwują przewodnika. Jest P nierozróżnialnych przewodników, każdy opiekuje się grupą turystów o rozmiarze G. Turystów musi być co najmniej 2*G.

Algorytm możeby opdzielić na 3 fazy:
1. Poczukiwanie przewodnika
2. Oczekiwanie na skompletowanie grupy
3. Wycieczka

### Założenia

$T$ - Zbiór turystów - $T_i$ i-ty turysta - Proces

$P$ - Zbiór przewodników - $P_j$ j-ty przewodnik - Zasób

$G$ - Rozmiar grupy turystów - Stała wartość

Liczba Turystów na przewodnika jest stała, to znaczy że każdy przewodnik ma przypisaną stałą i taką samą liczbę turystów którą musi skompletować do wycieczki.

Dodatkowo $|T| >= 2*G$, Musi być co najmniej 2 razy więcej turystów niż rozmiar grupy możliwej do rozpoczęcia wycieczki.

Turyści dokonują "samoorganizacji w grupy" - sami wybierają przewodnika, z którym chcą iść na wycieczkę. Przewodnik nie ma wpływu na to, z kim pójdzie na wycieczkę.

Preferowany powinien być przewodnik z największą liczbą turystów, ponieważ wtedy wycieczka ruszy szybciej. 
Wybór w inny sposób może powodować, że wycieczka nie ruszy, bo nie uzbiera się odpowiednia liczba turystów.
N.p.:
Mamy 6 turystów i 3 przewodników.
W takim przypadku maksymalny rozmiar grupy $G$ to 3.

Przypisujemy ich do zasobów w następujący sposób:
- $T_1$ - $P_1$
- $T_2$ - $P_2$
- $T_3$ - $P_3$
- $T_4$ - $P_1$
- $T_5$ - $P_2$
- $T_6$ - $P_3$

W takim przypadku żaden przewodnik nie ruszy na wycieczkę, bo żaden nie ma przypisanej grupy turystów o rozmiarze $G$ (3). A nie ma więcej turystów bez przypisanego przewodnika.

W sytuacji, gdy kilku przewodników ma przypisaną tą samą liczbę turystów, to turysta wybiera przewodnika o najmniejszym numerze.

Po przypisaniu się do przewodnika, turysta czeka na skompletowanie grupy. (Można wprowadzić 'timeout' po którym turysta zwalnia zasób my jednak przyjmujemy że zasób jest blokowany do czasu skompletowania wycieczki)

Podczas wycieczki turysta może zostać "pobity" i trafić do szpitala. W takim wypadku przestaje pytać o przewodnika przez pewien $t$ czas. Może jednak w tym czasie odpowiadać na "smsy" od innych turystów. I udzielać informacji czy jest przypisany do przewodnika.

### Przypisywanie Turyści-Przewodnicy

Przypisanie turystów do przewodników zachowaniem powyższych zasad.

Algorytm dzieli się na x fazy:
- Faza 1 - Turysta sprawdza która grupa przewodników ma ile przypisanych turystów; 
- Faza 2 - Turysta wybiera przewodnika z największą liczbą turystów nie większą niż $G$; 
- Faza 3 - Turysta wysyła zapytanie do wszystkich turystów czy może dołączyć do przewodnika - zasobu $P_j$. Zapytanie ma priorytet $p_i = time()$ równy czasowi zegara;
- Faza 4 - Turysta czeka na odpowiedź od wszystkich turystów, jeśli odpowiedź jest pozytywna to dołącza do grupy przewodnika $P_j$;
- Faza 5 - Turysta czeka na skompletowanie grupy $G$;
  
### Algorytm
Pseudokod dla procesu turysty $T_i$:
```python
global time = 0 #Zegar Lamporta
global T # zbiór turystów
global P # zbiór przewodników (pliki)
global G # rozmiar grupy

global ack_queue #Kolejka zapytań

#Pobranie rozmiarów grup przewodników
guides = new Array(size(P))
for i in 1 to size(P)
    guides[i] = get_size_of_group(P[i])
#Wybór przewodnika
p = get_guide_with_max_tourists(guides)


#Zapytanie do turystów o dołączenie do grupy przewodnika
response_counter = 0
for i in 1 to size(T)
    send(REQ,T[i], P[p], time)
#Oczekiwanie na odpowiedzi
while response_counter < size(T)
    msg = recv()
    time = max(time, msg.time) + 1
    if msg.type == ACK
        response_counter += 1
    if msg.type == REQ
        if msg.time < time
            send(ACK, msg.sender, time)
        else
            ack_queue.push(msg)




```
## 2

Wycieczka rusza, gdy grupa osiąga rozmiar G.

## 3
W czasie wycieczki turysta może zostać pobity. W takim wypadku trafia do szpitala i przez pewien czas nie bierze udziału w wycieczce.