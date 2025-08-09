import sys
import struct
import matplotlib.pyplot as plt

def read_data_seq(filename, num_results):
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

def read_data_ts(filename, num_results, prev_ts0=None, prev_ts2=None):
    def linspace_between(start, end, count):
        if count <= 1:
            return [start]
        step = (end - start) / (count - 1)
        return [start + i * step for i in range(count)]
    
    frame_size = num_results * 4 + 8 # number of uint16s per array
    frame_bytes = frame_size * 2     # bytes per array

    a0_all, a1_all, a2_all, a3_all = [], [], [], []
    x0_all, x1_all, x2_all, x3_all = [], [], [], [] # timestamp-based X values

    with open(filename, "rb") as f:
        frame_count = 0
        # prev_ts0 = prev_ts2 = None  # Initialize previous timestamps

        while True:
            chunk = f.read(frame_bytes)
            if len(chunk) < frame_bytes:
                break

            unpacked = struct.unpack(f"<{frame_size}H", chunk)
            voltages = unpacked[:num_results * 4]
            timestamp_words = unpacked[-8:]

            # extract channels
            a0 = voltages[0::4]
            a1 = voltages[1::4]
            a2 = voltages[2::4]
            a3 = voltages[3::4]

            # reconstruct 32-bit timestamps
            timestamps = []
            for i in range(0, 8, 2):
                low = timestamp_words[i]
                high = timestamp_words[i + 1]
                ts = (high << 16) | low
                timestamps.append(ts)

            ts0, ts1, ts2, ts3 = timestamps

            # prev_ts is the final timestamp from the previous file, or if not present, None
            # this causes artifacting in the beginning of the very first file
            if prev_ts0 is not None:
                # print(f"diff0: {ts0 - prev_ts0}")
                x0 = list(linspace_between(prev_ts0, ts0, num_results))
                x1 = list(linspace_between(prev_ts0, ts0, num_results))
            else:
                x0 = list(linspace_between(ts0, ts1, num_results))
                x1 = list(linspace_between(ts0, ts1, num_results))

            if prev_ts2 is not None:
                # print(f"diff2: {ts2 - prev_ts2}")
                x2 = list(linspace_between(prev_ts2, ts2, num_results))
                x3 = list(linspace_between(prev_ts2, ts2, num_results))
            else:
                x2 = list(linspace_between(ts2, ts3, num_results))
                x3 = list(linspace_between(ts2, ts3, num_results))

            
            # store the current ending timestamps to use next round
            prev_ts0 = ts0
            prev_ts2 = ts2

            a0_all.extend(a0)
            a1_all.extend(a1)
            a2_all.extend(a2)
            a3_all.extend(a3)

            x0_all.extend(x0)
            x1_all.extend(x1)
            x2_all.extend(x2)
            x3_all.extend(x3)

            frame_count += 1



    total_samples = frame_count * num_results
    print(f"{filename}: {total_samples} {len(a0_all)} samples per channel (from {frame_count} arrays)")

    return (x0_all, a0_all), (x1_all, a1_all), (x2_all, a2_all), (x3_all, a3_all), prev_ts0, prev_ts2

def plot_data(files, num_results, seq=False):
    colors = ["brown", "red", "orange", "yellow"]
    labels = ["a0", "a1", "a2", "a3"]

    plt.figure(figsize=(12, 6))
    prev_ts0, prev_ts2 = None, None  # Previous timestamps for timestamp-based plotting
    offset = 0  # Tracks where to start plotting each file

    for idx, file in enumerate(files):
        if seq:
            try:
                a0, a1, a2, a3 = read_data_seq(file, num_results)
                length = len(a0)
                x = list(range(offset, offset + length))

                for channel_data, color, label in zip([a0, a1, a2, a3], colors, labels):
                    # Only show legend once for each label
                    plt.plot(x, channel_data, linewidth=0.5, color=color, label=label if idx == 0 else "")

                offset += length  # Move the starting point forward

            except Exception as e:
                print(f"Error processing {file}: {e}")
        else:
            try:
                (x0, a0), (x1, a1), (x2, a2), (x3, a3), prev_ts0, prev_ts2 = read_data_ts(file, num_results, prev_ts0, prev_ts2)
                prev_ts0 = x0[-1]
                prev_ts2 = x2[-1]
                for x, y, color, label in zip(
                    [x0, x1, x2, x3],
                    [a0, a1, a2, a3],
                    colors,
                    labels
                ):
                    plt.plot(x, y, color=color, marker='o', markersize=1, linestyle='None', linewidth=0.5, label=label if idx == 0 else "")

            except Exception as e:
                print(f"Error processing {file}: {e}")
            

    if seq:
        plt.title("Voltage Readings with Sequential X-Axis")
        plt.xlabel("Sample Number")
    else:
        plt.title("Voltage Readings with Timestamp X-Axis")
        plt.xlabel("Timestamp (s)")
    plt.ylabel("Voltage (Raw 12-bit Value)")
    plt.legend(loc='upper right')
    plt.grid(True)
    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python dat_reader.py <-seq> [NUM_RESULTS] [files...]")
        print("\t-seq: Plot data sequentially (default behavior is to plot data in time order)")
        sys.exit(1)
    
    if sys.argv[1] == '-seq':
        seq = True
        files = sys.argv[3:]
    else:
        seq = False
        files = sys.argv[2:]
        
    try:
        if seq:
            num_results = int(sys.argv[2])
        else:
            num_results = int(sys.argv[1])
    except ValueError:
        print("NUM_RESULTS must be an integer.")
        sys.exit(1)

    files = sys.argv[2:]
    plot_data(files, num_results, seq=seq)
