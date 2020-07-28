# dagmatch #

### Alignment of complex trajectories from single-cell RNAseq experiments. ###

## Compiling dagmatch ##

To build dagmatch, simply run

```
make
```

## Running dagmatch ##

### Usage ###

```
trajan <tree_1> <map_1> <tree_2> <map_2> <distance_matrix> <align> <solver>
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
