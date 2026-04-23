# alif-sample-app

This repository is used to highlight features and bugs related to ALIF's E7 devkit.
Each of them will be in its own branch, and there is a template branch to start from.

## Multi-Core Shared GPIO

This branch demonstrates multi-core safe shared GPIO on the Alif E7, where both the
HE (M55_HE, 160 MHz) and HP (M55_HP, 400 MHz) cores can use the same GPIO controller
and receive interrupts on the same pin simultaneously.

### Problem

GPIO controllers on the E7 (e.g. `gpio6` at `0x49006000`) are shared peripherals —
both cores access the same hardware registers. The upstream DesignWare GPIO driver
(`gpio_dw.c`) blanket-disables all interrupts during init and has no multi-core
protection, so whichever core boots second clobbers the first core's interrupt
configuration.

### Solution

The driver is modified with a `CONFIG_GPIO_DW_MULTICORE` option that:

1. **Skips blanket interrupt clearing at init** — the `INTMASK`, `INTEN`, and
   `PORTA_EOI` registers are no longer reset to avoid clobbering the other core's
   configuration.

2. **Uses a hardware semaphore (`hwsem0`) to protect shared register access** —
   `pin_interrupt_configure()` acquires the hwsem before read-modify-write
   operations on shared GPIO registers, using master ID 1 for HE and 2 for HP.

3. **Enables NVIC IRQs lazily per pin** — instead of enabling all IRQs at init,
   each pin's NVIC line is enabled only when `gpio_pin_interrupt_configure()` is
   called, and disabled when the interrupt is removed.

4. **Tracks per-core pin ownership** — an `owned_pins` bitmask in the runtime data
   records which pins this core has configured for interrupts. Only owned pins are
   handled in the ISR.

5. **Uses per-pin ISR parameters** — on GPIO controllers with one IRQ per pin
   (E7 gpio0–gpio9), each `IRQ_CONNECT` passes a `{device, pin}` struct so the ISR
   knows exactly which pin fired without reading `INTSTATUS`. This is essential for
   shared-pin operation where the other core may have already cleared `INTSTATUS`.
   On controllers with a single shared IRQ (E4/E8 gpio16–17), a shared ISR reads
   `INTSTATUS` filtered by `owned_pins`.

6. **Both cores write `PORTA_EOI`** — the EOI register is write-1-to-clear, so
   duplicate writes are harmless. This avoids requiring a specific core to be
   running for the other to function.

### Shared Pin Interrupt Flow

When both cores configure the same pin (e.g. P6.1) for edge interrupts:

1. An edge on the pin causes the GPIO to pulse the per-pin NVIC IRQ line.
2. Both cores' NVICs independently latch the interrupt as pending.
3. Both cores enter their ISR. Each knows it's pin 1 from the ISR parameter.
4. Both write `PORTA_EOI` for pin 1 (one is effective, the other is a no-op).
5. Both fire their application callbacks.

### Test Setup

- **HE core** toggles GREEN LED (P6.4) as output every 500 ms.
- **HP core** toggles RED LED (P6.2) as output every 500 ms.
- **P6.2 (RED LED) is wired to P6.1** with a jumper.
- **Both cores** configure P6.1 (gpio6 pin 1) as edge-both interrupt input.
- Both cores log rising/falling edges independently:
  - HE: `"Rising edge detected on P6.1 (HE)"`
  - HP: `"Rising edge detected on P6.1 (HP)"`

### Configuration

Enabled in `project/base.conf`:
```
CONFIG_IPM=y
CONFIG_ALIF_HWSEM=y
CONFIG_GPIO_DW_MULTICORE=y
```

Both core overlays enable `gpio6` and `hwsem0`:
```dts
&hwsem0 { status = "okay"; };
&gpio6  { status = "okay"; };
```

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