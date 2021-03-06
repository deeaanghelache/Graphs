#include <iostream>
#include <fstream>
#include <stack>
#include <queue>
#include <vector>
#include <algorithm>
#include <limits>
#include <utility>

using namespace std;

const int maximHavel = 1001;

const int infinit = std::numeric_limits<int>::max();
const int maximDisjoint = 100001;

class Graf{
    int noduri, muchii;
    bool orientat; // daca e orientat -> true, daca e neorientat -> false
    bool ponderat; // daca e ponderat -> true, daca e neponderat -> false

    //lista de adiacenta
    vector<vector<int>> listaAdiacenta;

    //lista de adiacenta pentru graf ponderat
    vector<vector<pair<int, int>>> listaAdiacentaCuCosturi;

    void dfs(int nodCurent, vector<bool> &vizitate);
    void componenteBiconexeDfs(int nodCurent, int adancime, stack<int>& mystack, vector<int> &adancimeNod, vector<int> &nivelMinimNod, vector<bool> &vizitate, vector<vector<int>> &componenteBiconexe);
    void algoritmComponenteTareConexe(int nodCurent, int &pozitie, stack<int> &mystack, vector<int> &pozitiiParcurgere, vector<int> &pozitiiMinimeParcurgere, vector<bool> &elemPeStiva, vector<vector<int>> &listaComponenteTareConexe);
    void gasireMuchiiCritice(int nodCurent, int &adancime, vector<int> &adancimeParcurgere, vector<int> &adancimeMinimaParcurgere, vector<vector<int>> &muchiiCritice, vector<int> &parinti);
    pair<int, int> maximVector(vector<int> vector);
    //bool bfsEdmondsKarp(const int &nodStart, const int &nodFinal, vector<int> &tati, vector<vector<int>> capacitate, vector<vector<int>> flux);

public:
    //constructori
    Graf();
    Graf(int noduri, bool orientat);
    Graf(int noduri, bool orientat, bool ponderat);
    Graf(int noduri, int muchii, bool orientat, bool ponderat);

    //functii pentru crearea listelor de adiacenta, in functie de tipul grafului (Orientat/Neorientat)
    void pushListaAdiacentaGrafOrientat(const int &start, const int &final);
    void pushListaAdiacentaGrafNeorientat(const int &start, const int &final);
    void pushListaAdiacentaCuGradeInterioare(const int &start, const int &final, vector<int> &gradeInterioare);
    void pushListaAdiacentaCuCosturiOrientate(const int &start, const int &final, const int &cost);
    void pushListaAdiacentaCuCosturiNeorientate(const int &start, const int &final, const int &cost);

    //functii
    int numaraConexe();
    vector<int> bfs(int nodStart = 1);
    vector<int> sortareTopologica(vector<int> &gradeInterioare);
    vector<vector<int>> componenteBiconexe();
    vector<vector<int>> componenteTareConexe();
    vector<vector<int>> criticalConnections(int n, vector<vector<int>>& connections);
    int diametruArbore();
    vector<int> Dijkstra(int nodStart = 1);
    pair<vector<pair<int, int>>, int> ApmPrim(int nodStart = 1);
    void BellmanFord(ofstream &out, int nodStart = 1);
    vector<vector<long long>> royFloyd(vector<vector<long long>> &distante);
    //int EdmondsKarp(int start, int final);

    //afisari
    void afisareMatrice(vector<vector<long long>> matrice, std::ostream &out);
    void afisareDistante(vector<int> distante, std::ostream &out);
};


/* clasa pentru paduri de multimi disjuncte */

class Disjoint{
    int numarMultimi, numarOperatii;
    vector<int> vectorTata;
    vector<int> inaltimeArbore;

public:
    void citireDisjoint(const int &multimi, const int &operatii, istream &in, ostream &out);
    Disjoint(int numarMultimi, int numarOperatii);
    void initializare();
    int reprezentant(int nod);
    void reuneste(int nod1, int nod2);
};

Graf::Graf() {}

Graf::Graf(int noduri, bool orientat, bool ponderat) : noduri(noduri), orientat(orientat), ponderat(ponderat) {
    if(ponderat){
        listaAdiacentaCuCosturi.resize(noduri + 1, vector<pair<int, int>>());
    }
    else{
        listaAdiacenta.resize(noduri + 1, vector<int>());
    }
}

Graf::Graf(int noduri, bool orientat) : noduri(noduri), orientat(orientat) {}

Graf::Graf(int noduri, int muchii, bool orientat, bool ponderat) : noduri(noduri), muchii(muchii), orientat(orientat), ponderat(ponderat) {
    if(ponderat){
        listaAdiacentaCuCosturi.resize(noduri + 1, vector<pair<int, int>>());
    }
    else{
        listaAdiacenta.resize(noduri + 1, vector<int>());
    }
}

void Graf::pushListaAdiacentaGrafNeorientat(const int &start, const int &final) {
    listaAdiacenta[start].push_back(final);
    listaAdiacenta[final].push_back(start);
}

void Graf::pushListaAdiacentaGrafOrientat(const int &start, const int &final) {
    listaAdiacenta[start].push_back(final);
}

void Graf::pushListaAdiacentaCuGradeInterioare(const int &start, const int &final, vector<int> &gradeInterioare) {
    listaAdiacenta[start].push_back(final);
    gradeInterioare[final]++;
}

void Graf::pushListaAdiacentaCuCosturiOrientate(const int &start, const int &final, const int &cost) {
    listaAdiacentaCuCosturi[start].emplace_back(final, cost);
}

void Graf::pushListaAdiacentaCuCosturiNeorientate(const int &start, const int &final, const int &cost) {
    listaAdiacentaCuCosturi[start].emplace_back(final, cost);
    listaAdiacentaCuCosturi[final].emplace_back(start, cost);
}

/* DFS - Parcurgerea in adancime

    * Complexitate -> O(m + n)

    * Idee:
        - la fiecare pas, apelam RECURSIV functia dfs pentru PRIMUL vecin gasit

    * Algoritm:
        - parcurgem toate nodurile
        - apelam functia de dfs pentru nodurile nevizitate
        - daca un nod ramane nevizitat in urma apelurilor anterioare,
        atunci el formeaza o noua componenta conexa
        - in functia de dfs, apelam recursiv functia pentru primul vecin gasit al nodului curent

*/

int Graf::numaraConexe() {
    vector<bool> vizitate(noduri + 1, false);
    //std::fill(std::begin(vizitate), std::begin(vizitate)+maxim, false);

    int componenteConexe = 0;

    for(int i = 1; i < noduri + 1; i++)
    {
        if(!vizitate[i])
        {
            componenteConexe++;
            dfs(i, vizitate);
        }
    }

    return componenteConexe;
}

void Graf::dfs(int nodCurent, vector<bool> &vizitate) {
    vizitate[nodCurent] = true;

    for(int vecin : listaAdiacenta[nodCurent])
    {
        if(!vizitate[vecin])
        {
            vizitate[vecin] = true;
            dfs(vecin, vizitate);
        }
    }
}

/* BFS - Parcurgerea in latime

    * Complexitate -> O(m + n)

    * Idee:
         - implementare cu coada
         - la fiecare pas, dam push in coada TUTUROR vecinilor nevizitati ai nodului curent

    * Algoritm:
         - punem nodul de start in coada
         - cat timp coada nu e goala, dam push in coada tuturor vecinilor
         nevizitati ai nodului curent
         - distanta tuturor vecinilor nodului curent creste cu 1

*/

