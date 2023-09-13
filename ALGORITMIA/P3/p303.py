from typing import Tuple, Union, List, Iterable, Union
import numpy as np

# ---------------------------------------------------------------
# I-A Basic QuickSelect

def split(t: np.ndarray) -> Tuple[np.ndarray, int, np.ndarray]:
    """
    Function that splits the elements of t between two arrays. 
    
    Args:
        - t: array to be split.

    Returns: 
        - Tuple[np.ndarray, int, np.ndarray] with:
            - numpy array with elements smaller than the pivot.
            - integer number used to split the array (the pivot).
            - numpy array with elements greater than the pivot.
    """
    #Error Control
    if t is None:
        print("Error in given array.")
        return None
    
    #get pivot
    pivot=-1
    if len(t)>0:
        pivot = t[0]
    
    #partition
    l=[]
    r=[]
    for x in t:
        if x<pivot:
            l.append(x)
        elif x>pivot:
            r.append(x)
    left = np.array(l)
    right = np.array(r)
    
    return (left, pivot, right)

def qsel(t: np.ndarray, k: int) -> Union[int, None]:
    """
    Function that implements the recursive QuickSelect algorithm using the split function.
    
    Args:
        - t: array with elements.
        - k: index to search in the array.

    Returns: 
        - integer representing the element that would occupy the k position of ordering t (the list) 
        or None if this element does not exist (because the index is greater or smaller than the ones in the table).
    """
    
    #Error Control
    if t is None or k<0 or k>len(t):
        return None
    
    #array with 1 element
    if len(t)==1 and k==0:
        return t[0]
    
    #array with >1 element
    else:
        (left, pivot, right) = split(t)
        if pivot==-1:
            return None
        
        # m = index of pivot
        m = len(left)
        
        #if k is same as m then i found the element
        if k==m:
            return pivot
        elif k<m:
            #no need to adjust k since it is the left subarray
            return qsel(left, k)
        elif k>m:
            #adjust new k to the right subarray
            return qsel(right, k-m-1)

def qsel_nr(t: np.ndarray, k: int) -> Union[int, None]:
    """
    Function that implements QuickSelect algorithm with Split function and without recursive calls.
    
    Args:
        - t: array with elements.
        - k: index to search in the array. 

    Returns: 
        - integer representing the element that would occupy the k position of ordering t (the list)
        or None if this element does not exist (because the index is greater or smaller than the table indexes).  
    """
    #Error Control
    if k>len(t) or t is None or k<0:
        return None
    
    #Array with 1 element
    if len(t)==1 and k==0:
        return t[0]
    
    #Array with >1 element
    m=-1
    pivot=-1
    while k != len(t):
        (left, pivot, right) = split(t)
        if pivot==-1:
            return None
        # m = index of pivot
        m = len(left)
        if k==m:
            break
        elif k<m:
            t = left
        elif k>m:
            t = right
            k = k-m-1
    if pivot==-1:
        return None
    else: 
        return pivot

# I-B QuickSelect5

def split_pivot(t: np.ndarray, mid: int) -> Tuple[np.ndarray, int, np.ndarray]:
    """
    Function that splits the array using as a pivot the element t[mid].
    
    Args:
        - t: array with elements.
        - mid: index of element to use as pivot.

    Returns: 
        - Tuple with:
            - numpy array with elements smaller than the pivot.
            - integer number used to split the array.
            - numpy array with elements bigger than the pivot. 
    """
    #Error Control
    if t is None or mid<0:
        print("Error in given array or mid.")
        return None
    
    #partition
    l=[]
    r=[]
    for x in t:
        if x<mid:
            l.append(x)
        elif x>mid:
            r.append(x)
    left = np.array(l)
    right = np.array(r)
    
    return (left, mid, right)

def pivot5(t: np.ndarray) -> int:
    """
    Function returns the ”pivot5” of the array t accordingt to the procedure ”median of the medians of 5 elements”.
    
    Args:
        - t: array with elements.

    Returns: 
        - integer representing the pivot5. 
    """
    #Error Control
    if t is None:
        return None
    
    #Median Of Medians: mom
    if len(t)<5:
        return sorted(t)[int(len(t)/2)]
    
    mom=[]
    i=0
    
    while i < len(t):
        sub = len(t) - i
        if sub < 5:
            sub5 = t[i:]
            five = sorted(sub5)[int(len(sub5)/2)]
            mom.append(five)
            break
        else:
            sub5 = t[i:i+4]
            five = sorted(sub5)[int(len(sub5)/2)]
            mom.append(five) 
            i += 5

    pivot = qsel5_nr(mom, int(len(mom)/2))
    return pivot

def qsel5_nr(t: np.ndarray, k: int) -> Union[int, None]:
    """
    Function that implements QuickSelect5 algorithm with split_pivot function and without recursive calls.
    
    Args:
        - t: array with elements.
        - k: index to search in the array. 

    Returns: 
        - integer representing the element that would occupy the k position of ordering t 
        or None if this element does not exist. 
    """
    
    #Error Control
    if k>=len(t) or k<0 or t is None or len(t)<0:
        return None
    
    #Array with 1 element
    if len(t)==1 and k==0:
        return t[0]
    
    #Array with >1 element
    m=-1
    pivot=-1
    while k != len(t):
        #get pivot
        pivot = pivot5(t)
        if pivot==-1:
            return None
        else:
            #partition
            (left, pivot, right) = split_pivot(t, pivot)
            # m = index of pivot
            m = len(left)
            if k==m:
                break
            elif k<m:
                t = left
            elif k>m:
                t = right
                k = k-m-1
    return pivot

