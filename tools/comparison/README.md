Data comparison tool
====================

Compare two data files from GPU and CPU version of the algorithm.

Files **should be** in the same format:

| x   | y   | z   | mean  | k1    | k2    | etc.  |
| :-- | :-- | :-- | ----: | ----: | ----: | ----: |
| 1   | 2   | 0   | 0.11  | 0.22  | 0.11  | ...   |


If points aren't matching together on both files, it will find the closest
point. Coordinates should be in the same scale.


## Usage

```
./main <fileGPU> <fileCPU> <size> <errorType>
```

- computes the difference of results between the CPU version of the estimator and the GPU version
- GPU file : positions are between [0;size].
- CPU file : positions are between [0;2*size].
- Error type : 1 is l_1, 2 is l_2, 3 is l_oo.

## Example

```
./main file1.txt file2.txt 64 3
```
