# 🚀 OpenACC Asynchronous Execution Patterns

This project demonstrates three different OpenACC execution models to compare their performance using **Nsight Systems** profiling tools.

## 📊 Execution Models

### 🔹 1. Synchronous
Traditional blocking execution. Data transfers and kernel executions occur one after another.

### 🔹 2. Asynchronous
Leverages `async` queues to overlap data transfers and computations, reducing idle time.

### 🔹 3. Advanced Asynchronous
Employs multiple streams to maximize overlap and parallelism across data movement and kernel execution.

---

## 🛠️ How to Build

Use the following `nvc` compiler command to build the project:

```bash
nvc -acc -Minfo=accel -fast -o openacc_test openacc_test.c
```

## 📈 Profiling with Nsight Systems
Profile your executable to analyze execution and network communication performance:

```bash
nsys profile \
  --trace=cuda,openacc,nvtx \
  --nic-metrics=true \
  --stats=true \
  --force-overwrite=true \
  --output=profile_with_nic \
  ./openacc_test
```

## Why Use --nic-metrics=true?
The --nic-metrics=true flag enables profiling of network interface controller (NIC) activity — critical in high-performance and distributed computing environments.

It collects data such as:

✅ Packet send/receive rate

✅ Bandwidth utilization

✅ Error counts


<pre lang="markdown"> ## 📌 Benefits of NIC Profiling ### 1. 🔍 Identify Network Bottlenecks Determine if communication delays (not GPU/CPU) are the main cause of performance slowdowns. ### 2. 🔄 Correlate Network & Compute Activity See how NIC activity overlaps with GPU kernels, MPI calls, or CPU threads. Detect whether communication stalls computation. ### 3. 🧪 Optimize Distributed Workloads Useful for improving performance in: - MPI-based applications - Deep learning frameworks (e.g., Horovod, DDP) - Scientific simulations with halo exchange patterns ### 4. 📡 Ideal for InfiniBand & NVLink Fabrics Best used on systems with: - Mellanox/NVIDIA ConnectX NICs - NVLink-over-Fabric - InfiniBand clusters ### 5. ⚖️ Balance Communication & Computation Answer questions like: - Is your GPU waiting on data? - Are network links underutilized? - Can communication be overlapped with computation? --- ## 📄 What You Get in the Nsight Report With `--nic-metrics=true`, the Nsight Systems report will include: - 📊 NIC bandwidth usage (GB/s) - 📦 Packet counts and sizes - 🧵 NIC queue activity - 🕒 Timeline correlation with CPU and GPU operations --- ## 📚 Additional Resources - [NVIDIA Nsight Systems Documentation](https://docs.nvidia.com/nsight-systems/) - [OpenACC Programming Guide](https://www.openacc.org/) </pre>


## ✅ License

This project is open-source. Use it for learning, benchmarking, and profiling your own OpenACC applications.


