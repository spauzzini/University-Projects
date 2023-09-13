import numpy as np
from typing import Callable
import itertools as it

# ----------------------IA-------------------------------------------


def init_cd(n: int) -> np.ndarray:
    """Function that returns an array with values -1 in the positions {0,1,...,n-1}"""
    if n <= 0:
        print("Error in length. It should be > 0")
        return

    narr = -1 * np.ones(n, int)

    return narr


def union(rep_1: int, rep_2: int, p_set: np.array) -> int:
    """Function that returns the representative of the set obtained with the rank-union of the sets represented by the indices rep_1 and rep_2 in the disjoint set structure represented by the array p_set"""
    if (rep_1 < 0 or rep_2 < 0 or p_set is None):
        print("Arguments are not valid")
        print(rep_1)
        print(rep_2)
        return -1

    """We find the representatives"""
    n = find(rep_1, p_set)
    m = find(rep_2, p_set)
    range = p_set[m] + p_set[n]

    """We compute the most optimal union"""
    if p_set[n] > p_set[m]:
        p_set[n] = m
        p_set[m] = range
        return m

    else:
        p_set[m] > p_set[n]
        p_set[m] = n
        p_set[n] = range
        return n


def find(ind: int, p_set: np.ndarray) -> int:
    """Function that returns the representative of the element of index ind in the disjoint set structure p_set carrying out path compression"""
    if p_set is None or ind < 0:
        print("Error with the inputs in find function")
        return -1

    n = ind
    """Finding the representative"""
    while p_set[n] >= 0:
        n = p_set[n]

    """To compress the path"""
    while p_set[ind] >= 0:
        aux = p_set[ind]
        p_set[ind] = n
        ind = aux

    return n


def cd_2_dict(p_set: np.array) -> dict:
    """Function that returns a python dictionary in which the keys are the representatives of the sets, and where the value associated to a key u is a list with the elements of the set represented by u (oncluding u itself)."""
    if p_set is None:
        print("Error with the inputs in cd_2_dict function")
        return -1

    aux_d = {}

    """To get the keys from the set"""
    for ind, val in enumerate(p_set):
        if val < 0:
            aux_d[ind] = []

    """To get the values from the set"""
    for i in range(len(p_set)):
        n = find(i, p_set)
        aux_d[n].append(i)

    return aux_d

# ----------------------IB-------------------------------------------


def ccs(n: int, l: list) -> dict:
    """Function that returns the connected components of a graph."""
    cc = {}

    """Initialized a disjoint set structure with a set per node"""
    aux = init_cd(n)

    """Run through the edges of the graph"""
    for i in l:
        aux1 = find(i[0], aux)
        aux2 = find(i[1], aux)

        """If the two extrema of the edge belong to different sets will join them"""
        if (aux1 != aux2):
            union(i[0], i[1], aux)

    """When all the edges have been processed, the function will convert the structure into a dictionary and return it"""
    cc = cd_2_dict(aux)
    return cc


# ----------------------IIA-------------------------------------------


def dist_matrix(n_nodes: int, w_max=10) -> np.ndarray:
    """Function that generates the distance matrix for a graph of n cities"""

    if n_nodes < 0:
        """Error control"""
        return -1

    """Creating the matrix"""
    dist_m = np.random.randint(1, w_max, (n_nodes, n_nodes))
    dist_m = (dist_m + dist_m.T) // 2
    dist_m = dist_m - np.diag(np.diag(dist_m))
    # print(dist_m)
    return dist_m


def greedy_tsp(dist_m: np.ndarray, node_ini=0) -> list:
    """Function that receives the distance matrix and returns the circuit given the initial node"""

    num_cities = dist_m.shape[0]
    circuit = list()
    circuit.append(node_ini)

    while len(circuit) < num_cities:
        current_city = circuit[-1]

        """sort cities in ascending distance from current"""
        options = list(np.argsort(dist_m[current_city]))

        """add first city in sorted list not visited yet"""
        for city in options:
            if city not in circuit:
                circuit.append(city)
                break
    print((circuit + [node_ini]))
    return (circuit + [node_ini])


def len_circuit(circuit: list, dist_m: np.ndarray) -> int:
    """Function that receives the circuit and the distance matrix and returns the length of the circuit"""
    length = 0
    for i in range(1, len(circuit)):
        # print(dist_m[circuit[i-1]][circuit[i]])
        length += dist_m[circuit[i - 1]][circuit[i]]
    # print(length)
    return length


def repeated_greedy_tsp(dist_m: np.ndarray) -> list:
    """Function that applies the function greedy_tsp starting with all the nodes, and return the shortest circuit."""

    """We get the first circuit"""
    rep = greedy_tsp(dist_m, 0)
    """We get the length of the first circuit"""
    len_1 = len_circuit(rep, dist_m)

    """We go through the rest of the options"""
    for n in range(1, len(dist_m[0])):
        """We get the circuit"""
        aux1 = greedy_tsp(dist_m, n)
        """We get the length of the circuit"""
        len_2 = len_circuit(aux1, dist_m)

        """We compare the results and save the sortest path"""
        if len_2 < len_1:
            len_1 = len_2
            rep = aux1

    return rep


def exhaustive_greedy_tsp(dist_m: np.ndarray) -> list:
    """Function to solve TSP simply by enumerating all the circuits and choosing the one with the shortest distance"""

    aux = dist_m[0]
    """We compute the permutations"""
    aux1 = it.permutations(range(len(aux)))

    """We save infinity as the initial length"""
    len_1 = np.Infinity
    rep = ()
    aux2 = ()

    """We go through the elements in the permutation"""
    for n in aux1:
        """We cast into a list the element in the permutation"""
        aux2 = list(n)
        aux2.append(aux2[0])
        """We get the length of the circuit"""
        len_2 = len_circuit(aux2, dist_m)

        """We compare the results and save the sortest path"""
        if len_2 < len_1:
            len_1 = len_2
            rep = aux2

    return rep
