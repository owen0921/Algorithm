#include<stdio.h>
#include<stdlib.h>
#include<math.h>

struct Item{
    int weight;
    int value;
};

// calculate the profit of items
int calculateProfit(struct Item items[], int combination[], int itemNumber){
    int totalProfit = 0;
    for(int i = 0; i < itemNumber; i++){
        if(combination[i] == 1){
            totalProfit += items[i].value;
        }
    }
    return totalProfit;
}

// calculate the weight of items
int calculateWeight(struct Item items[], int combination[], int itemNumber){
    int totalWeight = 0;
    for(int i = 0; i < itemNumber; i++){
        if(combination[i] == 1){
            totalWeight += items[i].weight;
        }
    }
    return totalWeight;
}

void exhaustiveSearch(struct Item items[], int itemNumber, int capacity, FILE* outputFile){
    int bestProfit = 0;
    int bestCombin[itemNumber];
    int totalCombin = pow(2, itemNumber); // the total combination is 2^n

    // initialize the bestCombin
    for(int i = 0; i < itemNumber; i++){
        bestCombin[i] = 0;
    }

    for(int i = 0; i < totalCombin; i++){

        // initialize the combination
        int combination[itemNumber];
        for(int j = 0; j < itemNumber; j++){
            combination[j] = 0;
        }

        // use bitwise to list all the combination
        for(int j = 0; j < itemNumber; j++){
            if(i & (1 << j)){
                combination[j] = 1;
            }
            /*
            the combination is 00...00
                               00...01
                                   ...
                               11...10
                               11...11
            */
        }

        // first calculate the total weight to check if it is bigger than the capacity
        int currentWeight = calculateWeight(items, combination, itemNumber);

        // if it's weight is less than capacity, then compute it's profit
        if(currentWeight <= capacity){
            int currentProfit = calculateProfit(items, combination, itemNumber);

            // if the current profit is bigger than best profit, replace it
            if(currentProfit > bestProfit){
                bestProfit = currentProfit;
                for(int k = 0; k < itemNumber; k++){
                    bestCombin[k] = combination[k];
                }
            }
        }
    }

    // print the result
    fprintf(outputFile, "max profit: %d\n", bestProfit);
    fprintf(outputFile, "solution: ");

    for(int i = 0; i < itemNumber; i++){
        fprintf(outputFile, "%d", bestCombin[i]);
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

        items = (struct Item* )malloc(itemNumber * sizeof(struct Item));
        if(items == NULL){
            printf("Memory allocation error!");
            return -1;
        }

        for(int i = 0; i < itemNumber; i++){
            fscanf(input, "%d %d", &items[i].weight, &items[i].value);
        }

        // close the test data
        fclose(input);

        FILE* outputFile = fopen("ans.txt", "w");
        if(outputFile == NULL){
            printf("Cannot open ans.txt!");
            return -1;
        }

        // exhaustiveSearch algorithm
        exhaustiveSearch(items, itemNumber, capacity, outputFile);
        free(items);
    }
    return 0;
}
