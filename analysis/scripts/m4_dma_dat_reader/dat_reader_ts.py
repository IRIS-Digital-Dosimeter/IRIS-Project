
from itertools import pairwise
import sys
import struct
import matplotlib.pyplot as plt
from pprint import pprint
import pandas

def read_data(filename, num_results):
    frame_size = num_results * 4 + 8 # number of uint16s per array
    frame_bytes = frame_size * 2     # bytes per array

    a0_all, a1_all, a2_all, a3_all = [], [], [], []
    x0_all, x1_all, x2_all, x3_all = [], [], [], [] # timestamp-based X values

    with open(filename, "rb") as f:
        frame_count = 0
        prev_ts0 = prev_ts2 = None  # Initialize previous timestamps

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

            # Fix timestamp interpolation:
            # Use sliding timestamps from previous frame if available
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

            
            # Store the current ending timestamps to use next round
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

    df = pandas.DataFrame({
        "x0": x0_all,
        "x1": x1_all,
        "x2": x2_all,
        "x3": x3_all,
        "a0": a0_all,
        "a1": a1_all,
        "a2": a2_all,
        "a3": a3_all,
    })
    # df.to_csv('fuck.csv')
    # df.to_excel('fuck.xlsx')

    total_samples = frame_count * num_results
    print(f"{filename}: {total_samples} samples per channel (from {frame_count} arrays)")

    return (x0_all, a0_all), (x1_all, a1_all), (x2_all, a2_all), (x3_all, a3_all)

def linspace_between(start, end, count):
    if count <= 1:
        return [start]
    step = (end - start) / (count - 1)
    return [start + i * step for i in range(count)]


def plot_data(files, num_results):
    colors = ["brown", "red", "orange", "yellow"]
    labels = ["a0", "a1", "a2", "a3"]

    plt.figure(figsize=(12, 6))

    for idx, file in enumerate(files):
        try:
            (x0, a0), (x1, a1), (x2, a2), (x3, a3) = read_data(file, num_results)

            for x, y, color, label in zip(
                [x0, x1, x2, x3],
                [a0, a1, a2, a3],
                colors,
                labels
            ):
                # plt.plot(x[:1000] + x[-1000:], y[:1000] + y[-1000:], color=color, marker='o', markersize=0.2, linestyle='None', linewidth=0.5, label=label if idx == 0 else "")
                plt.plot(x, y, color=color, marker='o', markersize=1, linestyle='None', linewidth=0.5, label=label if idx == 0 else "")
                print(f"###{color}")
                # pprint(x[:3000])
                # for x,y in pairwise(x):
                    # if not x < y:
                    #     print(f"uhoh: {x} {y}")
                

        except Exception as e:
            print(f"Error processing {file}: {e}")

    plt.title("Voltage Readings with Timestamp X-Axis")
    plt.xlabel("Timestamp (s)")
    plt.ylabel("Voltage (Raw 12-bit Value)")
    plt.legend(loc='upper right')
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
