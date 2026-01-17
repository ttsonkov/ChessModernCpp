# Modern Chess

A modern, object-oriented chess application written in C++20 with SFML graphics and drag-and-drop piece movement.

## Features

- **Full Chess Rules**: Complete implementation of standard chess rules including:
  - Pawn movement and capture
  - Knight, Bishop, Rook, Queen, King piece movements
  - Castling support
  - En-passant capture
  - Check and checkmate detection

- **Interactive GUI**: 
  - Drag-and-drop piece movement
  - Visual board with alternating squares
  - Selected square highlighting
  - Real-time move validation

- **Clean Architecture**:
  - SOLID principles throughout
  - Interface-based design for extensibility
  - Decoupled input, rendering, and game logic
  - Modern C++20 with smart pointers

## Prerequisites

### Windows
- **Visual Studio 2022** (Community Edition or higher)
- **CMake 3.20+** ([Download](https://cmake.org/download/))
- **vcpkg** ([Installation Guide](https://github.com/Microsoft/vcpkg))

### macOS
- **Xcode Command Line Tools**: `xcode-select --install`
- **CMake 3.20+**: `brew install cmake`
- **vcpkg**: `brew install vcpkg`

### Linux (Ubuntu/Debian)
```bash
sudo apt-get update
sudo apt-get install cmake git build-essential pkg-config
git clone https://github.com/Microsoft/vcpkg.git ~/vcpkg
~/vcpkg/./bootstrap-vcpkg.sh
```

## Building from Source

### 1. Clone and Setup Dependencies

```bash
git clone <repository-url>
cd ModernChess

# On Windows (in PowerShell)
$env:VCPKG_ROOT = "C:\path\to\vcpkg"

# On macOS/Linux (in bash/zsh)
export VCPKG_ROOT=$HOME/vcpkg
```

### 2. Create Build Directory

```bash
mkdir build
cd build
```

### 3. Configure with CMake

**Windows (Visual Studio):**
```bash
cmake -G "Ninja" `
  -DCMAKE_TOOLCHAIN_FILE="$env:VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" `
  -DCMAKE_BUILD_TYPE=Debug `
  ..
```

**macOS/Linux:**
```bash
cmake -G "Ninja" \
  -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" \
  -DCMAKE_BUILD_TYPE=Debug \
  ..
```

### 4. Build

```bash
cmake --build .
```

Executable will be at: `./ModernChess` (or `ModernChess.exe` on Windows)

### 5. Add Assets

Create an `assets` folder next to the executable and add your chess piece sprites:

```bash
mkdir assets
# Copy pieces.png to assets/ folder
```

**Note**: Assets are automatically copied to the build output directory during the post-build step.

## Running the Application

```bash
./ModernChess          # macOS/Linux
ModernChess.exe        # Windows
```

### Controls

- **Left Mouse Button**: Click to select a piece, drag to move
- **Close Window**: Click the window close button to exit

## Architecture

### High-Level Overview

```
???????????????????????????????????????
?         Application (Main Loop)     ?
???????????????????????????????????????
?  • Coordinates game loop            ?
?  • Manages input and rendering      ?
?  • Delegates to specialized modules ?
???????????????????????????????????????
           ?
     ???????????????????????????????????????????
     ?            ?             ?              ?
??????????  ????????????  ????????????  ???????????
?  IGame ?  ?IRenderer ?  ?IInputHdlr?  ?IRenderer?
??????????  ????????????  ????????????  ???????????
? Logic  ?  ?  Draw    ?  ?  Input   ?  ? Render  ?
??????????  ????????????  ????????????  ???????????
     ?           ?             ?             ?
??????????  ????????????  ????????????      ?
?ChessGme?  ?SfmlRender?  ?SfmlInput ?      ?
??????????  ????????????  ????????????      ?
?• Board ?  ?• Sprites ?  ?• Events  ?      ?
?• Rules ?  ?• Glyphs  ?  ?• Drag    ?      ?
?• Moves ?  ?• Shapes  ?  ?• Clicks  ?      ?
??????????  ????????????  ????????????      ?
    ?                                        ?
???????????????????          ??????????????????
?  Game State     ?          ?   SFML Window  ?
???????????????????          ??????????????????
?• Pieces         ?          ?• Rendering     ?
?• Board Config   ?          ?• Input Events  ?
?• Move History   ?          ?• Display       ?
???????????????????          ??????????????????
```

### Module Breakdown

#### Core (Chess Logic)

- **`IGame`**: Abstract interface for game state and moves
  - Manages board state
  - Validates and applies moves
  - Tracks side to move
  - Provides legal move generation

- **`ChessGame`**: Concrete implementation of `IGame`
  - Maintains board and game rules
  - Handles castling rights and en-passant
  - Delegates rule validation to `Rules`

- **`Board`**: Internal board representation
  - 8×8 grid of optional pieces
  - Piece placement and movement
  - Initial position setup

- **`Rules`**: Chess rule engine
  - Legal move generation for each piece type
  - Check/checkmate detection
  - Move validation (ensuring king isn't in check)

- **`Move`**: Data structure for chess moves
  - Source and destination squares
  - Special move flags (castling, en-passant, promotion)

#### UI (User Interface)

- **`IRenderer`**: Abstract rendering interface
  - Defines rendering contract
  - Enables different rendering backends

- **`SfmlRenderer`**: SFML-based renderer
  - Draws chess board with alternating colors
  - Renders pieces (textures, glyphs, or shapes)
  - Shows selection highlights
  - Animates dragged pieces
  - Modular drawing methods:
    - `drawBoard()`: Board squares
    - `drawSelectionHighlight()`: Selected piece indicator
    - `drawPieces()`: All game pieces
    - `drawDraggedPiece()`: Piece being dragged

- **`IInputHandler`**: Abstract input interface
  - Decouples input handling from rendering
  - Enables keyboard/gamepad/network inputs

- **`SfmlInputHandler`**: SFML-based input
  - Processes mouse events
  - Converts pixels to board squares
  - Generates `Move` objects from drag operations
  - Maintains drag state across frames

#### App (Application)

- **`Application`**: Main application class
  - Coordinates game loop
  - Processes input ? validates with rules ? updates game state
  - Updates renderer with current state
  - Handles window lifecycle

## Project Structure

```
ModernChess/
??? src/
?   ??? main.cpp                    # Entry point
?   ??? app/
?   ?   ??? Application.hpp         # Main app coordinator
?   ?   ??? Application.cpp         # App implementation
?   ??? core/
?   ?   ??? IGame.hpp               # Game interface
?   ?   ??? ChessGame.hpp/cpp       # Game implementation
?   ?   ??? Board.hpp/cpp           # Board state
?   ?   ??? Rules.hpp/cpp           # Chess rules engine
?   ?   ??? Piece.hpp               # Piece types
?   ?   ??? Move.hpp                # Move structure
?   ??? ui/
?       ??? IRenderer.hpp           # Renderer interface
?       ??? SfmlRenderer.hpp/cpp    # SFML renderer
?       ??? IInputHandler.hpp       # Input interface
?       ??? SfmlInputHandler.hpp/cpp# SFML input
??? assets/
?   ??? pieces.png                  # Chess piece sprite sheet
??? CMakeLists.txt                  # Build configuration
??? README.md                        # This file
```

## Design Patterns

### SOLID Principles

1. **Single Responsibility**: Each class has one reason to change
   - `Board` manages piece placement only
   - `Rules` handles move validation only
   - `SfmlRenderer` handles rendering only

2. **Open/Closed**: Open for extension, closed for modification
   - New renderers via `IRenderer` implementation
   - New input methods via `IInputHandler` implementation
   - New piece logic via extending `Rules`

3. **Liskov Substitution**: Subtypes are interchangeable
   - Any `IGame` implementation works with `Application`
   - Any `IRenderer` implementation works seamlessly
   - Any `IInputHandler` works without modification

4. **Interface Segregation**: Clients depend only on what they need
   - `IRenderer` only exposes rendering
   - `IInputHandler` only exposes input
   - Small, focused interfaces

5. **Dependency Inversion**: Depend on abstractions, not concretions
   - `Application` uses `IGame`, not `ChessGame`
   - `Application` uses `IRenderer`, not `SfmlRenderer`
   - Loose coupling enables testing and extension

### Other Patterns

- **Strategy Pattern**: Different move generators for each piece type
- **Template Method**: Rule validation flow in `Rules::legalMoves()`
- **Factory Pattern**: `Application` creates concrete implementations

## Dependencies

- **SFML 2.6**: Graphics and windowing library
- **ImGui-SFML**: (Optional) For future UI enhancements

Both are managed via vcpkg and specified in `CMakeLists.txt`.

## Extending the Application

### Adding a New Renderer

1. Create `NewRenderer.hpp` inheriting from `IRenderer`
2. Implement `render(const chess::IGame&)` method
3. Update `Application` to instantiate your renderer
4. Existing code needs no changes (OCP)

### Adding Keyboard Input

1. Create `KeyboardInputHandler.hpp` inheriting from `IInputHandler`
2. Process keyboard events instead of mouse
3. Swap in `Application` constructor
4. No other code changes needed

### Adding AI Player

1. Create `AIPlayer` implementing `IGame`
2. Implement `makeMove()` to call AI logic
3. Wrap real game with AI adapter if needed
4. Application runs unchanged

## Troubleshooting

### Build Issues

**"CMake could not find SFML"**
- Ensure vcpkg is installed and `VCPKG_ROOT` is set correctly
- Run vcpkg installation: `vcpkg install sfml:x64-windows` (or your platform)

**"Compiler not found"**
- Windows: Ensure Visual Studio 2022 is installed
- macOS: Run `xcode-select --install`
- Linux: Install build tools (see Prerequisites)

### Runtime Issues

**"pieces.png not found"**
- Ensure `assets/pieces.png` exists next to executable
- Check file permissions
- Run from correct directory

**"Renderer is not a SfmlRenderer"**
- This means a different renderer type was used
- Should not occur with default build

## Performance Considerations

- **Piece movement**: O(1) board updates
- **Move generation**: O(1) per piece on typical board
- **Rendering**: O(64) for board squares + O(pieces) for rendering
- **Drag rendering**: Uses same piece sprites, minimal overhead

## Future Improvements

- [ ] Pawn promotion UI
- [ ] Move history display
- [ ] Game save/load (PGN format)
- [ ] AI opponent using minimax
- [ ] Network multiplayer
- [ ] Undo/redo functionality
- [ ] Adjustable board colors
- [ ] Sound effects

## License

MIT License - See LICENSE file for details

## Contributing

Pull requests welcome! Please ensure:
- Code follows existing style conventions
- All changes maintain SOLID principles
- New features include appropriate abstraction layers
- Build passes with no warnings

---

**Questions?** Open an issue on the repository.
