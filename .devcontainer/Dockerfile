FROM mcr.microsoft.com/devcontainers/cpp:1-debian-11

# Install necessary dependencies
# RUN apt-get update && \
#     export DEBIAN_FRONTEND=noninteractive && \
#     apt-get install -y --no-install-recommends

# Install LLVM 18
RUN wget https://apt.llvm.org/llvm.sh && \
    chmod +x llvm.sh && \
    ./llvm.sh 18 && \
    rm llvm.sh
