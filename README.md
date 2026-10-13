# Multithreaded BMP Steganography Tool

A high-performance command-line and interactive terminal tool written in C that conceals secret messages inside 24-bit uncompressed Bitmap (`.bmp`) images using **Least Significant Bit (LSB)** steganography.

The project demonstrates low-level systems programming and operating systems concepts, including binary struct packing, zero-copy memory-mapped file I/O (`mmap`), multi-core parallelism using POSIX threads (`pthreads`), and an interactive ANSI Terminal User Interface (TUI).

---

## Features

- **Least Significant Bit (LSB) Steganography**: Embeds secret text bits imperceptibly into the image's pixel bytes (8 color bytes per hidden character).
- **Zero-Copy Memory Mapping (`mmap`)**: Maps image files directly into the process's virtual address space via POSIX `mmap()` with `MAP_SHARED`, eliminating manual buffer allocations and streaming file writes directly to disk.
- **Multithreaded Encoding (`pthreads`)**: Divides the secret message across multiple worker threads running concurrently across CPU cores. Each thread processes a disjoint memory region, achieving lock-free parallelism without race conditions.
- **Dual-Mode User Interface**:
  - **Interactive TUI**: Cyberpunk-style ANSI hacker terminal with box drawing, image capacity inspection, and guided encode/decode workflows.
  - **Fast Scriptable CLI**: Standard command-line flags (`--encode`, `--decode`, `--help`) for scripting and automation.
- **Binary-Safe Struct Packing**: Custom `bmp_header` defined with `__attribute__((packed))` to parse BMP headers reliably without compiler-injected padding.

---

## Project Structure

```text
c_proj/
├── .gitignore               # Ignores build artifacts and binaries
├── LICENSE                  # MIT License
├── Makefile                 # Automated build system
├── README.md                # Project documentation
├── include/                 # Header files
│   ├── bmp.h                # BMP header struct definition
│   ├── steganography.h      # Core steganography prototypes
│   └── tui.h                # TUI styles, colors, and interface declarations
└── src/                     # Source files
    ├── main.c               # Entry point and CLI argument dispatcher
    ├── steganography.c      # Bitwise encoding, decoding & pthread workers
    └── tui.c                # Interactive ANSI terminal UI & handlers
```

---

## Prerequisites

- **Operating System**: Linux or POSIX-compliant Unix environment
- **Compiler**: `gcc` (with C99 or later support)
- **Libraries**: POSIX Threads (`pthreads`), Standard C Library (`libc`)
- **Build Tool**: GNU `make`

---

## Building the Project

Compile the project using the provided `Makefile`:

```bash
make
```

To clean up compiled binaries and build artifacts:

```bash
make clean
```

---

## Usage

### 1. Interactive Terminal User Interface (TUI)

Launch the interactive interface by running the executable without arguments:

```bash
./steg
```

The menu provides:
1. `[1] 🔒 Encode Secret Message into BMP` — Prompts for the image file, secret message, and thread count.
2. `[2] 🔓 Decode Secret Message from BMP` — Maps the image in read-only mode and reveals the hidden text.
3. `[3] 📊 Inspect BMP Header & Capacity` — Displays image dimensions, bits per pixel, file size, and maximum text capacity.
4. `[4] 🚪 Exit`

---

### 2. Command-Line Interface (CLI)

#### Encoding a Message
Embed a message into an image using multiple threads (default: 4 threads):

```bash
./steg --encode <image.bmp> "<message>" [threads]
```

**Example:**
```bash
./steg --encode test.bmp "Confidential Operation Alpha" 4
```

#### Decoding a Message
Extract a hidden message from an image:

```bash
./steg --decode <image.bmp>
```

**Example:**
```bash
./steg --decode test.bmp
```

#### Help Menu
```bash
./steg --help
```

---

## Operating Systems & Systems Programming Concepts

### 1. Memory-Mapped I/O (`mmap`)
Rather than relying on `fopen`, `fread`, and `fwrite` (which copy data between kernel cache and user buffers), the application uses POSIX `open()` and `mmap()`:
```c
unsigned char *file_data = mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
```
- Modifications to `file_data` directly modify the file pages in the OS page cache.
- Calling `munmap()` and `close()` automatically flushes dirty pages back to disk with zero user-space buffer copies.
- For decoding, the file is mapped with `PROT_READ` for memory protection.

### 2. Parallelism without Locks (`pthreads`)
The message is partitioned into $N$ contiguous slices. Each slice maps to a non-overlapping segment of the pixel array:
$$\text{Thread Pixel Offset} = \text{Thread Character Offset} \times 8$$
Because each thread writes to its own distinct memory boundary:
- No two threads write to the same byte.
- No mutexes or locks are required, avoiding synchronization overhead and contention.

---

## License

This project is licensed under the [MIT License](LICENSE).
