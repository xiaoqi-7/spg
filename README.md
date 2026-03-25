# Shortest Path Graph Search System

This project involves the development, implementation, testing, and optimisation of software system supporting shortest path search functionality based on the technical methods described in the following publication:
* Yiqi Wang, Long Yuan,  Wenjie Zhang, Zi Chen, Xuemin Lin, Qingqiang Sun, "Towards Efficient Shortest Path Graph Search on Large Road Networks", which is submitted to VLDB2026.

### Technical Environment

The software was developed and tested in a Linux-based environment using an Intel Xeon CPU, 384 GB of memory, Debian GNU/Linux 12, and g++ 12.2.0. The system was implemented in C++11 with compiler optimisation enabled using the -O3 flag.

### Development and Validation

Development work included backend software development, implementation of processing functions, execution testing, performance tuning, and validation of system outputs. This work resulted in a functional software system that can be used by other technical users to perform shortest path search tasks on large road network datasets.

### Testing Dataset

All the datasets in this paper can be downloaded from [DIMACS](http://www.diag.uniroma1.it/~challenge9/download.shtml) 

### Preliminary
There are three files in the `data` folder: <br>
 
* `NY.data` stores the whole graph data. Please refer to the format below, including sample data: <br>

  ```
  6  6     (there are total 6 vertices and 6 edges) 
  1  2  8  (an edge between vertex 1 and vertex 2 with the distance of 8)
  1  3  2
  2  4  1
  3  5  2
  4  5  3
  5  6  3
* `NY.qu` stores all queries.  Please refer to the format below, including sample data: <br>

  ```
  3    (there are total 3 queries)
  1 3  (a query: return shortest path graph between vertex 1 and vertex 3)
  2 2
  3 4

### Compile

* compile predata.cpp for preprocessing the raw graph data <br>
  `g++ -std=c++11 -O3 preprocessdata.cpp -o pre`
  
* compile index_bng.cpp for building CBV-SDP-Index for the original graph and doing query test <br>
  `g++ -std=c++11 -O3 cbvsdpindex_query.cpp -o cbv`
  
  
### Test
* Preprocess raw graph data from [DIMACS](http://www.diag.uniroma1.it/~challenge9/download.shtml) <br>
  `./pre oridata gendata`  <br>
  * `oridata`: the file path to the raw data <br>
  * `gendata`: the file path to generated data file, which stores the formatted data <br>
  
  eg: `./pre data/USA-road-d.NY.gr data/NY.data` <br>
  
* construct CBV-SDP-Index and do query test <br>
  `./cbv origraph query.data number`  <br>
  * `origraph`: the file path to a data file, containing the original graph data <br>
  * `query.data`: the file path to the data file, storing the query vertices <br>
  * `number`: the number of pairs of query vertices <br>
  
  eg: `./cbv data/NY.data data/NY.qu 10000` 


