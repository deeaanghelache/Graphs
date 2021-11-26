#include <iostream>
#include <fstream>
#include <stack>
#include <queue>
#include <vector>
#include <algorithm>
#include <limits>
#include <utility>

using namespace std;

//const int maxim = 200001;
const int maxim = 100001;
//const int maxim = 50001;

const int infinit = std::numeric_limits<int>::max();
const int maximDisjoint = 100001;

class Graf{
    int noduri, muchii;

    //lista de adiacenta
    vector<int> listaAdiacenta[maxim];

    //lista de adiacenta pentru graf ponderat
    vector<pair<int, int>> listaAdiacentaCuCosturi[maxim];

    void dfs(int nodCurent, vector<bool> &vizitate);
    void componenteBiconexeDfs(int nodCurent, int adancime, stack<int>& mystack, vector<int> &adancimeNod, vector<int> &nivelMinimNod, vector<bool> &vizitate, vector<vector<int>> &componenteBiconexe);
    void algoritmComponenteTareConexe(int nodCurent, int &pozitie, stack<int> &mystack, vector<int> &pozitiiParcurgere, vector<int> &pozitiiMinimeParcurgere, vector<bool> &elemPeStiva, vector<vector<int>> &listaComponenteTareConexe);
    void gasireMuchiiCritice(int nodCurent, int &adancime, vector<int> &adancimeParcurgere, vector<int> &adancimeMinimaParcurgere, vector<vector<int>> &muchiiCritice, vector<int> &parinti);
    pair<int, int> maximVector(vector<int> vector);

public:
    //functii pentru crearea listelor de adiacenta, in functie de tipul grafului (Orientat/Neorientat)
    void construiesteGrafOrientat(const int &start, const int &final);
    void construiesteGrafNeorientat(const int &start, const int &final);
    void construiesteGradeInterioare(const int &start, const int &final, vector<int> &gradeInterioare);
    void construiesteCuCosturiOrientate(const int &start, const int &final, const int &cost);
    void construiesteCuCosturiNeorientate(const int &start, const int &final, const int &cost);

    //constructori
    Graf();
    Graf(int noduri);
    Graf(int noduri, int muchii);

    //functii
    int numaraConexe();
    vector<int> bfs(int nodStart = 1);
    vector<int> sortareTopologica(vector<int> &gradeInterioare);
    vector<vector<int>> componenteBiconexe();
    vector<vector<int>> componenteTareConexe();
    vector<vector<int>> criticalConnections(int n, vector<vector<int>>& connections);
    void afisareDistante(vector<int> distante, std::ostream &out);
    vector<int> Dijkstra(int nodStart = 1);
    pair<vector<pair<int, int>>, int> ApmPrim(int nodStart = 1);
    void BellmanFord(ofstream &out, int nodStart = 1);
    vector<vector<long long>> royFloyd(vector<vector<long long>> &distante);
    void afisareMatrice(vector<vector<long long>> matrice, ofstream &out);
    int diametruArbore();
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

Graf::Graf(int noduri) : noduri(noduri) {}

Graf::Graf(int noduri, int muchii) : noduri(noduri), muchii(muchii) {}

void Graf::construiesteGrafNeorientat(const int &start, const int &final) {
    listaAdiacenta[start].push_back(final);
    listaAdiacenta[final].push_back(start);
}

void Graf::construiesteGrafOrientat(const int &start, const int &final) {
    listaAdiacenta[start].push_back(final);
}

void Graf::construiesteGradeInterioare(const int &start, const int &final, vector<int> &gradeInterioare) {
    listaAdiacenta[start].push_back(final);
    gradeInterioare[final]++;
}

void Graf::construiesteCuCosturiOrientate(const int &start, const int &final, const int &cost) {
    listaAdiacentaCuCosturi[start].emplace_back(final, cost);
}

void Graf::construiesteCuCosturiNeorientate(const int &start, const int &final, const int &cost) {
    listaAdiacentaCuCosturi[start].emplace_back(final, cost);
    listaAdiacentaCuCosturi[final].emplace_back(start, cost);
}

/* DFS - Parcurgerea in adancime */

// Complexitate DFS = O(m + n)

int Graf::numaraConexe() {
    vector<bool> vizitate;

    vizitate.resize(maxim);
    std::fill(std::begin(vizitate), std::begin(vizitate)+maxim, false);

    int componenteConexe = 0;

    for(int i=1; i<noduri+1; i++)
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
            vizitate[vecin]=true;
            dfs(vecin, vizitate);
        }
    }
}

