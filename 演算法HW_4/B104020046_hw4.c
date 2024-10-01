#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_ITEMS 50
#define POPULATION_SIZE 1000
#define CROSSOVER_RATE 0.6
#define MUTATION_RATE  0.01
#define TOURNAMENT_SIZE 4
#define NUM_RUNS 30

struct Item {
    int weight;
    int value;
};

struct Individual {
    int bits[MAX_ITEMS];
    int fitness;
};

int itemNumber = 0;
int capacity = 0;
struct Item *items = NULL;

void initialize_population(struct Individual population[POPULATION_SIZE]) {
    for (int i = 0; i < POPULATION_SIZE; ++i) {
        for (int j = 0; j < itemNumber; ++j) {
            population[i].bits[j] = rand() % 2;
        }
        population[i].fitness = 0;
    }
}

int calculate_fitness(struct Individual *individual) {
    int total_value = 0;
    int total_weight = 0;

    for (int i = 0; i < itemNumber; ++i) {
        total_value += individual->bits[i] * items[i].value;
        total_weight += individual->bits[i] * items[i].weight;
    }

    if (total_weight > capacity) {
        return 0;
    } else {
        return total_value;
    }
}

void update_population_fitness(struct Individual population[POPULATION_SIZE]) {
    for (int i = 0; i < POPULATION_SIZE; ++i) {
        population[i].fitness = calculate_fitness(&population[i]);
    }
}

void tournament_selection(struct Individual population[POPULATION_SIZE], struct Individual parents[2]) {
    int tournament_indices[TOURNAMENT_SIZE];

    for (int i = 0; i < TOURNAMENT_SIZE; ++i) {
        tournament_indices[i] = rand() % POPULATION_SIZE;
    }
    // selects the first parent (parents[0]) by iterating through the tournament_indices array
    parents[0] = population[tournament_indices[0]];
    for (int i = 1; i < TOURNAMENT_SIZE; ++i) {
        if (population[tournament_indices[i]].fitness > parents[0].fitness) {
            parents[0] = population[tournament_indices[i]];
        }
    }

    // After selecting the first parent, the tournament_indices are regenerated
    for (int i = 0; i < TOURNAMENT_SIZE; ++i) {
        tournament_indices[i] = rand() % POPULATION_SIZE;
    }

    parents[1] = population[tournament_indices[0]];
    for (int i = 1; i < TOURNAMENT_SIZE; ++i) {
        if (population[tournament_indices[i]].fitness > parents[1].fitness) {
            parents[1] = population[tournament_indices[i]];
        }
    }
}

void crossover(struct Individual *parent1, struct Individual *parent2, struct Individual *child1, struct Individual *child2) {
    // One-point Crossover
    int N = itemNumber;
    int crossover_point = rand() % N;

    for (int i = 0; i < crossover_point; ++i) {
        child1->bits[i] = parent1->bits[i];
        child2->bits[i] = parent2->bits[i];
    }

    for (int i = crossover_point; i < N; ++i) {
        child1->bits[i] = parent2->bits[i];
        child2->bits[i] = parent1->bits[i];
    }
}

void mutate(struct Individual *individual) {
    for (int i = 0; i < itemNumber; ++i) {
        if ((double)rand() / RAND_MAX < MUTATION_RATE) {
            individual->bits[i] = 1 - individual->bits[i];
        }
    }
}

int main(int argc, char **argv) {
    srand(time(NULL));

    if (argc != 3) {
        printf("Usage: %s <filename> <evaluation>\n", argv[0]);
        return -1;
    }

    FILE *input = fopen(argv[1], "r");
    int EVALUATION_MAX = atoi(argv[2]);

    if (input == NULL) {
        printf("The file cannot be opened.\n");
        return -1;
    } else {
        fscanf(input, "%d", &itemNumber);
        fscanf(input, "%d", &capacity);
        items = (struct Item *)malloc(itemNumber * sizeof(struct Item));
        if (items == NULL) {
            printf("Memory allocation error!");
            fclose(input);
            return -1;
        }

        for (int i = 0; i < itemNumber; i++) {
            fscanf(input, "%d %d", &items[i].weight, &items[i].value);
        }

        fclose(input);

        FILE *outputFile = fopen("ans.txt", "w");
        if (outputFile == NULL) {
            printf("Cannot open ans.txt!");
            free(items);
            return -1;
        }

        int global_best_solution = -1; // record the best value
        int record_solution[itemNumber]; // record the best solution (010101...)
        int best_solutions[NUM_RUNS]; // record the best solutions per round

        for (int run = 0; run < NUM_RUNS; run++) {
            // initialize the population
            struct Individual population[POPULATION_SIZE];
            initialize_population(population);

            // evaluation
            update_population_fitness(population);

            int eval = 1;
            while (eval <= EVALUATION_MAX) {
                struct Individual next_gen[POPULATION_SIZE];

                for (int i = 0; i < POPULATION_SIZE; i += 2) {
                    // selection
                    struct Individual parents[2];
                    tournament_selection(population, parents);

                    // crossover with CROSSOVER_RATE
                    if ((double)rand() / RAND_MAX < CROSSOVER_RATE) {
                        crossover(&parents[0], &parents[1], &next_gen[i], &next_gen[i + 1]);
                    } else {
                        next_gen[i] = parents[0];
                        next_gen[i + 1] = parents[1];
                    }

                    // mutation
                    mutate(&next_gen[i]);
                    mutate(&next_gen[i + 1]);
                }

                for (int i = 0; i < POPULATION_SIZE; ++i) {
                    population[i] = next_gen[i];
                }

                // evaluation
                update_population_fitness(population);
                eval += POPULATION_SIZE;
            }

            // record the best solution per round
            int best_solution = -1;
            int best_solution_index = -1;
            for (int i = 0; i < POPULATION_SIZE; ++i) {
                int current_fitness = population[i].fitness;
                if (current_fitness > best_solution) {
                    best_solution = current_fitness;
                    best_solution_index = i;
                }
            }
            best_solutions[run] = best_solution;

            // record the global best solution
            if (best_solution > global_best_solution) {
                global_best_solution = best_solution;

                // record the selection of items for the global best solution
                for (int j = 0; j < MAX_ITEMS; ++j) {
                    record_solution[j] = population[best_solution_index].bits[j];
                }
            }
        }

        // mean of the best solution per round
        int sum_request_solutions = 0;
        for (int i = 0; i < NUM_RUNS; ++i) {
            sum_request_solutions += best_solutions[i];
        }
        int average_request_solution = sum_request_solutions / NUM_RUNS;

        fprintf(outputFile, "evaluation_max: %d\n", EVALUATION_MAX);
        fprintf(outputFile, "best: %d\n", global_best_solution);
        fprintf(outputFile, "solution: ");
        for (int k = 0; k < itemNumber; k++) {
            fprintf(outputFile, "%d", record_solution[k]);
        }
        fprintf(outputFile, "\nrequested: %d\n", average_request_solution);

        free(items);
        fclose(outputFile);
    }
    return 0;
}
