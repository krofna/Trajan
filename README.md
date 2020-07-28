# dagmatch #

### Compute optimal dag matching ###

## Compiling dagmatch ##

In order to build dagmatch, you need to have Eigen3 library installed. To build dagmatch, simply run

```
cmake .
make
```

## Running dagmatch ##

### Usage ###

```
dagmatch_lp <tree_1> <map_1> <tree_2> <map_2> <distance_matrix> <align> <solver>
```

```
dagmatch_fpt <tree_1> <map_1> <tree_2> <map_2> <distance_matrix>
```

```
dagmatch_greedy <tree_1> <map_1> <tree_2> <map_2> <distance_matrix>
```

### Arguments ###

`<solver>`
  : 0=fractional  
  1=non-linear integral  
  2=warm-start integral from fractional <br>

#### Input/Output formats

Input: 

`<graph>`
  : input tree file in the following format:        
  [child node] [parent node] default [newline] ...

`<map_1>`
  : a map from row index in the distance matrix (start from 0) to nodes in t1:        
  [row index] [node] [newline] ...
  
 `<map_2>`
  : a map from column index in the distance matrix (start from 0) to nodes in t2:        
  [column index] [node] [newline] ...

Output:

`<align>`
  : output the final alignment in file in the following format:
  [node in first graph] [node in second graph] [fractional solution] [newline] ...
