#include <iostream>
#include <fstream>
#include <stack>
#include <queue>
#include <vector>
#include <algorithm>
#include <limits>
#include <utility>

using namespace std;

const int maxim = 100001;
const int infinit = std::numeric_limits<int>::max();
const int maximDisjoint = 100001;

class Graf{
    int noduri, muchii;

    //lista de adiacenta
    vector<int> listaAdiacenta[maxim];

    //lista de adiacenta pentru graf ponderat
    vector<pair<int, int>> listaAdiacentaCuCosturi[maxim];

    vector<bool> vizitate;
    vector<int> distante;

    //vector care retine gradele interioare ale nodurilor din graf
    int gradeInterioare[maxim] = {0};

    //vectori pentru Componente Biconexe
    vector<int> adancimeNod;
    vector<int> nivelMinimNod;
    vector<vector<int>> componenteBiconexe;

    //vectori pentru Componente Tare Conexe
    vector<int> pozitiiParcurgere;
    vector<int> pozitiiMinimeParcurgere;
    vector<int> elemPeStiva; // are valoare true daca elementul este in stiva si false altfel
    vector<vector<int>> listaComponenteTareConexe;

    //vectori pentru Muchii Critice
    vector<int> adancimeParcurgere;
    vector<int> adancimeMinimaParcurgere;
    vector<vector<int>> muchiiCritice;
    vector<int> parinti;

    // min-heap
    priority_queue<pair<int, int>, std::vector<pair<int, int>>, std::greater<>> minHeap;

    // coada
    queue<int> myqueue;

    // vectori pentru Prim
    vector<pair<int, int>> muchiiApm;
    vector<int> vectorTati;

    // vectori pentru Bellman-Ford
    vector<int> elemInCoada;
    vector<int> numarParcurgeri;

public:
    //functii pentru crearea listelor de adiacenta, in functie de tipul grafului (Orientat/Neorientat)
    void construiesteGrafOrientat(const int &start, const int &final);
    void construiesteGrafNeorientat(const int &start, const int &final);
    void construiesteGradeInterioare(const int &start, const int &final);
    void construiesteCuCosturiOrientate(const int &start, const int &final, const int &cost);
    void construiesteCuCosturiNeorientate(const int &start, const int &final, const int &cost);

    //constructori
    Graf();
    Graf(int noduri, int muchii);

