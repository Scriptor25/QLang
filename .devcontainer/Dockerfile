FROM mcr.microsoft.com/devcontainers/cpp:debian-12

# Install necessary dependencies
RUN export DEBIAN_FRONTEND=noninteractive
RUN apt-get update && \
	apt-get upgrade -y
RUN apt-get install -y --no-install-recommends \
	wget \
	gpg \
	software-properties-common \
	cmake \
	gdb
RUN apt-get update && apt-get autoremove -y

# Install LLVM 18
RUN wget https://apt.llvm.org/llvm.sh && \
    chmod +x llvm.sh && \
    ./llvm.sh 18 && \
    rm llvm.sh
