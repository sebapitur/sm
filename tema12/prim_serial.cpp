#include <iostream>
#include <bits/stdc++.h>
#include <list>
#include <fstream>

using namespace std;

#define INTMAX 10000

int V, numtasks;
int *parent;
int *keys;
char *used;

pair<int, int> minKey(int *keys, char *used)
{
	int minn = INTMAX, min_index = INTMAX;
	bool no_used = true;
	bool no_min = true;

	for (int v = 0; v < V; v++) {
		if (used[v] == 0) {
			no_used = false;
			if (keys[v] < minn) {
				minn = keys[v];
				min_index = v;
				no_min = false;
			}
		}
	}

	pair<int, int> index_key = pair<int, int>();
	index_key.first = min_index;
	index_key.second = minn;

	return index_key;
}

void printMST(int parent[], int keys[])
{
	printf("Edge \tWeight\n");
	for (int i = 1; i < V; i++)
		printf("%d - %d \t %d\n", parent[i], i, keys[i]);
}

void update_keys(vector<pair<int, int>> **adjacency_list, pair<int, int> index_key) 
{
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
		pair<int, int> index_key = minKey(keys, used);
		int minn = index_key.second;
        used[index_key.first] = 1;

		update_keys(adjacency_list, index_key);
	}

    // printMST(parent, keys);
}

int main(int argc, char **argv)
{
	ifstream fin(argv[1]);
	
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