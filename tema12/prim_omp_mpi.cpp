#include <iostream>
#include <bits/stdc++.h>
#include <list>
#include <fstream>
#include <mpi.h>
#include <omp.h>

using namespace std;

#define INTMAX INT_MAX
#define MASTER 0
int V, numtasks;
int *parent;
int *keys;
char *used;



pair<int, int> minKey(int *keys_arg, char *used_arg, int V)
{
	int minn = INTMAX, min_index = -1;
    omp_set_num_threads(2);

    #pragma omp parallel
    {
        int local_min = INTMAX;
        int local_min_index = -1;

        #pragma omp for nowait
        for (int v = 0; v < V; v++) {
            // cout << omp_get_thread_num() << endl;
            if (used_arg[v] == 0 && keys_arg[v] < local_min) {
                local_min = keys_arg[v];
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

void printMST(int parent[], int keys[])
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

void primMST(vector<pair<int, int>> **adjacency_list, int rank)
{
	// Initialize all keys as INFINITE

	int part = V / numtasks;

	if (rank == MASTER) {
		memset(parent, -1, V * sizeof(int));
	}

	int *scattered_keys = (int *)malloc(sizeof(int) * part);
	char *scattered_used = (char *)calloc(part, sizeof(char));


    int mins[numtasks];

	MPI_Scatter(keys, part, MPI_INT, scattered_keys, part, MPI_INT, MASTER, MPI_COMM_WORLD);

	for (int count = 0; count < V - 1; count++) {
		pair<int, int> index_key = minKey(scattered_keys, scattered_used, part);
		int minn = index_key.second;

		MPI_Gather(&minn, 1, MPI_INT, mins, 1, MPI_INT, MASTER, MPI_COMM_WORLD);

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
				MPI_Send(&truee, 1, MPI_CHAR, dest, 0, MPI_COMM_WORLD);
			} else {
				scattered_used[used_index] = 1;
			}


			for (int i = 1; i < numtasks; i++) {
				if (i != dest) {
					char falsee = 0;
					MPI_Send(&falsee, 1, MPI_CHAR, i, 0, MPI_COMM_WORLD);
				}
			}

			if (dest != 0) {
				MPI_Recv(&used_index, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			}

			used[dest * part + used_index] = 1;
			index_key.first = dest * part + used_index;
			update_keys(adjacency_list, index_key);
		} else {
			char modify;
			MPI_Recv(&modify, 1, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

			if (modify == 1) {
				scattered_used[index_key.first] = 1;
				int used_index = index_key.first;
				MPI_Send(&used_index, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD);
			}
		}

		MPI_Scatter(keys, part, MPI_INT, scattered_keys, part, MPI_INT, MASTER, MPI_COMM_WORLD);
	}


	if (rank == MASTER) {
		// printMST(parent, keys);
	}
}

int main(int argc, char *argv[])
{
	ifstream fin(argv[1]);
	int rank;
	bool is_root_set = false;
	fin >> V;
	used = (char *)calloc(V, sizeof(char));
	parent = (int *)malloc(V * sizeof(int));
	keys = (int *)malloc(V * sizeof(int));
	vector<pair<int, int>> **adjacency_list = (vector<pair<int, int>> **)malloc(sizeof(list<pair<int, int>> *) *  V);
	
	MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int part = V / numtasks;
	int *scattered_keys = (int *)malloc(sizeof(int) * part);

	for (int i = 0; i < part; i++) {
		scattered_keys[i] = INTMAX;
	}

	MPI_Gather(scattered_keys, part, MPI_INT, keys, part, MPI_INT, MASTER, MPI_COMM_WORLD);

	if (rank == MASTER) {
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
	}

	fin.close();
	
	// Print the solution
	primMST(adjacency_list, rank);

	MPI_Finalize();
	return 0;
}
