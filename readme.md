# Sorting Visualizer

A **C++ application built with SFML** to visualize **10 sorting algorithms** with an interactive user interface, control panel, real-time stats, pause/resume functionality, and speed control. 

---

## Features

### Sorting Algorithms

* **O(n²):** Bubble Sort, Selection Sort, Insertion Sort
* **O(n log n):** Quick Sort, Merge Sort, Heap Sort, Tim Sort
* **O(n + k) / O(nk):** Counting Sort, Radix Sort, Bucket Sort

### Visualization

* Displays 100 bars with dynamic animations for comparisons, pivots, and placements.
* Smooth animations at 60 FPS.

### User Interface

* Control panel with clickable algorithm buttons (hover and active states)
* Instructions for keyboard controls
* Stats panel showing comparisons, swaps, and speed
* Details panel with algorithm name, complexity, and description

### Interactivity

* Select algorithms via mouse clicks or keys (1–0)
* Pause/resume with 'P', adjust speed with '+' / '-', reset with 'R', stop with any key

### Font

Uses **DejaVuSans.ttf** or any TrueType font (e.g., arial.ttf).

---

## Prerequisites

* C++ compiler (GCC, Clang, or MSVC, C++11 or later)
* SFML 2.5 or later (`sfml-graphics`, `sfml-window`, `sfml-system`)
* TrueType font (e.g., DejaVuSans.ttf)

---

## Installation

### Install SFML

**Linux (Debian/Ubuntu):**

```bash
sudo apt update
sudo apt install libsfml-dev
```

**Linux (Fedora):**

```bash
sudo dnf install sfml sfml-devel
```

**Windows:**

* Download SFML from [sfml-dev.org](https://www.sfml-dev.org/).
* Extract to a directory (e.g., `C:\SFML`).
* Add `C:\SFML\bin` to your system PATH.
* Link libraries in your IDE/project settings.

**macOS:**

```bash
brew install sfml
```

### Install C++ Compiler

**Linux:**

```bash
sudo apt install g++       # Ubuntu
sudo dnf install gcc-c++   # Fedora
```

**Windows:** Use MinGW or Visual Studio (MSVC).

**macOS:** Install Xcode or:

```bash
brew install gcc
```

### Set Up Font

* Download DejaVuSans.ttf from [dejavu-fonts.org](https://dejavu-fonts.github.io/).
* Or use a system font (e.g., `arial.ttf` on Windows).
* Place the font in your project directory or update the path in `sorting_visualizer.cpp`:

```cpp
font.loadFromFile("path/to/DejaVuSans.ttf");
```

---

## Get the Code

Clone or download `sorting_visualizer.cpp` into your project directory.

---

## Compilation and Running

### Compile

**Linux/macOS:**

```bash
g++ sorting_visualizer.cpp -o visualizer -lsfml-graphics -lsfml-window -lsfml-system
```

**Windows (MinGW):**

```bash
g++ sorting_visualizer.cpp -o visualizer -I"C:\SFML\include" -L"C:\SFML\lib" -lsfml-graphics -lsfml-window -lsfml-system
```

### Run

**Linux/macOS:**

```bash
./visualizer
```

**Windows:** Run `visualizer.exe` or use your IDE.

---

## Usage

* **Select Algorithm:** Click buttons or press keys `1–0` to choose an algorithm.
* **Controls:**

  * `P`: Pause/resume sorting
  * `+ / -`: Adjust animation speed (faster/slower)
  * `R`: Reset to a new random array
  * Any key: Stop current sorting



## Troubleshooting

* **Compilation Errors:** Verify SFML and compiler installation. Check library/include paths.
* **Font Not Found:** Ensure the font file is accessible or use a system font.
* **UI Issues:** Adjust text sizes/positions in the constructor.
* **Pause/Resume Issues:** Check `isPaused` usage in sorting methods; test with a slower `delayMs`.
* **Performance:** Reduce bar count or increase `delayMs` for smoother animations.

---

## Contributing

Contributions are welcome! Fork the repository, make changes, and submit a pull request with a clear description of your updates.

---
