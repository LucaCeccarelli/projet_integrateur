FROM bensuperpc/tinycore:latest

# Install build tools to compile C code
RUN tce-load -wi gcc \
    && tce-load -wi gcc_libs \
    && tce-load -wi gcc_libs-dev \
    && tce-load -wi gcc_base-dev \
    && tce-load -wi compiletc \
    && tce-load -wi node.tcz
