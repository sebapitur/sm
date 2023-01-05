#include <iostream>
#include <bits/stdc++.h>
#include <list>
#include <fstream>
#include <omp.h>

using namespace std;

#define INTMAX 10000	
int V, numtasks;
int omp_num_threads;
int *parent;
int *keys;
char *used;

pair<int, int> minKey()
{
	int minn = INTMAX, min_index = -1;

	#pragma omp parallel
    {
        int local_min = INTMAX;
        int local_min_index = -1;

        #pragma omp for nowait
        for (int v = 0; v < V; v++) {
            // cout << omp_get_thread_num() << endl;
            if (used[v] == 0 && keys[v] < local_min) {
                local_min = keys[v];
                local_min_index = v;
            }
        }

         #pragma omp critical 
         {
            if (local_min < minn) {
                minn = local_min;
                min_index = local_min_index;
            }
         }
    }

	pair<int, int> index_key = pair<int, int>();
	index_key.first = min_index;
	index_key.second = minn;

	return index_key;
}

void printMST()
{
	printf("Edge \tWeight\n");
	for (int i = 1; i < V; i++)
		printf("%d - %d \t %d\n", parent[i], i, keys[i]);
}

void update_keys(vector<pair<int, int>> **adjacency_list, pair<int, int> index_key) 
{	
	#pragma omp parallel for
	for (pair<int, int> neighbor_weight : *adjacency_list[index_key.first]) {
		int neighbor = neighbor_weight.first;
		int weight = neighbor_weight.second;

		if (used[neighbor] == 0 && weight < keys[neighbor]) {
			parent[neighbor] = index_key.first;
			keys[neighbor] = weight;
		}
	}
}

void primMST(vector<pair<int, int>> **adjacency_list)
{
	for (int count = 0; count < V - 1; count++) {
		pair<int, int> index_key = minKey();
		used[index_key.first] = 1;
		int minn = index_key.second;

		update_keys(adjacency_list, index_key);
	}


	#pragma omp master
	{
		printMST();
	}
}

int main(int argc, char *argv[])
{

	if (argc != 3) {
		cout << "Correct usage: ./prim_omp <input> <NUM_THREADS>";
		return 0;
	}

	ifstream fin(argv[1]);
	omp_set_num_threads(atoi(argv[2]));
	bool is_root_set = false;
	fin >> V;
	used = (char *)calloc(V, sizeof(char));
	parent = (int *)malloc(V * sizeof(int));
	keys = (int *)malloc(V * sizeof(int));
	memset(keys, INTMAX, sizeof(int) * V);

	vector<pair<int, int>> **adjacency_list = (vector<pair<int, int>> **)malloc(sizeof(list<pair<int, int>> *) *  V);

        for (int i = 0; i < V; i++) {
            int ties;
            fin >> ties;
			adjacency_list[i] = new vector<pair<int, int>>();
            for (int j = 0; j < ties; j++) {
                    pair<int, int> neighbor_weight;
                    fin >> neighbor_weight.first >> neighbor_weight.second;

					if (!is_root_set) {
						keys[i] = 0;
						parent[i] = -1;
						is_root_set = true;
					}

                    adjacency_list[i]->push_back(neighbor_weight);
            }
        }

	fin.close();
	
	// Print the solution
	primMST(adjacency_list);

	return 0;
}
