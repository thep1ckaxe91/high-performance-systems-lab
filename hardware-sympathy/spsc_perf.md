
### NOTE: All result below were test with Iteration = 1e4, queue size = 10

## The first perf use:

This is the perf stat from commit 386073d59a2055f3fe53ccc4e6cb51804136c237

```
Performance counter stats for './spsc':

        32,907,537      task-clock                       #    1.853 CPUs utilized             
                 5      context-switches                 #  151.941 /sec                      
                 3      cpu-migrations                   #   91.165 /sec                      
               143      page-faults                      #    4.346 K/sec                     
       109,017,211      cpu_atom/instructions/           #    1.08  insn per cycle              (44.16%)
       234,389,937      cpu_core/instructions/           #    1.78  insn per cycle              (2.80%)
       101,089,066      cpu_atom/cycles/                 #    3.072 GHz                         (42.50%)
       131,554,107      cpu_core/cycles/                 #    3.998 GHz                         (2.80%)
        23,273,109      cpu_atom/branches/               #  707.227 M/sec                       (42.49%)
        51,753,731      cpu_core/branches/               #    1.573 G/sec                       (2.80%)
           181,246      cpu_atom/branch-misses/          #    0.78% of all branches             (54.78%)
            96,156      cpu_core/branch-misses/          #    0.19% of all branches             (2.80%)
 #     38.0 %  tma_backend_bound      
                                                  #      2.5 %  tma_bad_speculation    
                                                  #     21.3 %  tma_frontend_bound     
                                                  #     38.1 %  tma_retiring             (2.80%)
 #      7.8 %  tma_bad_speculation    
                                                  #     31.3 %  tma_retiring             (73.02%)
 #     36.9 %  tma_backend_bound      
                                                  #     24.0 %  tma_frontend_bound       (91.25%)
        31,569,883      cpu_atom/L1-dcache-loads/        #  959.351 M/sec                       (78.88%)
        68,400,062      cpu_core/L1-dcache-loads/        #    2.079 G/sec                       (2.80%)
           159,927      cpu_core/L1-dcache-load-misses/  #    0.23% of all L1-dcache accesses   (2.80%)
           225,233      cpu_atom/LLC-loads/              #    6.844 M/sec                       (60.64%)
            86,776      cpu_core/LLC-loads/              #    2.637 M/sec                       (2.80%)
                 0      cpu_atom/LLC-load-misses/                                               (42.41%)
            13,481      cpu_core/LLC-load-misses/        #   15.54% of all LL-cache accesses    (2.80%)

       0.017758781 seconds time elapsed

       0.023300000 seconds user
       0.010000000 seconds sys
```

### Hypothesis

We can notice that process switch between cores, make L1 cache miss happen more frequent, 
and also getting swap between performance core and efficient core (im using a laptop)

So to test this, we gonna pin it to a single performance core first.

### Test result

```
 Performance counter stats for 'taskset -c 0 ./spsc':

        18,929,729      task-clock                       #    0.979 CPUs utilized             
             2,225      context-switches                 #  117.540 K/sec                     
                 1      cpu-migrations                   #   52.827 /sec                      
               208      page-faults                      #   10.988 K/sec                     
     <not counted>      cpu_atom/instructions/                                                  (0.00%)
       129,190,251      cpu_core/instructions/           #    1.67  insn per cycle            
     <not counted>      cpu_atom/cycles/                                                        (0.00%)
        77,558,376      cpu_core/cycles/                 #    4.097 GHz                       
     <not counted>      cpu_atom/branches/                                                      (0.00%)
        27,898,146      cpu_core/branches/               #    1.474 G/sec                     
     <not counted>      cpu_atom/branch-misses/                                                 (0.00%)
           121,041      cpu_core/branch-misses/          #    0.43% of all branches           
 #     43.3 %  tma_backend_bound      
                                                  #      3.2 %  tma_bad_speculation    
                                                  #     21.5 %  tma_frontend_bound     
                                                  #     32.0 %  tma_retiring           
     <not counted>      cpu_atom/L1-dcache-loads/                                               (0.00%)
        36,877,366      cpu_core/L1-dcache-loads/        #    1.948 G/sec                     
           186,976      cpu_core/L1-dcache-load-misses/  #    0.51% of all L1-dcache accesses 
     <not counted>      cpu_atom/LLC-loads/                                                     (0.00%)
           122,345      cpu_core/LLC-loads/              #    6.463 M/sec                     
     <not counted>      cpu_atom/LLC-load-misses/                                               (0.00%)
             4,259      cpu_core/LLC-load-misses/        #    3.48% of all LL-cache accesses  

       0.019335442 seconds time elapsed

       0.003865000 seconds user
       0.015454000 seconds sys
```

