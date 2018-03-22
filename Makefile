MAKEFLAGS=--warn-undefined-variables
dir=/mnt

getdents: getdents.c
	gcc -std=c99 -Werror -Wall -O2 getdents.c -o getdents

clean:
	rm -fv getdents

test: getdents
	ls         $(dir)/$(filesystem) >/dev/null
	rm -rf     $(dir)/$(filesystem)/*
	touch      $(dir)/$(filesystem)/{0..9}
	./getdents $(dir)/$(filesystem)

device:
	dd if=/dev/zero of=/tmp/ext2  bs=1M count=100
	dd if=/dev/zero of=/tmp/ext3  bs=1M count=100
	dd if=/dev/zero of=/tmp/ext4  bs=1M count=100
	dd if=/dev/zero of=/tmp/xfs   bs=1M count=100
	dd if=/dev/zero of=/tmp/btrfs bs=1M count=100
	dd if=/dev/zero of=/tmp/minix bs=1M count=100

mkfs:
	yes | mkfs.ext2   /tmp/ext2
	yes | mkfs.ext3   /tmp/ext3
	yes | mkfs.ext4   /tmp/ext4
	yes | mkfs.xfs    /tmp/xfs
	yes | mkfs.btrfs  /tmp/btrfs
	yes | mkfs.minix  /tmp/minix

mount:
	mkdir -p /mnt/{ext2,ext3,ext4,xfs,btrfs,minix,tmpfs}
	mount -o loop -t ext2  /tmp/ext2  /mnt/ext2
	mount -o loop -t ext3  /tmp/ext3  /mnt/ext3
	mount -o loop -t ext4  /tmp/ext4  /mnt/ext4
	mount -o loop -t xfs   /tmp/xfs   /mnt/xfs
	mount -o loop -t btrfs /tmp/btrfs /mnt/btrfs
	mount -o loop -t minix /tmp/minix /mnt/minix
	mount -o loop -t tmpfs tmpfs      /mnt/tmpfs

umount:
	umount /mnt/*
