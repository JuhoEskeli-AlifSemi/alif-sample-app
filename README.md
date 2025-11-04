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

With LFS on OSPI, the HE core crashes when running in dual-boot mode with the HP core, whereas everything works correctly when either HP or HE runs alone.

### Steps to Reproduce


1. Open the project in the `devcontainer` using VS Code.
2. Run the **Initialize** task (available via the button at the bottom of VS Code).
3. Run the **Update** task.
5. Run the **Configure** task, select `HE & HP`
5. Run the **Build** task.
6. Run the **Flash** task.
7. Open serial monitor on HE UART and HP UART

```
[00:00:00.000,000] <err> os: ***** HARD FAULT *****
[00:00:00.000,000] <err> os:   Fault escalation (see below)
[00:00:00.000,000] <err> os: ***** USAGE FAULT *****
[00:00:00.000,000] <err> os:   Illegal use of the EPSR
[00:00:00.000,000] <err> os: r0/a1:  0x00000001  r1/a2:  0x00000000  r2/a3:  0x00000000
[00:00:00.000,000] <err> os: r3/a4:  0x00000000 r12/ip:  0x00000000 r14/lr:  0x80013681
[00:00:00.000,000] <err> os:  xpsr:  0x20000071
[00:00:00.000,000] <err> os: s[ 0]:  0x20000f6c  s[ 1]:  0x800124f7  s[ 2]:  0x00000061  s[ 3]:  0x8000421b
[00:00:00.000,000] <err> os: s[ 4]:  0x00000000  s[ 5]:  0xfffffffd  s[ 6]:  0xc5044e98  s[ 7]:  0x3edb9c51
[00:00:00.000,000] <err> os: s[ 8]:  0x64403a7e  s[ 9]:  0xce0a75f1  s[10]:  0xc2380a21  s[11]:  0x033ecded
[00:00:00.000,000] <err> os: s[12]:  0x3b84cf7c  s[13]:  0x5e5b606c  s[14]:  0xa54b8553  s[15]:  0x750fd133
[00:00:00.000,000] <err> os: fpscr:  0x5f4c763f
[00:00:00.000,000] <err> os: Faulting instruction address (r15/pc): 0x00000000
[00:00:00.000,000] <err> os: >>> ZEPHYR FATAL ERROR 35: Unknown error on CPU 0
[00:00:00.000,000] <err> os: Fault during interrupt handling

[00:00:00.000,000] <err> os: Current thread: 0x20000a68 (main)
[00:00:00.000,000] <err> os: Halting system
```