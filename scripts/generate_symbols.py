#!/usr/bin/python3

import sys
import subprocess

print('#include <util/symlist.h>')
print()
print('struct symlist_t symlist[] = {')

highest = 0xffffffffffffffff

try:
    output = subprocess.check_output(['nm', sys.argv[1], '-n']).decode(sys.stdout.encoding)

    for line in output.split('\n'):
        line = line.strip()
        print(f'// {line}')
        address, typ, name = line.split(' ', 2)
        if typ in [
            't', 'T',   # in text section
            'i', 'I',   # Indirect symbol
            'U',        # undefined
        ]:
            print(f'    {{0x{address}ull, "{repr(name)[1:-1]}"}},')
        highest = int(address, 16)
except Exception as e:
    print('//' + str(e))

print('    {' + hex(highest) + ', "__kernel_end"}')
print('};')
print('')
