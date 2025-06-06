# **Retard Player**  
A cross-platform video player built in C++.

## **How to Build and Run**

### **Step 1: Clone the Repository**  
Clone the repository using Git or download the ZIP file.

```bash
git clone <https://github.com/RetardDev/RetardPlayer.git>
```

Alternatively, download the ZIP from: [repository link](https://github.com/RetardDev/RetardPlayer/archive/refs/heads/main.zip)

### **Step 2: Navigate to the Project Directory**

```bash
cd RetardPlayer
```

### **Step 3: Create a Build Directory**  
It's recommended to use a separate build directory for organizing files.

```bash
mkdir build
cd build
```

### **Step 4: Configure the Build System**  
Run `cmake` to configure the build system. Ensure you have `cmake` installed on your system. 

For **Windows**:
```bash
cmake .. -G "MinGW Makefiles"
```

For **Linux** and **Mac**:
```bash
cmake ..
```

### **Step 5: Compile the Project**  

On **Windows**:
```bash
mingw32-make
```

On **Linux** and **Mac**:
```bash
make
```

### **Step 6: Run the Application**

```bash
./RetardPlayer
```

On **Windows**, the command may be:
```bash
RetardPlayer.exe
```

---

## **Output**  
The application should look like this: (RetardDev/RetardPlayer/raw/main/screenshots/example.png)

---

### **Prerequisites**

Ensure the following dependencies are installed:  
- **CMake** (3.10 or newer)  
- **A C++ Compiler**:
  - GCC (Linux)
  - Clang (Mac)
  - MinGW/Visual Studio (Windows)  

- SDL2 and FFmpeg Libraries

