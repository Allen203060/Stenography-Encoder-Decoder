# C Project Specification: Steganography Encoder/Decoder

## 1. Project Overview
This project involves building a command-line Steganography tool in C that can hide secret text inside a Bitmap (.bmp) image file, and later extract that text back out. The project utilizes Least Significant Bit (LSB) steganography to alter the image's binary data imperceptibly. Furthermore, it incorporates core Operating System concepts such as memory-mapped files and multithreading to optimize performance for large images.

## 2. Learning Objectives
### C Programming Concepts
*   **Bitwise Operations**: Using `&`, `|`, `<<`, `>>` for precise byte-level manipulation.
*   **Struct Packing**: Defining binary-safe structs using `__attribute__((packed))` to correctly parse file headers without compiler-injected padding bytes.
*   **Pointer Math**: Navigating memory buffers and casting data types safely.

### Operating System Concepts
*   **Virtual Memory & File Systems**: Utilizing the `mmap()` system call to map files directly into process memory, bypassing standard buffered I/O for high-performance file manipulation.
*   **Concurrency**: Using POSIX threads (`pthreads`) to divide the encoding/decoding workload across multiple CPU cores.
*   **Process Synchronization**: Avoiding race conditions when calculating memory offsets for threads.

## 3. Project Structure
To adhere to the best practices defined in your project guidelines, the codebase will follow a standard C project layout:

```text
c_steg_project/
├── .gitignore               # Ignore .o files and executable
├── Makefile                 # Build automation script
├── README.md                # Usage instructions and project details
├── LICENSE                  # Open-source license (e.g., MIT)
├── include/                 # Header files
│   ├── bmp.h                # BMP header struct definitions
│   └── steganography.h      # Core logic function prototypes
└── src/                     # Source files
    ├── main.c               # CLI argument parsing and entry point
    ├── bmp_parser.c         # Functions to read/validate BMP files
    └── steganography.c      # Bitwise encoding/decoding logic
```

## 4. Implementation Phases

### Phase 1: BMP Header Parsing
*   **Goal**: Open a `.bmp` file and correctly parse the first 54 bytes to determine the image's width, height, and pixel array offset.
*   **Details**: Create a `bmp_header` struct. It is critical to use compiler directives to prevent the compiler from padding the struct, ensuring a direct 1:1 mapping with the file bytes.

### Phase 2: LSB Encoding/Decoding (The Core Logic)
*   **Goal**: Hide a message string inside the image's pixel data.
*   **Algorithm**:
    1.  Take a character of the secret message (e.g., `A` = `01000001`).
    2.  Extract the first bit (`0`).
    3.  Take a single color byte from a pixel (e.g., a Red value `11001010`).
    4.  Clear its least significant bit and insert the secret bit (`11001010`).
    5.  Repeat this for 8 pixels to hide a single character.
*   **Null Terminator**: Ensure a `\0` byte is encoded at the end of the message so the decoder knows when to stop reading.

### Phase 3: OS Integration - Memory Mapping (`mmap`)
*   **Goal**: Replace `fopen`/`fread`/`malloc` with `mmap`.
*   **Details**: Use `open()` to get a file descriptor, then `mmap()` to map the image file into virtual memory with `PROT_READ | PROT_WRITE`. This allows modifying the image directly in memory without manually managing data buffers.

### Phase 4: Concurrency with Pthreads
*   **Goal**: Speed up the processing of very large images.
*   **Details**: Spin up `N` threads (e.g., 4 threads). Divide the image pixel array into 4 chunks. Calculate exact byte offsets for each thread so they can encode different parts of the message concurrently without overlapping.

## 5. Command-Line Interface (CLI) Usage
The tool will be executed from the terminal with the following arguments:

**Encoding a message:**
```bash
./steg --encode --image input.bmp --message "Secret String" --output hidden.bmp
```

**Decoding a message:**
```bash
./steg --decode --image hidden.bmp
```

## 6. Next Steps
1. Create the base project directory.
2. Initialize the `Makefile`, `.gitignore`, and the `src/` & `include/` directories.
3. Write a simple `main.c` to parse the CLI arguments (`--encode`, `--decode`, etc.).
