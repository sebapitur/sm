#include <iostream>
#include <bits/stdc++.h>
#include <list>
#include <fstream>
#include <pthread.h>

using namespace std;

#define INTMAX 10000
int num_threads;
int V, numtasks;
int *parent;
int *keys;
char *used;

typedef struct thread_arg thread_arg;

struct thread_arg
{
	int id;
	int index;
	vector<pair<int, int>> *adjacency_list;
};

void* minKey(void * arg)
{
	int minn = INTMAX, min_index = -1;
	int id = *(int *)arg;

	int start = id * (V / num_threads);
	int end = (id + 1) * (V / num_threads);
	for (int v = start; v < end; v++) {
		if (used[v] == 0 && keys[v] < minn) {
			minn = keys[v];
			min_index = v;
		}
	}

	pair<int, int> *index_key = new pair<int, int>();
	index_key->first = min_index;
	index_key->second = minn;
	pthread_exit(index_key);
}

void printMST(int parent[], int keys[])
{
	printf("Edge \tWeight\n");
	for (int i = 1; i < V; i++)
		printf("%d - %d \t %d\n", parent[i], i, keys[i]);
}

void* update_keys(void * arg) 
{
	thread_arg * targ = (thread_arg *) arg;
	vector<pair<int, int>> *adjacency_list = targ->adjacency_list;
	int id = targ->id;
	int index = targ->index;

	if (adjacency_list->size() > V)
		pthread_exit(NULL);

	int part = adjacency_list->size() / num_threads;
	int remainder = adjacency_list->size() % num_threads;

	int start = id * part;
	int end = (id + 1) * part;

	if (id == num_threads - 1) {
		end += remainder;
	}


	for (int i = start; i < end; i++) {
		pair<int, int> neighbor_weight = adjacency_list->at(i);
		int neighbor = neighbor_weight.first;
		int weight = neighbor_weight.second;
		if (used[neighbor] == 0 && weight < keys[neighbor]) {
			parent[neighbor] = index;
			keys[neighbor] = weight;
		}
	}

	pthread_exit(NULL);
}

void primMST(vector<pair<int, int>> **adjacency_list)
{
	pthread_t threads[num_threads];
	int int_args[num_threads];
	thread_arg t_args[num_threads];
	pair<int, int> *ret_vals[num_threads];

	for (int count = 0; count < V - 1; count++) {
		for (int i = 0; i < num_threads; i++) {
			int_args[i] = i;
			pthread_create(&threads[i], NULL, minKey, &int_args[i]);
		}

		for (int i = 0; i < num_threads; i++) {
			pthread_join(threads[i], (void **)&ret_vals[i]);
		}

		int index = -1, minn = INTMAX;
		for (int i = 0; i < num_threads; i++) {
			if (ret_vals[i]->second < minn) {
				minn = ret_vals[i]->second;
				index = ret_vals[i]->first;
				delete ret_vals[i];
			}
		}

		used[index] = 1;
		
		if ((int)adjacency_list[index]->size() > 0 && (int)adjacency_list[index]->size() < V) {
			for (int i = 0; i < num_threads; i++) {
				t_args[i].adjacency_list = adjacency_list[index];
				t_args[i].id = i;
				t_args[i].index = index;

				pthread_create(&threads[i], NULL, update_keys, &t_args[i]);
			}

			for (int i = 0; i < num_threads; i++) {
				pthread_join(threads[i], NULL);
			}
		}
			
	}

	// printMST(parent, keys);
}

int main(int argc, char *argv[])
{
	ifstream fin(argv[1]);
	num_threads = atoi(argv[2]);
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
