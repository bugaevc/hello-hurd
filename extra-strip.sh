#! /bin/bash

set -e

size=$(readelf --section-headers "$1" | awk '/shstrtab/ { print $6 }')
# Convert from hex:
size=$((16#"$size"))

echo Truncating "$1" to "$size" bytes

truncate -s "$size" "$1"

# Patch the ELF header:
dd of="$1" bs=1 seek=32 count=4 conv=notrunc status=none < /dev/zero
dd of="$1" bs=1 seek=46 count=6 conv=notrunc status=none < /dev/zero
