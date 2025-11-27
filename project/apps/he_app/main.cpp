#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/device.h>
#include <zephyr/net/net_if.h>

LOG_MODULE_REGISTER(app);

auto main() -> int
{
  // Wait for early boot logs to flush
  k_msleep(2000);

  LOG_INF("=== Application Started ===");

  // List all network interfaces
  LOG_INF("Network interfaces:");
  struct net_if *iface = net_if_get_default();
  if (iface)
  {
    LOG_INF("  Default interface: %p", iface);
    LOG_INF("  Interface up: %d", net_if_is_up(iface));
  }
  else
  {
    LOG_ERR("  No default network interface!");
  }

  // Count all interfaces
  int if_count = 0;
  STRUCT_SECTION_FOREACH(net_if, net_if)
  {
    if_count++;
    LOG_INF("  Interface #%d: %p", if_count, net_if);
  }

  LOG_INF("Total network interfaces: %d", if_count);

// Get WiFi device using proper DT macros
#if DT_HAS_COMPAT_STATUS_OKAY(infineon_airoc_wifi)
  printk("DT: infineon_airoc_wifi compatible found in devicetree\n");
  LOG_INF("DT: infineon_airoc_wifi compatible found in devicetree");

  const struct device *wifi_dev = DEVICE_DT_GET_ONE(infineon_airoc_wifi);
  printk("WiFi device pointer: %p\n", wifi_dev);

  if (wifi_dev)
  {
    LOG_INF("WiFi device found via DEVICE_DT_GET_ONE: %p", wifi_dev);

    // CRITICAL: Check if device is ready before accessing its fields
    if (device_is_ready(wifi_dev))
    {
      LOG_INF("  Device is READY");
      LOG_INF("  Device name: %s", wifi_dev->name);
    }
    else
    {
      LOG_ERR("  Device is NOT READY - initialization incomplete");
    }
  }
  else
  {
    LOG_ERR("WiFi device is NULL despite DT compatible found!");
  }
#else
  LOG_ERR("DT: infineon_airoc_wifi compatible NOT found in devicetree!");
#endif

  while (true)
  {
    LOG_INF("Hello World");
    k_msleep(10000);
  }
}