from ctypes import sizeof
import queue
import numpy as np
import random
from typing import List, Callable
import timeit

############################### I-A ##############################################
def matrix_multiplication(m_1: np.ndarray, m_2: np.ndarray)-> np.ndarray:
    c1 = m_1.shape[1]
    r1 = m_1.shape[0]
    r2 = m_2.shape[0]
    c2 = m_2.shape[1]
    
    if(c1 != r2):
        return None

    r = np.zeros([r1,c2])

    for i in range(r1):
        for j in range(c2):
            s=0
            for u in range(c1):
                s+= m_1[i,u]*m_2[u,j]
            r[i,j] = s
    return r

def mkmat(n):
    dim=10+n
    m = np.random.uniform(0., 1., (dim, dim))
    return m

#if __name__ == "__main__":
#    l_timings = []
#    for i in range(10,100,10):
#        st = "matrix_multiplication(m, m)"
#        sup = "from p103 import mkmat; from p103 import matrix_multiplication; m = mkmat(%d); m = mkmat(%d)" % (i, i)
#        t = timeit.Timer(stmt = st, setup=sup)
#
#        x = t.timeit(30)
#        l_timings += [x]
#    print(l_timings)

############################### I-B ##############################################

def rec_bb(t: List, f: int, l: int, key: int)-> int:
    if(f>l):
        return None
    else:
        med = (f+l) // 2
        
        if (t[med]==key):
            return med

        elif (t[med]>key):
            return rec_bb(t, f, med-1, key)

        elif (t[med]<key):
            return rec_bb(t, med+1, l, key)

def bb(t: List, f: int, l: int, key: int)-> int:
    
    while (f<=l):
        med = (f+l) // 2
        if (t[med]==key):
            return med
        elif (t[med]<key):
            f = med + 1
        else:
            l = med - 1
    return None


############################### II-A #############################################
def left(t):
    return 2*t +1

def right(t):
    return 2*t +2

def min_heapify(h: np.ndarray, i: int):
    k=i
    l = left(k)
    r = right(k)

    if (l<len(h) and h[l]<h[i]):
        k=l

    if (r<len(h) and h[r]<h[k]):
        k=r

    if (k!=i):
        h[i], h[k] = h[k], h[i]
        min_heapify(h, k)

def insert_min_heap(h: np.ndarray, k: int)-> np.ndarray:
    if h is None:
        h = []
    
    h = np.append(h, k)
    j = len(h) - 1
    
    while j >= 1 and h[(j-1) // 2] > h[j]:
        h[(j-1) // 2], h[j] = h[j], h[(j-1) // 2]
        j = (j-1) // 2

    return h

def create_min_heap(h: np.ndarray):
    for i in range(len(h) // 2,-1, -1):
        min_heapify(h, i)
    return h

############################### II-B #############################################

def pq_ini():
    pq = []
    return pq

def pq_insert(h: np.ndarray, k: int)-> np.ndarray:
    h = np.append(h, k)
    return h

def pq_remove(h: np.ndarray)-> np.ndarray:
    max_val = len(h)-1
    for i in range(len(h)):
        if int(h[i]) < h[max_val]:
            max_val = i
    elem = h[max_val]
    h = np.delete(h, max_val)
    return elem, h





##################################################################################
############################### II-C #############################################

def select_min_heap(h: np.ndarray, k: int)-> int:
    min_heapify(h, 0)
    aux = -1*np.array(h)
    aux2 = aux[:k]

    create_min_heap(aux2)
    
    for i in range(k, len(h)):
        if aux[i] > aux2[0]:
            lelem = aux2[len(aux2)-1]
            aux2[0] = lelem
            aux2 = np.delete(aux2, len(aux2)-1)
            min_heapify(aux2, 0)
            aux2 = insert_min_heap(aux2, (aux[i]))
    
    return -1*aux2[0]

#h = create_min_heap(np.array([8, 7, 6, 5, 4, 3, 2, 1]))
#print(h)
#h1 = select_min_heap(h, 0)
#print(h1)
