#include<mpi.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

#define MASTER1 0
#define MASTER2 1
#define MASTER3 2

int main(int argc, char * argv[]) {
	int rank;
	int nProcesses;
	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nProcesses);

	if (rank == MASTER1) { 
		int nrWorkers; 
		int nrWorkers1;
		int nrWorkers2;
		int *workers;
		int *workers1;
		int *workers2;
		FILE *myFile = fopen("cluster0.txt", "r");
		fscanf(myFile, "%d", &nrWorkers);
		workers = malloc(nrWorkers * sizeof(int));
		for (int i = 0; i < nrWorkers; i++) {
			fscanf(myFile, "%d", &workers[i]);
		}
		MPI_Send(&nrWorkers, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
		printf("M(0,1)\n");
		MPI_Send(workers, nrWorkers, MPI_INT, 1, 0, MPI_COMM_WORLD);
		printf("M(0,1)\n");
		MPI_Send(&nrWorkers, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
		printf("M(0,2)\n");
		MPI_Send(workers, nrWorkers, MPI_INT, 2, 0, MPI_COMM_WORLD);
		printf("M(0,2)\n");
		// primul MASTER trimite topologia sa celorlalti 2 coordonatori
		MPI_Status status1;
		MPI_Recv(&nrWorkers1, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &status1); 
		MPI_Status status2;
		workers1 = malloc(nrWorkers1 * sizeof(int));
		MPI_Recv(workers1, nrWorkers1, MPI_INT, 1, 0, MPI_COMM_WORLD, &status2); 
		MPI_Status status3;
		MPI_Recv(&nrWorkers2, 1, MPI_INT, 2, 0, MPI_COMM_WORLD, &status3); 
		MPI_Status status4;
		workers2 = malloc(nrWorkers2 * sizeof(int));
		MPI_Recv(workers2, nrWorkers2, MPI_INT, 2, 0, MPI_COMM_WORLD, &status4); 
		// primul MASTER primeste de la ceilalti 2 coordonatori topologia fiecaruia 
		char text1[500] = "";
		for (int i = 0; i < nrWorkers; i++) {
			char string[6];
			if (i < nrWorkers - 1) {
				sprintf(string, "%d,", workers[i]);
			}
			else {
				sprintf(string, "%d", workers[i]);
			}
			strcat(text1, string);
		}
		char text2[500] = "";
		for (int i = 0; i < nrWorkers1; i++) {
			char string[6];
			if (i < nrWorkers1 - 1) {
				sprintf(string, "%d,", workers1[i]);
			}
			else {
				sprintf(string, "%d", workers1[i]);
			}
			strcat(text2, string);
		}
		char text3[500] = "";
		for (int i = 0; i < nrWorkers2; i++) {
			char string[6];
			if (i < nrWorkers2 - 1) {
				sprintf(string, "%d,", workers2[i]);
			}
			else {
				sprintf(string, "%d", workers2[i]);
			}
			strcat(text3, string);
		}
		printf("0 -> 0:%s 1:%s 2:%s\n", text1, text2, text3);
		for (int i = 0; i < nrWorkers; i++) {
			MPI_Send(&rank, 1, MPI_INT, workers[i], 0, MPI_COMM_WORLD);
			printf("M(0,%d)\n", workers[i]);
			MPI_Send(&nrWorkers, 1, MPI_INT, workers[i], 0, MPI_COMM_WORLD);
			printf("M(0,%d)\n", workers[i]);
			MPI_Send(workers, nrWorkers, MPI_INT, workers[i], 0, MPI_COMM_WORLD);
			printf("M(0,%d)\n", workers[i]);
			MPI_Send(&nrWorkers1, 1, MPI_INT, workers[i], 0, MPI_COMM_WORLD);
			printf("M(0,%d)\n", workers[i]);
			MPI_Send(workers1, nrWorkers1, MPI_INT, workers[i], 0, MPI_COMM_WORLD);
			printf("M(0,%d)\n", workers[i]);
			MPI_Send(&nrWorkers2, 1, MPI_INT, workers[i], 0, MPI_COMM_WORLD);
			printf("M(0,%d)\n", workers[i]);
			MPI_Send(workers2, nrWorkers2, MPI_INT, workers[i], 0, MPI_COMM_WORLD);
			printf("M(0,%d)\n", workers[i]);
		}
		// coordonatorul trimite rangul sau workerilor corespunzatori 
		// si apoi trimite workerilor intreaga topologie 
		int *v;
		int N = atoi(argv[1]);
		int totalWorkers = nrWorkers + nrWorkers1 + nrWorkers2; 
		v = malloc(N * sizeof(int));
		for (int i = 0; i < N; i++) {
			v[i] = i;
		}
		int elemPerWorker = N / totalWorkers; 
		int rest = N % totalWorkers; 
		int elemPerWorkerRecv;
		int *vectorRecv = malloc(elemPerWorker * nrWorkers * sizeof(int)); 
		int dimvectorRecv1 = elemPerWorker * nrWorkers; 
		// trimit cate elemPerWorker elemente workerilor acestui cluster
		for (int i = 1; i < nrWorkers + 1; i++) {
			MPI_Send(&elemPerWorker, 1, MPI_INT, workers[i - 1], 0, MPI_COMM_WORLD);
			printf("M(0,%d)\n", workers[i - 1]);
			for (int j = (i - 1) * elemPerWorker; j < i * elemPerWorker; j++) {
				MPI_Send(&v[j], 1, MPI_INT, workers[i - 1], 0, MPI_COMM_WORLD);
				printf("M(0,%d)\n", workers[i - 1]);
			}
		}
		// trimit vectorul catre ceilalti 2 coordonatori
		MPI_Send(&N, 1, MPI_INT, MASTER2, 0, MPI_COMM_WORLD);
		printf("M(0,1)\n");
		MPI_Send(v, N, MPI_INT, MASTER2, 0, MPI_COMM_WORLD);
		printf("M(0,1)\n");
		MPI_Send(&elemPerWorker, 1, MPI_INT, MASTER2, 0, MPI_COMM_WORLD);
		printf("M(0,1)\n");
		MPI_Send(&N, 1, MPI_INT, MASTER3, 0, MPI_COMM_WORLD);
		printf("M(0,2)\n");
		MPI_Send(v, N, MPI_INT, MASTER3, 0, MPI_COMM_WORLD);
		printf("M(0,2)\n");
		MPI_Send(&elemPerWorker, 1, MPI_INT, MASTER3, 0, MPI_COMM_WORLD);
		printf("M(0,2)\n");
		MPI_Send(&rest, 1, MPI_INT, MASTER3, 0, MPI_COMM_WORLD);
		printf("M(0,2)\n");
		// primeste vectorii formati de catre proprii lui workeri
		int a = 0;
		for (int i = 0; i < nrWorkers; i++) {
			MPI_Status status5;
			MPI_Recv(&elemPerWorkerRecv, 1, MPI_INT, workers[i], 0, MPI_COMM_WORLD, &status5);
			int *vect = malloc(elemPerWorkerRecv * sizeof(int));
			MPI_Status status6;
			MPI_Recv(vect, elemPerWorkerRecv, MPI_INT, workers[i], 0, MPI_COMM_WORLD, &status6);
			for (int o = 0; o < elemPerWorkerRecv; o++) {
				vectorRecv[a] = vect[o];
				a++;
			}
		}
		int *vectorRecv2;
		int *vectorRecv3;
		int dimvectorRecv2;
		int dimvectorRecv3;
		MPI_Status status7;
		MPI_Recv(&dimvectorRecv2, 1, MPI_INT, MASTER2, 0, MPI_COMM_WORLD, &status7);
		MPI_Status status8;
		MPI_Recv(&dimvectorRecv3, 1, MPI_INT, MASTER3, 0, MPI_COMM_WORLD, &status8);
		vectorRecv2 = malloc(dimvectorRecv2 * sizeof(int));
		vectorRecv3 = malloc(dimvectorRecv3 * sizeof(int));
		MPI_Status status9;
		MPI_Recv(vectorRecv2, dimvectorRecv2, MPI_INT, MASTER2, 0, MPI_COMM_WORLD, &status9);
		MPI_Status status10;
		MPI_Recv(vectorRecv3, dimvectorRecv3, MPI_INT, MASTER3, 0, MPI_COMM_WORLD, &status10);
		int *finalVec = malloc((dimvectorRecv1 + dimvectorRecv2 + dimvectorRecv3) * sizeof(int));
		for (int i = 0; i < dimvectorRecv1; i++) {
			finalVec[i] = vectorRecv[i];
		}
		for (int i = 0; i < dimvectorRecv2; i++) {
			finalVec[i + dimvectorRecv1] = vectorRecv2[i];
		}
		for (int i = 0; i < dimvectorRecv3; i++) {
			finalVec[i + dimvectorRecv1 + dimvectorRecv2] = vectorRecv3[i];
		}
		printf("Rezultat: ");
		for (int i = 0; i < N; i++) {
			if (i != N - 1) {
				printf("%d ", finalVec[i]);
			}
			else {
				printf("%d\n", finalVec[N - 1]);
			}
		}
    }

	if (rank == MASTER2) { 
		int nrWorkers; 
		// procesul 1 
		int nrWorkers1;
		// procesul 0
		int nrWorkers2;
		// procesul 2
		int *workers;
		int *workers1;
		int *workers2;
		int *vector;
		int N;
		int elemPerWorker;
		FILE *myFile = fopen("cluster1.txt", "r");
		fscanf(myFile, "%d", &nrWorkers);
		workers = malloc(nrWorkers * sizeof(int));
		for (int i = 0; i < nrWorkers; i++) {
			fscanf(myFile, "%d", &workers[i]);
		}
		MPI_Send(&nrWorkers, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
		printf("M(1,0)\n");
		MPI_Send(workers, nrWorkers, MPI_INT, 0, 0, MPI_COMM_WORLD);
		printf("M(1,0)\n");
		MPI_Send(&nrWorkers, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
		printf("M(1,2)\n");
		MPI_Send(workers, nrWorkers, MPI_INT, 2, 0, MPI_COMM_WORLD);
		printf("M(1,2)\n");
		// al doilea MASTER trimite topologia sa celorlalti 2 coordonatori
		MPI_Status status1;
		MPI_Recv(&nrWorkers1, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status1); 
		MPI_Status status2;
		workers1 = malloc(nrWorkers1 * sizeof(int));
		MPI_Recv(workers1, nrWorkers1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status2); 
		MPI_Status status3;
		MPI_Recv(&nrWorkers2, 1, MPI_INT, 2, 0, MPI_COMM_WORLD, &status3); 
		MPI_Status status4;
		workers2 = malloc(nrWorkers2 * sizeof(int));
		MPI_Recv(workers2, nrWorkers2, MPI_INT, 2, 0, MPI_COMM_WORLD, &status4); 
		// primul MASTER primeste de la ceilalti 2 coordonatori topologia fiecaruia 
		char text1[500] = "";
		for (int i = 0; i < nrWorkers1; i++) {
			char string[6];
			if (i < nrWorkers1 - 1) {
				sprintf(string, "%d,", workers1[i]);
			}
			else {
				sprintf(string, "%d", workers1[i]);
			}
			strcat(text1, string);
		}
		char text2[500] = "";
		for (int i = 0; i < nrWorkers; i++) {
			char string[6];
			if (i < nrWorkers - 1) {
				sprintf(string, "%d,", workers[i]);
			}
			else {
				sprintf(string, "%d", workers[i]);
			}
			strcat(text2, string);
		}
		char text3[500] = "";
		for (int i = 0; i < nrWorkers2; i++) {
			char string[6];
			if (i < nrWorkers2 - 1) {
				sprintf(string, "%d,", workers2[i]);
			}
			else {
				sprintf(string, "%d", workers2[i]);
			}
			strcat(text3, string);
		}
		printf("1 -> 0:%s 1:%s 2:%s\n", text1, text2, text3);
		for (int i = 0; i < nrWorkers; i++) {
			MPI_Send(&rank, 1, MPI_INT, workers[i], 0, MPI_COMM_WORLD);
			printf("M(1,%d)\n", workers[i]);
			MPI_Send(&nrWorkers1, 1, MPI_INT, workers[i], 0, MPI_COMM_WORLD);
			printf("M(1,%d)\n", workers[i]);
			MPI_Send(workers1, nrWorkers1, MPI_INT, workers[i], 0, MPI_COMM_WORLD);
			printf("M(1,%d)\n", workers[i]);
			MPI_Send(&nrWorkers, 1, MPI_INT, workers[i], 0, MPI_COMM_WORLD);
			printf("M(1,%d)\n", workers[i]);
			MPI_Send(workers, nrWorkers, MPI_INT, workers[i], 0, MPI_COMM_WORLD);
			printf("M(1,%d)\n", workers[i]);
			MPI_Send(&nrWorkers2, 1, MPI_INT, workers[i], 0, MPI_COMM_WORLD);
			printf("M(1,%d)\n", workers[i]);
			MPI_Send(workers2, nrWorkers2, MPI_INT, workers[i], 0, MPI_COMM_WORLD);
			printf("M(1,%d)\n", workers[i]);
		}
		MPI_Status status5;
		MPI_Recv(&N, 1, MPI_INT, MASTER1, 0, MPI_COMM_WORLD, &status5); 
		vector = malloc(N * sizeof(int));
		MPI_Status status6;
		MPI_Recv(vector, N, MPI_INT, MASTER1, 0, MPI_COMM_WORLD, &status6); 
		MPI_Status status7;
		MPI_Recv(&elemPerWorker, 1, MPI_INT, MASTER1, 0, MPI_COMM_WORLD, &status7); 
		for (int i = 1; i < nrWorkers + 1; i++) {
			MPI_Send(&elemPerWorker, 1, MPI_INT, workers[i - 1], 0, MPI_COMM_WORLD);
			printf("M(1,%d)\n", workers[i - 1]);
			for (int j = (nrWorkers1 + i - 1) * elemPerWorker; j < (i + nrWorkers1) * elemPerWorker; j++) {
				MPI_Send(&vector[j], 1, MPI_INT, workers[i - 1], 0, MPI_COMM_WORLD);
				printf("M(1,%d)\n", workers[i - 1]);
			}
		}
		int a = 0;
		int elemPerWorkerRecv;
		int *vectorRecv = malloc(elemPerWorker * nrWorkers * sizeof(int)); 
		int dim = elemPerWorker * nrWorkers; 
		for (int i = 0; i < nrWorkers; i++) {
			MPI_Status status5;
			MPI_Recv(&elemPerWorkerRecv, 1, MPI_INT, workers[i], 0, MPI_COMM_WORLD, &status5);
			int *vect = malloc(elemPerWorkerRecv * sizeof(int));
			MPI_Status status6;
			MPI_Recv(vect, elemPerWorkerRecv, MPI_INT, workers[i], 0, MPI_COMM_WORLD, &status6);
			for (int o = 0; o < elemPerWorkerRecv; o++) {
				vectorRecv[a] = vect[o];
				a++;
			}
		}
		MPI_Send(&dim, 1, MPI_INT, MASTER1, 0, MPI_COMM_WORLD);
		printf("M(1,0)\n");
		MPI_Send(vectorRecv, dim, MPI_INT, MASTER1, 0, MPI_COMM_WORLD);	
		printf("M(1,0)\n");
    }

	if (rank == MASTER3) { 
		int nrWorkers; 
		// procesul 2
		int nrWorkers1;
		// procesul 0
		int nrWorkers2;
		// procesul 1
		int *workers;
		int *workers1;
		int *workers2;
		int N;
		int *vector;
		int elemPerWorker; 
		int rest;
		int lastWorkerElem; 
		FILE *myFile = fopen("cluster2.txt", "r");
		fscanf(myFile, "%d", &nrWorkers);
		workers = malloc(nrWorkers * sizeof(int));
		for (int i = 0; i < nrWorkers; i++) {
			fscanf(myFile, "%d", &workers[i]);
		}
		MPI_Send(&nrWorkers, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
		printf("M(2,0)\n");
		MPI_Send(workers, nrWorkers, MPI_INT, 0, 0, MPI_COMM_WORLD);
		printf("M(2,0)\n");
		MPI_Send(&nrWorkers, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
		printf("M(2,1)\n");
		MPI_Send(workers, nrWorkers, MPI_INT, 1, 0, MPI_COMM_WORLD);
		printf("M(2,1)\n");
		// al treilea MASTER trimite topologia sa celorlalti 2 coordonatori
		MPI_Status status1;
		MPI_Recv(&nrWorkers1, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status1); 
		MPI_Status status2;
		workers1 = malloc(nrWorkers1 * sizeof(int));
		MPI_Recv(workers1, nrWorkers1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status2); 
		MPI_Status status3;
		MPI_Recv(&nrWorkers2, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &status3); 
		MPI_Status status4;
		workers2 = malloc(nrWorkers2 * sizeof(int));
		MPI_Recv(workers2, nrWorkers2, MPI_INT, 1, 0, MPI_COMM_WORLD, &status4); 
		// primul MASTER primeste de la ceilalti 2 coordonatori topologia fiecaruia 
		char text1[500] = "";
		for (int i = 0; i < nrWorkers1; i++) {
			char string[6];
			if (i < nrWorkers1 - 1) {
				sprintf(string, "%d,", workers1[i]);
			}
			else {
				sprintf(string, "%d", workers1[i]);
			}
			strcat(text1, string);
		}
		char text2[500] = "";
		for (int i = 0; i < nrWorkers2; i++) {
			char string[6];
			if (i < nrWorkers2 - 1) {
				sprintf(string, "%d,", workers2[i]);
			}
			else {
				sprintf(string, "%d", workers2[i]);
			}
			strcat(text2, string);
		}
		char text3[500] = "";
		for (int i = 0; i < nrWorkers; i++) {
			char string[6];
			if (i < nrWorkers - 1) {
				sprintf(string, "%d,", workers[i]);
			}
			else {
				sprintf(string, "%d", workers[i]);
			}
			strcat(text3, string);
		}
		printf("2 -> 0:%s 1:%s 2:%s\n", text1, text2, text3);
		for (int i = 0; i < nrWorkers; i++) {
			MPI_Send(&rank, 1, MPI_INT, workers[i], 0, MPI_COMM_WORLD);
			printf("M(2,%d)\n", workers[i]);
			MPI_Send(&nrWorkers1, 1, MPI_INT, workers[i], 0, MPI_COMM_WORLD);
			printf("M(2,%d)\n", workers[i]);
			MPI_Send(workers1, nrWorkers1, MPI_INT, workers[i], 0, MPI_COMM_WORLD);
			printf("M(2,%d)\n", workers[i]);
			MPI_Send(&nrWorkers2, 1, MPI_INT, workers[i], 0, MPI_COMM_WORLD);
			printf("M(2,%d)\n", workers[i]);
			MPI_Send(workers2, nrWorkers2, MPI_INT, workers[i], 0, MPI_COMM_WORLD);
			printf("M(2,%d)\n", workers[i]);
			MPI_Send(&nrWorkers, 1, MPI_INT, workers[i], 0, MPI_COMM_WORLD);
			printf("M(2,%d)\n", workers[i]);
			MPI_Send(workers, nrWorkers, MPI_INT, workers[i], 0, MPI_COMM_WORLD);
			printf("M(2,%d)\n", workers[i]);
		}
		MPI_Status status5;
		MPI_Recv(&N, 1, MPI_INT, MASTER1, 0, MPI_COMM_WORLD, &status5); 
		vector = malloc(N * sizeof(int));
		MPI_Status status6;
		MPI_Recv(vector, N, MPI_INT, MASTER1, 0, MPI_COMM_WORLD, &status6); 
		MPI_Status status7;
		MPI_Recv(&elemPerWorker, 1, MPI_INT, MASTER1, 0, MPI_COMM_WORLD, &status7); 
		MPI_Status status8;
		MPI_Recv(&rest, 1, MPI_INT, MASTER1, 0, MPI_COMM_WORLD, &status8); 
		lastWorkerElem = rest + elemPerWorker;
		for (int i = 1; i < nrWorkers + 1; i++) {
			MPI_Send(&elemPerWorker, 1, MPI_INT, workers[i - 1], 0, MPI_COMM_WORLD);
			printf("M(2,%d)\n", workers[i - 1]);
			if (i != nrWorkers) {
				for (int j = (nrWorkers1 + nrWorkers2 + i - 1) * elemPerWorker; j < (i + nrWorkers1 + nrWorkers2) * elemPerWorker; j++) {
					MPI_Send(&vector[j], 1, MPI_INT, workers[i - 1], 0, MPI_COMM_WORLD);
					printf("M(2,%d)\n", workers[i - 1]);
				}
			}
			else {
				MPI_Send(&lastWorkerElem, 1, MPI_INT, workers[nrWorkers - 1], 0, MPI_COMM_WORLD);
				printf("M(2,%d)\n", workers[nrWorkers - 1]);
				for (int j = (nrWorkers + nrWorkers1 + nrWorkers2 - 1) * elemPerWorker; j < N; j++) {
					MPI_Send(&vector[j], 1, MPI_INT, workers[i - 1], 0, MPI_COMM_WORLD);
					printf("M(2,%d)\n", workers[i - 1]);
				}
			}
		}

		int a = 0;
		int elemPerWorkerRecv;
		int *vectorRecv = malloc((elemPerWorker * nrWorkers + rest) * sizeof(int)); 
		int dim = elemPerWorker * nrWorkers + rest;
		for (int i = 0; i < nrWorkers; i++) {
			MPI_Status status5;
			MPI_Recv(&elemPerWorkerRecv, 1, MPI_INT, workers[i], 0, MPI_COMM_WORLD, &status5);
			int *vect = malloc(elemPerWorkerRecv * sizeof(int));
			MPI_Status status6;
			MPI_Recv(vect, elemPerWorkerRecv, MPI_INT, workers[i], 0, MPI_COMM_WORLD, &status6);
			for (int o = 0; o < elemPerWorkerRecv; o++) {
				vectorRecv[a] = vect[o];
				a++;
			}
		}
		MPI_Send(&dim, 1, MPI_INT, MASTER1, 0, MPI_COMM_WORLD);
		printf("M(2,0)\n");
		MPI_Send(vectorRecv, dim, MPI_INT, MASTER1, 0, MPI_COMM_WORLD);
		printf("M(2,0)\n");
    }

	if ((rank != MASTER1) && (rank != MASTER2) && (rank != MASTER3)){
		int coord_rank;
		int MASTER1_nrWorkers; 
		int MASTER2_nrWorkers; 
		int MASTER3_nrWorkers; 
		int *MASTER1_workers;
		int *MASTER2_workers;
		int *MASTER3_workers;
		int *vector;
		int elemPerWorker; 
		int lastWorkerElem;
		MPI_Status status1;
		MPI_Recv(&coord_rank, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status1); 
		MPI_Status status2;
		MPI_Recv(&MASTER1_nrWorkers, 1, MPI_INT, coord_rank, 0, MPI_COMM_WORLD, &status2); 
		MASTER1_workers = malloc(MASTER1_nrWorkers * sizeof(int));
		MPI_Status status3;
		MPI_Recv(MASTER1_workers, MASTER1_nrWorkers, MPI_INT, coord_rank, 0, MPI_COMM_WORLD, &status3); 
		MPI_Status status4;
		MPI_Recv(&MASTER2_nrWorkers, 1, MPI_INT, coord_rank, 0, MPI_COMM_WORLD, &status4); 
		MASTER2_workers = malloc(MASTER2_nrWorkers * sizeof(int));
		MPI_Status status5;
		MPI_Recv(MASTER2_workers, MASTER2_nrWorkers, MPI_INT, coord_rank, 0, MPI_COMM_WORLD, &status5); 
		MPI_Status status6;
		MPI_Recv(&MASTER3_nrWorkers, 1, MPI_INT, coord_rank, 0, MPI_COMM_WORLD, &status6); 
		MASTER3_workers = malloc(MASTER3_nrWorkers * sizeof(int));
		MPI_Status status7;
		MPI_Recv(MASTER3_workers, MASTER3_nrWorkers, MPI_INT, coord_rank, 0, MPI_COMM_WORLD, &status7); 
		// worker-ul afiseaza topologia
		char text1[500] = "";
		for (int i = 0; i < MASTER1_nrWorkers; i++) {
			char string[6];
			if (i < MASTER1_nrWorkers - 1) {
				sprintf(string, "%d,", MASTER1_workers[i]);
			}
			else {
				sprintf(string, "%d", MASTER1_workers[i]);
			}
			strcat(text1, string);
		}
		char text2[500] = "";
		for (int i = 0; i < MASTER2_nrWorkers; i++) {
			char string[6];
			if (i < MASTER2_nrWorkers - 1) {
				sprintf(string, "%d,", MASTER2_workers[i]);
			}
			else {
				sprintf(string, "%d", MASTER2_workers[i]);
			}
			strcat(text2, string);
		}
		char text3[500] = "";
		for (int i = 0; i < MASTER3_nrWorkers; i++) {
			char string[6];
			if (i < MASTER3_nrWorkers - 1) {
				sprintf(string, "%d,", MASTER3_workers[i]);
			}
			else {
				sprintf(string, "%d", MASTER3_workers[i]);
			}
			strcat(text3, string);
		}
		printf("%d -> 0:%s 1:%s 2:%s\n", rank, text1, text2, text3);
		if ((coord_rank == MASTER1) || (coord_rank == MASTER2)) {
			MPI_Status status8;
			MPI_Recv(&elemPerWorker, 1, MPI_INT, coord_rank, 0, MPI_COMM_WORLD, &status8); 
			vector = malloc(elemPerWorker * sizeof(int));
			for (int k = 0; k < elemPerWorker; k++) {
				MPI_Status status9;
				MPI_Recv(&vector[k], 1, MPI_INT, coord_rank, 0, MPI_COMM_WORLD, &status9); 
				vector[k] *= 2; 
			}
			// trimit vectorul format catre coordonatorul cluster-ului
			MPI_Send(&elemPerWorker, 1, MPI_INT, coord_rank, 0, MPI_COMM_WORLD);
			printf("M(%d,%d)\n", rank, coord_rank);
			MPI_Send(vector, elemPerWorker, MPI_INT, coord_rank, 0, MPI_COMM_WORLD);
			printf("M(%d,%d)\n", rank, coord_rank);
		}
		if (coord_rank == MASTER3) {
			if (rank != MASTER3_workers[MASTER3_nrWorkers - 1]) {
				MPI_Status status8;
				MPI_Recv(&elemPerWorker, 1, MPI_INT, coord_rank, 0, MPI_COMM_WORLD, &status8); 
				vector = malloc(elemPerWorker * sizeof(int));
				for (int k = 0; k < elemPerWorker; k++) {
					MPI_Status status9;
					MPI_Recv(&vector[k], 1, MPI_INT, coord_rank, 0, MPI_COMM_WORLD, &status9); 
					vector[k] *= 2; 
				}
				MPI_Send(&elemPerWorker, 1, MPI_INT, coord_rank, 0, MPI_COMM_WORLD);
				printf("M(%d,%d)\n", rank, coord_rank);
				MPI_Send(vector, elemPerWorker, MPI_INT, coord_rank, 0, MPI_COMM_WORLD);
				printf("M(%d,%d)\n", rank, coord_rank);
			}
			else {
				MPI_Status status8;
				MPI_Recv(&elemPerWorker, 1, MPI_INT, coord_rank, 0, MPI_COMM_WORLD, &status8); 
				MPI_Status status9;
				MPI_Recv(&lastWorkerElem, 1, MPI_INT, coord_rank, 0, MPI_COMM_WORLD, &status9); 
				vector = malloc(lastWorkerElem * sizeof(int));
				for (int k = 0; k < lastWorkerElem; k++) {
					MPI_Status status10;
					MPI_Recv(&vector[k], 1, MPI_INT, coord_rank, 0, MPI_COMM_WORLD, &status10); 
					vector[k] *= 2; 
				}	
				MPI_Send(&lastWorkerElem, 1, MPI_INT, coord_rank, 0, MPI_COMM_WORLD);
				printf("M(%d,%d)\n", rank, coord_rank);
				MPI_Send(vector, lastWorkerElem, MPI_INT, coord_rank, 0, MPI_COMM_WORLD);
				printf("M(%d,%d)\n", rank, coord_rank);
			}
		}
	}

MPI_Finalize();
    return 0;
}   
    
