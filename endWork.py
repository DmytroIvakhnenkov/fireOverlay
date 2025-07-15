import csv
from datetime import datetime

startTime = None
with open("/home/dmytro/fireOverlay/timeRecords.csv", 'r') as timeRecords:
    csv_reader = csv.reader(timeRecords)
    for row in csv_reader:
        last_row = row
        startTime = last_row[0]

finishTime = None   
with open("/home/dmytro/fireOverlay/timeRecords.csv", 'a') as timeRecords:
    writer = csv.writer(timeRecords)
    now = datetime.now()
    finishTime = now.strftime("%Y-%m-%d %H:%M:%S")
    writer.writerow([finishTime, "finish"])



startTime_object = datetime.strptime(startTime, "%Y-%m-%d %H:%M:%S")
finishTime_object = datetime.strptime(finishTime, "%Y-%m-%d %H:%M:%S")
print(finishTime_object - startTime_object)