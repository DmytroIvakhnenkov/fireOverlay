#!/usr/bin/env python3
import csv
import json
from datetime import datetime, timedelta

CSV_FILE = "/home/dmytro/fireOverlay/timeRecords.csv"
MAX_HOURS = 8
DATE_FORMAT = "%Y-%m-%d %H:%M:%S"
today = datetime.now().date()

# Load and parse CSV
sessions = []
with open(CSV_FILE, newline="") as f:
    reader = csv.DictReader(f)
    for row in reader:
        ts = datetime.strptime(row["timestamp"], DATE_FORMAT)
        if ts.date() == today:
            sessions.append((ts, row["startOrFinish"]))

# Sort and pair start-finish
sessions.sort()
work_time = timedelta()
i = 0
while i < len(sessions) - 1:
    if sessions[i][1] == "start" and sessions[i + 1][1] == "finish":
        start, end = sessions[i][0], sessions[i + 1][0]
        if end > start:
            work_time += end - start
        i += 2
    else:
        i += 1
if len(sessions) > 1:        
    end = datetime.now()
    if sessions[-1][1] == "start":
        work_time += end - sessions[-1][0]

# Compute progress
hours = work_time.total_seconds() / 3600
percent = min(hours / MAX_HOURS, 1.0)
blocks = int(percent * 24)
bar = "[" + "█ " * blocks + "  " * (24 - blocks) + "]"

# Output for Waybar
output = {
    "text": f"{bar} {hours:.1f}h",
    "alt": "baka",
    "tooltip": f"Deep work today: {hours:.1f} hours",
    "class": "deepwork"
}
print(json.dumps(output))
