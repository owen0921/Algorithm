#include <stdio.h>
#include <stdlib.h>

struct Item{
    int weight;
    int value;
};

int max(int a, int b){
    return (a > b) ? a : b;
}

void knapsackDP(struct Item items[], int itemNumber, int capacity, FILE* outputFile){
    // create a table to store the results of subproblems
    int **dp = (int **)malloc((itemNumber + 1) * sizeof(int *));
    for (int i = 0; i <= itemNumber; i++) {
        dp[i] = (int *)malloc((capacity + 1) * sizeof(int));
    }

    // do the DP
    for (int i = 0; i <= itemNumber; i++){ // list all the items
        for (int w = 0; w <= capacity; w++){   // list all the capacity
            if (i == 0 || w == 0) { // boundary
                dp[i][w] = 0;
            }
            else if (items[i - 1].weight <= w){ // if the items can put in
                dp[i][w] = max(items[i - 1].value + dp[i - 1][w - items[i - 1].weight], dp[i - 1][w]);
            }
            else { // the items can not put in
                dp[i][w] = dp[i - 1][w];
            }
        }
    }

    // Find the selected items
    int i = itemNumber;
    int w = capacity;
    int *select = (int *)malloc(itemNumber * sizeof(int));

    // 0 represents the item did not be choosed
    for (int j = 0; j < itemNumber; j++) {
        select[j] = 0;
    }

    while (i > 0 && w > 0){
        if (dp[i][w] != dp[i - 1][w]){ // if the item is in the bag
            select[i - 1] = 1;
            w -= items[i - 1].weight;
        }
        i--;
    }

    // Print the results
    fprintf(outputFile, "max profit: %d\n", dp[itemNumber][capacity]);
    fprintf(outputFile, "solution: ");
    for (int j = 0; j < itemNumber; j++){
        fprintf(outputFile, "%d", select[j]);
    }

    // Free memory
    for (int i = 0; i <= itemNumber; i++){
        free(dp[i]);
    }
    free(dp);
    free(select);
}

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return -1;
    }

    FILE* input = fopen(argv[1], "r");
    if (input == NULL) {
        printf("The file cannot be opened.\n");
        return -1;
    }
    else {
        int itemNumber = 0; // the number of items
        int capacity = 0;   // the capacity of the bag
        struct Item *items = NULL;

        fscanf(input, "%d", &itemNumber);
        fscanf(input, "%d", &capacity);

        items = (struct Item *)malloc(itemNumber * sizeof(struct Item));
        if (items == NULL) {
            printf("Memory allocation error!");
            return -1;
        }

        for (int i = 0; i < itemNumber; i++) {
            fscanf(input, "%d %d", &items[i].weight, &items[i].value);
        }

        // close the test data
        fclose(input);

        // open the ans file
        FILE* outputFile = fopen("ans.txt", "w");
        if (outputFile == NULL) {
            printf("Cannot open ans.txt!");
            return -1;
        }

        // knapsack dynamic programming algorithm
        knapsackDP(items, itemNumber, capacity, outputFile);
        fclose(outputFile);
        free(items);
    }
    return 0;
}

