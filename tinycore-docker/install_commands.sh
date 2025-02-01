#!/bin/sh


# Call it from the .ashrc with sudo /bin/sh /abs/path/to/install_commands.sh
BINARIES_DIR="$(dirname "$(readlink -f "$0")")"
BINARIES="neighborshow_cmd neighborshow_agent ifshow_cmd ifnetshow_client ifnetshow_agent"
INSTALL_PATH="/usr/local/bin"

# echo "Installing commands from $BINARIES_DIR to $INSTALL_PATH..."

mkdir -p "$INSTALL_PATH"

for BIN in $BINARIES; do
    if [ -f "$BINARIES_DIR/$BIN" ]; then
        cp "$BINARIES_DIR/$BIN" "$INSTALL_PATH/"
        chmod +x "$INSTALL_PATH/$BIN"
        # echo "Installed: $BIN"
    fi
done

# echo "Installation complete."

start_service() {
    local service_name="$1"
    local exec_path="$INSTALL_PATH/$service_name"

    if [ ! -x "$exec_path" ]; then
        echo "Error: $exec_path not found or not executable!" >&2
        return 1
    fi

    # echo "Starting $service_name..."
    nohup "$exec_path" >/var/log/$service_name.log 2>&1 &
    echo $! > "/var/run/$service_name.pid"
}

# echo "Starting services..."

start_service "neighborshow_agent"
start_service "ifnetshow_agent"

# echo "All services started successfully."