void Graf::afisareDistante(vector<int> distante, std::ostream &out){
    for(int i = 1; i <= noduri; i++)
        out << distante[i] << " ";
}

vector<int> Graf::bfs(int nodStart) {

    vector<bool> vizitate(noduri + 1, false);
    vector<int> distante(noduri + 1, -1);

    queue<int> queueBfs;

    queueBfs.push(nodStart);
    vizitate[nodStart] = true;
    distante[nodStart] = 0;

    while(!queueBfs.empty())
    {
        int nodCurent = queueBfs.front();
        for(int i = 0; i < listaAdiacenta[nodCurent].size(); i++)
        {
            if(!vizitate[listaAdiacenta[nodCurent][i]])
            {
                vizitate[listaAdiacenta[nodCurent][i]] = true;
                queueBfs.push(listaAdiacenta[nodCurent][i]);
                distante[listaAdiacenta[nodCurent][i]] = 1 + distante[nodCurent];
            }
        }
        queueBfs.pop();
    }

    return distante;
}

/* Sortare topologica

    * Complexitate -> O(m + n)

    * Idee:
         - implementare cu grade interioare + coada

    * Algoritm:
         - punem in coada toate nodurile cu gradul interior 0
         - cat timp coada nu e goala, parcurgem vecinii nodului curent
         si scadem 1 din gradele lor interioare (simulam eliminarea muchiilor de la
         nodul curent la vecini)
         - daca in urma scaderii, am gasit un nod al carui grad interior
         a devenit 0, il adaugam si pe el in coada

*/


vector<int> Graf::sortareTopologica(vector<int> &gradeInterioare) {
    vector<int> noduriSortateTopologic;

    queue<int> myqueue;

    for(int i = 1; i <= noduri; i++)
        if (gradeInterioare[i] == 0)
        {
            myqueue.push(i);
        }

    while(!myqueue.empty())
    {
        int nodCurent = myqueue.front();

        for(int i = 0; i < listaAdiacenta[nodCurent].size(); i++)
        {
            gradeInterioare[listaAdiacenta[nodCurent][i]]--;
            if(gradeInterioare[listaAdiacenta[nodCurent][i]] == 0)
                myqueue.push(listaAdiacenta[nodCurent][i]);
        }

        myqueue.pop();

        noduriSortateTopologic.push_back(nodCurent);
    }

    return noduriSortateTopologic;
}

/* Havel-Hakimi

    * Complexitate -> O(n^2 * log n) <- cu sort-ul din STL
                   -> O((n+max) * n) = O(n^2) <- cu CountSort

    * Algoritmul lui Havel-Hakimi verifica daca se poate construi
    un graf neorientat, avand secventa gradelor data

    * Algoritm:
         - NU se poate daca:
                -> avem suma gradelor impara
                -> gasim un grad > noduri
                -> gasim un grad negativ
         - intr-un loop infinit, sortam gradele (cu countSort sau sort-ul din STL)
         - luam cel mai mare grad (notam gr) si il "legam" de urmatoarele
         gr cele mai mari grade (simulam un numar de gr muchii)

*/

vector<int> countSort(vector<int> vectorParametru){
    vector<int> frecventa(maximHavel, 0);
//    frecventa.resize(maxim);
//    std::fill(std::begin(frecventa), std::begin(frecventa)+maxim, 0);

    int elementMaxim = vectorParametru[0];
    for(auto element : vectorParametru){
        frecventa[element]++;

        if(elementMaxim < element){
            elementMaxim = element;
        }
    }

    int contor = 0;
    for(int i = elementMaxim; i >= 0; i--){
        for(int j = 0; j < frecventa[i]; j++){
            vectorParametru[contor] = i;
            contor++;
        }
    }

    return vectorParametru;
}

int suma(const vector<int>& grade){
    int sumaGrade = 0;

    for(auto grad : grade){
        sumaGrade += grad;
    }
    return sumaGrade;
}

void havelHakimi(vector<int> grade, std::ostream &out){
    // Conditie de oprire: suma este impara
    if(suma(grade) % 2 == 1){
        out << "Nu se poate construi un grad cu secventa gradelor data, pentru ca suma gradelor este impara!";
        return;
    }

    int n = grade.size();

    while(true) {
        //sort(grade.begin(), grade.end(), greater<>());
        grade = countSort(grade);

        //fiind sortate descrescator, daca primul element e 0 si suntem inca in functie, atunci toate sunt 0
        if (grade[0] == 0) {
            out << "Putem construi un graf cu secventa gradelor data :)";
            return;
        }

        //memoram gradul curent si il stergem din vector
        int gradCurent = grade[0];
        grade.erase(grade.begin() + 0);

        if (gradCurent > n-1){
            out << "Nu se poate construi un grad cu secventa gradelor data, pentru ca cel putin un nod are gradul mai mare decat " << n;
            return;
        }

        //parcurgem celelalte grade (sunt ordonate descrescator) si scadem 1 din primele gradCurent de grade descrescatoare
        for (int i = 0; i < gradCurent; i++){
            grade[i]--;

            if(grade[i] < 0){
                out << "Stop! Am gasit un grad < 0!!!";
                return;
            }
        }
    }
}

/* Componente Biconexe

    * Complexitate -> O(m + n)

    * Idee:
        - implementare cu stiva

    * Algoritm:
        - apelam functia pentru cautare componente biconexe dfs pentru nodul de start
        - in functia pentru cautare componente biconexe dfs:
            -> parcurgem vecinii nodului curent
            -> daca gasim un vecin nevizitat, apelam functia recursiv si pentru vecinul respectiv:
                - daca are un copil(vecin) care nu se poate duce mai sus de el in
                arbore, atunci avem o componenta biconexa
                - dam pop pe stiva pana gasim vecinul curent si salvam rezultatele
                intr-un vector de componenta biconexa
                - componenta biconexa gasita o punem intr-un vector care retine toate
                componentele biconexe din graful curent
            -> daca gasim un vecin deja vizitat, actualizam nivelul minim al nodului curent
            (nodul curent se poate duce mai sus in arbore)

*/

vector<vector<int>> Graf::componenteBiconexe(){
    //vectori pentru Componente Biconexe
    vector<int> adancimeNod(noduri + 1, -1);
    vector<int> nivelMinimNod(noduri + 1, -1);
    vector<bool> vizitate(noduri + 1, false);
    vector<vector<int>> componenteBiconexe;

    stack<int> mystack;

    int radacina = 1, adancimeRadacina = 1;
    componenteBiconexeDfs(radacina, adancimeRadacina, mystack, adancimeNod, nivelMinimNod, vizitate, componenteBiconexe);

    return componenteBiconexe;
}

