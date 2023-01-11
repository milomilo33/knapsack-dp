#include <stdio.h>
#include <omp.h>

int max(int a, int b) { return (a > b) ? a : b; }

int knapsack_bottom_up_seq(int capacity, int weights[], int values[], int num_items)
{
	int i, w;
	int table[num_items+1][capacity+1];

	for (i = 0; i <= num_items; i++) {
		for (w = 0; w <= capacity; w++) {
			if (i == 0 || w == 0)
				table[i][w] = 0;
			else if (weights[i-1] <= w)
				table[i][w] = max(table[i-1][w], table[i-1][w-weights[i-1]] + values[i-1]);
			else
				table[i][w] = table[i-1][w];
		}
	}

	return table[num_items][capacity];
}

int knapsack_bottom_up_parallel(int capacity, int weights[], int values[], int num_items)
{
	int i, w;
	int table[num_items+1][capacity+1];

	for (i = 0; i <= num_items; i++) {
        #pragma omp parallel for shared(i) private(w) schedule(static)
		for (w = 0; w <= capacity; w++) {
			if (i == 0 || w == 0)
				table[i][w] = 0;
			else if (weights[i-1] <= w)
				table[i][w] = max(table[i-1][w], table[i-1][w-weights[i-1]] + values[i-1]);
			else
				table[i][w] = table[i-1][w];
		}
	}

	return table[num_items][capacity];
}

int main()
{
    int weights[] = {1, 3, 4, 5};
    int values[] = {1, 4, 5, 7};
	int capacity = 9;
	int num_items = sizeof(values) / sizeof(values[0]);

    double wtime = omp_get_wtime();
    int seq_val = knapsack_bottom_up_seq(capacity, weights, values, num_items);
    wtime = omp_get_wtime() - wtime;
    printf("Sequential algorithm took %f seconds\n", wtime);

    wtime = omp_get_wtime();
    int parallel_val = knapsack_bottom_up_parallel(capacity, weights, values, num_items);
    wtime = omp_get_wtime() - wtime;
    printf("Parallel algorithm took %f seconds\n", wtime);

	printf("Maximum value %d in knapsack with capacity %d\n", seq_val, capacity);
    printf("Maximum value %d in knapsack with capacity %d\n", parallel_val, capacity);

	return 0;
}
