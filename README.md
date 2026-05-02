# cflash
A simple tool for creating bootable drives.

## Dependencies
To use cflash `dd` and `sync` need to be installed.
A child process is created to write to the drive using dd, after which sync is called in another child process
to ensure all data is written.

## Installation
Clone the repo and install cflash with make:
```
git clone https://github.com/vxnxnt/cflash.git
cd cflash
make
sudo make install
```

To uninstall with make:
```
sudo make uninstall
```

## Usage
To use cflash, simply execute the binary with a source and destination file.\
To get help using cflash, try the `-h` or `--help` option.
```
cflash [option] [source] [destination]
```

For example, you could do the following if you want to flash `/dev/sda` with a Debian image. 
```
cflash debian-12.11.0-amd64-netinst.iso /dev/sda
```

To verify the validity of a boot image's SHA256 checksum, use the `-s` or `-f` option.
The former allows you to parse a SHA256 hash to cflash, while the latter allows a file containing the hash to be read.
```
cflash -s e36e08961573d9388c7f225e1f47e1b31052a2e061510b6e91d3edc730c06ee8 alpine-extended-3.23.3-x86_64.iso
cflash -f alpine-extended-3.23.3-x86_64.iso.sha256 alpine-extended-3.23.3-x86_64.iso
```