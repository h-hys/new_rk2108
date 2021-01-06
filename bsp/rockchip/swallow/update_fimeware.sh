#!/bin/sh

../tools/upgrade_tool db Image/rk3308_loader_uart2_nor_db.bin
../tools/upgrade_tool wl 0 rtthread.bin
../tools/upgrade_tool rd