# I-C QuickSort5

def qsort_5(t: np.ndarray) -> np.ndarray:
    """
    Function that obtains a version of QuickSort with worst-case cost of O(n log n).
    
    Args:
        - t: array with elements.

    Returns: 
        - sorted numpy array.
    """
    #Error Control
    if t is None or len(t)<0:
        return None
    
    if len(t)==0 or len(t)==1:
        return t
    
    pivot = pivot5(t)
    if pivot==-1:
        return None
    
    (left, pivot, right) = split_pivot(t, pivot)
    
    l = qsort_5(left)
    r = qsort_5(right)
    qs = np.append(l, pivot)
    
    return np.append(qs, r)


# ---------------------------------------------------------------
# II-A Distance between strings and longest common substring

def edit_distance(str_1: str, str_2: str) -> int:
    """
    Function that returns the edit distance between the two strings.
    
    Args:
        - str_1: first string.
        - str_2: second string.

    Returns: 
        - integer representing the edit distance between the two strings.
    """
    #Error Control
    if str_1 is None or str_2 is None:
        return None
    
    #lengths of strings
    m = len(str_1)
    n = len(str_2)
    
    #To store our results
    d = np.zeros((m+1, n+1), int)
    
    for i in range (m+1):
        for j in range (n+1):
            #CASE 1: str_1 is empty -> INSERT second string
            if i==0:
                d[i][j] = j #min=j
            
            #CASE 2: str_2 is empty -> INSERT first string
            elif j==0:
                d[i][j] = i #min=i
            
            #CASE 3: final chars are equal -> ignore last char
            elif str_1[i-1] == str_2[j-1]:
                d[i][j] = d[i-1][j-1]
                
            #CASE 4: rest of possibilities -> find min
            else:
                #--------------/--INSERT--/--DELETE--/--DELETE+INSERT--/
                d[i][j] = 1+min(d[i][j-1], d[i-1][j], d[i-1][j-1])
    
    return d[m][n]

def max_subsequence_length(str_1: str, str_2: str) -> int:
    """
    Function that returns the length of the longest subsequence common both strings, even is not contiguous
    and using as little memory as possible.
    
    Args:
        - str_1: first string.
        - str_2: second string.

    Returns: 
        - integer representing the length.
    """
    #Error Control
    if str_1 is None or str_2 is None:
        return None
    
    #lengths of strings
    m = len(str_1)
    n = len(str_2)
    
    if m<0 or n<0:
        return None
    
    #To store our results
    d = np.zeros((m+1, n+1), int)
    
    for i in range (1, m+1):
        for j in range (1, n+1):
            if str_1[i-1]==str_2[j-1]:
                d[i][j] = 1 + d[i-1][j-1]
            else:
                d[i][j] = max(d[i-1][j],d[i][j-1])
                
    return d[m][n]

def max_common_subsequence(str_1: str, str_2: str) -> str:
    """
    Function that returns the longest subsequence common to both strings, even is not contiguous.
    
    Args:
        - str_1: first string.
        - str_2: second string.

    Returns: 
        - string representing the subsequence. 
    """
    
    #Error Control
    if str_1 is None or str_2 is None:
        return None
    
    #lengths of strings
    m = len(str_1)
    n = len(str_2)
    
    if m<0 or n<0:
        return None
    
    ss=""
    d = np.zeros((m+1, n+1), int)
    
    while m>0 and n>0:
        for i in range (1, m+1):
            for j in range (1, n+1):
                if str_1[i-1]==str_2[j-1]:
                    d[i][j] = 1 + d[i-1][j-1]
                else:
                    d[i][j] = max(d[i-1][j],d[i][j-1])
                if str_1[m-1] == str_2[n-1]:
                    ss += str_1[m-1]
                    m -= 1
                    n -= 1
                elif d[m-1][n] > d[m][n-1]:
                    m -= 1
                else:
                    n -= 1        
                
    return ss[::-1]

# II-B Optimal Matrix Multiplication

def min_mult_matrix(l_dims : List[int]) -> int:
    """
    Function that returns the minimal number of multiplications necessary to multiply n matrices whose dimensions are contained
    in the given list.
    
    Args:
        - l_dims: list with dimensions (n+1 elements). The first element is the number of rows of the first matrix, and the others
        are the number of columns of the n matrices.

    Returns: 
        - integer representing the number of multiplications.
    """
    n = len(l_dims)
    mul = np.ones((n, n)) * np.inf
    mul[0, 0] = 0
    
    for x in range(n):
        mul[x][x] = 0
 
    for i in range(1, n):
        for j in range(1, n-i+1):
            z = j+i-1
            for k in range(j, z):
                c = mul[j][k] + mul[k + 1][z] + l_dims[j-1] * l_dims[k] * l_dims[z]
                if c < mul[j][z]:
                    mul[j][z] = c
 
    return mul[1][n-1]