void Graf::componenteBiconexeDfs(int nodCurent, int adancime, stack<int>& mystack, vector<int> &adancimeNod, vector<int> &nivelMinimNod, vector<bool> &vizitate, vector<vector<int>> &componenteBiconexe) {

    mystack.push(nodCurent);
    vizitate[nodCurent] = true;
    nivelMinimNod[nodCurent] = adancime;
    adancimeNod[nodCurent] = adancime;

    for(auto vecin : listaAdiacenta[nodCurent]){
        if(!vizitate[vecin]){
            //vecinul nu a fost vizitat

            vizitate[vecin] = true; //il vizitam
            componenteBiconexeDfs(vecin, adancime+1, mystack, adancimeNod, nivelMinimNod, vizitate, componenteBiconexe);

            if(nivelMinimNod[vecin] >= adancimeNod[nodCurent]){

                int varfStiva;
                vector<int> componentaBiconexa;

                do{
                    varfStiva = mystack.top();
                    componentaBiconexa.push_back(varfStiva);

                    mystack.pop();

                }while(varfStiva != vecin);

                //trebuie sa dam push_back in componenta biconexa si nodului critic, care este nodul curent
                componentaBiconexa.push_back(nodCurent);

                //adaugam componenta biconexa gasita la lista cu componente biconexe
                componenteBiconexe.push_back(componentaBiconexa);
            }
            //update la nivel minim pentru nodul curent
            nivelMinimNod[nodCurent] = min(nivelMinimNod[nodCurent], nivelMinimNod[vecin]);
        }
        else{
            //vecinul curent a fost vizitat
            nivelMinimNod[nodCurent] = min(nivelMinimNod[nodCurent], adancimeNod[vecin]);
        }
    }
}

/* Componente Tare Conexe

    * Complexitate -> O(m + n)

    * Idee:
        - implementare cu stiva

    * Algoritm:
        - in functia pentru algoritm Componente Tare Conexe:
            -> parcurgem vecinii nodului curent
            -> daca gasim un vecin nevizitat, apelam functia recursiv si pentru vecinul respectiv si actualizam
            pozitia minima a nodului curent (nodul curent se poate duce mai sus in arbore)
            -> daca gasim un vecin deja vizitat:
                - daca nu e pe stiva, el a fost gasit in alta componenta tare conexa
                - daca e pe stiva, actualizam pozitia minima a nodului curent
            -> daca dupa ce parcurgem vecinii, pozitia initiala de parcurgere a nodului curent este
            egala cu pozitia sa minima de parcurgere, atunci el este radacina unei componente
            tare conexe
            -> dam pop pe stiva pana gasim nodul curent si salvam nodurile gasite
            intr-o componenta tare conexa

*/

vector<vector<int>> Graf::componenteTareConexe() {
    //vectori pentru Componente Tare Conexe
    vector<int> pozitiiParcurgere(noduri + 1, -1);
    vector<int> pozitiiMinimeParcurgere(noduri + 1, -1);
    vector<bool> elemPeStiva(noduri + 1, false); // are valoare true daca elementul este in stiva si false altfel (la inceput, niciun element nu e pe stiva)
    vector<vector<int>> listaComponenteTareConexe;

    stack<int> mystack;

    int pozitie = 1;
    for(int i = 1; i <= noduri; i++)
    {
        if(pozitiiParcurgere[i] == -1) // elementul nu a fost vizitat pana acum
        {
            algoritmComponenteTareConexe(i, pozitie, mystack, pozitiiParcurgere, pozitiiMinimeParcurgere, elemPeStiva, listaComponenteTareConexe);
        }
    }

    return listaComponenteTareConexe;

}

void Graf::algoritmComponenteTareConexe(int nodCurent, int &pozitie, stack<int> &mystack, vector<int> &pozitiiParcurgere, vector<int> &pozitiiMinimeParcurgere, vector<bool> &elemPeStiva, vector<vector<int>> &listaComponenteTareConexe){
    mystack.push(nodCurent);
    elemPeStiva[nodCurent] = true; //elementul curent este pe stiva
    pozitiiParcurgere[nodCurent] = pozitie;
    pozitiiMinimeParcurgere[nodCurent] = pozitie;

    pozitie += 1;

    for(auto vecin : listaAdiacenta[nodCurent]){
        if(pozitiiParcurgere[vecin] == -1){
            //vecinul nu a fost vizitat, deci are indexul -1

            algoritmComponenteTareConexe(vecin, pozitie, mystack, pozitiiParcurgere, pozitiiMinimeParcurgere, elemPeStiva, listaComponenteTareConexe);

            //update la nivel minim pentru nodul curent
            pozitiiMinimeParcurgere[nodCurent] = min(pozitiiMinimeParcurgere[nodCurent], pozitiiMinimeParcurgere[vecin]);
        }
        else{
            //a fost vizitat, dar nu e pe stiva -> e in alta componenta tare conexa pe care deja am gasit-o
            if(elemPeStiva[vecin])
            //vecinul curent a fost vizitat
                pozitiiMinimeParcurgere[nodCurent] = min(pozitiiMinimeParcurgere[nodCurent], pozitiiParcurgere[vecin]);
        }
    }


    //nodul curent este radacina unei componente tare conexe
    if(pozitiiMinimeParcurgere[nodCurent] == pozitiiParcurgere[nodCurent]){

        vector<int> componentaTareConexa;

        int varfStiva;
        do{
            varfStiva = mystack.top();
            componentaTareConexa.push_back(varfStiva);

            // dupa ce il scoatem din stiva, setam elemPeStiva[varfStiva] = false!
            elemPeStiva[varfStiva] = false;

            mystack.pop();

        }while(varfStiva != nodCurent);

        sort(componentaTareConexa.begin(), componentaTareConexa.end());

        //adaugam componenta tare conexa gasita la lista cu componente biconexe
        listaComponenteTareConexe.push_back(componentaTareConexa);
    }
}

/* Muchii Critice

    * Complexitate -> O(m + n)

    * Algoritm:
    - pentru un nod curent, parcurgem toti vecinii
    - daca vecinul este parintele lui, continue
    - daca vecinul e vizitat, actualizam adancimea minima a nodului curent
    - daca vecinul e nevizitat:
        -> apelam functia recursiv si pentru el
        -> daca vecinul nu se poate duce mai sus de nodul curent in graf,
        atunci muchia dintre el si nodul curent este critica

*/

void Graf::gasireMuchiiCritice(int nodCurent, int &adancime, vector<int> &adancimeParcurgere, vector<int> &adancimeMinimaParcurgere, vector<vector<int>> &muchiiCritice, vector<int> &parinti){
    adancimeMinimaParcurgere[nodCurent] = adancime;
    adancimeParcurgere[nodCurent] = adancime;

    for(auto vecin : listaAdiacenta[nodCurent]){
        if(vecin == parinti[nodCurent])
            continue;
        if (adancimeParcurgere[vecin] != -1){
           // if(parinti[nodCurent] != vecin) {
                //vecinul a fost vizitat si nu e parintele lui
                //nu avem muchie de la copil la parinte intr-un graf neorientat, deci nu actualizam oricum adancimea minima a copilului, ci numai daca vecinul vizitat nu este si parintele lui!
                //ne ducem in jos in arbore pe copil, dar de la copil nu ne intoarcem la parinte (graf neorientat)
                adancimeMinimaParcurgere[nodCurent] = min(adancimeMinimaParcurgere[nodCurent],
                                                          adancimeParcurgere[vecin]);
            }
       // }
        else{
            //vecinul nu a fost vizitat
            adancime += 1;
            parinti[vecin] = nodCurent;
            gasireMuchiiCritice(vecin, adancime, adancimeParcurgere, adancimeMinimaParcurgere, muchiiCritice, parinti);

            adancimeMinimaParcurgere[nodCurent] = min(adancimeMinimaParcurgere[nodCurent], adancimeMinimaParcurgere[vecin]);

            if (adancimeMinimaParcurgere[vecin] > adancimeParcurgere[nodCurent]) {
                muchiiCritice.push_back({nodCurent, vecin});
            }
        }
    }
}

