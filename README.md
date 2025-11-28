# alif-sample-app

This repository is used to highlight features and bugs related to ALIF's E7 devkit.
Each of them will be in its own branch, and there is a template branch to start from.

## Prerequisites

### Git & Docker

```bash
sudo apt install git
sudo apt install docker
sudo apt install docker-buildx
sudo usermod -aG docker $USER
```

### UDEV rules

```bash
wget -O 60-openocd.rules https://sf.net/p/openocd/code/ci/master/tree/contrib/60-openocd.rules?format=raw
sudo cp 60-openocd.rules /etc/udev/rules.d
sudo udevadm control --reload
```

## Description

This example runs the https://github.com/alifsemi/zephyr_alif/tree/main/samples/drivers/spi_flash test on HE & HP cores. The example has been modified so that OSPI1_RESERVE() is called before OSPI flash operations. And when operation is finished, OSPI1_RELEASE() is called. These operations use HW semaphore to synchronize resource access. The operations also toggle OSPI1 IRQ on and off, so when the other core is doing something with OSPI1, it does not trigger IRQ routines on the another core.

Modification has also been made to flash_ospi_is25wx.c in the referred zephyr_alif repository. The modifications block entry into init routine using HW semaphores. IRQ for OSPI1 is also turned off at the end of the initialization sequence. In addition pending interrupt is cleared before enabling IRQ in the init routine.

### Steps to Reproduce


1. Open the project in the `devcontainer` using VS Code.
2. Run the **Initialize** task (available via the button at the bottom of VS Code).
3. Run the **Update** task.
5. Run the **Configure** task, select `HE & HP`
5. Run the **Build** task.
6. Run the **Flash** task.
7. Open serial monitor on HE UART and HP UART

```
Hello from HP

ospi_flash@0 OSPI flash testing
========================================
****Flash Configured Parameters******
* Num Of Sectors : 16384
* Sector Size : 4096
* Page Size : 4096
* Erase value : 255
* Write Blk Size: 1
* Total Size in MB: 64

Test 1: Flash erase
Flash erase succeeded!

Test 1: Flash write
Attempting to write 4 bytes

Test 1: Flash read
Data read matches data written. Good!!
Sleep for 5 seconds.
*** Booting Zephyr OS build 15dd44461e60 ***

Hello from HE

ospi_flash@0 OSPI flash testing
========================================
****Flash Configured Parameters******
* Num Of Sectors : 16384
* Sector Size : 4096
* Page Size : 4096
* Erase value : 255
* Write Blk Size: 1
* Total Size in MB: 64

Test 1: Flash erase
Flash erase succeeded!

Test 1: Flash write
Attempting to write 4 bytes

Test 1: Flash read
Data read matches data written. Good!!
Sleep for 5 seconds.
*** Booting Zephyr OS build 15dd44461e60 ***
```