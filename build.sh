#!/usr/bin/env bash
# build.sh - Linux version of Carim make.ps1
# Usage: ./build.sh --setup --build --server

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

# ========================
# Configurations (from make.ps1)
# ========================
readonly MOD="Carim"
readonly ROOT="$SCRIPT_DIR"
readonly LOCAL_MODS="$ROOT/build"
readonly MISSIONS="$ROOT/missions"
readonly MISSION_NAME="dayzOffline.chernarusplus"

# Steam paths (adjust if different)
readonly STEAM_ROOT="$HOME/.local/share/Steam/steamapps/common"
readonly CLIENT_DIR="$STEAM_ROOT/DayZ"
readonly SERVER_DIR="$STEAM_ROOT/DayZServer"
readonly TOOLS_DIR="$STEAM_ROOT/DayZ Tools"

# PBO Builder - use our Python script
PBO_BUILDER="python3 $SCRIPT_DIR/../dayz_pbo_pack.py"

# Key file (for signing - optional for testing)
readonly KEY_FILE=""  # Add your .biprivatekey path if you have one

# ========================
# Helper Functions
# ========================
log() {
    echo "[$(date '+%H:%M:%S')] $*"
}

fail() {
    log "ERROR: $*"
    exit 1
}

check_result() {
    local exit_code=$?
    if [[ $exit_code -ne 0 ]]; then
        fail "Command failed with exit code $exit_code"
    fi
}

# ========================
# Setup-Project
# ========================
setup_project() {
    log "Setting up project..."
    
    # Clean previous setups
    if [[ -d "/tmp/dayz_project" ]]; then
        log "Cleaning previous setup..."
        rm -rf /tmp/dayz_project
    fi
    
    # Create mission directory for testing
    if [[ ! -d "$MISSIONS/$MISSION_NAME" ]]; then
        log "Setting up mission directory..."
        mkdir -p "$MISSIONS"
        
        # Copy mission from server
        if [[ -d "$SERVER_DIR/mpmissions/$MISSION_NAME" ]]; then
            cp -r "$SERVER_DIR/mpmissions/$MISSION_NAME" "$MISSIONS/"
        fi
        
        # Copy server config
        if [[ -f "$SERVER_DIR/serverDZ.cfg" ]]; then
            cp "$SERVER_DIR/serverDZ.cfg" "$MISSIONS/"
        fi
    fi
    
    # Create profiles directory
    mkdir -p "$MISSIONS/profiles/server"
    mkdir -p "$MISSIONS/profiles/client"
    
    log "Setup complete!"
}

# ========================
# Build-Project
# ========================
build_project() {
    log "Building project..."
    
    # Clean build directory
    rm -rf "$LOCAL_MODS/@${MOD}"
    mkdir -p "$LOCAL_MODS/@${MOD}/addons"
    
    # Copy Carim files to addons folder
    log "Copying Carim files..."
    cp -r "$ROOT/Carim/"* "$LOCAL_MODS/@${MOD}/addons/"
    
    # Copy mod.cpp
    cp "$ROOT/Carim/mod.cpp" "$LOCAL_MODS/@${MOD}/"
    
    # Build PBOs
    if [[ -n "$PBO_BUILDER" ]]; then
        log "Building PBOs with $PBO_BUILDER..."
        
        cd "$LOCAL_MODS/@${MOD}/addons"
        
        # Build Carim.pbo
        if [[ -d "Carim" ]]; then
            log "Building Carim.pbo..."
            $PBO_BUILDER "Carim" "$LOCAL_MODS/@${MOD}/addons/Carim.pbo"
        fi
        
        # Build CarimDeprecated.pbo
        if [[ -d "CarimDeprecated" ]]; then
            log "Building CarimDeprecated.pbo..."
            $PBO_BUILDER "CarimDeprecated" "$LOCAL_MODS/@${MOD}/addons/CarimDeprecated.pbo"
        fi
        
        # Build CarimMapStyle.pbo
        if [[ -d "CarimMapStyle" ]]; then
            log "Building CarimMapStyle.pbo..."
            $PBO_BUILDER "CarimMapStyle" "$LOCAL_MODS/@${MOD}/addons/CarimMapStyle.pbo"
        fi
        
        # Remove source folders after building PBOs
        rm -rf "Carim" "CarimDeprecated" "CarimMapStyle"
        
        cd "$SCRIPT_DIR"
        
        log "PBOs built successfully!"
        ls -la "$LOCAL_MODS/@${MOD}/addons/"
    else
        log "No PBO builder found. Using folder structure (file-patching mode)"
    fi
    
    log "Build complete: $LOCAL_MODS/@${MOD}"
}

