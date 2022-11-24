#include <iostream>
#include <bits/stdc++.h>
#include <list>
#include <fstream>
#include <omp.h>

using namespace std;

#define INTMAX INT_MAX
int V, numtasks;


pair<int, int> minKey(int *keys, char *used)
{
	int minn = INTMAX, min_index = INTMAX;

	#pragma omp parallel for
	for (int v = 0; v < V; v++) {
		if (used[v] == 0 && keys[v] < minn) {
			minn = keys[v];
			min_index = v;
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

void primMST(list<pair<int, int>> adjacency_list[], int rank)
{
	int parent[V];
	const int const_v = V;
	int keys[V];
	char used[V] = {0};

	#pragma omp master 
	{
		memset(parent, -1, V * sizeof(int));
		memset(used, 0, V * sizeof(char));
	}

	// Initialize all keys as INFINITE
	#pragma omp parallel for
	for (int i = 0; i < V; i++) {
		keys[i] = INTMAX;
	}

	#pragma omp master 
	{
		keys[0] = 0;
		parent[0] = -1;
	}

	

	for (int count = 0; count < V - 1; count++) {
		pair<int, int> index_key = minKey(keys, used);
		used[index_key.first] = 1;
		int minn = index_key.second;

		for (pair<int, int> neighbor_weight : adjacency_list[index_key.first]) {
			int neighbor = neighbor_weight.first;
			int weight = neighbor_weight.second;

			if (used[neighbor] == 0 && weight < keys[neighbor]) {
				parent[neighbor] = index_key.first;
				keys[neighbor] = weight;
			}
		}
	}


	#pragma omp master
	{
		printMST(parent, keys);
	}
}

int main(int argc, char *argv[])
{
	ifstream fin("input.txt");
	int rank;

	fin >> V;
	
	list<pair<int, int>> adjacency_list[V];	

	#pragma omp master 
	{
		for (int i = 0; i < V; i++) {
			int ties;
			fin >> ties;
			for (int j = 0; j < ties; j++) {
				pair<int, int> neighbor_weight;
				fin >> neighbor_weight.first >> neighbor_weight.second;
				adjacency_list[i].push_back(neighbor_weight);
			}
		}
	}
	


	fin.close();
	
	// Print the solution
	primMST(adjacency_list, rank);

	return 0;
}
