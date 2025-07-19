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
To use cflash, simply execute the binary with a source and destination file.
```
cflash [source] [destination]
```
For example, you could do the following if you want to flash `/dev/sda` with a Debian image. 
```
cflash debian-12.11.0-amd64-netinst.iso /dev/sda
```