vector<vector<int>> Graf::criticalConnections(int n, vector<vector<int>>& connections) {
    //vectori pentru Muchii Critice
    vector<int> adancimeParcurgere(noduri + 1, -1);
    vector<int> adancimeMinimaParcurgere(noduri + 1, -1);
    vector<vector<int>> muchiiCritice;
    vector<int> parinti(noduri + 1, 0);

    for(int i = 0; i < connections.size(); i++){
        listaAdiacenta[connections[i][1]].push_back(connections[i][0]);
        listaAdiacenta[connections[i][0]].push_back(connections[i][1]);
    }

    int radacina = 0, adancimeRadacina = 0;
    gasireMuchiiCritice(radacina, adancimeRadacina, adancimeParcurgere, adancimeMinimaParcurgere, muchiiCritice, parinti);

    return muchiiCritice;

    // cout << "[";
    // // for(auto i = muchiiCritice.begin(); i != muchiiCritice.end(); i++)
    // for(int i = 0; i < muchiiCritice.size(); i++)
    //     cout << "[" << muchiiCritice[i].first << "," << muchiiCritice[i].second << "]";
    // cout << "]";
}

/* Algoritmul lui Dijkstra - Drumuri Minime In Graf Ponderat Cu Ponderi Pozitive

    * Complexitate = O(n^2) <- cu vector
                   = O(m * log n) <- cu heap (priority-queue)
                   = O(n * log n + m) <- cu heap Fibonacci

    * Idee: retinem intr-un min heap distantele minime

    * Constrangeri: muchiile trebuie sa aiba costuri pozitive!

    * Algoritm:
        - initial, in minHeap avem distanta 0 si nodul de start
        - cat timp heap-ul nu este gol, luam front-ul
        - daca front-ul a fost vizitat, continue, nu ne mai intereseaza (nu vrem sa calculam de
        doua ori acelasi lucru pentru el)
        - daca front-ul nu a fost vizitat:
                -> il vizitam
                -> ii parcurgem vecinii
                -> daca gasim un vecin nevizitat caruia ii actualizam distanta (distanta[front] + costul
                muchiei de la front la vecin < distanta[vecin]), ii dam push in heap
                -> heap-ul se rearanjeaza, astfel incat, in varf sa fie mereu minimul
        - returnam vectorul de distante
*/

vector<int> Graf::Dijkstra(int nodStart) {

    // initializare Dijkstra
    vector<int> distante(noduri + 1, infinit);
    vector<bool> vizitate(noduri + 1, false);

    // min-heap
    priority_queue<pair<int, int>, std::vector<pair<int, int>>, std::greater<>> minHeap;

    distante[nodStart] = 0;
    //vizitate[nodStart] = true;

    //punem nodul de start in heap
    minHeap.push({distante[nodStart], nodStart});

    while (!minHeap.empty()){
        //minimul din heap e in top
        pair<int, int> elementSiDistantaMin = minHeap.top();

        //dam pop pe priority queue imediat cum salvam top-ul
        minHeap.pop();

        // daca am trecut prin nodul din top, nu mai facem tot algoritmul pentru el
        if (vizitate[elementSiDistantaMin.second]){
            continue;
        }

        // vizitam un nod doar cand el ajunge in top-ul heap-ului
        vizitate[elementSiDistantaMin.second] = true;
        // pair.first -> distanta
        // pair.second -> nodul

        for( auto vecinSiCost : listaAdiacentaCuCosturi[elementSiDistantaMin.second]){
            // listaAdiacenta.first -> vecinul
            // listaAdiacenta.second -> costul muchiei nodCurent - vecin

            if(!vizitate[vecinSiCost.first]){
                // daca nu e vizitat, el nu e in arbore

                int distantaNouaVecin = distante[elementSiDistantaMin.second] + vecinSiCost.second;

                if (distantaNouaVecin < distante[vecinSiCost.first]){
                    distante[vecinSiCost.first] = distantaNouaVecin;

                    // pe prima pozitie din pair am distanta!!!
                    minHeap.push({distante[vecinSiCost.first], vecinSiCost.first});
                }
            }
        }
    }

    return distante;

}

/* APM - Arbori Partiali de Cost Minim

 Complexitate
 * Prim -> O(n^2) <- cu vector de vizitate
        -> O(m * log n) <- cu heap (min-heap de noduri)
 * Kruskal -> O(m * log n + n^2) <- cu vector de reprezentanti
           -> O(m * log m) = O(m * log n) <- structuri pentru multimi disjuncte (union/find)

 * PRIM:
    * Idee: la un pas se selecteaza o muchie de cost minim de la un varf care a fost deja adaugat
    in arbore la unul neadaugat (avem un singur arbore la care alipim noduri)

    * Algoritm:
        - pornim dintr-un varf de start (il punem in heap ca tuplu {0, start}, unde 0 este distanta[nodStart])
        - adaugam pe rand cate un varf in arborele deja construit (cu proprietatea ca muchia dintre
        varful care este in graf si varful pe care il adaugam in graf sa fie minima -> cost minim)
        - aceeasi ideea cu min heap ca la Dijkstra
        - cat timp heap-ul nu este gol, extragem minimul
        - daca minimul a fost vizitat -> continue (nu vrem sa adaugam de doua ori distanta lui in costul APM-ului)
        - daca minimul nu a fost vizitat:
                -> adaugam la costul APM-ului distanta lui
                -> il vizitam
                -> ii parcurgem vecinii si gasim un vecin nevizitat (nu e in arbore) pe care il putem
                adauga in arbore (are costul muchiei curente mai mic decat distanta anterioare),
                il adaugam in heap, actualizam distanta si ii retinem tatal in vectorul de tati
        - parcurgem vectorul de tati si retinem intr-un vector nodurile care nu au tatal = 0 (singurul nod
        cu tatal = 0 este radacina) -> nodurile astea sunt nodurile din APM
        - returnam vectorul de noduri si costul minim al APM-ului (calculat in while)

 * KRUSKAL:
    * Idee: avem mai multi arbori si ii unim formand paduri

*/

pair<vector<pair<int, int>>, int> Graf::ApmPrim(int nodStart) {
    // initializare Prim

    // min-heap
    priority_queue<pair<int, int>, std::vector<pair<int, int>>, std::greater<>> minHeap;

    // vectori pentru Prim
    vector<int> distante(noduri + 1, infinit);
    vector<bool> vizitate(noduri + 1, false);
    vector<pair<int, int>> muchiiApm;
    vector<int> vectorTati(noduri + 1, 0);

    distante[nodStart] = 0;

    //punem nodul de start in heap
    minHeap.push({distante[nodStart], nodStart});

    int costMinimApm = 0;

    while (!minHeap.empty()){
        //minimul din heap e in top
        pair<int, int> elementSiDistantaMin = minHeap.top();

        //dam pop pe priority queue imediat cum salvam top-ul
        minHeap.pop();

        //daca nodul curent (din top) a fost vizitat, trecem peste el (nu vrem sa mai actualizam costul minim
        //al apm-ului)
        if(vizitate[elementSiDistantaMin.second]){
            continue;
        }

        costMinimApm += elementSiDistantaMin.first;

        // vizitam un nod doar cand el ajunge in top-ul heap-ului
        vizitate[elementSiDistantaMin.second] = true;
        // pair.first -> distanta
        // pair.second -> nodul

        for( auto vecinSiCost : listaAdiacentaCuCosturi[elementSiDistantaMin.second]){
            // listaAdiacenta.first -> vecinul
            // listaAdiacenta.second -> costul muchiei nodCurent - vecin

            if(!vizitate[vecinSiCost.first]){
                // daca nu a fost vizitat, el nu e in arbore

                if (vecinSiCost.second < distante[vecinSiCost.first]){
                    distante[vecinSiCost.first] = vecinSiCost.second;

                    // pe prima pozitie din pair am distanta!!!
                    minHeap.push({distante[vecinSiCost.first], vecinSiCost.first});

                    // retinem si tatal vecinului -> pentru a reconstrui solutia
                    // (in final, muchiile din arbore vor fi de forma [nod, tata[nod])
                    vectorTati[vecinSiCost.first] = elementSiDistantaMin.second;
                }
            }
        }
    }

    for(int i = 2; i <= noduri; i++)
        // singurul nod care trebuie sa aiba tatal 0 e radacina (nodul 1)
        if(vectorTati[i] != 0){
            muchiiApm.emplace_back(i, vectorTati[i]);
        }

    return {muchiiApm, costMinimApm};

}

