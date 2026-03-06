#!/usr/bin/env bash
# build_carim.sh - Build Carim Mod with MyGroups integration

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

readonly MOD_NAME="Carim"
readonly BUILD_DIR="$SCRIPT_DIR/build"
readonly CARIM_DIR="$SCRIPT_DIR/Carim"

echo "=== Carim Mod Builder (with MyGroups) ==="
echo ""

# Clean build directory
echo "Cleaning build directory..."
rm -rf "$BUILD_DIR/@${MOD_NAME}"
mkdir -p "$BUILD_DIR/@${MOD_NAME}"

# Copy Carim files
echo "Copying Carim files..."
cp -r "$CARIM_DIR/"* "$BUILD_DIR/@${MOD_NAME}/"

# Copy our MyGroups scripts (already in Carim/scripts/)
echo "MyGroups scripts already integrated in Carim/scripts/"

# Copy mod.cpp
echo "Copying mod.cpp..."
cp "$CARIM_DIR/mod.cpp" "$BUILD_DIR/@${MOD_NAME}/"

# Create key file (for testing, unsigned)
echo "Creating unsigned mod (for testing)..."

# Show result
echo ""
echo "=== Build complete ==="
ls -la "$BUILD_DIR/@${MOD_NAME}/"
echo ""
echo "Scripts:"
find "$BUILD_DIR/@${MOD_NAME}/scripts" -name "*.c" | head -20
echo ""
echo "=== To install ==="
echo "cp -r $BUILD_DIR/@${MOD_NAME} /path/to/DayZ/Mods/"
echo ""
echo "=== Or test directly ==="
echo "cp -r $BUILD_DIR/@${MOD_NAME} /home/alca/games/DayZ/Mods/"