    //functii
    int numaraConexe();
    void dfs(int nodCurent);
    void bfs(int nodStart, std::ostream &out);
    void sortareTopologica(std::ostream &out);
    void initializareCompBiconexe(ostream &out);
    void componenteBiconexeDfs(int nodCurent, int adancime, stack<int>& mystack);
    void initializareCompTareConexe(ostream &out);
    void componenteTareConexe(int nodCurent, int &pozitie, stack<int>& mystack);
    void gasireMuchiiCritice(int nodCurent, int &adancime);
    vector<vector<int>> criticalConnections(int n, vector<vector<int>>& connections);
    void afisareDistante(std::ostream &out);
    void Dijkstra(ostream &out);
    void initializareDijkstra();
    void ApmPrim(ofstream &out);
    void initializarePrim();
    void afisarePrim(int cost, ostream &out);
    void BellmanFord(ofstream &out);
    void initializareBellManFord();
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

Graf::Graf(int noduri, int muchii) : noduri(noduri), muchii(muchii) {}

void Graf::construiesteGrafNeorientat(const int &start, const int &final) {
    listaAdiacenta[start].push_back(final);
    listaAdiacenta[final].push_back(start);
}

void Graf::construiesteGrafOrientat(const int &start, const int &final) {
    listaAdiacenta[start].push_back(final);
}

void Graf::construiesteGradeInterioare(const int &start, const int &final) {
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

/* functii pentru DFS */
int Graf::numaraConexe() {
    int componenteConexe = 0;

    for(int i=1; i<noduri+1; i++)
    {
        if(!vizitate[i])
        {
            componenteConexe++;
            dfs(i);
        }
    }

    return componenteConexe;
}

void Graf::dfs(int nodCurent) {
    vizitate[nodCurent] = true;

    for(int vecin : listaAdiacenta[nodCurent])
    {
        if(!vizitate[vecin])
        {
            vizitate[vecin]=true;
            dfs(vecin);
        }
    }
}

/* functii pentru BFS */
void Graf::afisareDistante(std::ostream &out){
    for(int i = 1; i <= noduri; i++)
        out<<distante[i]<<" ";
}

void Graf::bfs(int nodStart, std::ostream &out) {
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
        for(int i=0; i<listaAdiacenta[nodCurent].size(); i++)
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

    afisareDistante(out);
}

/* Sortare topologica */

void Graf::sortareTopologica(std::ostream &out) {
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
        out << nodCurent << " ";
    }
}

/* Havel-Hakimi */

Graf::Graf() {}

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

void Graf::initializareCompBiconexe(ostream &out) {
    adancimeNod.resize(maxim);
    nivelMinimNod.resize(maxim);

    std::fill(std::begin(adancimeNod), std::begin(adancimeNod)+maxim, -1);
    std::fill(std::begin(nivelMinimNod), std::begin(nivelMinimNod)+maxim, -1);

    stack<int> mystack;

//    for(int i = 0; i<adancimeNod.size(); i++)
//        out<<adancimeNod[i]<<" ";

    int radacina = 1, adancimeRadacina = 1;
    componenteBiconexeDfs(radacina, adancimeRadacina, mystack);

    //afisarea componentelor biconexe
    unsigned int nrComponenteBiconexe = componenteBiconexe.size();
    out << nrComponenteBiconexe << "\n";

    for(int i = 0; i < nrComponenteBiconexe; i++){
        for(int j = 0; j < componenteBiconexe[i].size(); j++){
            out << componenteBiconexe[i][j] << " ";
        }
        out << "\n";
    }
}

void Graf::componenteBiconexeDfs(int nodCurent, int adancime, stack<int>& mystack) {
    mystack.push(nodCurent);
    vizitate[nodCurent] = true;
    nivelMinimNod[nodCurent] = adancime;
    adancimeNod[nodCurent] = adancime;

    for(auto vecin : listaAdiacenta[nodCurent]){
        if(!vizitate[vecin]){
            //vecinul nu a fost vizitat

            vizitate[vecin] = true; //il vizitam
            componenteBiconexeDfs(vecin, adancime+1, mystack);

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

void Graf::initializareCompTareConexe(ostream &out) {
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
            componenteTareConexe(i, pozitie, mystack);
        }
    }

    //afisarea componentelor tare conexe
    unsigned int nrComponenteTareConexe = listaComponenteTareConexe.size();
    out << nrComponenteTareConexe << "\n";

    for(int i = 0; i < nrComponenteTareConexe; i++){
        for(int j = 0; j < listaComponenteTareConexe[i].size(); j++){
            out << listaComponenteTareConexe[i][j] << " ";
        }
        out << "\n";
    }

//    for(int i = 1; i <= noduri; i++){
//        cout<<"\n"<<i<<" "<<pozitiiParcurgere[i];
//    }

}

void Graf::componenteTareConexe(int nodCurent, int &pozitie, stack<int> &mystack){
    mystack.push(nodCurent);
    elemPeStiva[nodCurent] = true; //elementul curent este pe stiva
    pozitiiParcurgere[nodCurent] = pozitie;
    pozitiiMinimeParcurgere[nodCurent] = pozitie;

    pozitie += 1;

    for(auto vecin : listaAdiacenta[nodCurent]){
        if(pozitiiParcurgere[vecin] == -1){
            //vecinul nu a fost vizitat, deci are indexul -1

            componenteTareConexe(vecin, pozitie, mystack);

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

void Graf::gasireMuchiiCritice(int nodCurent, int &adancime){
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
            gasireMuchiiCritice(vecin, adancime);

            adancimeMinimaParcurgere[nodCurent] = min(adancimeMinimaParcurgere[nodCurent], adancimeMinimaParcurgere[vecin]);

            if (adancimeMinimaParcurgere[vecin] > adancimeParcurgere[nodCurent]) {
                muchiiCritice.push_back({nodCurent, vecin});
            }
        }
    }
}

vector<vector<int>> Graf::criticalConnections(int n, vector<vector<int>>& connections) {
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

//         for(int i = 1; i <= n; i++){
//             if (pozitiiParcurgere[i] == -1){

//             }
//         }

    int radacina = 0, adancimeRadacina = 0;
    gasireMuchiiCritice(radacina, adancimeRadacina);

    return muchiiCritice;

    // cout << "[";
    // // for(auto i = muchiiCritice.begin(); i != muchiiCritice.end(); i++)
    // for(int i = 0; i < muchiiCritice.size(); i++)
    //     cout << "[" << muchiiCritice[i].first << "," << muchiiCritice[i].second << "]";
    // cout << "]";
}

/* Algoritmul lui Dijkstra */

void Graf::Dijkstra(ostream &out) {
    initializareDijkstra();

    while (!minHeap.empty()){
        //minimul din heap e in top
        pair<int, int> elementSiDistantaMin = minHeap.top();

        //dam pop pe priority queue imediat cum salvam top-ul
        minHeap.pop();

        // vizitam un nod doar cand el ajunge in top-ul heap-ului
        vizitate[elementSiDistantaMin.second] = true;
        // pair.first -> distanta
        // pair.second -> nodul

        for( auto vecinSiCost : listaAdiacentaCuCosturi[elementSiDistantaMin.second]){
            // listaAdiacenta.first -> vecinul
            // listaAdiacenta.second -> costul muchiei nodCurent - vecin

            if(!vizitate[vecinSiCost.first]){
                int distantaNouaVecin = distante[elementSiDistantaMin.second] + vecinSiCost.second;

                if (distantaNouaVecin < distante[vecinSiCost.first]){
                    distante[vecinSiCost.first] = distantaNouaVecin;

                    // pe prima pozitie din pair am distanta!!!
                    minHeap.push({distante[vecinSiCost.first], vecinSiCost.first});
                }
            }
        }
    }

    // incepem de la 2, pentru ca nodul 1 e start-ul
    for(int i = 2; i <= noduri; i++){
        if (distante[i] != infinit){
            out << distante[i] << " ";
        }
        else {
            out << 0 << " ";
        }
    }
}

void Graf::initializareDijkstra(){
    distante.resize(maxim);
    vizitate.resize(maxim);

    fill(std::begin(distante), std::begin(distante)+maxim, infinit);
    fill(std::begin(vizitate), std::begin(vizitate)+maxim, false);

    distante[1] = 0;
    vizitate[1] = true;

    //punem nodul de start in heap
    minHeap.push({distante[1], 1});
}

/* APM */

void Graf::ApmPrim(ofstream &out) {
    initializarePrim();

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

    afisarePrim(costMinimApm, out);

}

void Graf::initializarePrim() {
    distante.resize(maxim);
    vizitate.resize(maxim);
    vectorTati.resize(maxim);

    fill(std::begin(distante), std::begin(distante)+maxim, infinit);
    fill(std::begin(vizitate), std::begin(vizitate)+maxim, false);
    fill(std::begin(vectorTati), std::begin(vectorTati)+maxim, 0);

    distante[1] = 0;

    //punem nodul de start in heap
    minHeap.push({distante[1], 1});
}

void Graf::afisarePrim(int cost, ostream &out) {
    out << cost << "\n";

    out << muchiiApm.size() << "\n";

    for(auto pereche : muchiiApm){
        out << pereche.first << " " << pereche.second << "\n";
    }
}


/* Algoritm Bellman-Ford */

void Graf::BellmanFord(ofstream &out) {
    initializareBellManFord();

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

void Graf::initializareBellManFord() {
    distante.resize(maxim);
    elemInCoada.resize(maxim);
    numarParcurgeri.resize(maxim);

    fill(std::begin(distante), std::begin(distante)+maxim, infinit);
    fill(std::begin(elemInCoada), std::begin(elemInCoada)+maxim, false);
    fill(std::begin(numarParcurgeri), std::begin(numarParcurgeri)+maxim,0);

    distante[1] = 0;
    elemInCoada[1] = true;

    myqueue.push(1);
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


int main() {

//    ifstream in("bfs.in");
//    ofstream out("bfs.out");

//    ifstream in("dfs.in");
//    ofstream out("dfs.out");

//    ifstream in("sortaret.in");
//    ofstream out("sortaret.out");

//    ifstream in("sortaret.in");
//    ofstream out("sortaret.out");

//    ifstream in("hh.in");
//    ofstream out("hh.out");

//    ifstream in("biconex.in");
//    ofstream out("biconex.out");

//    ifstream in("ctc.in");
//    ofstream out("ctc.out");

//    ifstream in("criticalConnections.in");
//    ofstream out("criticalConnections.out");

//    ifstream in("dijkstra.in");
//    ofstream out("dijkstra.out");

//    ifstream in("apm.in");
//    ofstream out("apm.out");

    ifstream in("bellmanford.in");
    ofstream out("bellmanford.out");

//    ifstream in("disjoint.in");
//    ofstream out("disjoint.out");

    int noduri, muchii, extremitateInitiala, extremitateFinala, nodStart;

    in >> noduri >> muchii;

    // vector<vector<int>> listaMuchii; //pentru muchii critice

    /* pentru BFS -> citim si nodul de start */
    //in >> noduri >> muchii >> nodStart;

    Graf mygraf(noduri, muchii);

    for(int i = 0; i < muchii; i++)
    {
        in >> extremitateInitiala >> extremitateFinala;

        /* citire BFS -> pentru graf orientat */
        //mygraf.construiesteGrafOrientat(extremitateInitiala, extremitateFinala);

        /* citire DFS -> pentru graf neorientat */
        //mygraf.construiesteGrafNeorientat(extremitateInitiala, extremitateFinala);

        /* citire Sortare Topologica -> pentru graf orientat */
        //mygraf.construiesteGradeInterioare(extremitateInitiala, extremitateFinala);

        /* citire Componente Biconexe -> pentru graf neorientat */
        //mygraf.construiesteGrafNeorientat(extremitateInitiala, extremitateFinala);

        /* citire Componente Biconexe -> pentru graf neorientat */
        //mygraf.construiesteGrafNeorientat(extremitateInitiala, extremitateFinala);

        /* citire Componente Tare Conexe -> pentru graf orientat */
        //mygraf.construiesteGrafOrientat(extremitateInitiala, extremitateFinala);

        /* citire Muchii Critice -> pentru graf neorientat */
        //listaMuchii.push_back({extremitateInitiala, extremitateFinala});

        /* citire Dijkstra -> pentru graf orientat ponderat */
//        int costMuchie;
//        in >> costMuchie;
//
//        mygraf.construiesteCuCosturiOrientate(extremitateInitiala, extremitateFinala, costMuchie);

        /* citire Bellman-Ford -> pentru graf orientat ponderat */
        int costMuchie;
        in >> costMuchie;

        mygraf.construiesteCuCosturiOrientate(extremitateInitiala, extremitateFinala, costMuchie);

        /* citire APM -> pentru graf neorientat ponderat */
//        int costMuchie;
//        in >> costMuchie;
//
//        mygraf.construiesteCuCosturiNeorientate(extremitateInitiala, extremitateFinala, costMuchie);

    }

    /* citire Havel-Hakimi */
//    int numar, valoare;
//
//    vector<int> listaGrade;
//    in >> numar;
////    cout << numar;
//
//    for(int i = 0; i < numar; i++)
//    {
//        in >> valoare;
//        listaGrade.push_back(valoare);
//    }

    /* algoritm Havel-Hakimi */

//    cout<<suma(listaGrade);

//    havelHakimi(listaGrade, out);

    /* apel BFS */

    //mygraf.bfs(nodStart, out);

    /* apel DFS */
//    std::fill(std::begin(vizitate), std::begin(vizitate)+maxim, false);
    //apelam functia numaraConexe()
    //out << mygraf.numaraConexe();

    /* apel Sortare Topologica */
    //mygraf.sortareTopologica(out);

    /* apel Componente Biconexe */
//    std::fill(std::begin(vizitate), std::begin(vizitate)+maxim, false);
//    mygraf.initializareCompBiconexe(out);

    /* apel Componente Tare Conexe */
//    mygraf.initializareCompTareConexe(out);

    /* apel Muchii Critice */
//    vector<vector<int>> muchiiCritice = mygraf.criticalConnections(noduri, listaMuchii);
//
//    for(auto muchie : muchiiCritice){
//        out << muchie[0] << " " << muchie[1] << "\n";
//    }

    /* apel Dijkstra */
//    mygraf.Dijkstra(out);

    /* apel Apm */
//    mygraf.ApmPrim(out);

    /* apel Bellman-Ford */
    mygraf.BellmanFord(out);

    /* apel Paduri de multimi disjuncte */

//    // n -> numarul de multimi
//    // m -> numarul de operatii
//
//    int numarOperatii, numarMultimi;
//    in >> numarMultimi >> numarOperatii;
//
//    Disjoint padureDeMultimiDisjuncte(numarMultimi, numarOperatii);
//
//    padureDeMultimiDisjuncte.initializare();
//
//    padureDeMultimiDisjuncte.citireDisjoint(numarMultimi, numarOperatii, in, out);


    in.close();
    out.close();
    return 0;
}
