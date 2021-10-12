#!/bin/bash
SIZE=48
if [ $# -ne 0 ]; then
    SIZE=$1
fi
{
sudo mkdir -p /mnt/ram
sudo mkfs.ext4 -F /dev/pmem12
sudo mount -t ext4 -o dax /dev/pmem12 /mnt/ram
sudo chmod -R 777 /mnt/ram
} 1>/dev/null 2>&1
