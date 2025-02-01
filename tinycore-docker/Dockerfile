FROM bensuperpc/tinycore:latest

# Install build tools to compile C code and Node.js package
RUN tce-load -wi gcc \
    && tce-load -wi gcc_libs \
    && tce-load -wi gcc_libs-dev \
    && tce-load -wi gcc_base-dev \
    && tce-load -wi compiletc \
    && tce-load -wi bash.tcz \
    && tce-load -wi node.tcz

# Set the environment variable so GitHub Actions uses our installed Node.js
ENV ACTIONS_NODE_PATH=/usr/local/bin/node