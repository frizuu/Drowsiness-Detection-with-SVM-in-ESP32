import serial
import csv
from datetime import datetime

PORT = "COM13"       # sesuaikan
BAUD = 115200

#LABEL = 0           # 0 normal
LABEL = 1         # mengantuk

ser = serial.Serial(PORT, BAUD)

filename = f"dataset_{datetime.now().strftime('%Y%m%d_%H%M%S')}.csv"

with open(filename, "w", newline="") as file:

    writer = csv.writer(file)

    writer.writerow([
        "timestamp",
        "ax","ay","az",
        "gx","gy","gz",
        "pitch","roll",
        "label"
    ])

    while True:

        try:

            line = ser.readline().decode().strip()

            if "," not in line:
                continue

            data = line.split(",")

            if len(data) != 9:
                continue

            data.append(LABEL)

            writer.writerow(data)

            print(data)

        except KeyboardInterrupt:
            print("Recording stopped")
            break