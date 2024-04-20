# Use an official Python runtime as a parent image
FROM python:3.10-bookworm

# Set a maintainer name to let people know who made this image
LABEL maintainer="Shan Jiang <sjiang340@gatech.edu>"

# Install CMake, g++, make, and any other dependencies
RUN apt-get update && apt-get install -y \
    sudo \
    cmake \
    g++ \
    make \
    vim \
    # libonnxruntime-dev \
    # Additional libraries here
    && rm -rf /var/lib/apt/lists/*

# Copy just the requirements.txt first to leverage Docker cache
COPY requirements.txt /tmp/
RUN pip install --requirement /tmp/requirements.txt || \
    (echo "Failed to install dependencies from requirements.txt" && exit 1)

# Set the working directory to /app
WORKDIR /app

# Copy the current directory contents into the container at /app
COPY . /app

# Define environment variable
ENV NAME World

# Make port 80 available to the world outside this container
EXPOSE 80

# Add user for running the application
# User setup
RUN useradd -m shan -s /bin/bash \
    && echo 'shan ALL=(ALL) NOPASSWD: ALL' >> /etc/sudoers \
    && chown -R shan:shan /app

USER shan

# Run app.py when the container launches
# CMD ["python", "app.py"]
