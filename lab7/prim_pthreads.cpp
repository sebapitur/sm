#include <iostream>
#include <bits/stdc++.h>
#include <list>
#include <fstream>
#include <pthread.h>

using namespace std;

#define INTMAX INT_MAX
#define NUM_THREADS 4
int V, numtasks;

typedef struct thread_arg thread_arg;

struct thread_arg
{
	int *keys;
	char *used;
	int id;
};

void* minKey(void * arg)
{
	int minn = INTMAX, min_index = INTMAX;
	thread_arg * targ = (thread_arg *)arg;
	int *keys = targ->keys;
	char *used = targ->used;
	int id = targ->id;

	int start = id * (V / NUM_THREADS);
	int end = (id + 1) * (V / NUM_THREADS);

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

void primMST(list<pair<int, int>> adjacency_list[])
{
	int parent[V];
	const int const_v = V;
	int keys[V];
	char used[V] = {0};

	memset(parent, -1, V * sizeof(int));
	memset(used, 0, V * sizeof(char));

	// Initialize all keys as INFINITE
	for (int i = 0; i < V; i++) {
		keys[i] = INTMAX;
	}

	keys[0] = 0;
	parent[0] = -1;

	pthread_t threads[NUM_THREADS];
	thread_arg args[NUM_THREADS];
	pair<int, int> *ret_vals[NUM_THREADS];

	for (int count = 0; count < V - 1; count++) {
		for (int i = 0; i < NUM_THREADS; i++) {
			args[i].keys = keys;
			args[i].used = used;
			args[i].id = i;
			pthread_create(&threads[i], NULL, minKey, &args[i]);
		}

		for (int i = 0; i < NUM_THREADS; i++) {
			pthread_join(threads[i], (void **)&ret_vals[i]);
		}

		int index = -1, minn = INTMAX;
		for (int i = 0; i < NUM_THREADS; i++) {
			if (ret_vals[i]->second < minn) {
				minn = ret_vals[i]->second;
				index = ret_vals[i]->first;
				delete ret_vals[i];
			}
		}

		used[index] = 1;

		for (pair<int, int> neighbor_weight : adjacency_list[index]) {
			int neighbor = neighbor_weight.first;
			int weight = neighbor_weight.second;

			if (used[neighbor] == 0 && weight < keys[neighbor]) {
				parent[neighbor] = index;
				keys[neighbor] = weight;
			}
		}
	}

	printMST(parent, keys);
}

int main(int argc, char *argv[])
{
	ifstream fin("input.txt");
	int rank;

	fin >> V;
	
	list<pair<int, int>> adjacency_list[V];	

	for (int i = 0; i < V; i++) {
		int ties;
		fin >> ties;
		for (int j = 0; j < ties; j++) {
			pair<int, int> neighbor_weight;
			fin >> neighbor_weight.first >> neighbor_weight.second;
			adjacency_list[i].push_back(neighbor_weight);
		}
	}
	
	fin.close();
	
	// Print the solution
	primMST(adjacency_list);

	return 0;
}
