FROM ghcr.io/userver-framework/ubuntu-22.04-userver-pg:latest

RUN apt-get install -y --no-install-recommends \
    libsodium-dev libsodium23 \
    && rm -rf /var/lib/apt/lists/*

