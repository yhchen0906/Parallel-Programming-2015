#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define MAXPOINTS 1000000
#define MAXSTEPS 1000000
#define MINPOINTS 20
#define PI 3.14159265

void check_param(void);
void printfinal(void);

int nsteps, tpoints;
float  values[MAXPOINTS+2];

void check_param(void) {
	char tchar[20];
	while ((tpoints < MINPOINTS) || (tpoints > MAXPOINTS)) {
		printf("Enter number of points along vibrating string [%d-%d]: "
				,MINPOINTS, MAXPOINTS);
		scanf("%s", tchar);
		tpoints = atoi(tchar);
		if ((tpoints < MINPOINTS) || (tpoints > MAXPOINTS))
			printf("Invalid. Please enter value between %d and %d\n", 
					MINPOINTS, MAXPOINTS);
	}
	while ((nsteps < 1) || (nsteps > MAXSTEPS)) {
		printf("Enter number of time steps [1-%d]: ", MAXSTEPS);
		scanf("%s", tchar);
		nsteps = atoi(tchar);
		if ((nsteps < 1) || (nsteps > MAXSTEPS))
			printf("Invalid. Please enter value between 1 and %d\n", MAXSTEPS);
	}
	printf("Using points = %d, steps = %d\n", tpoints, nsteps);
}

__global__ void wave(float* oldval_d, float* values_d, float* newval_d, int nsteps, int tpoints) {
	int idx = threadIdx.x;
	float x, fac = 2.0 * PI, k = idx, tmp = tpoints - 1;
	x = k / tmp;
	values_d[idx] = sin(fac * x);
	float dtime = 0.3, c = 1.0, dx = 1.0;
	float tau = c * dtime / dx;
	float sqtau = tau * tau;
	oldval_d[idx] = values_d[idx];
	for (int i = 0; i < nsteps; i++) {
		newval_d[idx] = (2.0 * values_d[idx]) - oldval_d[idx] + (sqtau * (-2.0) * values_d[idx]);
		oldval_d[idx] = values_d[idx];
		values_d[idx] = newval_d[idx];
	}
}

void printfinal() {
	for (int i = 0; i < tpoints; i++) {
		printf("%6.4f ", values[i]);
		if (i % 10 == 9)
			printf("\n");
	}
}

int main(int argc, char *argv[]) {
	float *oldval_d, *values_d, *newval_d;
	sscanf(argv[1],"%d",&tpoints);
	sscanf(argv[2],"%d",&nsteps);
	check_param();
	printf("Initializing points on the line...\n");
	printf("Updating all points for all time steps...\n");
	cudaMalloc((void**)&oldval_d, sizeof(float) * tpoints);
	cudaMalloc((void**)&values_d, sizeof(float) * tpoints);
	cudaMalloc((void**)&newval_d, sizeof(float) * tpoints);
	wave<<<1, tpoints>>>(oldval_d, values_d, newval_d, nsteps, tpoints);
	cudaMemcpy(values, values_d, sizeof(float) * tpoints, cudaMemcpyDeviceToHost);
	cudaFree(oldval_d);
	cudaFree(values_d);
	cudaFree(newval_d);
	printf("Printing final results...\n");
	printfinal();
	printf("\nDone.\n\n");
	return 0;
}
