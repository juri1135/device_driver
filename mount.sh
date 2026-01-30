#!/bin/sh

sudo mount -o loop buildroot/output/images/rootfs.ext4 /mnt \
sudo cp stack /mnt/usr/bin \
sync \
sudo umount /mnt