/* Algoritm Bellman-Ford - drumuri minime de la start la celelalte noduri

    * Complexitate -> O(m * n)

    * Constrangeri:
        - arcele pot avea costuri negative si pozitive
        - daca exista circuite negative le detecteaza!

    * Idee: la un pas relaxam toate muchiile
        - pentru OPTIMIZARE este suficient ca la un pas sa relaxam arcele din varfurile
        modificate anterior (le retinem intr-o coada)

    * Algoritm:
        - initial, avem doar nodul de start in coada
        - cat timp coada nu e goala, verificam daca am parcurs nodul curent de mai mult de n-1 ori: daca da,
        break, pentru ca, daca actualizam la infinit distanta unui varf, atunci el se afla intr-un circuit negativ
        - daca nu am iesit din functie, scoatem nodul curent din coada si il vizitam (vizitam un nod doar
        atunci cand il gasim in front-ul cozii)
        - ii parcurgem vecinii si, daca putem sa actualizam distanta vecinului curent (distanta[front] + costul
        muchiei (front, vecin) < distante[vecin]), o actualizam
        - daca vecinul pe care tocmai l-am actualizat nu este in coada, il punem si pe el
        - afisam distantele
*/

void Graf::BellmanFord(ofstream &out, int nodStart) {
    // initializare Bellman-Ford
    vector<int> elemInCoada(noduri + 1, false);
    vector<int> numarParcurgeri(noduri + 1, 0);
    vector<int> distante(noduri + 1, infinit);

    queue<int> myqueue;

    distante[nodStart] = 0;
    elemInCoada[nodStart] = true;

    //parametru nodStart
    myqueue.push(nodStart);

    while(!myqueue.empty()) {
        int nodCurent = myqueue.front();

        // daca numarul parcurgerilor nodului curent este mai mare egal decat numarul nodurilor,
        // atunci avem un ciclu negativ -> inca se fac actualizari la pasul n
        if(numarParcurgeri[nodCurent] >= noduri){
            out << "Ciclu negativ!";
            return;
        }

        myqueue.pop();

        numarParcurgeri[nodCurent]++;
        elemInCoada[nodCurent] = false;

        for (auto vecinSiCost: listaAdiacentaCuCosturi[nodCurent]) {
            int vecin = vecinSiCost.first;
            int cost = vecinSiCost.second;

            if (distante[nodCurent] + cost < distante[vecin]) {
                distante[vecin] = distante[nodCurent] + cost;

                //le punem in coada doar pe cele actualizate
                if(!elemInCoada[vecin]) {
                    myqueue.push(vecin);
                    elemInCoada[vecin] = true;
                }
            }
        }
    }

    for(int i = 2; i <= noduri; i++){
        if(distante[i] != infinit)
            out << distante[i] << " ";
        else{
            out << 0 << " ";
        }
    }
}

/* Disjoint - Paduri de Multimi Disjuncte

    * Complexitate: O(m * log n)
        - Initializare -> O(1), dar se face de n ori -> n * O(1) = O(n)
        - Reprezentant -> O(log n), liniar in inaltimea arborelui, dar se apeleaza de 2*m ori -> O(m * log n)
        - Reuneste -> O(1), dupa determinarea reprezentantilor celor 2 noduri (pentru fiecare nod este O(log n)), dar se apeleaza pentru n-1 noduri -> O(n * log n)

    * Algoritm:
        - Initializare:
                -> initial, fiecare nod formeaza o multime (n noduri izolate -> n multimi de un element)
                -> tatal fiecarui nod este 0 (fiecare nod este radacina in arborele sau)
                -> inaltimea fiecarui nod este 0 (fiecare nod formeaza un arbore cu inaltimea 0)
        - Reprezentant (liniar in inaltimea arborelui):
                -> pentru un nod dat ca parametru, functia returneaza nodul-radacina al arborelui
                din care face parte nodul curent
                -> compresia de cale (optimizarea functiei): toate nodurile de pe lantul de la nodul
                curent la radacina isi vor seta tatal = radacina arborelui (apel recursiv al functiei
                Reprezentant pentru tatal fiecarui nod de pe drum)
                !! OBSERVATIE: In urma compresiei de cale, NU actualizam inaltimea arborelui!
        - Reuneste:
                -> unim 2 noduri din arbori diferiti
                -> calculam reprezentantii pentru cele 2 noduri pe care vrem sa le unim (radacinile
                lor)
                -> daca avem 2 arbori cu inaltimi diferite, radacina arborelui cu inaltimea mai mica
                va deveni fiu al radacinii arborelui cu inaltimea mai mare (Nu actualizam inaltimea
                arborelui rezultat)
                -> daca cei 2 arbori au aceeasi inaltime, radacina unuia va deveni fiu al radacinii celui
                de-al doilea SI crestem si inaltimea arborelui rezultat cu 1
                !! EXEMPLU: daca avem 2 arbori cu inaltimea = 1, dupa ce ii unim, avem un arbore cu inaltimea = 2

 */

void Disjoint::citireDisjoint(const int &multimi, const int &operatii, istream &in, ostream &out) {
    int operatie, x, y;

    for(int i = 0; i < operatii; i++){
        in >> operatie >> x >> y;

        if (operatie == 1){
            // operatia de tip 1 -> reunim elementele x si y

            reuneste(x, y);

        }

        else {
            // operatia de tip 2 -> spunem daca elementele x si y se afla in aceeasi multime
            if(reprezentant(x) == reprezentant(y)){
                out << "DA" << "\n";
            }
            else{
                out << "NU" << "\n";
            }
        }
    }
}

Disjoint::Disjoint(int numarMultimi, int numarOperatii) : numarOperatii(numarOperatii), numarMultimi(numarMultimi) {}

void Disjoint::initializare() {
    vectorTata.resize(maximDisjoint, 0);
    inaltimeArbore.resize(maximDisjoint, 0);
}

int Disjoint::reprezentant(int nod) {
    //daca am ajuns in radacina arborelui, o returnam
    if(vectorTata[nod] == 0){
        return nod;
    }

    //daca nu, apelam recursiv functia pana cand tatal nodului curent "va deveni" radacina arborelui -> compresie de cale
    vectorTata[nod] = reprezentant(vectorTata[nod]);
    return vectorTata[nod];
}

