import random
from bayes_opt import BayesianOptimization

# 資料集
item_data = [
    (382745, 825594),
    (799601, 1677009),
    (909247, 1676628),
    (729069, 1523970),
    (467902, 943972),
    (44328, 97426),
    (34610, 69666),
    (698150, 1296457),
    (823460, 1679693),
    (903959, 1902996),
    (853665, 1844992),
    (551830, 1049289),
    (610856, 1252836),
    (670702, 1319836),
    (488960, 953277),
    (951111, 2067538),
    (323046, 675367),
    (446298, 853655),
    (931161, 1826027),
    (31385, 65731),
    (496951, 901489),
    (264724, 577243),
    (224916, 466257),
    (169684, 369261)
]

# 背包容量和物品數量
capacity = 6404180
num_items = 24

# 基因表示法：每個基因表示一個物品的選擇
def generate_individual():
    return [random.randint(0, 1) for _ in range(num_items)]

# 計算個體的適應度
def fitness(individual):
    total_weight = 0
    total_value = 0
    for i in range(num_items):
        if individual[i] == 1:
            total_weight += item_data[i][0]
            total_value += item_data[i][1]
    if total_weight > capacity:
        return 0  # 拒絕超過容量的解答
    else:
        return total_value

# 交叉操作：單點交叉
def crossover(parent1, parent2):
    crossover_point = random.randint(1, num_items - 1)
    child1 = parent1[:crossover_point] + parent2[crossover_point:]
    child2 = parent2[:crossover_point] + parent1[crossover_point:]
    return child1, child2

# 突變操作：單點突變
def mutate(individual, mutation_rate):
    for i in range(num_items):
        if random.random() < mutation_rate:
            individual[i] = 1 - individual[i]
    return individual

# 選擇操作：錦標賽選擇
def select(population, fitness_values, tournament_size):
    selected_indices = random.sample(range(len(population)), tournament_size)
    winner_index = max(selected_indices, key=lambda i: fitness_values[i])
    return population[winner_index]

# 主要演算法
def genetic_algorithm(population_size, generations, crossover_rate, mutation_rate, tournament_size):
    population = [generate_individual() for _ in range(population_size)]

    for generation in range(generations):
        fitness_values = [fitness(individual) for individual in population]

        new_population = []
        for _ in range(population_size // 2):
            parent1 = select(population, fitness_values, tournament_size)
            parent2 = select(population, fitness_values, tournament_size)

            if random.random() < crossover_rate:
                child1, child2 = crossover(parent1, parent2)
            else:
                child1, child2 = parent1, parent2

            child1 = mutate(child1, mutation_rate)
            child2 = mutate(child2, mutation_rate)

            new_population.extend([child1, child2])

        population = new_population

        # 修正選擇最佳個體的程式碼
        best_individual_index = max(range(population_size), key=lambda i: fitness_values[i] if i < len(fitness_values) else float('-inf'))
        best_individual = population[best_individual_index]
        best_fitness = fitness_values[best_individual_index]

    return best_individual, best_fitness


# 定義貝葉斯優化的目標函數
def bayesian_optimization(population_size, generations, crossover_rate, mutation_rate, tournament_size):
    # 注意：這裡將最佳參數轉換為整數
    population_size = int(population_size)
    generations = int(generations)
    tournament_size = int(tournament_size)

    _, best_fitness = genetic_algorithm(population_size, generations, crossover_rate, mutation_rate, tournament_size)
    return -best_fitness  # 注意這裡取負號

pbounds = {
    'population_size': (100, 200),
    'generations': (30, 30),
    'crossover_rate': (0.6, 1.0),
    'mutation_rate': (0.01, 0.1),
    'tournament_size': (2, 5),
}

import numpy as np

# Number of runs
num_runs = 2

# Lists to store results
best_fitness_list = []
best_params_list = []
best_solution_list = []

for run in range(num_runs):
    # Initialize Bayesian optimization
    optimizer = BayesianOptimization(
        f=bayesian_optimization,
        pbounds=pbounds,
        random_state=np.random.randint(1, 1000),  # Use different random seed for each run
    )

    # Perform optimization
    optimizer.maximize(
        init_points=5,  # Initial exploration points
        n_iter=95,       # Iterations for optimization
    )

    # Extract best parameters and fitness from the optimization result
    best_params = optimizer.max['params']
    best_params['population_size'] = int(best_params['population_size'])
    best_params['generations'] = int(best_params['generations'])
    best_params['tournament_size'] = int(best_params['tournament_size'])

    best_fitness = -optimizer.max['target']  # Reverse the sign as the optimization maximizes the negative fitness
    best_solution, _ = genetic_algorithm(
        best_params['population_size'],
        best_params['generations'],
        best_params['crossover_rate'],
        best_params['mutation_rate'],
        best_params['tournament_size']
    )

    # Store results
    best_fitness_list.append(best_fitness)
    best_params_list.append(best_params)
    best_solution_list.append(best_solution)

# Find the index of the run with the highest fitness
best_index = np.argmax(best_fitness_list)

# Print the best results
print("Best solution (across all runs):", best_solution_list[best_index])
print("Best fitness (across all runs):", best_fitness_list[best_index])
print("Best parameters (across all runs):", best_params_list[best_index])

