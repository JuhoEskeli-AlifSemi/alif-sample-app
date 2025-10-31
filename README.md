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

When starting up with the Murata 1LV driver, it crashes at `k_event_init(&emmc->irq_event);` in `intel_emmc_host.c`.

### Steps to Reproduce

1. Open the project in the `devcontainer` using VS Code.
2. Run the **Initialize** task (available via the button at the bottom of VS Code).
3. Run the **Update** task.
4. Run the **Blob** task
5. Run the **Build HE** task.
6. Run the **Flash HE** task.
7. Start the debugger task `ALIF E7 DK RTSS HE` with vscode
8. Pause the debugger and watch the callstack

```
arch_system_halt@0x800313ca (/workspaces/alif-sample-app/zephyr/kernel/fatal.c:30)
k_sys_fatal_error_handler@0x80020b5c (/workspaces/alif-sample-app/zephyr/kernel/fatal.c:44)
z_fatal_error@0x80020c84 (/workspaces/alif-sample-app/zephyr/kernel/fatal.c:119)
z_arm_fatal_error@0x8000ffea (/workspaces/alif-sample-app/zephyr/arch/arm/core/fatal.c:86)
z_arm_fault@0x80010894 (/workspaces/alif-sample-app/zephyr/arch/arm/core/cortex_m/fault.c:1080)
z_arm_usage_fault@0x800108dc (/workspaces/alif-sample-app/zephyr/arch/arm/core/cortex_m/fault_s.S:102)
<signal handler called>@0xffffffed (Unknown Source:0)
emmc_init@0x8001dd80 (/workspaces/alif-sample-app/zephyr/drivers/sdhc/intel_emmc_host.c:1326)
do_device_init@0x800313d8 (/workspaces/alif-sample-app/zephyr/kernel/init.c:311)
z_sys_init_run_level@0x80020dda (/workspaces/alif-sample-app/zephyr/kernel/init.c:369)
bg_thread_main@0x80020f36 (/workspaces/alif-sample-app/zephyr/kernel/init.c:519)
z_thread_entry@0x80002a3e (/workspaces/alif-sample-app/zephyr/lib/os/thread_entry.c:48)
arch_switch_to_main_thread@0x80010c44 (/workspaces/alif-sample-app/zephyr/arch/arm/core/cortex_m/thread.c:546)
??@0xaaaaaaaa (Unknown Source:0)
```
