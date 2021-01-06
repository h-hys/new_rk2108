#!/bin/bash

../tools/openocd -f ftdi.cfg -f scr1.cfg & ../../../../prebuilts/gcc/linux-x86/riscv64/riscv64-unknown-elf-gcc-8.2.0-2019.05.3-x86_64-linux/bin/riscv64-unknown-elf-gdb rtthread.elf --batch -ex "set mem inaccessible-by-default off" -ex "set arch riscv:rv32" -ex "set remotetimeout 250" -ex "target extended-remote localhost:3333" -ex "monitor reset halt" -ex "load" -ex "monitor resume" -ex "monitor shutdown" -ex "quit" && \
echo "Successfully uploaded"