/* BFS - Parcurgerea in latime */

// Complexitate BFS = O(m + n)

void Graf::afisareDistante(vector<int> distante, std::ostream &out){
    for(int i = 1; i <= noduri; i++)
        out<<distante[i]<<" ";
}

vector<int> Graf::bfs(int nodStart) {

    vector<bool> vizitate;
    vector<int> distante;

    distante.resize(maxim);
    vizitate.resize(maxim);

    std::fill(std::begin(vizitate), std::begin(vizitate)+maxim, false);
    std::fill(std::begin(distante), std::begin(distante)+maxim, -1);

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

/* Sortare topologica */

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

/* Havel-Hakimi */

// Complexitate Havel-Hakimi = O(n^2 * log n) <- cu sort-ul din STL
//                           = O((n+max) * n) <- cu CountSort



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
        sort(grade.begin(), grade.end(), greater<>());

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

/* Componente Biconexe */

vector<vector<int>> Graf::componenteBiconexe(){
    //vectori pentru Componente Biconexe
    vector<int> adancimeNod;
    vector<int> nivelMinimNod;
    vector<bool> vizitate;
    vector<vector<int>> componenteBiconexe;

    vizitate.resize(maxim);
    adancimeNod.resize(maxim);
    nivelMinimNod.resize(maxim);

    std::fill(std::begin(adancimeNod), std::begin(adancimeNod)+maxim, -1);
    std::fill(std::begin(nivelMinimNod), std::begin(nivelMinimNod)+maxim, -1);
    std::fill(std::begin(vizitate), std::begin(vizitate)+maxim, false);

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

/* Componente Tare Conexe */

vector<vector<int>> Graf::componenteTareConexe() {
    //vectori pentru Componente Tare Conexe
    vector<int> pozitiiParcurgere;
    vector<int> pozitiiMinimeParcurgere;
    vector<bool> elemPeStiva; // are valoare true daca elementul este in stiva si false altfel
    vector<vector<int>> listaComponenteTareConexe;

    pozitiiMinimeParcurgere.resize(maxim);
    pozitiiParcurgere.resize(maxim);
    elemPeStiva.resize(maxim);

    std::fill(std::begin(pozitiiParcurgere), std::begin(pozitiiParcurgere)+maxim, -1);
    std::fill(std::begin(pozitiiMinimeParcurgere), std::begin(pozitiiMinimeParcurgere)+maxim, -1);
    std::fill(std::begin(elemPeStiva), std::begin(elemPeStiva)+maxim, false); // la inceput, niciun element nu e pe stiva

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

/* Muchii Critice */

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
    vector<int> adancimeParcurgere;
    vector<int> adancimeMinimaParcurgere;
    vector<vector<int>> muchiiCritice;
    vector<int> parinti;

    for(int i = 0; i < connections.size(); i++){
        listaAdiacenta[connections[i][1]].push_back(connections[i][0]);
        listaAdiacenta[connections[i][0]].push_back(connections[i][1]);
    }

    adancimeParcurgere.resize(maxim);
    adancimeMinimaParcurgere.resize(maxim);
    parinti.resize(maxim);

    std::fill(std::begin(adancimeParcurgere), std::begin(adancimeParcurgere)+maxim, -1);
    std::fill(std::begin(adancimeMinimaParcurgere), std::begin(adancimeMinimaParcurgere)+maxim, -1);
    std::fill(std::begin(parinti), std::begin(parinti)+maxim, 0);

    int radacina = 0, adancimeRadacina = 0;
    gasireMuchiiCritice(radacina, adancimeRadacina, adancimeParcurgere, adancimeMinimaParcurgere, muchiiCritice, parinti);

    return muchiiCritice;

    // cout << "[";
    // // for(auto i = muchiiCritice.begin(); i != muchiiCritice.end(); i++)
    // for(int i = 0; i < muchiiCritice.size(); i++)
    //     cout << "[" << muchiiCritice[i].first << "," << muchiiCritice[i].second << "]";
    // cout << "]";
}

/* Algoritmul lui Dijkstra */

vector<int> Graf::Dijkstra(int nodStart) {

    // initializare Dijkstra
    vector<int> distante;
    vector<bool> vizitate;

    distante.resize(noduri + 1);
    vizitate.resize(noduri + 1);

    fill(std::begin(distante), std::begin(distante) + noduri + 1, infinit);
    fill(std::begin(vizitate), std::begin(vizitate) + noduri + 1, false);

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

/* APM */

pair<vector<pair<int, int>>, int> Graf::ApmPrim(int nodStart) {
    // initializare Prim

    // min-heap
    priority_queue<pair<int, int>, std::vector<pair<int, int>>, std::greater<>> minHeap;

    // vectori pentru Prim
    vector<int> distante;
    vector<bool> vizitate;
    vector<pair<int, int>> muchiiApm;
    vector<int> vectorTati;

    distante.resize(maxim);
    vizitate.resize(maxim);
    vectorTati.resize(maxim);

    fill(std::begin(distante), std::begin(distante)+maxim, infinit);
    fill(std::begin(vizitate), std::begin(vizitate)+maxim, false);
    fill(std::begin(vectorTati), std::begin(vectorTati)+maxim, 0);

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

/* Algoritm Bellman-Ford */

void Graf::BellmanFord(ofstream &out, int nodStart) {
    // initializare Bellman-Ford
    vector<int> elemInCoada;
    vector<int> numarParcurgeri;
    vector<int> distante;

    distante.resize(maxim);
    elemInCoada.resize(maxim);
    numarParcurgeri.resize(maxim);

    fill(std::begin(distante), std::begin(distante)+maxim, infinit);
    fill(std::begin(elemInCoada), std::begin(elemInCoada)+maxim, false);
    fill(std::begin(numarParcurgeri), std::begin(numarParcurgeri)+maxim,0);

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

/* Paduri de multimi disjuncte */

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
    vectorTata.resize(maximDisjoint);
    inaltimeArbore.resize(maximDisjoint);

    fill(std::begin(vectorTata), std::begin(vectorTata)+maximDisjoint, 0);
    fill(std::begin(inaltimeArbore), std::begin(inaltimeArbore)+maximDisjoint, 0);
}

int Disjoint::reprezentant(int nod) {
    //daca am ajuns in radacina arborelui, o returnam
    if(vectorTata[nod] == 0){
        return nod;
    }

    //daca nu, apelam recursiv functia pana cand tatal nodului curent "va deveni" radacina arborelui
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


vector<vector<long long>> Graf::royFloyd(vector<vector<long long>> &distante) {
    // Complexitate -> O(n^3)

    // Vrem sa determinam distanta minima de la un nod x la un nod y, pentru
    // oricare x si y noduri in graf

    // Idee: distante[x][y] = min{distante[x][y], distante[x][k] + distante[k][y]}

    // Algoritm:
    // - pentru fiecare nod (varf intermediar) parcurgem matricea distantelor (for-ul dupa k)
    // - daca gasim un drum de la x la y care trece prin k, de lungime
    // mai mica decat lungimea drumului anterior de la x la y, actualizam
    // distante[x][y]

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

void Graf::afisareMatrice(vector<vector<long long>> matrice, ofstream &out) {
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

/* Diametrul unui arbore */

int Graf::diametruArbore() {

    // Complexitate -> O(n)
    // Diametrul unui arbore  reprezintă lungimea drumului (ca numar de noduri) intre cele mai indepartate două frunze

    // Idee:
    // -> facem bfs dintr-un nod oarecare
    // -> intrucat bfs-ul ne da distantele de la nodul pentru care a fost apelat, la toate nodurile, retinem care este nodul cu distanta maxima
    // din vectorul de distante (cu functia maximVector)
    // -> apelam bfs pentru nodul cu distanta maxima
    // -> diametrul arborelui va fi egal cu distanta maxima dupa al doilea bfs + 1 (nodul de start are distanta 0 in bfs -> numara diametru-1 noduri)

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


/* Rezolvari InfoArena si LeetCode */

void rezolvareDFS(){
    // https://www.infoarena.ro/problema/dfs

    ifstream in("dfs.in");
    ofstream out("dfs.out");

    int noduri, muchii, extremitateInitiala, extremitateFinala;

    in >> noduri >> muchii;

    Graf mygraf(noduri, muchii);

    for(int i = 0; i < muchii; i++)
    {
        in >> extremitateInitiala >> extremitateFinala;

        /* citire DFS -> pentru graf neorientat */
        mygraf.construiesteGrafNeorientat(extremitateInitiala, extremitateFinala);

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

    Graf mygraf(noduri, muchii);

    for(int i = 0; i < muchii; i++)
    {
        in >> extremitateInitiala >> extremitateFinala;

        /* citire BFS -> pentru graf orientat */
        mygraf.construiesteGrafOrientat(extremitateInitiala, extremitateFinala);
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

    Graf mygraf(noduri, muchii);

    for(int i = 0; i < muchii; i++)
    {
        in >> extremitateInitiala >> extremitateFinala;

        /* citire Componente Biconexe -> pentru graf neorientat */
        mygraf.construiesteGrafNeorientat(extremitateInitiala, extremitateFinala);

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

    Graf mygraf(noduri, muchii);

    for(int i = 0; i < muchii; i++)
    {
        in >> extremitateInitiala >> extremitateFinala;

        /* citire Componente Tare Conexe -> pentru graf orientat */
        mygraf.construiesteGrafOrientat(extremitateInitiala, extremitateFinala);

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

    Graf mygraf(noduri, muchii);

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
    vector<int> gradeInterioare;
    gradeInterioare.resize(maxim);
    std::fill(std::begin(gradeInterioare), std::begin(gradeInterioare)+maxim, 0);

    vector<int> noduriSortTopo;

    int noduri, muchii, extremitateInitiala, extremitateFinala;

    in >> noduri >> muchii;

    Graf mygraf(noduri, muchii);

    for(int i = 0; i < muchii; i++)
    {
        in >> extremitateInitiala >> extremitateFinala;

        /* citire Sortare Topologica -> pentru graf orientat */
        mygraf.construiesteGradeInterioare(extremitateInitiala, extremitateFinala, gradeInterioare);

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

    Graf mygraf(noduri, muchii);

    for(int i = 0; i < muchii; i++)
    {
        in >> extremitateInitiala >> extremitateFinala;

        /* citire APM -> pentru graf neorientat ponderat */
        int costMuchie;
        in >> costMuchie;

        mygraf.construiesteCuCosturiNeorientate(extremitateInitiala, extremitateFinala, costMuchie);

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

    Graf mygraf(noduri, muchii);

    for(int i = 0; i < muchii; i++)
    {
        in >> extremitateInitiala >> extremitateFinala;

        /* citire Dijkstra -> pentru graf orientat ponderat */
        int costMuchie;
        in >> costMuchie;

        mygraf.construiesteCuCosturiOrientate(extremitateInitiala, extremitateFinala, costMuchie);

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

    Graf mygraf(noduri, muchii);

    for(int i = 0; i < muchii; i++)
    {
        in >> extremitateInitiala >> extremitateFinala;

        /* citire Bellman-Ford -> pentru graf orientat ponderat */
        int costMuchie;
        in >> costMuchie;

        mygraf.construiesteCuCosturiOrientate(extremitateInitiala, extremitateFinala, costMuchie);

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

    Graf mygraf(noduri);

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

    Graf mygraf(noduri);

    //e arbore

    for(int i = 0; i <= noduri-1; i++){
        in >> extremitateInitiala >> extremitateFinala;

        mygraf.construiesteGrafNeorientat(extremitateInitiala, extremitateFinala);
    }

    out << mygraf.diametruArbore();

    in.close();
    out.close();
}

int main() {

    /* apel DFS */
//    rezolvareDFS();

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
    rezolvareDarb();

//    cout << infinit;

    return 0;
}
