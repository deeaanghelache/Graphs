#include <iostream>
#include <fstream>
#include <stack>
#include <queue>
#include <vector>
#include <algorithm>

using namespace std;

const int maxim = 100001;
bool vizitate[maxim];
int distante[maxim];

class Graf{
    int noduri, muchii;

    //lista de adiacenta
    vector<int> listaAdiacenta[maxim];

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

public:
    //functii pentru crearea listelor de adiacenta, in functie de tipul grafului (Orientat/Neorientat)
    void construiesteGrafOrientat(int start, int final);
    void construiesteGrafNeorientat(int start, int final);
    void construiesteGradeInterioare(int start, int final);

    //constructori
    Graf();
    Graf(int noduri, int muchii);

    //functii
    int numaraConexe();
    void dfs(int nodCurent);
    void bfs(int nodStart);
    void sortareTopologica(std::ostream &out);
    void initializareCompBiconexe(ostream &out);
    void componenteBiconexeDfs(int nodCurent, int adancime, stack<int>& mystack);
    void initializareCompTareConexe(ostream &out);
    void componenteTareConexe(int nodCurent, int &pozitie, stack<int>& mystack);
};

Graf::Graf(int noduri, int muchii) : noduri(noduri), muchii(muchii) {}

void Graf::construiesteGrafNeorientat(int start, int final) {
    listaAdiacenta[start].push_back(final);
    listaAdiacenta[final].push_back(start);
}

void Graf::construiesteGrafOrientat(int start, int final) {
    listaAdiacenta[start].push_back(final);
}

void Graf::construiesteGradeInterioare(int start, int final) {
    listaAdiacenta[start].push_back(final);
    gradeInterioare[final]++;
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
void afisareDistante(int nrNoduri, std::ostream &out){
    for(int i=1; i<=nrNoduri; i++)
        out<<distante[i]<<" ";
}

void Graf::bfs(int nodStart) {
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

    ifstream in("ctc.in");
    ofstream out("ctc.out");

    int noduri, muchii, extremitateInitiala, extremitateFinala, nodStart;

    in >> noduri >> muchii;

    /* pentru BFS -> citim si nodul de start */
//    in >> noduri >> muchii >> nodStart;

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
        mygraf.construiesteGrafOrientat(extremitateInitiala, extremitateFinala);
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
//    std::fill(std::begin(vizitate), std::begin(vizitate)+maxim, false);
//    std::fill(std::begin(distante), std::begin(distante)+maxim, -1);

//    mygraf.bfs(nodStart);

//    afisareDistante(noduri, out);

    /* apel DFS */
//    std::fill(std::begin(vizitate), std::begin(vizitate)+maxim, false);

    /* apel Sortare Topologica */
    //mygraf.sortareTopologica(out);

    /* apel Componente Biconexe */
//    std::fill(std::begin(vizitate), std::begin(vizitate)+maxim, false);
//    mygraf.initializareCompBiconexe(out);

    /* apel Componente Tare Conexe */
    mygraf.initializareCompTareConexe(out);

    in.close();
    out.close();
    return 0;
}
