#include <fstream>
#include <vector>

 

using namespace std;

 

// fisiere de intrare si iesire
ifstream f("in.txt");
ofstream g("out.txt");

 

// stari pentru comutatorul elementar
enum SWITCH_TYPE {
    UNCONFIGURED,
    STRAIGHT,
    CROSS
};

 

int nr_test;                        /* numarul de teste */
int N;                                /* dimensiunea retelei Benes */
int h;                                /* numar blocuri per etaj in reteaua Benes - numar linii in matricea de stari */
int w;                                /* numar etaje in reteaua Benes - numar coloane in matricea de stari */
vector<int> p;                        /* vectorul permutare directa */
vector<int> p_inv;                    /* vectorul permutare inversa */
vector<vector<SWITCH_TYPE>> benes;     /* matrice de stari ale comutatoarelor din reteaua Benes */

 

/**
 * functie de verificare daca un numar dat ca parametru este de forma 2^k
 * returneaza 1 daca numarul este putere a lui 2
 * 0, altfel
 */
inline bool is_power_of_two(int n) {
    bool ret = !(n == 0) && !(n & (n - 1));
    return ret;
}

 

// functie ce intoarce numarul de biti necesari pentru reprezentarea in baza 2 a unui numar zecimal
inline int nr_bits(int n) {
    int cnt = 0;
    while (n) {
        cnt++;
        n >>= 1;
    }
    return cnt;
}

 

// functie ce intoarce elementul y care face parte alaturi de x intr-un comutator
inline int dual(int x) {
    return x ^ 1;
}

 

vector<int> inv_permutation(vector<int> p) {
    int n = p.size();
    vector<int> p_inv(n);

 

    for (int i = 0; i < n; ++i) {
        p_inv[p[i]] = i;
    }

 

    return p_inv;
}

 

void print_topology(vector<vector<SWITCH_TYPE>>& topology) {
    for (int i = 0; i < topology[0].size(); ++i) {
        g << "Etajul " << i << ":\n";
        for (int j = 0; j < topology.size(); ++j) {
            g << "Blocul " << j << " ";
            if (topology[j][i] == 1)
                if (j != topology.size() - 1) {
                    g << "Straight -> ";
                }

 

                else {
                    g << "Straight";
                }

 

            else
                if (j != topology.size() - 1) {
                    g << "Cross -> ";
                }

 

                else {
                    g << "Cross";
                }
        }
        g << "\n";
        g << "----------------------------------------------\n";
    }

 

    g << "###################################################\n";
}

 

/**
  * functie recursiva de construire a retelei conform explicatiilor oferite la laborator
  * @param topology - topologia din reteaua Benes
  * @param p - vectorul de permutare
  */
void benes_routing(vector<vector<SWITCH_TYPE>>& topology, vector<int> p) {
    int h = topology.size();
    int w = topology[0].size();
    int n = 2 * h;
    int k = (w + 1) / 2;
    vector<int> p_inv = inv_permutation(p);
    vector<pair<int, int>> C1, C2; // multimi de permutare pentru partea de input si output in retea
    vector<int> visited(n); // vector de indecsi vizitati

 

    // caz de baza
    if (k == 1) {
        if (p[0] == 0) {
            topology[0][0] = STRAIGHT;
        }
        else {
            topology[0][0] = CROSS;
        }
        return;
    }

 

    for (int idx = 0; idx < n; ++idx) {
        while (visited[idx] == 0) {
            visited[idx] = 1;
            int a = p[idx];
            int switch_id_input = idx >> 1;
            int switch_id_output = a >> 1;
            topology[switch_id_input][0] = idx & 1 ? CROSS : STRAIGHT;
            topology[switch_id_output][w - 1] = a & 1 ? CROSS : STRAIGHT;

 

            int b = dual(a);
            C1.push_back(make_pair(idx, a));
            int c = p_inv[b];
            visited[c] = 1;
            C2.push_back(make_pair(c, b));
            idx = dual(c);
        }
    }

 

    // generam permutarile de input si ouput pentru apelurile recursive
    vector<int> P1(n / 2);
    for (auto p : C1) {
        P1[p.first / 2] = p.second / 2;
    }

 

    vector<int> P2(n / 2);
    for (auto p : C2) {
        P2[p.first / 2] = p.second / 2;
    }

 

    // determinam dimensiunile pentru cele doua topologii - inferioara si superioara
    int half_k = k - 1;
    int half_n = 1 << half_k;
    int half_h = half_n / 2;
    int half_w = 2 * half_k - 1;

 

    vector<vector<SWITCH_TYPE>> upper(half_h);
    vector<vector<SWITCH_TYPE>> lower(half_h);

 

    for (auto& row : upper) {
        row.resize(half_w);
    }

 

    for (auto& row : lower) {
        row.resize(half_w);
    }

 

    benes_routing(upper, P1);
    benes_routing(lower, P2);

 

    // aducem rezultatele in topologia initiala
    for (int i = 0; i < h; ++i) {
        for (int j = 1; j < w - 1; ++j) {
            if (i < h / 2) {
                topology[i][j] = upper[i][j - 1];
            }
            else {
                topology[i][j] = lower[i - h / 2][j - 1];
            }
        }
    }
}

 

int main()
{
    int i, j;
    f >> nr_test;
    for (i = 0; i < nr_test; ++i) {
        g << "Rezultatul testului " << i << ":\n";
        f >> N;
        if (!is_power_of_two(N)) {
            g << "Dimensiunea retelei Benes nu este putere de 2!\n";
            exit(0);
        }

 

        p.resize(N);
        p_inv.resize(N);
        w = 2 * nr_bits(N) - 3;
        h = N >> 1;
        benes.resize(h);
        for (auto& row : benes) {
            row.resize(w);
        }

 

        for (j = 0; j < N; ++j) {
            /**
             * sursa reprezinta vectorul de indecsi al permutarii
             * avem valorile consecutive de la 0 la N - 1
             */
            f >> p[j];
            if ((p[j] >= N)) {
                g << "Valorile permutarii trebuie sa fie numere mai mici decat " << N << "\n";
                exit(0);
            }
            p_inv[p[i]] = i;
        }

 

        benes_routing(benes, p);
        print_topology(benes);
    }

 

    return 0;
}