import csv
from datetime import datetime

with open("/home/dmytro/fireOverlay/timeRecords.csv", 'a') as timeRecords:
    writer = csv.writer(timeRecords)
    now = datetime.now()
    current_time = now.strftime("%Y-%m-%d %H:%M:%S")
    writer.writerow([current_time, "start"])