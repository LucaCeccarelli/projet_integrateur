#!/bin/sh

BINARIES="neighborshow_cmd neighborshow_agent ifshow_cmd ifnetshow_client ifnetshow_agent"
INSTALL_PATH="/usr/local/bin"

echo "Installing commands to $INSTALL_PATH..."

# Ensure /usr/local/bin exists and is writable
mkdir -p "$INSTALL_PATH"

for BIN in $BINARIES; do
    if [ -f "$BIN" ]; then
        cp "$BIN" "$INSTALL_PATH/"
        chmod +x "$INSTALL_PATH/$BIN"
        echo "Installed: $BIN"
    else
        echo "Warning: $BIN not found in current directory!" >&2
    fi
done

echo "Installation complete."

# Setup TinyCore init scripts instead of systemd
setup_init_script() {
    local service_name="$1"
    local exec_path="$INSTALL_PATH/$service_name"
    local init_script="/etc/init.d/$service_name"

    echo "Setting up init script for $service_name..."

    cat <<EOF > "$init_script"
#!/bin/sh
case "\$1" in
    start)
        echo "Starting $service_name..."
        nohup "$exec_path" >/var/log/$service_name.log 2>&1 &
        echo \$! > /var/run/$service_name.pid
        ;;
    stop)
        echo "Stopping $service_name..."
        kill \$(cat /var/run/$service_name.pid)
        rm -f /var/run/$service_name.pid
        ;;
    restart)
        \$0 stop
        \$0 start
        ;;
    *)
        echo "Usage: \$0 {start|stop|restart}"
        exit 1
        ;;
esac
EOF

    chmod +x "$init_script"

    # Enable script at boot
    ln -sf "$init_script" /etc/init.d/rcS.d/S99"$service_name"

    echo "$service_name init script installed."
}

echo "Configuring init scripts..."

setup_init_script "neighborshow_agent"
setup_init_script "ifnetshow_agent"

echo "All services configured to start on boot."
