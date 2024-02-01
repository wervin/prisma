# Prisma Ray Marching Renderer
![test](https://github.com/wervin/prisma/assets/29201790/71ffc6af-18e8-47dc-95ca-1ffc162d8f46)
## Introduction


Prisma is a ray marching renderer developed as a hobby project. It's a platform for experimenting with 3D rendering and learning new techniques in graphics programming.

## Key Features

- Ray Marching Rendering: Basic implementation for educational purposes.
- Shader Exploration: Experiment with simple shaders.
- User Interface: Straightforward and easy to use.

## Setup
This project is developed and tested on Ubuntu 23.10. There are no current plans to port it to Windows or Mac.

## Prerequisites
- CMake
- GCC
- libgtk-3-dev
- python3
- LuaJIT

## Building and Running Prisma

1. **Clone the Repository**

   Clone the **Prisma** repository to your local machine using Git:
   ```
   git clone https://github.com/wervin/prisma.git
   cd prisma
   ```

2. **Configure and Build the Project**

   Create a build directory and navigate into it:
   ```
   mkdir build
   cd build
   ```

   Configure the project build with CMake:
   ```
   cmake ..
   ```

   Compile the project:
   ```
   make
   ```

3. **Running Prisma**

    After successful compilation, run **Prisma**:

    ```
    ./prisma
    ```