void Disjoint::reuneste(int nod1, int nod2) {
    int reprezentantNod1 = reprezentant(nod1);
    int reprezentantNod2 = reprezentant(nod2);

    if (inaltimeArbore[reprezentantNod1] > inaltimeArbore[reprezentantNod2]){
        // il facem pe reprezentantul nodului 2 copilul reprezentantului nodului 1
        vectorTata[reprezentantNod2] = reprezentantNod1;
    }
    else{
        // il facem pe reprezentantul nodului 1 copilul reprezentantului nodului 2
        vectorTata[reprezentantNod1] = reprezentantNod2;

        // daca inaltimile sunt egale -> crestem inaltimea cu 1
        if (inaltimeArbore[reprezentantNod1] == inaltimeArbore[reprezentantNod2]){
            inaltimeArbore[reprezentantNod2]++;
        }
    }
}

/* Roy-Floyd -> Distantele Dintre Oricare 2 Perechi De Noduri

    * Complexitate -> O(n^3)

    * Vrem sa determinam distanta minima de la un nod x la un nod y, pentru
     oricare x si y noduri in graf

    * Idee: distante[x][y] = min{distante[x][y], distante[x][k] + distante[k][y]}

    * Algoritm:
     - pentru fiecare nod (varf intermediar) parcurgem matricea distantelor (for-ul dupa k)
     - daca gasim un drum de la x la y care trece prin k, de lungime
     mai mica decat lungimea drumului anterior de la x la y, actualizam
     distante[x][y]

    * Observatie: Matricea distantelor minime pentru graful neorientat este simetrica!

*/

vector<vector<long long>> Graf::royFloyd(vector<vector<long long>> &distante) {

    for(int k = 1; k <= noduri; k++){
        for(int i = 1; i <= noduri; i++){
            for(int j = 1; j <= noduri; j++){

                // spargem drumul de la i la j in suma drumurilor de la i la k si de la k la j

                if (distante[i][j] > (distante[i][k] + distante[k][j])){
                    distante[i][j] = distante[i][k] + distante[k][j];
                }
            }
        }
    }

    return distante;

}

void Graf::afisareMatrice(vector<vector<long long>> matrice, std::ostream &out) {
    for(int i = 1; i <= noduri; i++){
        for(int j = 1; j <= noduri; j++){
            if(i != j || matrice[i][j] != infinit)
                out << matrice[i][j] << " ";
            else
                out << 0 << " ";
        }
        out << "\n";
    }
}

/* Diametrul unui arbore

    * Diametrul unui arbore reprezint?? lungimea drumului (ca numar de noduri) intre cele mai indepartate dou?? frunze

    * Complexitate -> O(n), facem de doua ori BFS, deci avem complexitate O(2 * (n+m)), dar fiind arbore, m = n-1 -> O(4*n) = O(n)

    * Algoritm:
            -> facem bfs dintr-un nod oarecare
            -> intrucat bfs-ul ne da distantele de la nodul pentru care a fost apelat, la toate nodurile, retinem care este nodul cu distanta maxima
     din vectorul de distante (cu functia maximVector)
            -> apelam bfs pentru nodul cu distanta maxima
            -> diametrul arborelui va fi egal cu distanta maxima dupa al doilea bfs + 1 (nodul de start are distanta 0 in bfs -> numara diametru-1 noduri)

*/

int Graf::diametruArbore() {

    vector<int> distante;
    distante = bfs();

    int primulCapat, ultimulCapat;

    //functia maxim vector returneaza o pereche care are pe prima pozitie maximul dintr-un vector
    //dat ca parametru si pe a doua pozitie, pozitia maximului in vector
    primulCapat = maximVector(distante).second;

    distante = bfs(primulCapat);

    ultimulCapat = maximVector(distante).second;

    return distante[ultimulCapat] + 1;

}

// functie care imi returneaza maximul dintr-un vector si pozitia maximului
pair<int, int> Graf::maximVector(vector<int> vector) {
    int valoareMaxima = vector[1], pozitie = 1;

    for(int i = 2; i <= noduri; i++){
        if (vector[i] > valoareMaxima){
            valoareMaxima = vector[i];
            pozitie = i;
        }
    }

    return {valoareMaxima, pozitie};
}

/* Edmonds-Karp - Flux Maxim

    * Complexitate: O(n * m^2)

    * Algoritm:
        - folosim 2 matrice n x n:
                -> capacitate[i][j]: reprezinta cantitatea maxima care se poate trimite pe muchia (i, j)
                -> flux[i][j]: reprezinta cantitatea care a fost trimisa pana la un moment dat pe muchia (i, j)
        - initial, matricea cu capacitati este initializata cu costurile muchiilor
        - initial, matricea cu fluxuri este initializata cu 0 (nu am trimis nimic pe nicio muchie)
        - cat timp cautarea in latime(BFS) ne gaseste un drum de la start la final in graf (putem merge pe
        muchia (i, j) doar daca cantitate[i][j] - flux[i][j] > 0)
        - calculam costul minim al muchiilor din drumul gasit
        - il adaugam la variabila flux maxim
        - pentru fiecare muchie din drumul gasit, reactualizam fluxul:
                -> daca muchia (i, j) apartine drumului curent, flux[i][j] += fluxMinim (cazul cu muchie directa)
                -> daca muchia (j, i) apartine drumului curent, flux[j][i] -= fluxMinim (cazul cu muchie intoarsa)
        - daca nu mai gasim drumuri cu BFS, returnam variabila fluxMaxim

*/

/*
bool Graf::bfsEdmondsKarp(const int &nodStart, const int &nodFinal, vector<int> &tati, vector<vector<int>> capacitate, vector<vector<int>> flux) {

    std::fill(std::begin(tati), std::begin(tati) + noduri + 1, 0);

    vector<bool> vizitate(noduri + 1, false);

    queue<int> queueBfs;

    queueBfs.push(nodStart);
    vizitate[nodStart] = true;

    while(!queueBfs.empty())
    {
        int nodCurent = queueBfs.front();
        for(int i = 0; i < listaAdiacentaCuCosturi[nodCurent].size(); i++)
        {
            int nodVecin = listaAdiacentaCuCosturi[nodCurent][i].first;

            //vecinul nu a fost vizitat si inca ne putem duce pe drumul nodCurent -> nodVecin
            if(!vizitate[nodVecin] && (capacitate[nodCurent][nodVecin] - flux[nodCurent][nodVecin] > 0)) {
                vizitate[nodVecin] = true;
                tati[nodVecin] = nodCurent;
                queueBfs.push(nodVecin);

                if (nodVecin == nodFinal) {
                    return true;
                }
            }
        }
        queueBfs.pop();
    }
    return false;
}

int Graf::EdmondsKarp(int start, int final) {
    int fluxMaxim = 0;

    vector<int> vectorTati(noduri + 1, 0);

    // Cat poate sa duca pe o muchie
    vector<vector<int>> capacitati(noduri + 1, vector<int>(noduri + 1));

    // Cat a dus pe o muchie intr-un drum anterior
    vector<vector<int>> flux(noduri + 1, vector<int>(noduri + 1));

    for(int nod = 1; nod <= noduri; nod++){
        for(auto vecinSiCapacitate : listaAdiacentaCuCosturi[nod]){
            int vecin = vecinSiCapacitate.first;
            int capacitate = vecinSiCapacitate.second;

            capacitati[nod][vecin] = capacitate;
        }
    }

    // initial, pe nicio muchie nu s-a dus nimic -> matricea de flux este plina cu 0-uri
    for(int i = 0; i <= noduri; i++){
        for(int j = 0; j <= noduri; j++){
            flux[i][j] = 0;
        }
    }

    // mai putem determina un drum in Gf
    while(bfsEdmondsKarp(start, final, vectorTati, capacitati, flux)){
        int nodCurent = final, fluxMinimDrum = infinit;

        // parcurgem vectorul de tati ca sa gasim drumul
        while(nodCurent != start){
            int tataNodCurent = vectorTati[nodCurent];

//            if(capacitati[tataNodCurent][nodCurent] < fluxMinimDrum){
//                fluxMinimDrum = capacitati[tataNodCurent][nodCurent];
//            }

            fluxMinimDrum = min(fluxMinimDrum, capacitati[tataNodCurent][nodCurent] - flux[tataNodCurent][nodCurent]);

            nodCurent = tataNodCurent;
        }

        fluxMaxim += fluxMinimDrum;

        nodCurent = final;

        while(nodCurent != start){
            int tataNodCurent = vectorTati[nodCurent];

            flux[tataNodCurent][nodCurent] += fluxMinimDrum;

            flux[nodCurent][tataNodCurent] -= fluxMinimDrum;

            nodCurent = tataNodCurent;
        }
    }

    return fluxMaxim;

}

*/

