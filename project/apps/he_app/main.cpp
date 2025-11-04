#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/fs/fs.h>
#include <zephyr/fs/littlefs.h>
#include <zephyr/storage/flash_map.h>

#define GREEN_LED_NODE DT_ALIAS(led0)

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(GREEN_LED_NODE, gpios);

LOG_MODULE_REGISTER(app);

auto main() -> int
{
  int ret;

// Mount LittleFS
#if 1
  FS_FSTAB_DECLARE_ENTRY(DT_NODELABEL(lfshe));
  ret = fs_mount(&FS_FSTAB_ENTRY(DT_NODELABEL(lfshe)));
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
    LOG_INF("Blink from HE!");
    ret = gpio_pin_toggle_dt(&led);
    if (ret < 0)
    {
      LOG_ERR("Led toggle failed");
      return 0;
    }

    k_msleep(500);
  }
}