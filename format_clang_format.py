#!/usr/bin/env python3

import os
import sys


def sort_config_file(file_path):
    """
    Sorts lines alphabetically within each block of a config file.
    Blocks are separated by "---" lines, and comments (#) are preserved in place.
    """
    if not os.path.exists(file_path):
        print(f"Error: File '{file_path}' does not exist.")
        return

    with open(file_path, "r") as file:
        lines = file.readlines()

    sorted_lines = []
    current_block = []

    for line in lines:
        stripped_line = line.strip()

        # Detect block separator
        if (
            stripped_line == "---"
            or stripped_line.startswith("Language:")
            or stripped_line.endswith(":")
            or stripped_line.startswith("- ")
            or stripped_line.startswith("#")
            or not stripped_line
        ):
            # Sort the current block before adding the separator
            if current_block:
                sorted_lines.extend(sorted(current_block, key=str.lower))
                current_block = []
            sorted_lines.append(line)
        else:
            # Add lines to the current block
            current_block.append(line)

    # Sort and add the last block if it exists
    if current_block:
        sorted_lines.extend(sorted(current_block, key=str.lower))

    # Write the sorted lines back to the file
    with open(file_path, "w") as file:
        file.writelines(sorted_lines)

    print(f"File '{file_path}' has been sorted successfully.")


# Example usage
if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python3 format_clang_format.py <file_path>")
        sys.exit(1)

    config_file_path = sys.argv[1]
    sort_config_file(config_file_path)
