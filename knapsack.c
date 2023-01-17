#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int max(int a, int b) { return (a > b) ? a : b; }

int knapsack_bottom_up_seq(int capacity, int weights[], int values[], int num_items)
{
	int i, w;
	// int table[num_items+1][capacity+1];

	// int **table = malloc(num_items+1 * sizeof(int *));
	// if (!table) {
	// 	printf("Memory allocation failed");
	// }
    // for (size_t iter = 0; iter < num_items+1; iter++) {
    //     table[iter] = malloc(capacity+1 * sizeof(int));
	// 	if (table[iter] == NULL) {
	// 		printf("Memory allocation failed");
	// 	}
	// }

	int (*table) [capacity+1] = malloc(sizeof(int[num_items+1][capacity+1]));

	for (i = 0; i <= num_items; i++) {
		for (w = 0; w <= capacity; w++) {
			// printf("%d, %d\n", i, w);
			if (i == 0 || w == 0)
				table[i][w] = 0;
			else if (weights[i-1] <= w)
				table[i][w] = max(table[i-1][w], table[i-1][w-weights[i-1]] + values[i-1]);
			else
				table[i][w] = table[i-1][w];
			// printf("%d, %d, %d\n", i, w, table[i][w]);
		}
	}

	int ret_val = table[num_items][capacity];

	// for (size_t iter = 0; iter < num_items+1; iter++)
    //     free(table[iter]);
	free(table);

	return ret_val;
}

int knapsack_bottom_up_parallel(int capacity, int weights[], int values[], int num_items)
{
	int i, w;
	// int table[num_items+1][capacity+1];
	int (*table) [capacity+1] = malloc(sizeof(int[num_items+1][capacity+1]));

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

	int ret_val = table[num_items][capacity];

	free(table);

	return ret_val;
}

// int knapsack_bottom_up_parallel_blocks(int capacity, int weights[], int values[], int num_items)
// {
// 	int i, w, block;
// 	int table[num_items+1][capacity+1];

// 	int num_blocks = omp_get_max_threads();
// 	printf("%d\n", num_blocks);

// 	int num_threads_done_for_row = 0;
// 	int done = 0;

// 	#pragma omp parallel private(block, i, w) shared(num_threads_done_for_row, done) num_threads(num_blocks)
// 	{
// 		#pragma omp for
// 		for (block = 0; block < num_blocks; block++) {
// 			printf("xd\n");
// 			for (i = 0; i <= num_items; i++) {


// 				for (w = block * capacity/num_blocks; w < block+1 * capacity/num_blocks, w <= capacity; w++) {
// 					if (i == 0 || w == 0)
// 						table[i][w] = 0;
// 					else if (weights[i-1] <= w)
// 						table[i][w] = max(table[i-1][w], table[i-1][w-weights[i-1]] + values[i-1]);
// 					else
// 						table[i][w] = table[i-1][w];
// 				}

// 				#pragma omp atomic
// 				num_threads_done_for_row++;

// 				int num_done_local = -1;
// 				do
// 				{
// 					#pragma omp atomic read
// 					num_done_local = num_threads_done_for_row;
// 				}
// 				while (num_done_local != num_blocks && !done);
// 			}
// 		}
// 	}

// 	return table[num_items][capacity];
// }

int knapsack_bottom_up_parallel_blocks(int capacity, int weights[], int values[], int num_items)
{
	int i, w;
	// int table[num_items+1][capacity+1];
	int (*table) [capacity+1] = malloc(sizeof(int[num_items+1][capacity+1]));

	int num_blocks = omp_get_max_threads();
	printf("%d\n", num_blocks);

	#pragma omp parallel private(i, w) num_threads(num_blocks)
	{
		for (i = 0; i <= num_items; i++) {
			int block = omp_get_thread_num();
			for (w = block * capacity/num_blocks; w < block+1 * capacity/num_blocks, w <= capacity; w++) {
				if (i == 0 || w == 0)
					table[i][w] = 0;
				else if (weights[i-1] <= w)
					table[i][w] = max(table[i-1][w], table[i-1][w-weights[i-1]] + values[i-1]);
				else
					table[i][w] = table[i-1][w];
			}

			#pragma omp barrier
		}
	}

	int ret_val = table[num_items][capacity];

	free(table);

	return ret_val;
}


int* read_files(const char* file_name, int* out_num_items) 
{
    FILE * file;

    file = fopen(file_name, "r");

    size_t read;
    char * line = NULL;
    size_t line_len = 0;

    size_t buffer_size = 10;
    int * buffer = (int *)malloc(sizeof(int) * buffer_size);

    int seek = 0;
    while((read = getline(&line, &line_len, file)) != -1) {
        buffer[seek++] = atoi(line);

        if (seek % 10 == 0) {
            buffer_size += 10;
            buffer = (int *)realloc(buffer, sizeof(int) * buffer_size);
        }
    }

    // for (int i = 0; i < seek; i++) {
    //     printf("%d\n", buffer[i]);
    // }

	*out_num_items = (int) buffer_size - 10;
    fclose(file);

	return buffer;
}

int main()
{
    // int weights[] = {1, 3, 4, 5, 6, 2, 7, 5, 4, 9, 1, 2, 3};
    // int values[] = {1, 4, 5, 7, 8, 1, 9, 4, 4, 10, 2, 3, 4};
	// int capacity = 14;
	// int num_items = sizeof(values) / sizeof(values[0]);

	const char file_name_1[] = "./weights.txt";
	const char file_name_2[] = "./values.txt";
	int num_items;
	int* weights = read_files(file_name_1, &num_items);
	int* values = read_files(file_name_2, &num_items);
	int capacity = 100000;

    double wtime = omp_get_wtime();
    int seq_val = knapsack_bottom_up_seq(capacity, weights, values, num_items);
    wtime = omp_get_wtime() - wtime;
    printf("Sequential algorithm took %f seconds\n", wtime);

    wtime = omp_get_wtime();
    int parallel_val = knapsack_bottom_up_parallel(capacity, weights, values, num_items);
    wtime = omp_get_wtime() - wtime;
    printf("Parallel algorithm (inner loop) took %f seconds\n", wtime);

	wtime = omp_get_wtime();
    int parallel_blocks_val = knapsack_bottom_up_parallel_blocks(capacity, weights, values, num_items);
    wtime = omp_get_wtime() - wtime;
    printf("Parallel algorithm (blocks) took %f seconds\n", wtime);

	printf("Maximum value %d in knapsack with capacity %d\n", seq_val, capacity);
    printf("Maximum value %d in knapsack with capacity %d\n", parallel_val, capacity);
	printf("Maximum value %d in knapsack with capacity %d\n", parallel_blocks_val, capacity);

	return 0;
}