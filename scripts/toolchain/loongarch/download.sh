#!/bin/bash

# Get the desired destination directory from command-line arguments or use a default
dest_dir="${LOTOOLCHAIN_DIR}"

URL1='https://gitlab.eduxiji.net/scuccs-cst-sra-group/loos-rootfs-release/-/raw/main/rootfs.img.gz'
URL2='https://gitlab.educg.net/wangmingjian/os-contest-2024-image/-/raw/master/gcc-13.2.0-loongarch64-linux-gnu-nw.tgz'
URL3='https://gitlab.educg.net/wangmingjian/os-contest-2024-image/-/raw/master/qemu-2k1000-static.20240526.tar.xz'

#error exit cleanup function
function cleanup() {
   ${RM} ${dest_dir}
}

trap cleanup SIGINT SIGTERM SIGHUP ERR 
set -e

downloaded=1

# Function to download a file with specified URL and destination directory
function download_file() {
  url="$1"
  dest_dir="$2"

  # Check if destination directory exists, create it if not
  if ! [[ -d "$dest_dir" ]]; then
    ${MKDIR} "$dest_dir"
  fi

  if [[ -f "$dest_dir/$(basename "$url")" ]]; then
     echo file "$dest_dir/$(basename "$url")" already exists.
     return
  fi

  downloaded=0
  # Download the file
  wget -q --show-progress -O "$dest_dir/$(basename "$url")" "$url"
}

# Example usage:
download_file $URL1 "$dest_dir"
download_file $URL2 "$dest_dir"
download_file $URL3 "$dest_dir"

if [ "$downloaded" == "1" ]; then
	echo "all file have downloaded!"
	exit 0
fi

gzip -kdf "$dest_dir/$(basename $URL1)"
tar xfz "$dest_dir/$(basename $URL2)" -C "$dest_dir"
tar xfJ "$dest_dir/$(basename $URL3)" -C "$dest_dir"

cp -f ${src_tree}/scripts/toolchain/loongarch/runqemu "$dest_dir/qemu"
cp -f ${src_tree}/scripts/toolchain/loongarch/runqemu-debug "$dest_dir/qemu"
cp -f ${src_tree}/scripts/toolchain/loongarch/u-boot-with-spl.bin "$dest_dir/qemu/2k1000"
cp -f "$dest_dir/rootfs.img" "$dest_dir/qemu/2k1000"

exit 0