/* Rezolvari InfoArena si LeetCode */

void rezolvareDFS(){
    // https://www.infoarena.ro/problema/dfs

    ifstream in("dfs.in");
    ofstream out("dfs.out");

    int noduri, muchii, extremitateInitiala, extremitateFinala;

    in >> noduri >> muchii;

    Graf mygraf(noduri, muchii, false, false);

    for(int i = 0; i < muchii; i++)
    {
        in >> extremitateInitiala >> extremitateFinala;

        /* citire DFS -> pentru graf neorientat */
        mygraf.pushListaAdiacentaGrafNeorientat(extremitateInitiala, extremitateFinala);

    }

    /* apel DFS */
    //apelam functia numaraConexe()
    out << mygraf.numaraConexe();

    in.close();
    out.close();
}

void rezolvareBFS(){
    // https://www.infoarena.ro/problema/bfs

    ifstream in("bfs.in");
    ofstream out("bfs.out");

    int noduri, muchii, extremitateInitiala, extremitateFinala, nodStart;

    vector<int> distanteBfs;

    in >> noduri >> muchii >> nodStart;

    Graf mygraf(noduri, muchii, true, false);

    for(int i = 0; i < muchii; i++)
    {
        in >> extremitateInitiala >> extremitateFinala;

        /* citire BFS -> pentru graf orientat */
        mygraf.pushListaAdiacentaGrafOrientat(extremitateInitiala, extremitateFinala);
    }

    /* apel BFS */

    distanteBfs = mygraf.bfs(nodStart);

    mygraf.afisareDistante(distanteBfs, out);

    in.close();
    out.close();
}

void rezolvareBiconex(){
    // https://www.infoarena.ro/problema/biconex

    ifstream in("biconex.in");
    ofstream out("biconex.out");

    int noduri, muchii, extremitateInitiala, extremitateFinala;
    vector<vector<int>> componenteBiconexe;

    in >> noduri >> muchii;

    Graf mygraf(noduri, muchii, false, false);

    for(int i = 0; i < muchii; i++)
    {
        in >> extremitateInitiala >> extremitateFinala;

        /* citire Componente Biconexe -> pentru graf neorientat */
        mygraf.pushListaAdiacentaGrafNeorientat(extremitateInitiala, extremitateFinala);

    }

    componenteBiconexe = mygraf.componenteBiconexe();

    //afisarea componentelor biconexe
    unsigned int nrComponenteBiconexe = componenteBiconexe.size();
    out << nrComponenteBiconexe << "\n";

    for(int i = 0; i < nrComponenteBiconexe; i++){
        for(int j = 0; j < componenteBiconexe[i].size(); j++){
            out << componenteBiconexe[i][j] << " ";
        }
        out << "\n";
    }

    in.close();
    out.close();
}

void rezolvareCTC(){
    // https://www.infoarena.ro/problema/ctc

    ifstream in("ctc.in");
    ofstream out("ctc.out");


    int noduri, muchii, extremitateInitiala, extremitateFinala;
    vector<vector<int>> componenteTareConexe;

    in >> noduri >> muchii;

    Graf mygraf(noduri, muchii, true, false);

    for(int i = 0; i < muchii; i++)
    {
        in >> extremitateInitiala >> extremitateFinala;

        /* citire Componente Tare Conexe -> pentru graf orientat */
        mygraf.pushListaAdiacentaGrafOrientat(extremitateInitiala, extremitateFinala);

    }

    componenteTareConexe = mygraf.componenteTareConexe();

    //afisarea componentelor tare conexe
    unsigned int nrComponenteTareConexe = componenteTareConexe.size();
    out << nrComponenteTareConexe << "\n";

    for(int i = 0; i < nrComponenteTareConexe; i++){
        for(int j = 0; j < componenteTareConexe[i].size(); j++){
            out << componenteTareConexe[i][j] << " ";
        }
        out << "\n";
    }

//    for(int i = 1; i <= noduri; i++){
//        cout<<"\n"<<i<<" "<<pozitiiParcurgere[i];
//    }

    in.close();
    out.close();
}

void rezolvareHavelHakimi(){

    ifstream in("hh.in");
    ofstream out("hh.out");

    /* citire Havel-Hakimi */
    int numar, valoare;

    vector<int> listaGrade;
    in >> numar;
//    cout << numar;

    for(int i = 0; i < numar; i++)
    {
        in >> valoare;
        listaGrade.push_back(valoare);
    }

    /* algoritm Havel-Hakimi */

    havelHakimi(listaGrade, out);

    in.close();
    out.close();
}

void rezolvareMuchieCritica(){

    ifstream in("criticalConnections.in");
    ofstream out("criticalConnections.out");

    int noduri, muchii, extremitateInitiala, extremitateFinala;

    in >> noduri >> muchii;

    vector<vector<int>> listaMuchii; //pentru muchii critice
    vector<vector<int>> muchiiCritice;

    Graf mygraf(noduri, muchii, false, false);

    for(int i = 0; i < muchii; i++)
    {
        in >> extremitateInitiala >> extremitateFinala;

        /* citire Muchii Critice -> pentru graf neorientat */
        listaMuchii.push_back({extremitateInitiala, extremitateFinala});

    }

    muchiiCritice = mygraf.criticalConnections(noduri, listaMuchii);

    for(auto muchie : muchiiCritice){
        out << muchie[0] << " " << muchie[1] << "\n";
    }

    in.close();
    out.close();
}

void rezolvareSortareTopologica(){
    // https://www.infoarena.ro/problema/sortaret

    ifstream in("sortaret.in");
    ofstream out("sortaret.out");

    // vector care retine gradele interioare ale nodurilor din graf

    vector<int> noduriSortTopo;

    int noduri, muchii, extremitateInitiala, extremitateFinala;

    in >> noduri >> muchii;
    vector<int> gradeInterioare(noduri + 1, 0);

    Graf mygraf(noduri, muchii, true, false);

    for(int i = 0; i < muchii; i++)
    {
        in >> extremitateInitiala >> extremitateFinala;

        /* citire Sortare Topologica -> pentru graf orientat */
        mygraf.pushListaAdiacentaCuGradeInterioare(extremitateInitiala, extremitateFinala, gradeInterioare);

    }

    /* apel Sortare Topologica */
    noduriSortTopo = mygraf.sortareTopologica(gradeInterioare);

    for(auto nod : noduriSortTopo){
        out << nod << " ";
    }

    in.close();
    out.close();
}

