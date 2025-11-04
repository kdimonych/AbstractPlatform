#!/usr/bin/env python3

import re
import os
import sys


def replace_includes_line(content, prefixes):
    # Replace includes with the specified prefix
    for prefix in prefixes:
        content = re.sub(
            rf'(.*?)#include([\s\t])"{prefix}(.*?)"',
            rf"\1#include\2<{prefix}\3>",
            content,
        )
    return content


def replace_includes_in_file(file_path, prefixes):
    with open(file_path, "r") as file:
        content = file.read()

    # Replace includes with the specified prefixes
    updated_content = replace_includes_line(content, prefixes)

    # Write the updated content back to the file
    with open(file_path, "w") as file:
        file.write(updated_content)


# Read the prefixes from the .header_prefixes.txt file
def read_header_prefixes(file_path=".header_prefixes.txt"):
    if not os.path.exists(file_path):
        print(f"Error: File '{file_path}' does not exist.")
        return []

    with open(file_path, "r") as file:
        # Read lines and strip whitespace, ignoring empty lines
        # and comments (lines starting with '#')

        prefixes = file.read().splitlines()
        # Ignore lines that are empty or start with '#'
        prefixes = [line for line in prefixes if line and not line.startswith("#")]
        # Strip whitespace from each prefix
        prefixes = [line.strip() for line in prefixes if line.strip()]
        # Remove any leading or trailing whitespace from each prefix
        prefixes = [prefix.strip() for prefix in prefixes if prefix.strip()]
        # Return a list of prefixes, ensuring no empty strings are included
        prefixes = [prefix for prefix in prefixes if prefix]  # Remove empty strings

    return [prefix.strip() for prefix in prefixes if prefix.strip()]


def test():
    test_file_content = """
    #include "AbstractPlatform/Platform.h"
    #include "AbstractPlatform/PlatformUtils.h"
        #include "AbstractPlatform/PlatformConfig.h"
    //#include "AbstractPlatform/PlatformLogger.h"
    /*#include "AbstractPlatform/PlatformException.h"*/
    #include    "AbstractPlatform/PlatformThread.h"
    #include\t"AbstractPlatform/PlatformFile.h"
    #include "SomethingElse/PlatformNetwork.h"
    #
    #
    """

    test_out = replace_includes_line(
        test_file_content, prefixes=["AbstractPlatform/", "SomethingElse/"]
    )

    print("Test Output:")
    print(test_out)


# test()

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python3 header_format_convention_linter.py <file_path>")
        sys.exit(1)

    file_path = sys.argv[1]

    prefixes = read_header_prefixes()
    if not prefixes:
        print("No prefixes found in .header_prefixes.txt. Exiting.")
        sys.exit(1)

    replace_includes_in_file(file_path, prefixes)
