Data comparison tool
====================

Compare two data files from GPU and CPU version of the algorithm.

Files **should be** in the same format:

| x   | y   | z   | mean  | k1    | k2    | etc.  |
| :-- | :-- | :-- | ----: | ----: | ----: | ----: |
| 1   | 2   | 0   | 0.11  | 0.22  | 0.11  | ...   |


If points aren't matching together on both files, it will find the closest
point. Coordinates should be in the same scale.

## Compilation

```
make
```

It doesn't have any external dependencies.

## Usage

```
./main <fileGPU> <fileCPU> <enableEmbedding>
```

Computes the difference of results between the CPU version of the estimator and the GPU version
- GPU file : positions are between [0;size].
- CPU file : positions are between [0;2*size].
- enableEmbedding : 1 if the second file is a CPU file, 0 if the second file is a GPU file too.

## Example

```
./main file1.txt file2.txt 64 3
```
