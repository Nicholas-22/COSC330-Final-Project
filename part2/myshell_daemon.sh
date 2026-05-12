#!/bin/bash
# Background daemon version of the Part 1 shell — runs headless under systemd.
# Logs a status line every 30 seconds so journalctl output is meaningful.
while true; do
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] myshell service is running | host=$(hostname) | uptime=$(uptime -p)"
    sleep 30
done
