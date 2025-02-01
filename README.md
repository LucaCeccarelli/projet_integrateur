# Build the project
```bash
make
```

## Clean the intermediary object files
```bash
make clean
```

## Clear all files
```bash
make distclean
```

# How to use it
## Run the Standalone ifshow Command:
   - To list all interfaces:
     ```bash
     ./ifshow -a
     ```
   - To list only a specific interface (e.g., `eth0`):
     ```bash
     ./ifshow -i eth0
     ```

## Run the Agent and Client:
   - On the **remote machine**, run the **agent**:
     ```bash
     ./ifnetshow_agent
     ```
   - From the **client machine**, run the **client**:
     ```bash
     ./ifnetshow_client -n <remote_IP> -a
     ```
     or
     ```bash
     ./ifnetshow_client -n <remote_IP> -i eth0
     ```