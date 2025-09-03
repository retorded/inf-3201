FROM ubuntu:22.04

# Avoid interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Install system dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    gdb \
    valgrind \
    cmake \
    git \
    curl \
    wget \
    vim \
    nano \
    htop \
    tree \
    sudo \
    && rm -rf /var/lib/apt/lists/*

# Install OpenMPI
RUN apt-get update && apt-get install -y \
    openmpi-bin \
    openmpi-common \
    libopenmpi-dev \
    && rm -rf /var/lib/apt/lists/*

# Create a non-root user
RUN useradd -m -s /bin/bash -G sudo dev && \
    echo "dev ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers

# Set up MPI environment for the user
USER dev
WORKDIR /home/dev

# Configure MPI to allow running as any user (needed for containers)
RUN echo 'export OMPI_ALLOW_RUN_AS_ROOT=1' >> ~/.bashrc && \
    echo 'export OMPI_ALLOW_RUN_AS_ROOT_CONFIRM=1' >> ~/.bashrc && \
    echo 'export OMPI_MCA_btl_vader_single_copy_mechanism=none' >> ~/.bashrc

# Verify installations
RUN mpicc --version && \
    mpiexec --version && \
    echo "MPI development environment ready!"

# Set the default command
CMD ["/bin/bash"]