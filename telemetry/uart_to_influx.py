#!/usr/bin/env python3
"""Reads sensor lines from the STM32 UART console and writes them to InfluxDB."""
import argparse
import re
import sys
import time

import serial
from influxdb_client import InfluxDBClient, Point
from influxdb_client.client.write_api import SYNCHRONOUS

LINE_RE = re.compile(
    r"temp=(?P<temp>-?\d+\.\d+)\s*C\s+humidity=(?P<humidity>-?\d+\.\d+)\s*%RH"
)


def parse_args():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("--port", default="/dev/ttyACM0", help="UART serial device")
    p.add_argument("--baud", type=int, default=115200)
    p.add_argument("--influx-url", default="http://localhost:8086")
    p.add_argument("--influx-token", required=True)
    p.add_argument("--influx-org", required=True)
    p.add_argument("--influx-bucket", default="fridge")
    p.add_argument("--measurement", default="hts221")
    return p.parse_args()


def main():
    args = parse_args()

    client = InfluxDBClient(url=args.influx_url, token=args.influx_token, org=args.influx_org)
    write_api = client.write_api(write_options=SYNCHRONOUS)

    ser = serial.Serial(args.port, args.baud, timeout=1)
    print(f"listening on {args.port} @ {args.baud}, writing to {args.influx_bucket}", file=sys.stderr)

    while True:
        raw = ser.readline()
        if not raw:
            continue
        line = raw.decode("utf-8", errors="replace").strip()
        if not line:
            continue

        match = LINE_RE.search(line)
        if not match:
            print(f"skip: {line}", file=sys.stderr)
            continue

        point = (
            Point(args.measurement)
            .field("temperature_c", float(match["temp"]))
            .field("humidity_pct", float(match["humidity"]))
            .time(time.time_ns())
        )
        write_api.write(bucket=args.influx_bucket, record=point)
        print(f"wrote: {line}", file=sys.stderr)


if __name__ == "__main__":
    main()
