#!/bin/bash
# build.sh - Build script for hover-blast project

set -e # Exit on any error

PROJECT_NAME="hover-blast"
BUILD_DIR="build"
WEB_BUILD_DIR="build-web"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

print_usage() {
  echo "Usage: $0 [web|web-debug|desktop|clean|setup|serve]"
  echo "  web       - Build for web using Emscripten (Release)"
  echo "  web-debug - Build for web using Emscripten (Debug, no optimization)"
  echo "  desktop   - Build for desktop"
  echo "  clean     - Clean build directories"
  echo "  serve     - Serve web build locally"
  echo "  help      - Show this help message"
}

print_info() {
  echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
  echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
  echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
  echo -e "${RED}[ERROR]${NC} $1"
}

check_emscripten() {
  if ! command -v emcc &>/dev/null; then
    print_error "Emscripten not found. Please install and activate Emscripten SDK."
    print_info "Visit: https://emscripten.org/docs/getting_started/downloads.html"
    exit 1
  fi
  print_info "Emscripten version: $(emcc --version | head -n1)"
}

check_cmake() {
  if ! command -v cmake &>/dev/null; then
    print_error "CMake not found. Please install CMake."
    exit 1
  fi
  print_info "CMake version: $(cmake --version | head -n1)"
}

build_desktop() {
  print_info "Building for desktop..."
  check_cmake

  # Create build directory
  mkdir -p $BUILD_DIR
  cd $BUILD_DIR

  # Configure and build
  cmake .. -DCMAKE_BUILD_TYPE=Release
  cmake --build . --config Release -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

  cd ..
  print_success "Desktop build completed successfully!"
  print_info "Executable: $BUILD_DIR/$PROJECT_NAME/$PROJECT_NAME"
}

build_web() {
  print_info "Building for web..."
  check_cmake
  check_emscripten

  # Create web build directory
  mkdir -p $WEB_BUILD_DIR
  cd $WEB_BUILD_DIR

  # Configure with Emscripten
  print_info "Configuring web build (without Closure Compiler for compatibility)..."
  emcmake cmake .. -DCMAKE_BUILD_TYPE=Release -DPLATFORM=Web

  # Build with error handling
  print_info "Building web version..."
  if cmake --build . --config Release -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4); then
    cd ..
    print_success "Web build completed successfully!"
    print_info "Web files: $WEB_BUILD_DIR/$PROJECT_NAME/"
    print_info "Open $WEB_BUILD_DIR/$PROJECT_NAME/$PROJECT_NAME.html in a web server"
    print_info "Run '$0 serve' to start a local web server"
  else
    cd ..
    print_error "Web build failed!"
    print_info "Try building with debug mode: $0 web-debug"
    exit 1
  fi
}

build_web_debug() {
  print_info "Building for web (Debug mode)..."
  check_cmake
  check_emscripten

  # Create web build directory
  mkdir -p $WEB_BUILD_DIR
  cd $WEB_BUILD_DIR

  # Configure with Emscripten in debug mode
  emcmake cmake .. -DCMAKE_BUILD_TYPE=Debug -DPLATFORM=Web

  # Build
  cmake --build . --config Debug -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

  cd ..
  print_success "Web debug build completed successfully!"
  print_info "Web files: $WEB_BUILD_DIR/$PROJECT_NAME/"
}

clean_builds() {
  print_info "Cleaning build directories..."
  rm -rf $BUILD_DIR $WEB_BUILD_DIR
  print_success "Build directories cleaned"
}

serve_web() {
  if [ -f "$WEB_BUILD_DIR/$PROJECT_NAME/$PROJECT_NAME.html" ]; then
    print_info "Starting local web server..."
    cd $WEB_BUILD_DIR/$PROJECT_NAME

    if command -v python3 &>/dev/null; then
      print_info "Server running at http://localhost:8000"
      python3 -m http.server 8000
    elif command -v python &>/dev/null; then
      print_info "Server running at http://localhost:8000"
      python -m SimpleHTTPServer 8000
    else
      print_error "Python not found. Please serve the files manually."
      print_info "Serve the contents of: $(pwd)"
    fi
  else
    print_error "Web build not found. Run '$0 web' first."
  fi
}

# Main script logic
case "${1:-}" in
"web")
  build_web
  ;;
"web-debug")
  build_web_debug
  ;;
"desktop")
  build_desktop
  ;;
"clean")
  clean_builds
  ;;
"serve")
  serve_web
  ;;
"help" | "--help" | "-h")
  print_usage
  ;;
"")
  print_warning "No target specified. Building for desktop..."
  build_desktop
  ;;
*)
  print_error "Unknown target: $1"
  print_usage
  exit 1
  ;;
esac
