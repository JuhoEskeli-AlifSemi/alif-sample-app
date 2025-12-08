#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/fs/fs.h>
#include <zephyr/fs/littlefs.h>
#include <zephyr/storage/flash_map.h>

// hwsem
#include <zephyr/drivers/hwsem_ipm.h>
#define DEVICE_DT_GET_AND_COMMA(node_id) DEVICE_DT_GET(node_id),

/* Generate a list of devices for all instances of the "compat" */
#define DEVS_FOR_DT_COMPAT(compat) DT_FOREACH_STATUS_OKAY(compat, DEVICE_DT_GET_AND_COMMA)

static const struct device *const devices[] = {
#ifdef CONFIG_ALIF_HWSEM
	DEVS_FOR_DT_COMPAT(alif_hwsem)
#endif
};

#if defined(CONFIG_SOC_AE722F80F55D5XX_RTSS_HP)
#define MASTER_ID 0xF00DF00D
#elif defined(CONFIG_SOC_AE722F80F55D5XX_RTSS_HE)
#define MASTER_ID 0xC0DEC0DE
#endif

#define OSPI1_IRQ (IRQn_Type)97

#define GREEN_LED_NODE DT_ALIAS(led0)

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(GREEN_LED_NODE, gpios);

LOG_MODULE_REGISTER(app);

static void OSPI1_RESERVE() {
	/* Use only the single HWSEM device instance for this test */
	const struct device *device = devices[0];

	/* First trylock: can return 0 (success) or -EBUSY (busy, locked by another core) */
	while (hwsem_trylock(device, MASTER_ID) != 0)
	{
		printf("Waiting for HWSEM.");
		k_msleep(1000);
	} /* spinwait for our turn */

	// After init the IRQ is disabled
	NVIC_EnableIRQ(OSPI1_IRQ);
}

static void OSPI1_RELEASE() {
	/* Use only the single HWSEM device instance for this test */
	const struct device *device = devices[0];

	// After init the IRQ is disabled
	NVIC_DisableIRQ(OSPI1_IRQ);
	hwsem_unlock(device, MASTER_ID);
}

auto main() -> int
{
  int ret;


  OSPI1_RESERVE();

  // Mount LittleFS
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

  OSPI1_RELEASE();

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