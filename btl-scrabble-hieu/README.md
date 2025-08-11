# Scrabble Game - Bài Tập Lớn OOP

## Mô Tả
Đây là game Scrabble được phát triển bằng C++ với thư viện SDL2 cho môn Lập Trình Hướng Đối Tượng.

## Tính Năng
- Giao diện đồ họa với SDL2
- Board game Scrabble 15x15 với các ô premium
- Hệ thống chấm điểm chuẩn Scrabble
- Kiểm tra từ điển tự động
- 2 người chơi luân phiên
- Drag & drop để đặt gạch
- Hiển thị điểm số và thông tin game

## Yêu Cầu Hệ Thống
- C++ Compiler (g++ hoặc Visual Studio)
- SDL2 library
- SDL2_ttf library
- Make (cho Linux/macOS) hoặc Visual Studio (cho Windows)

## Cài Đặt

### Windows (với Visual Studio)
1. Tải và cài đặt Visual Studio Community
2. Tải SDL2 development libraries từ https://www.libsdl.org/
3. Tải SDL2_ttf từ https://www.libsdl.org/projects/SDL_ttf/
4. Giải nén và copy các file vào thư mục dự án
5. Cấu hình include paths và library paths trong Visual Studio

### Linux (Ubuntu/Debian)
```bash
# Cài đặt dependencies
sudo apt update
sudo apt install build-essential libsdl2-dev libsdl2-ttf-dev

# Biên dịch
make

# Chạy game
./scrabble
```

### macOS (với Homebrew)
```bash
# Cài đặt dependencies
brew install sdl2 sdl2_ttf

# Biên dịch
make

# Chạy game
./scrabble
```

## Cấu Trúc Project
```
btl-scrabble-hieu/
├── src/                 # Mã nguồn C++
│   ├── main.cpp        # File chính
│   ├── Game.cpp/.h     # Lớp Game chính
│   ├── Board.cpp/.h    # Lớp bàn cờ
│   ├── Player.cpp/.h   # Lớp người chơi
│   ├── Tile.cpp/.h     # Lớp gạch chữ
│   ├── TileBag.cpp/.h  # Lớp túi gạch
│   ├── Dictionary.cpp/.h # Lớp từ điển
│   └── TextRenderer.cpp/.h # Lớp render text
├── assets/fonts/       # Font chữ
├── dictionary.txt      # File từ điển
├── Makefile           # Build script
└── README.md          # File này
```

## Cách Chơi
1. **Bắt đầu**: Player 1 đi trước, phải đặt từ qua ô giữa (★)
2. **Đặt gạch**: Click và kéo gạch từ tay lên board
3. **Tạo từ**: Các gạch phải tạo thành từ hợp lệ
4. **Kết nối**: Từ mới phải kết nối với từ đã có trên board
5. **Xác nhận**: Nhấn ENTER để xác nhận, ESC để hủy
6. **Chấm điểm**: Điểm tính theo giá trị gạch và ô premium

## Điều Khiển
- **Click + Drag**: Kéo gạch lên board
- **ENTER**: Xác nhận từ đã đặt
- **ESC**: Hủy bỏ từ đang đặt
- **SPACE**: Bỏ lượt (tạm thời)

## Hệ Thống Chấm Điểm
- Mỗi gạch có điểm riêng (A=1, B=3, C=3, ...)
- **2L**: Nhân đôi điểm chữ cái
- **3L**: Nhân ba điểm chữ cái  
- **2W**: Nhân đôi điểm từ
- **3W**: Nhân ba điểm từ
- **Bingo**: +50 điểm khi dùng hết 7 gạch

## Các Lớp Chính

### Game
- Quản lý vòng lặp game chính
- Xử lý input và events
- Render giao diện

### Board
- Quản lý bàn cờ 15x15
- Xử lý việc đặt gạch
- Tính toán từ được tạo

### Player
- Quản lý thông tin người chơi
- Quản lý tay gạch (7 gạch)
- Tính điểm

### TileBag
- Quản lý túi gạch (100 gạch)
- Phân phối gạch ngẫu nhiên
- Theo chuẩn phân phối Scrabble

### Dictionary
- Kiểm tra tính hợp lệ của từ
- Tải từ điển từ file
- Hỗ trợ từ điển cơ bản

## Tác Giả
- Sinh viên: [Tên sinh viên]
- Lớp: [Tên lớp]
- Môn: Lập Trình Nâng Cao
- Năm: 2025

## License
Dự án giáo dục - Chỉ dành cho mục đích học tập
