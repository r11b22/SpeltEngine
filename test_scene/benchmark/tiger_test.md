# Tiger Test Results
Benchmarks were created using the tiger model. Benchmark results were recorded using mangohud
 

## Simple render queue

### Specs
* OS: Arch Linux
* Kernel version: Linux 7.0.12-arch1-1
* CPU: Intel(R) Core(TM) Ultra 7 255H (16) @ 5.10 GHz
* GPU: NVIDIA RTX PRO 1000 Blackwell Generation Laptop GPU
* Memory: 32GB
* Resolution: 1920x1080


### results
|Count|FPS|Frame Time (ms)|GPU usage (%)|CPU usage (%)|Memory usage (MiB)|
|-----|---|----------|---------|---------|------------|
|100|850 - 950|1.1 - 1.3|41|7|176.3|
|1000|91 - 98|10.1 - 10.8|25|8|176.5|
|10000|10|93.9-98.5|25|7|189.9|


## Instance Uniform Grouping

This version groups all drawcommands with the same "static" uniforms togheter and only changes the dynamic uniforms within that same drawcall.
This does not yet use hardware instancing.

### Specs
* OS: Arch Linux
* Kernel version: Linux 7.0.12-arch1-1
* CPU: Intel(R) Core(TM) Ultra 7 255H (16) @ 5.10 GHz
* GPU: NVIDIA RTX PRO 1000 Blackwell Generation Laptop GPU
* Memory: 32GB
* Resolution: 1920x1080


### results
|Count|FPS|Frame Time (ms)|GPU usage (%)|CPU usage (%)|Memory usage (MiB)|
|-----|---|----------|---------|---------|------------|
|100|900 - 1000|1.0|59|7|147.4|
|1000|148-160|6.3 - 6.8|40|8|147.5|
|10000|17|60.2-61.4|35|8|162.6|


## Hardware Instancing

This version groups draw commands with the same mesh into one single hardware instanced draw command.

### Specs
* OS: Arch Linux
* Kernel version: Linux 7.0.12-arch1-1
* CPU: Intel(R) Core(TM) Ultra 7 255H (16) @ 5.10 GHz
* GPU: NVIDIA RTX PRO 1000 Blackwell Generation Laptop GPU
* Memory: 32GB
* Resolution: 1920x1080


### results
|Count|FPS|Frame Time (ms)|GPU usage (%)|CPU usage (%)|Memory usage (MiB)|
|-----|---|----------|---------|---------|------------|
|100|700-850|1.2|61|7|146.8|
|1000|140|7.0|80|6|146.9|
|10000|18|56-57|35|8-10|156.4|


## Transform optimsations

This versions uses the fact that transform matrices have a special form to reduce matrix multiplications as much as possible. Matrix caching is also used but because the tigers are rotating this will not have any effect on the result.

These optimisations wore chosen after profiling the engine.


### Specs
* OS: Arch Linux x86_64
* Kernel version: Linux 7.1.2-arch3-1
* CPU: Intel(R) Core(TM) Ultra 7 255H (16) @ 5.10 GHz
* GPU: NVIDIA RTX PRO 1000 Blackwell Generation Laptop GPU
* Memory: 32GB
* Resolution: 1920x1080


### results
|Count|FPS|Frame Time (ms)|GPU usage (%)|CPU usage (%)|Memory usage (MiB)|
|-----|---|----------|---------|---------|------------|
|100|830|1.2|63|5|192.2|
|1000|170|5.8|80|7|192.2|
|10000|21|47-48|40|8|199.4|


## Release mode

This version does not improve any code. It justs enables release mode.

These measurements were made to set a new benchmarking standard and should not be compared to previous results.


### Specs
* OS: Arch Linux x86_64
* Kernel version: Linux 7.1.2-arch3-1
* CPU: Intel(R) Core(TM) Ultra 7 255H (16) @ 5.10 GHz
* GPU: NVIDIA RTX PRO 1000 Blackwell Generation Laptop GPU
* Memory: 32GB
* Resolution: 1920x1080


### results
|Count|FPS|Frame Time (ms)|GPU usage (%)|CPU usage (%)|Memory usage (MiB)|
|-----|---|----------|---------|---------|------------|
|100|2400|0.4|60|4|189.9|
|1000|700|1.5|82|4|190.2|
|10000|95-105|9.8|90|8|196.7|
|50000|21|45.5-47.0|95|226.5|
