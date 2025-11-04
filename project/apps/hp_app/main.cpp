#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/fs/fs.h>
#include <zephyr/fs/littlefs.h>

#define RED_LED_NODE DT_ALIAS(led0)

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(RED_LED_NODE, gpios);

LOG_MODULE_REGISTER(app);

auto main() -> int
{
  int ret;

  LOG_INF("Hello from HP");

#if 0
  // Mount LittleFS
  FS_FSTAB_DECLARE_ENTRY(DT_NODELABEL(lfshp));
  ret = fs_mount(&FS_FSTAB_ENTRY(DT_NODELABEL(lfshp)));
  if (ret < 0)
  {
    LOG_ERR("Error mounting littlefs [%d]", ret);
  }
  else
  {
    LOG_INF("LittleFS mounted successfully at /data");
  }

#endif
  if (!gpio_is_ready_dt(&led))
  {
    LOG_ERR("Led not ready\n");
    return 0;
  }

  ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
  if (ret < 0)
  {
    LOG_ERR("Led config failed\n");
    return 0;
  }

  while (true)
  {
    LOG_INF("Blink from HP!");
    ret = gpio_pin_toggle_dt(&led);
    if (ret < 0)
    {
      LOG_ERR("Led toggle failed");
      return 0;
    }

    k_msleep(10000);
  }
}