# ========================
# Start-Workbench (not available on Linux)
# ========================
start_workbench() {
    log "Workbench not available on Linux"
    log "Use DayZDiag_x64.exe with Wine for debugging"
}

# ========================
# Diag-Project (Start DayZDiag - Server ONLY)
# ========================
diag_project() {
    log "Starting DayZDiag Server (client must be started separately)..."
    
    local mods="$LOCAL_MODS/@${MOD};$LOCAL_MODS/@${MOD}MapStyle"
    
    # Create separate Wine prefixes for server/client
    local server_prefix="$MISSIONS/wine_server"
    local client_prefix="$MISSIONS/wine_client"
    
    mkdir -p "$server_prefix" "$client_prefix"
    
    # Start server with DayZDiag (via Wine)
    cd "$SERVER_DIR"
    
    log "Starting server..."
    WINEPREFIX="$server_prefix" wine DayZServer_x64.exe \
        -mod="$mods" \
        -profiles="$MISSIONS/profiles/server" \
        -doLogs \
        -config="$MISSIONS/serverDZ.cfg" \
        -limitFPS=1000 \
        -logs \
        -scriptDebug \
        -nosound \
        -noPause &
    
    log "Server started in background"
    log ""
    log "To start client separately:"
    log "  WINEPREFIX=$client_prefix wine $CLIENT_DIR/DayZDiag_x64.exe -mod=$mods -profiles=$MISSIONS/profiles/client -doLogs -name=cnofafva -connect=127.0.0.1 -port=2302"
}

# ========================
# Run-Server (Linux Native + File-Patching)
# ========================
run_server() {
    log "Starting server (Linux native with file-patching)..."
    
    local mods="$LOCAL_MODS/@${MOD}"
    
    cd "$SERVER_DIR"
    
    # Start server (Linux native) with file-patching
    ./DayZServer \
        -filepatching \
        -mod="$mods" \
        -profiles="$MISSIONS/profiles/server" \
        -doLogs \
        -config="$MISSIONS/serverDZ.cfg" \
        -limitFPS=1000 \
        -logs \
        -scriptDebug \
        -nosound \
        -noPause &
    
    SERVER_PID=$!
    log "Server started with PID $SERVER_PID"
    log ""
    log "Check logs at: $SERVER_DIR/profile/*.RPT"
    log "Tail logs: tail -f $SERVER_DIR/profile/*.RPT"
}

# ========================
# Main Logic
# ========================
show_help() {
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  --setup    Setup project (extract data, create directories)"
    echo "  --build    Build project (copy files, create PBOs)"
    echo "  --workbench Start Workbench (not available on Linux)"
    echo "  --diag     Start DayZDiag (server + client)"
    echo "  --server   Start server only"
    echo "  --help     Show this help"
    echo ""
    echo "Examples:"
    echo "  $0 --setup --build --server"
    echo "  $0 --diag"
}

# Parse arguments
SETUP=false
BUILD=false
WORKBENCH=false
DIAG=false
SERVER=false

while [[ $# -gt 0 ]]; do
    case $1 in
        --setup)
            SETUP=true
            shift
            ;;
        --build)
            BUILD=true
            shift
            ;;
        --workbench)
            WORKBENCH=true
            shift
            ;;
        --diag)
            DIAG=true
            shift
            ;;
        --server)
            SERVER=true
            shift
            ;;
        --help)
            show_help
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            show_help
            exit 1
            ;;
    esac
done

# Execute requested actions
if [[ "$SETUP" == true ]]; then
    setup_project
fi

if [[ "$BUILD" == true ]]; then
    build_project
fi

if [[ "$WORKBENCH" == true ]]; then
    start_workbench
fi

if [[ "$DIAG" == true ]]; then
    diag_project
fi

if [[ "$SERVER" == true ]]; then
    run_server
fi

# If no action specified, show help
if [[ "$SETUP" == false && "$BUILD" == false && "$WORKBENCH" == false && "$DIAG" == false && "$SERVER" == false ]]; then
    show_help
fi