Suprisingly, we got even more L1 miss this time

I tested with only 4 core of the performance core also, but the result are even worse:
```
 Performance counter stats for 'taskset -c 0-3 ./spsc':

        24,573,746      task-clock                       #    1.838 CPUs utilized             
                 4      context-switches                 #  162.775 /sec                      
                 1      cpu-migrations                   #   40.694 /sec                      
               207      page-faults                      #    8.424 K/sec                     
        47,883,145      cpu_atom/instructions/           #    0.65  insn per cycle              (1.63%)
       140,451,959      cpu_core/instructions/           #    1.37  insn per cycle              (98.37%)
        73,932,228      cpu_atom/cycles/                 #    3.009 GHz                         (1.63%)
       102,434,280      cpu_core/cycles/                 #    4.168 GHz                         (98.37%)
         9,807,882      cpu_atom/branches/               #  399.120 M/sec                       (1.63%)
        30,586,359      cpu_core/branches/               #    1.245 G/sec                       (98.37%)
           460,489      cpu_atom/branch-misses/          #    4.70% of all branches             (1.63%)
           125,600      cpu_core/branch-misses/          #    0.41% of all branches             (98.37%)
 #     52.7 %  tma_backend_bound      
                                                  #      3.0 %  tma_bad_speculation    
                                                  #     15.0 %  tma_frontend_bound     
                                                  #     29.3 %  tma_retiring             (98.37%)
 #     17.0 %  tma_bad_speculation    
                                                  #     17.3 %  tma_retiring             (1.63%)
 #     32.1 %  tma_backend_bound      
                                                  #     33.6 %  tma_frontend_bound       (1.63%)
     <not counted>      cpu_atom/L1-dcache-loads/                                               (0.00%)
        40,239,588      cpu_core/L1-dcache-loads/        #    1.638 G/sec                       (98.37%)
           305,924      cpu_core/L1-dcache-load-misses/  #    0.76% of all L1-dcache accesses   (98.37%)
     <not counted>      cpu_atom/LLC-loads/                                                     (0.00%)
           220,348      cpu_core/LLC-loads/              #    8.967 M/sec                       (98.37%)
     <not counted>      cpu_atom/LLC-load-misses/                                               (0.00%)
             3,253      cpu_core/LLC-load-misses/        #    1.48% of all LL-cache accesses    (98.37%)

       0.013366309 seconds time elapsed

       0.012527000 seconds user
       0.012495000 seconds sys
```



