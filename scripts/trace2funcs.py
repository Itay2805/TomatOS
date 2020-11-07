import fileinput
import re
import subprocess
import sys

ansi_escape = re.compile(r'\x1B\[[0-?]*[ -/]*[@-~]')
reg_pattern = re.compile(r'([fF]{8}81[a-fA-F0-9]{6})')


def addr2line(addr):
    out = subprocess.check_output("addr2line -pfse {} {}".format(sys.argv[1], addr), shell=True)
    return re.sub(r'(\(discriminator [0-9]*\))', '', out.decode("utf-8").strip())


if len(sys.argv) < 2:
    print("missing required argument: <filename>")
    sys.exit(1)


with fileinput.FileInput("-") as file_in:
    for line in file_in:
        line = ansi_escape.sub('', line).strip('\n')
        reg_match = reg_pattern.search(line)
        if not reg_match:
            print(line)
        else:
            s = addr2line(reg_match.group(1))
            if '?? ??:0' not in s:
                print(line + " -> " + s)
            else:
                print(line)