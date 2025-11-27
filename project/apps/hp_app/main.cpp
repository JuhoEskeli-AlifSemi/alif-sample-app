#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/sys/util.h>

#define RED_LED_NODE DT_ALIAS(led0)

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(RED_LED_NODE, gpios);

LOG_MODULE_REGISTER(app);

// Configuration ADC - Référence directe à l'ADC24
static const struct device *adc_dev = DEVICE_DT_GET(DT_NODELABEL(adc24));

// Configuration du channel 0 (P0_0)
static const struct adc_channel_cfg channel_cfg = {
    .gain = ADC_GAIN_1,
    .reference = ADC_REF_VDD_1,
    .acquisition_time = ADC_ACQ_TIME_DEFAULT,
    .channel_id = 0,
    .differential = 0};

auto main() -> int
{
  int ret;
  uint32_t buf; // ADC24 nécessite 4 octets (32 bits)
  struct adc_sequence sequence = {
      .buffer = &buf,
      .buffer_size = sizeof(buf),
  };

  // Configuration LED
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

  // Configuration ADC
  if (!device_is_ready(adc_dev))
  {
    LOG_ERR("ADC24 device not ready");
    return 0;
  }

  ret = adc_channel_setup(adc_dev, &channel_cfg);
  if (ret < 0)
  {
    LOG_ERR("Could not setup ADC channel (%d)", ret);
    return 0;
  }

  // Configuration de la séquence ADC
  sequence.channels = BIT(0); // Channel 0 (P0_0)

  LOG_INF("ADC24 initialized on P0_0 - Single shot mode every 1s");

  while (true)
  {
    // Toggle LED
    ret = gpio_pin_toggle_dt(&led);
    if (ret < 0)
    {
      LOG_ERR("Led toggle failed");
      return 0;
    }

    // Lecture ADC en single shot
    ret = adc_read(adc_dev, &sequence);
    if (ret < 0)
    {
      LOG_ERR("Could not read ADC (%d)", ret);
    }
    else
    {
      // Afficher la valeur brute
      LOG_INF("ADC P0_0: raw value = %d", buf);
    }

    // Attendre 1 seconde
    k_msleep(1000);
  }
}