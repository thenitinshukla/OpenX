# Compiler and Tools
NVCC       = nvc
NSYS       = nsys profile

# Directories
NVHPC_ROOT = /leonardo/prod/spack/06/install/0.22/linux-rhel8-icelake/gcc-8.5.0/nvhpc-24.5-torlmnyzcexnrs6pq4cccabv7ehkv3xy/Linux_x86_64/24.5
CUDA_VER   = 11.8
CUDA_PATH  = $(NVHPC_ROOT)/cuda/$(CUDA_VER)/targets/x86_64-linux

# NVTX include and lib paths
INC_NVTX   = -I$(CUDA_PATH)/include
LIB_NVTX   = -L$(CUDA_PATH)/lib -lnvToolsExt

# Compilation Flags
ACCFLAGS   = -acc
GPG        = -gpu=cc80
OPTFLAGS   = -fast
DEBUGFLAGS = -Minfo=accel
CFLAGS     = $(ACCFLAGS) $(GPG) $(OPTFLAGS) $(DEBUGFLAGS) $(INC_NVTX)
LDFLAGS    = $(LIB_NVTX)

# Nsight Systems options
NSYSFLAGS  = --trace=osrt,openacc,cuda,nvtx --stats=true --force-overwrite=true 

## --sample=cpu --backtrace=fp  --capture-range=nvtx --nvtx-capture='main_loop' --env-var=NSYS_NVTX_PROFILER_REGISTER_ONLY=0

# Source and Targets
SRC_FILES  = openacc_test.c
TARGETS    = $(SRC_FILES:.c=)

.PHONY: all clean profile help

# Help info
help:
	@echo "Usage:"
	@echo "  make           # build all targets"
	@echo "  make profile   # run Nsight profiling"
	@echo "  make clean     # clean binaries and reports"
	@echo ""
	@echo "To load NVIDIA compiler: module load nvhpc"
	@echo "Optional: make OPT= or USE=managed for tweaking"

# Default build
all: $(TARGETS)

# Compile rule
%: %.c
	$(NVCC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# Profile all targets with Nsight Systems
profile: $(TARGETS)
	$(foreach tgt,$^,$(NSYS) $(NSYSFLAGS) -o report_$(tgt) ./$(tgt);)

# Cleanup
clean:
	rm -f $(TARGETS) report_* *.nsys-rep *.sqlite

