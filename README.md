# Modern Chess

A modern chess application built with C++20 and SFML, featuring smooth animations, drag-and-drop interaction, and clean architecture following SOLID principles.

> **📖 The idea of the project is to showcase the best practices in C++ described here: [Coding Guidelines (CLAUDE.md)](CLAUDE.md) for project standards and conventions.**

![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)
![SFML](https://img.shields.io/badge/SFML-2.6-green.svg)
![CMake](https://img.shields.io/badge/CMake-3.20+-orange.svg)

## ✨ Features

### Chess Rules
- Complete standard chess implementation
- All piece movements (Pawn, Knight, Bishop, Rook, Queen, King)
- Special moves: Castling, En-passant, Pawn promotion
- Check and checkmate detection
- Legal move validation

### User Interface
- **Drag-and-drop** piece movement
- **Smooth animations** for piece movement
- **Legal move highlighting** (dots for moves, rings for captures)
- **Selection highlighting** for picked pieces
- Three rendering modes: textures, Unicode glyphs, or simple shapes

### Architecture
- Modern C++20 with smart pointers and RAII
- Interface-based design (SOLID principles)
- Decoupled modules: Core logic, UI, Application
- Extensible for new renderers or input methods

---

## 🚀 Quick Start

### Prerequisites

| Platform | Requirements |
|----------|-------------|
| **Windows** | Visual Studio 2022, CMake 3.20+, vcpkg |
| **macOS** | Xcode CLI tools, CMake 3.20+, vcpkg |
| **Linux** | GCC 11+/Clang 14+, CMake 3.20+, vcpkg |

### Build

``` bash
# Clone repository
git clone https://github.com/ttsonkov/ChessModernCpp.git
cd ChessModernCpp

# Configure (adjust VCPKG_ROOT path)
cmake -B build -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" \
  -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build

# Run
./build/ModernChess
```

### Assets

Place `pieces.png` sprite sheet in `assets/` folder. The application searches:
- Current working directory
- Executable directory
- Parent directories

---

## 🎮 Controls

| Action | Input |
|--------|-------|
| Select piece | Left-click on piece |
| Move piece | Drag and drop |
| Cancel selection | Click empty square |
| Quit | Close window |

---

## 📁 Project Structure

```
ModernChess/
├── src/
│   ├── main.cpp                  # Application entry point
│   ├── Config.hpp                # Global configuration constants
│   ├── app/
│   │   ├── Application.hpp/cpp   # Main game loop coordinator
│   ├── core/                     # Chess logic (no UI dependencies)
│   │   ├── Types.hpp             # Common types (Square, Piece, Color)
│   │   ├── Fwd.hpp               # Forward declarations
│   │   ├── Piece.hpp             # Piece type (legacy include)
│   │   ├── Move.hpp              # Move data structure
│   │   ├── Board.hpp/cpp         # 8x8 board representation
│   │   ├── IGame.hpp             # Game interface
│   │   ├── ChessGame.hpp/cpp     # Game implementation
│   │   └── Rules.hpp/cpp         # Legal move generation
│   └── ui/                       # User interface layer
│       ├── IRenderer.hpp         # Renderer interface
│       ├── SfmlRenderer.hpp/cpp  # SFML renderer implementation
│       ├── IInputHandler.hpp     # Input interface
│       └── SfmlInputHandler.hpp/cpp
├── assets/
│   └── pieces.png                # Sprite sheet (6x2 grid)
├── CMakeLists.txt                # Build configuration
├── CLAUDE.md                     # Coding guidelines
└── README.md
```

---

## 🏗️ Architecture

### Layer Diagram

```
┌─────────────────────────────────────────────┐
│              app::Application               │
│    (Coordinates game loop, connects layers) │
└─────────────────┬───────────────────────────┘
                  │
        ┌─────────┴─────────┐
        ▼                   ▼
┌───────────────┐   ┌───────────────┐
│   ui::        │   │  chess::      │
│ IRenderer     │   │  IGame        │
│ IInputHandler │   │  Board        │
│ SfmlRenderer  │   │  Rules        │
│ SfmlInput...  │   │  ChessGame    │
└───────────────┘   └───────────────┘
```

### Key Types

| Type | Purpose |
|------|---------|
| `chess::Square` | Board coordinates (rank, file) |
| `chess::Piece` | Piece with type and color |
| `chess::Move` | Move from one square to another |
| `chess::Board` | 8x8 board state |
| `chess::IGame` | Game interface |
| `ui::IRenderer` | Rendering interface |
| `ui::IInputHandler` | Input interface |

### Namespaces

| Namespace | Purpose |
|-----------|---------|
| `chess` | Core chess logic |
| `ui` | User interface |
| `app` | Application layer |
| `config` | Configuration constants |

### Design Principles

- **Single Responsibility**: Each class has one job
- **Open/Closed**: Extend via new implementations
- **Liskov Substitution**: Interfaces are interchangeable
- **Interface Segregation**: Small, focused interfaces
- **Dependency Inversion**: Depend on abstractions

---

## 🎨 Rendering

`SfmlRenderer` supports three fallback modes:

1. **Sprite Textures** (preferred): Loads `pieces.png` sprite sheet
2. **Unicode Glyphs**: Falls back to chess Unicode characters (♔♕♖♗♘♙)
3. **Simple Shapes**: Circles with crown markers for non-pawns

### Sprite Sheet Format

```
┌─────┬─────┬─────┬─────┬─────┬─────┐
│  K  │  Q  │  R  │  B  │  N  │  P  │  ← White (row 0)
├─────┼─────┼─────┼─────┼─────┼─────┤
│  k  │  q  │  r  │  b  │  n  │  p  │  ← Black (row 1)
└─────┴─────┴─────┴─────┴─────┴─────┘
```

---

## 🔧 Extending

### Add a New Renderer

```cpp
class ConsoleRenderer : public ui::IRenderer {
public:
    void render(const chess::IGame& game) override {
        // Print board to console
    }
    void present() noexcept override { 
        std::cout.flush(); 
    }
};
```

### Add Keyboard Input

```cpp
class KeyboardInput : public ui::IInputHandler {
public:
    std::optional<chess::Move> processInput() override {
        // Handle arrow keys + enter
    }
    bool isRunning() const noexcept override { 
        return running_; 
    }
};
```

### Add AI Opponent

```cpp
class AIGame : public chess::IGame {
    // Wrap ChessGame and inject AI moves for one color
};
```

---

## 🐛 Troubleshooting

| Issue | Solution |
|-------|----------|
| CMake can't find SFML | Set `VCPKG_ROOT` and run `vcpkg install sfml` |
| pieces.png not found | Place in `assets/` next to executable |
| Pieces render as shapes | Check `pieces.png` path and format |
| Slow performance | Ensure Release build |

---

## 📋 Future Roadmap

- [ ] Pawn promotion dialog
- [ ] Move history panel
- [ ] PGN import/export
- [ ] AI opponent (minimax with alpha-beta)
- [ ] Network multiplayer
- [ ] Undo/redo
- [ ] Customizable themes
- [ ] Sound effects

---

## 📄 License

MIT License - see [LICENSE](LICENSE) for details.

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch
3. Follow [CLAUDE.md](CLAUDE.md) coding guidelines
4. Ensure build passes with no warnings
5. Submit a pull request

---

**Made with ♟️ and modern C++**
