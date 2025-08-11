# Hướng Dẫn Cài Đặt Môi Trường Phát Triển Scrabble Game

## 1. Windows Setup

### Option A: Visual Studio (Recommended for beginners)
1. Tải và cài đặt Visual Studio Community 2022 (miễn phí)
2. Chọn workload "Desktop development with C++"
3. Tạo new project -> "Empty Project"
4. Copy tất cả files trong src/ vào project
5. Cấu hình SDL2:
   - Tải SDL2-devel-2.x.x-VC.zip từ https://www.libsdl.org/download-2.0.php
   - Tải SDL2_ttf-devel-2.x.x-VC.zip từ https://www.libsdl.org/projects/SDL_ttf/
   - Giải nén vào thư mục libs/
   - Project Properties -> C/C++ -> Additional Include Directories: libs/SDL2/include, libs/SDL2_ttf/include
   - Project Properties -> Linker -> Additional Library Directories: libs/SDL2/lib/x64, libs/SDL2_ttf/lib/x64
   - Project Properties -> Linker -> Additional Dependencies: SDL2.lib, SDL2main.lib, SDL2_ttf.lib

### Option B: MSYS2/MinGW (Advanced users)
1. Cài đặt MSYS2 từ https://www.msys2.org/
2. Mở MSYS2 terminal và chạy:
   ```bash
   pacman -Syu
   pacman -S mingw-w64-x86_64-gcc
   pacman -S mingw-w64-x86_64-SDL2
   pacman -S mingw-w64-x86_64-SDL2_ttf
   pacman -S make
   ```
3. Thêm C:\msys64\mingw64\bin vào PATH
4. Compile bằng: `make`

## 2. Linux Setup (Ubuntu/Debian)

```bash
# Update package list
sudo apt update

# Install build tools
sudo apt install build-essential

# Install SDL2 libraries
sudo apt install libsdl2-dev libsdl2-ttf-dev

# Navigate to project directory
cd path/to/btl-scrabble-hieu

# Compile
make

# Run
./scrabble
```

## 3. macOS Setup

### Với Homebrew:
```bash
# Install Homebrew if not already installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install sdl2 sdl2_ttf

# Compile
make

# Run
./scrabble
```

## 4. Xác Minh Cài Đặt

Để kiểm tra xem mọi thứ đã được cài đặt đúng:

### Windows (Command Prompt):
```cmd
g++ --version
```

### Linux/macOS:
```bash
g++ --version
sdl2-config --version
```

## 5. Troubleshooting

### Lỗi thường gặp:

1. **"SDL.h not found"**
   - Kiểm tra đường dẫn include
   - Đảm bảo SDL2 dev libraries đã được cài đặt

2. **"undefined reference to SDL_main"**
   - Thêm `-lSDL2main` vào linker flags
   - Thêm `#define SDL_MAIN_HANDLED` trước #include <SDL.h>

3. **"Cannot find -lSDL2"**
   - Kiểm tra library paths
   - Đảm bảo SDL2 libraries có trong system PATH

4. **Font not found errors**
   - Đảm bảo thư mục assets/fonts/ tồn tại
   - Kiểm tra đường dẫn font trong code

### Debug tips:
- Compile với flag `-g` để có debug info
- Sử dụng gdb trên Linux/macOS hoặc Visual Studio debugger trên Windows
- Kiểm tra console output để debug logic errors

## 6. IDE Recommendations

- **Windows**: Visual Studio Community, Visual Studio Code, CLion
- **Linux**: Visual Studio Code, CLion, Qt Creator, vim/emacs
- **macOS**: Xcode, Visual Studio Code, CLion

## 7. Additional Resources

- SDL2 Documentation: https://wiki.libsdl.org/
- SDL2_ttf Documentation: https://www.libsdl.org/projects/SDL_ttf/docs/
- C++ Reference: https://cppreference.com/
- Git tutorial: https://git-scm.com/docs/gittutorial