void rezolvareAPM(){
    // https://www.infoarena.ro/problema/apm

    ifstream in("apm.in");
    ofstream out("apm.out");

    int noduri, muchii, extremitateInitiala, extremitateFinala;

    in >> noduri >> muchii;

    Graf mygraf(noduri, muchii, false, false);

    for(int i = 0; i < muchii; i++)
    {
        in >> extremitateInitiala >> extremitateFinala;

        /* citire APM -> pentru graf neorientat ponderat */
        int costMuchie;
        in >> costMuchie;

        mygraf.pushListaAdiacentaCuCosturiNeorientate(extremitateInitiala, extremitateFinala, costMuchie);

    }

    auto rezultat = mygraf.ApmPrim();

    auto cost = rezultat.second;
    auto muchiiApm = rezultat.first;

    out << cost << "\n";

    out << muchiiApm.size() << "\n";

    for(auto pereche : muchiiApm){
        out << pereche.first << " " << pereche.second << "\n";
    }

    in.close();
    out.close();
}

void rezolvareDijkstra(){
    // https://www.infoarena.ro/problema/dijkstra

    ifstream in("dijkstra.in");
    ofstream out("dijkstra.out");

    int noduri, muchii, extremitateInitiala, extremitateFinala;
    vector<int> distante;

    in >> noduri >> muchii;

    // pentru cazul in care citim si nodul de start
    // int nodStart;
    // in >> nodStart;

    Graf mygraf(noduri, muchii, true, true);

    for(int i = 0; i < muchii; i++)
    {
        in >> extremitateInitiala >> extremitateFinala;

        /* citire Dijkstra -> pentru graf orientat ponderat */
        int costMuchie;
        in >> costMuchie;

        mygraf.pushListaAdiacentaCuCosturiOrientate(extremitateInitiala, extremitateFinala, costMuchie);

    }

    distante = mygraf.Dijkstra();
    // distante = mygraf.Dijkstra(nodStart);

    // incepem de la 2, pentru ca nodul 1 e start-ul
    for(int i = 2; i <= noduri; i++){
        if (distante[i] != infinit){
            out << distante[i] << " ";
        }
        else {
            out << 0 << " ";
        }
    }

    in.close();
    out.close();
}

void rezolvareDisjoint(){
    // https://www.infoarena.ro/problema/disjoint

    ifstream in("disjoint.in");
    ofstream out("disjoint.out");

    // n -> numarul de multimi
    // m -> numarul de operatii

    int numarOperatii, numarMultimi;
    in >> numarMultimi >> numarOperatii;

    Disjoint padureDeMultimiDisjuncte(numarMultimi, numarOperatii);

    padureDeMultimiDisjuncte.initializare();

    padureDeMultimiDisjuncte.citireDisjoint(numarMultimi, numarOperatii, in, out);

    in.close();
    out.close();
}

void rezolvareBellmanFord(){
    // https://www.infoarena.ro/problema/bellmanford

    ifstream in("bellmanford.in");
    ofstream out("bellmanford.out");


    int noduri, muchii, extremitateInitiala, extremitateFinala, nodStart;

    in >> noduri >> muchii;

    // in cazul in care citim si nodul de start

    // int nodStart;
    // in >> nodStart;

    Graf mygraf(noduri, muchii, true, true);

    for(int i = 0; i < muchii; i++)
    {
        in >> extremitateInitiala >> extremitateFinala;

        /* citire Bellman-Ford -> pentru graf orientat ponderat */
        int costMuchie;
        in >> costMuchie;

        mygraf.pushListaAdiacentaCuCosturiOrientate(extremitateInitiala, extremitateFinala, costMuchie);

    }

    // mygraf.BellmanFord(out, nodStart);
    mygraf.BellmanFord(out);

    in.close();
    out.close();
}

void rezolvareFloydWarshall(){
    // https://infoarena.ro/problema/royfloyd

    ifstream in ("royfloyd.in");
    ofstream out("royfloyd.out");

    int noduri, distanta;

    in >> noduri;

    // primul parametru este numarul de linii
    // al doilea parametru este container-ul si size-ul container-ului (numarul de coloane)
    vector<vector<long long>> matriceDistanteMinime(noduri + 1, vector<long long>(noduri + 1));

    for(int i = 1; i <= noduri; i++){
        for(int j = 1; j <= noduri; j++){

            in >> distanta;

            // daca citim 0 -> nu exista muchie intre i si j
            // vrem sa lasam distanta 0 doar pentru muchiile (i, i)
            if(i != j && distanta == 0){
                matriceDistanteMinime[i][j] = infinit;
            }
            else{
                matriceDistanteMinime[i][j] = distanta;
            }
        }
    }

    Graf mygraf(noduri, true);

    matriceDistanteMinime = mygraf.royFloyd(matriceDistanteMinime);

    mygraf.afisareMatrice(matriceDistanteMinime, out);

    in.close();
    out.close();
}

void rezolvareDarb(){
    //https://infoarena.ro/problema/darb

    ifstream in ("darb.in");
    ofstream out("darb.out");

    int noduri, extremitateInitiala, extremitateFinala;

    in >> noduri;

    Graf mygraf(noduri, false);

    //e arbore

    for(int i = 0; i <= noduri-1; i++){
        in >> extremitateInitiala >> extremitateFinala;

        mygraf.pushListaAdiacentaGrafNeorientat(extremitateInitiala, extremitateFinala);
    }

    out << mygraf.diametruArbore();

    in.close();
    out.close();
}

// la Maxflow imi dau 50 de puncte pe infoarena

/*
void rezolvareEdmondsKarp(){
    ifstream in("maxflow.in");
    ofstream out("maxflow.out");

    int noduri, muchii, capacitate, extremitateInitiala, extremitateFinala, start, final;

    in >> noduri >> muchii;

    //daca citim nodurile de start si final
    //in >> start >> final;

    Graf mygraf(noduri, muchii);

    for(int i = 0; i < muchii; i++){
        in >> extremitateInitiala >> extremitateFinala >> capacitate;

        mygraf.pushListaAdiacentaCuCosturiOrientate(extremitateInitiala, extremitateFinala, capacitate);
    }

    //daca nu citim si nodurile de start si final
    start = 1;
    final = noduri;

    out << mygraf.EdmondsKarp(start, final);

    in.close();
    out.close();
}
*/

int main() {

    /* apel DFS */
    rezolvareDFS();

    /* apel BFS */
//    rezolvareBFS();

    /* apel Sortare Topologica */
//    rezolvareSortareTopologica();

    /* apel Componente Biconexe */
//    rezolvareBiconex();

    /* apel Componente Tare Conexe */
//    rezolvareCTC();

    /* apel Muchii Critice */
//    rezolvareMuchieCritica();

    /* apel Havel-Hakimi */
//    rezolvareHavelHakimi();

    /* apel Dijkstra */
//    rezolvareDijkstra();

    /* apel Apm */
//    rezolvareAPM();

    /* apel Bellman-Ford */
//    rezolvareBellmanFord();

    /* apel Paduri de multimi disjuncte */
//    rezolvareDisjoint();

    /* apel Floyd-Warshall */
//    rezolvareFloydWarshall();

    /* apel Diametrul unui arbore */
//    rezolvareDarb();

    /* apel Edmonds-Karp */
//    rezolvareEdmondsKarp();

//    cout << infinit;

    return 0;
}
