import argparse
import json
import sys
import re
import tomllib
from pathlib import Path
from pprint import pprint

def iter_flatten(d, parent_key="", sep="."):
    for k, v in d.items():
        if isinstance(v, dict):
            yield from iter_flatten(v)
        else:
            yield k, v


def modify_define(filename, updates):
    # Read file
    with open(filename, 'r') as file:
        lines = file.readlines()

    # Update lines
    with open(filename, 'w') as file:
        for line in lines:
            modified = False
            for key, value in updates.items():
                pattern = re.compile(rf'#define\s+{key}\s+\S+')
                # print(f"pattern: {pattern}")
                # print(f"line: {line}")
                # print(f"out: #define {key} {value}")
                # input()
                # print()
                if pattern.match(line):
                    file.write(f'#define {key} {value}\n')
                    modified = True
                    break
            if not modified:
                file.write(line)

def main():
    parser = argparse.ArgumentParser(
        description="Update #define values in a header file from TOML config"
    )
    parser.add_argument("header", help="Path to the header file (e.g., config.h)")
    parser.add_argument("toml", help="Path to the TOML file with updates")

    args = parser.parse_args()

    pprint(vars(args))

    # Load TOML key/value pairs
    try:
        with open(args.toml, "rb") as f:
            updates = tomllib.load(f)
    except Exception as e:
        print(f"Error reading TOML file: {e}", file=sys.stderr)
        sys.exit(1)

    pprint(dict(iter_flatten(updates)))
    
    # Perform modifications
    modify_define(args.header, dict(iter_flatten(updates)))

if __name__ == "__main__":
    main()