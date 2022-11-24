#include <iostream>
#include <bits/stdc++.h>
#include <list>
#include <fstream>
#include <mpi.h>

using namespace std;

#define INTMAX INT_MAX
#define MASTER 0
int V, numtasks;


pair<int, int> minKey(int *keys, char *used, int V)
{
	int minn = INTMAX, min_index = INTMAX;

	// cout << "minKeyIndex=" << min_index <<  " " << "minKeyVal=" << minn << endl;
	for (int v = 0; v < V; v++) {
		if (used[v] == 0 && keys[v] < minn) {
			minn = keys[v];
			min_index = v;
		}
	}

	pair<int, int> index_key = pair<int, int>();
	index_key.first = min_index;
	// cout << "minKeyIndex=" << min_index <<  " " << "minKeyVal=" << minn << endl;
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

	// Initialize all keys as INFINITE


	int part = V / numtasks;

	if (rank == MASTER) {
		// cout << "part=" << part << endl;
		// cout << "V=" << V << endl;
		memset(parent, -1, V * sizeof(int));
	}

	int scattered_keys[part];
	char scattered_used[part];
    int mins[numtasks];
	memset(scattered_used, 0, part);

	for (int i = 0; i < part; i++) {
		scattered_keys[i] = INTMAX;
	}

	MPI_Gather(scattered_keys, part, MPI_INT, keys, part, MPI_INT, MASTER, MPI_COMM_WORLD);

	if (rank == MASTER) {
		scattered_keys[0] = 0;
		parent[0] = -1;
		// cout << "finish initialize\n";
	}

	for (int count = 0; count < V - 1; count++) {
		// if (rank == MASTER) {
		// 	cout << "scattered keys\n";
		// 	for (int i = 0; i < V; i++) {
		// 		cout << keys[i] << " ";
		// 	}

		// 	cout << endl;
		// }

		pair<int, int> index_key = minKey(scattered_keys, scattered_used, part);
		int minn = index_key.second;

		MPI_Gather(&minn, 1, MPI_INT, mins, 1, MPI_INT, MASTER, MPI_COMM_WORLD);
		
		// if (rank == MASTER) {
		// 	cout << "gathered keys\n";
		// 	for (int i = 0; i < numtasks; i++) {
		// 		cout << mins[i] << " ";
		// 	}

		// 	cout << endl;
		// }
		// MPI_Barrier(MPI_COMM_WORLD);

		if (rank == MASTER) {
			int global_min = INTMAX;
			int dest = 0;
			for (int i = 0; i < numtasks; i++) {
				if (mins[i] <  global_min) {
					dest = i;
					global_min = mins[i];
				}
			}

			int used_index = index_key.first;

			if (dest != 0) {
				char truee = 1;
				// cout << "send master" << endl;
				MPI_Send(&truee, 1, MPI_CHAR, dest, 0, MPI_COMM_WORLD);
			}


			for (int i = 1; i < numtasks; i++) {
				if (i != dest) {
					char falsee = 0;
					MPI_Send(&falsee, 1, MPI_CHAR, i, 0, MPI_COMM_WORLD);
					// cout << "send master 2\n";
				}
			}

			if (dest != 0) {
				MPI_Recv(&used_index, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				// cout << "recv master" << endl;
			}

			used[dest * part + used_index] = 1;
			// cout << "dest=" << dest << " " << "part=" << part << " " << "used_index=" << used_index << " " << endl;
			for (pair<int, int> neighbor_weight : adjacency_list[dest * part + used_index]) {
				int neighbor = neighbor_weight.first;
				int weight = neighbor_weight.second;

				// cout << neighbor << endl;  
				// cout << "used[neighbor]=" << (char)used[neighbor] + '0' << " " << "keys[neighbor]=" << keys[neighbor] <<  endl;    
				if (used[neighbor] == 0 && weight < keys[neighbor]) {
					parent[neighbor] = dest * part + used_index;
					keys[neighbor] = weight;
					// cout << "parent" << parent[neighbor] << " " << keys[neighbor] <<  endl;
					// cout << "weight=" << weight << endl;
				}
			}
		} else {
			char modify;
			MPI_Recv(&modify, 1, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			// cout << "recv worker " <<  modify + '0' << endl;

			if (modify == 1) {
				// cout << "index_key.first" << index_key.first << endl;
				scattered_used[index_key.first] = 1;
				int used_index = index_key.first;
				MPI_Send(&used_index, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD);
				// cout << "send worker" << endl;
			}
		}

		// cout << "count=" << count << endl;
		MPI_Scatter(keys, part, MPI_INT, scattered_keys, part, MPI_INT, MASTER, MPI_COMM_WORLD);
	}


	if (rank == MASTER) {
		printMST(parent, keys);
	}
}

int main(int argc, char *argv[])
{
	ifstream fin("input.txt");
	int rank;

	fin >> V;
	
	list<pair<int, int>> adjacency_list[V];
	
	MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	

	if (rank == MASTER) {
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

	MPI_Finalize();
	return 0;
}
