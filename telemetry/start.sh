#!/usr/bin/env bash

set -euo pipefail

TELEMETRY_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$TELEMETRY_DIR"

if [ ! -f .env ]; then
    echo "Missing telemetry/.env - copy .env.example and fill in real secrets first." >&2
    exit 1
fi
set -a
source .env
set +a

if [ ! -d .venv ]; then
    python3 -m venv .venv
fi
./.venv/bin/pip install -q -r requirements.txt

docker compose up -d

SERIAL_PORT="$(ls /dev/serial/by-id/usb-STMicroelectronics_STM32_STLink_*-if02 2>/dev/null | head -1)"
SERIAL_PORT="${SERIAL_PORT:-/dev/ttyACM0}"

UNIT_FILE="/etc/systemd/system/uart-to-influx.service"
UNIT_CONTENT="$(cat <<EOF
[Unit]
Description=STM32 UART to InfluxDB telemetry bridge
After=network-online.target docker.service
Wants=network-online.target

[Service]
Type=simple
User=$(whoami)
WorkingDirectory=${TELEMETRY_DIR}
ExecStart=${TELEMETRY_DIR}/.venv/bin/python3 ${TELEMETRY_DIR}/uart_to_influx.py --port ${SERIAL_PORT} --influx-url http://localhost:8086 --influx-token ${INFLUXDB_INIT_ADMIN_TOKEN} --influx-org ${INFLUXDB_INIT_ORG} --influx-bucket ${INFLUXDB_INIT_BUCKET}
Restart=always
RestartSec=3

[Install]
WantedBy=multi-user.target
EOF
)"

if [ ! -f "$UNIT_FILE" ] || [ "$(sudo cat "$UNIT_FILE" 2>/dev/null)" != "$UNIT_CONTENT" ]; then
    echo "$UNIT_CONTENT" | sudo tee "$UNIT_FILE" > /dev/null
    sudo systemctl daemon-reload
fi
sudo systemctl enable --now uart-to-influx.service

echo
echo "InfluxDB: http://localhost:8086"
echo "Grafana:  http://localhost:3002 (admin / see telemetry/.env)"
sudo systemctl --no-pager --lines=3 status uart-to-influx.service