non pin:
```
    49,665,738,966      task-clock                       #    2.000 CPUs utilized             
               245      context-switches                 #    4.933 /sec                      
                25      cpu-migrations                   #    0.503 /sec                      
               145      page-faults                      #    2.920 /sec                      
    34,507,967,618      cpu_atom/instructions/           #    0.21  insn per cycle              (0.06%)
    52,858,782,987      cpu_core/instructions/           #    0.24  insn per cycle              (99.90%)
   161,219,809,249      cpu_atom/cycles/                 #    3.246 GHz                         (0.06%)
   222,004,004,042      cpu_core/cycles/                 #    4.470 GHz                         (99.90%)
     3,992,785,214      cpu_atom/branches/               #   80.393 M/sec                       (0.07%)
     5,965,833,053      cpu_core/branches/               #  120.120 M/sec                       (99.90%)
         5,364,814      cpu_atom/branch-misses/          #    0.13% of all branches             (0.08%)
        91,104,563      cpu_core/branch-misses/          #    1.53% of all branches             (99.90%)
 #     59.4 %  tma_backend_bound      
                                                  #     27.5 %  tma_bad_speculation    
                                                  #      7.3 %  tma_frontend_bound     
                                                  #      5.7 %  tma_retiring             (99.90%)
 #     31.2 %  tma_bad_speculation    
                                                  #      5.3 %  tma_retiring             (0.08%)
 #     55.8 %  tma_backend_bound      
                                                  #      7.8 %  tma_frontend_bound       (0.08%)
    17,664,635,873      cpu_atom/L1-dcache-loads/        #  355.670 M/sec                       (0.06%)
    23,592,063,291      cpu_core/L1-dcache-loads/        #  475.017 M/sec                       (99.90%)
     1,031,447,009      cpu_core/L1-dcache-load-misses/  #    4.37% of all L1-dcache accesses   (99.90%)
       879,216,807      cpu_atom/LLC-loads/              #   17.703 M/sec                       (0.05%)
     1,014,342,486      cpu_core/LLC-loads/              #   20.423 M/sec                       (99.90%)
                 0      cpu_atom/LLC-load-misses/                                               (0.05%)
            13,584      cpu_core/LLC-load-misses/        #    0.00% of all LL-cache accesses    (99.90%)

      24.835676402 seconds time elapsed
```

pinned:
```
 Performance counter stats for './spsc_pin':

    30,402,288,028      task-clock                       #    2.000 CPUs utilized             
               542      context-switches                 #   17.828 /sec                      
                 2      cpu-migrations                   #    0.066 /sec                      
               145      page-faults                      #    4.769 /sec                      
    10,704,399,025      cpu_atom/instructions/           #    0.33  insn per cycle              (0.00%)
    59,039,799,983      cpu_core/instructions/           #    0.44  insn per cycle              (100.00%)
    32,319,988,819      cpu_atom/cycles/                 #    1.063 GHz                         (0.00%)
   135,348,762,061      cpu_core/cycles/                 #    4.452 GHz                         (100.00%)
     2,235,691,868      cpu_atom/branches/               #   73.537 M/sec                       (0.00%)
     7,344,907,489      cpu_core/branches/               #  241.591 M/sec                       (100.00%)
       236,054,726      cpu_atom/branch-misses/          #   10.56% of all branches             (0.00%)
        10,880,788      cpu_core/branch-misses/          #    0.15% of all branches             (100.00%)
 #     14.8 %  tma_backend_bound      
                                                  #     47.8 %  tma_bad_speculation    
                                                  #     17.5 %  tma_frontend_bound     
                                                  #     19.9 %  tma_retiring             (100.00%)
 #     22.1 %  tma_bad_speculation    
                                                  #      9.9 %  tma_retiring             (0.00%)
 #     25.8 %  tma_backend_bound      
                                                  #     42.2 %  tma_frontend_bound       (0.00%)
     <not counted>      cpu_atom/L1-dcache-loads/                                               (0.00%)
    25,402,737,692      cpu_core/L1-dcache-loads/        #  835.553 M/sec                       (100.00%)
         1,017,707      cpu_core/L1-dcache-load-misses/  #    0.00% of all L1-dcache accesses   (100.00%)
     <not counted>      cpu_atom/LLC-loads/                                                     (0.00%)
           362,414      cpu_core/LLC-loads/              #   11.921 K/sec                       (100.00%)
     <not counted>      cpu_atom/LLC-load-misses/                                               (0.00%)
            26,748      cpu_core/LLC-load-misses/        #    7.38% of all LL-cache accesses    (100.00%)

      15.204190142 seconds time elapsed

      28.530492000 seconds user
       1.848338000 seconds sys
```