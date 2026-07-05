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
|100|650 - 700|1.3|61|5|147.1|
|1000|130-150|6.2 - 6.7|80|6|147.7|
|10000|17|56-59|35|7-9|161.0|
