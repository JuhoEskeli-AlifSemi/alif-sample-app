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

## Problem

The UART shell for the HE core is not functioning properly, no logs are received when using the default `uart2` interface.
However, if the HE core is manually configured to use `uart4` (via `project/he_app/boards/alif_e7_dk_ae722f80f55d5xx_rtss_he.overlay`), the logs are successfully received.

### Steps to Reproduce

1. Open the project in the `devcontainer` using VS Code.
2. Run the **Initialize** task (available via the button at the bottom of VS Code).
3. Run the **Update** task.
4. Run the **Build HE** task.
5. Run the **Flash HE** task.
6. Open `/dev/ttyACM1`. You should see logs like `<inf> app: Blink !` appearing every 500 ms.
7. **No logs displayed**

**Note:** To ensure the system runs in single-core mode (HE only), execute the **Erase MRAM** task before reproduce steps.

If you switch from `uart2` to `uart4` with `project/he_app/boards/alif_e7_dk_ae722f80f55d5xx_rtss_he.overlay` then logs are displayed.  