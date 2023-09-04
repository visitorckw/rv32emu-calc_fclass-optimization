# rv32emu-calc_fclass-optimization
This repository focuses on improving the efficiency of [rv32emu](https://github.com/sysprog21/rv32emu)'s inline function calc_fclass by implementing a binary decision tree optimization. Here, you'll find code designed to evaluate the performance of this enhanced version of calc_fclass. Our testing results reveal that the optimized function performs approximately 40% faster than the original implementation. These tests were conducted with Turbo Boost and SMT disabled to ensure accurate performance measurements.

## Test Environment
```
$ gcc --version
gcc (Ubuntu 9.4.0-1ubuntu1~20.04.1) 9.4.0

$ lscpu
Architecture:                    x86_64
CPU op-mode(s):                  32-bit, 64-bit
Byte Order:                      Little Endian
Address sizes:                   39 bits physical, 48 bits virtual
CPU(s):                          12
On-line CPU(s) list:             0-11
Thread(s) per core:              2
Core(s) per socket:              6
Socket(s):                       1
NUMA node(s):                    1
Vendor ID:                       GenuineIntel
CPU family:                      6
Model:                           165
Model name:                      Intel(R) Core(TM) i5-10500 CPU @ 3.10GHz
Stepping:                        3
CPU MHz:                         3100.000
CPU max MHz:                     4500.0000
CPU min MHz:                     800.0000
BogoMIPS:                        6199.99
Virtualization:                  VT-x
L1d cache:                       192 KiB
L1i cache:                       192 KiB
L2 cache:                        1.5 MiB
L3 cache:                        12 MiB
NUMA node0 CPU(s):               0-11
```

## Result
```
  Elapsed time(old):   13417047800
  Elapsed time(new):   4155587892
  New is faster by:    69.027554%
```