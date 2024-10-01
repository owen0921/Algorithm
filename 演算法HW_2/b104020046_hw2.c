#include<stdio.h>
#include<stdlib.h>

struct Item{
    int weight;
    int value;
    float CP;
};

// return the index of bestCP
int chooseBestCP(struct Item items[], int itemNumber){
    int bestCP_index = 0;
    for(int i = 1; i < itemNumber; i++){
        if(items[i].CP > items[bestCP_index].CP){
            bestCP_index = i;
        }
    }
    return bestCP_index;
}

void greedyAlgorithm(struct Item items[], int itemNumber, int capacity, FILE* outputFile){
    int currWeight = 0; // record the current weight of the bag
    int profit = 0; // record the profit in the bag

    // select records which items are selected
    int select[itemNumber];
    for(int i = 0; i < itemNumber; i++){
        select[i] = 0;
    }

    for(int i = 0; i < itemNumber; i++){
        int currBestCP_index = chooseBestCP(items, itemNumber);

        // if the current best CP can put in the bag
        if(currWeight + items[currBestCP_index].weight <= capacity){
            select[currBestCP_index] = 1;
            currWeight += items[currBestCP_index].weight;
            profit += items[currBestCP_index].value;
        }

        // set the current best CP to -1, so that we can choose the second best CP next round
        items[currBestCP_index].CP = -1;
    }

    fprintf(outputFile, "max profit: %d\n", profit);
    fprintf(outputFile, "solution: ");

    for(int i = 0; i < itemNumber; i++){
        fprintf(outputFile, "%d", select[i]);
    }
}

int main(int argc, char** argv){
    if(argc != 2){
        printf("Usage: %s <filename>\n", argv[0]);
        return -1;
    }

    FILE* input = fopen(argv[1], "r");
    if(input == NULL){
        printf("The file cannot be opened.\n");
        return -1;
    }
    else{
        int itemNumber = 0; // the number of items
        int capacity = 0; // the capacity of the bag
        struct Item *items = NULL;

        fscanf(input, "%d", &itemNumber);
        fscanf(input, "%d", &capacity);

        items = (struct Item*)malloc(itemNumber * sizeof(struct Item));
        if(items == NULL){
            printf("Memory allocation error!");
            return -1;
        }

        for(int i = 0; i < itemNumber; i++){
            fscanf(input, "%d %d", &items[i].weight, &items[i].value);

            // compute the CP of the items
            items[i].CP = (float)items[i].value / items[i].weight;
        }

        // close the test data
        fclose(input);

        // open the ans file
        FILE* outputFile = fopen("ans.txt", "w");
        if(outputFile == NULL){
            printf("Cannot open ans.txt!");
            return -1;
        }

        // greedy algorithm
        greedyAlgorithm(items, itemNumber, capacity, outputFile);
        fclose(outputFile);
        free(items);
    }
    return 0;
}
