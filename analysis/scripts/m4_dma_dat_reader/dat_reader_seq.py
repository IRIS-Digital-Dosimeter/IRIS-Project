import sys
import struct
import matplotlib.pyplot as plt
import os

def read_data(filename, num_results):
    frame_size = num_results * 4 + 8         # Number of uint16s per array
    frame_bytes = frame_size * 2             # Bytes per array

    a0_all, a1_all, a2_all, a3_all = [], [], [], []

    with open(filename, "rb") as f:
        frame_count = 0
        while True:
            chunk = f.read(frame_bytes)
            if len(chunk) < frame_bytes:
                break  # Ignore incomplete array at the end

            unpacked = struct.unpack(f"<{frame_size}H", chunk)
            voltages = unpacked[:num_results * 4]

            a0 = voltages[0::4]
            a1 = voltages[1::4]
            a2 = voltages[2::4]
            a3 = voltages[3::4]

            a0_all.extend(a0)
            a1_all.extend(a1)
            a2_all.extend(a2)
            a3_all.extend(a3)

            frame_count += 1

    total_samples = frame_count * num_results
    print(f"{filename}: {total_samples} samples per channel (from {frame_count} arrays)")

    return a0_all, a1_all, a2_all, a3_all

def plot_data(files, num_results):
    colors = ["brown", "red", "orange", "yellow"]
    labels = ["a0", "a1", "a2", "a3"]

    plt.figure(figsize=(12, 6))

    offset = 0  # Tracks where to start plotting each file

    for idx, file in enumerate(files):
        try:
            a0, a1, a2, a3 = read_data(file, num_results)
            length = len(a0)
            x = list(range(offset, offset + length))

            for channel_data, color, label in zip([a0, a1, a2, a3], colors, labels):
                # Only show legend once for each label
                plt.plot(x, channel_data, linewidth=0.5, color=color, label=label if idx == 0 else "")

            offset += length  # Move the starting point forward

        except Exception as e:
            print(f"Error processing {file}: {e}")

    plt.title("Voltage Readings from a0–a3")
    plt.xlabel("Sample Index")
    plt.ylabel("Voltage (Raw 16-bit Value)")
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python plotter.py [NUM_RESULTS] [files...]")
        sys.exit(1)

    try:
        num_results = int(sys.argv[1])
    except ValueError:
        print("NUM_RESULTS must be an integer.")
        sys.exit(1)

    files = sys.argv[2:]
    plot_data(files, num_results)
