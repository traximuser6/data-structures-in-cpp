# 🧠 Data Structures in C++

A clean, modern practice repository for classic data structures and algorithms — all implemented in **C++17**! 💻✨

Perfect for learning, experimenting, or brushing up your fundamentals with real code.

---

## 🛠️ Requirements

- **C++17-compatible compiler**  
  (GCC 9+, Clang 10+, MSVC 19.20+)
- **CMake ≥ 3.16**
- **Build system**: Make, Ninja, or MinGW (or just use an IDE!)

---

## 🗂️ Project Structure

```
├── exercises/          → 🚀 One `.cpp` file = one standalone executable (great for quick experiments!)
├── projects/
│   └── banking-app/    → 💼 A small but complete project example (OOP + data structures in action)
├── samples/            → 📚 Clean, well-commented reference implementations
└── README.md           → You're reading it! 😊
```

---

## ▶️ How to Build & Run

### 💡 With CLion (recommended)
1. Open the folder in CLion  
2. CMake auto-configures everything  
3. Press **Run** or **Build** — done! 🎯

### 💻 From Terminal

```bash
# Clone (if needed) and enter the repo
cd data-structures-cpp

# Create and enter build directory
mkdir -p build && cd build

# Configure with CMake
cmake .. -G "MinGW Makefiles"    # 🪟 Windows
# OR
cmake .. -G "Unix Makefiles"     # 🐧 Linux / 🍏 macOS

# Build everything in Release mode
cmake --build . --config Release

# Run your code!
./exercises/exercise_001          # 🔍 Try an exercise
./projects/banking-app/banking_app # 💰 Explore the banking demo
./samples/sample_001              # 📘 Study a clean implementation
```

> ✅ **Pro Tip**: All executables are built with the same name as their source file!

---

## ➕ Adding a New Exercise

Just drop a new `.cpp` file into the `exercises/` folder — **CMake auto-detects it** and builds a dedicated executable! 🪄

Example:
```bash
echo 'int main() { return 0; }' > exercises/my_new_ds.cpp
# Re-run cmake --build . and you're ready to go!
```

---

Happy coding! 🎉  
May your pointers be safe and your builds green. 🌿
