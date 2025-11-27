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
static const struct adc_channel_cfg channel0_cfg = {
    .gain = ADC_GAIN_1,
    .reference = ADC_REF_VDD_1,
    .acquisition_time = ADC_ACQ_TIME_DEFAULT,
    .channel_id = 0,
    .differential = 0};

// Configuration du channel 1 (P0_1)
static const struct adc_channel_cfg channel1_cfg = {
    .gain = ADC_GAIN_1,
    .reference = ADC_REF_VDD_1,
    .acquisition_time = ADC_ACQ_TIME_DEFAULT,
    .channel_id = 1,
    .differential = 0};

auto main() -> int
{
  int ret;
  uint32_t buf[2]; // Buffer pour 2 channels (CH0 et CH1)
  struct adc_sequence sequence = {
      .buffer = buf,
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

  // Configuration channel 0
  ret = adc_channel_setup(adc_dev, &channel0_cfg);
  if (ret < 0)
  {
    LOG_ERR("Could not setup ADC channel 0 (%d)", ret);
    return 0;
  }

  // Configuration channel 1
  ret = adc_channel_setup(adc_dev, &channel1_cfg);
  if (ret < 0)
  {
    LOG_ERR("Could not setup ADC channel 1 (%d)", ret);
    return 0;
  }

  // Configuration de la séquence ADC - Lire les deux channels
  sequence.channels = BIT(0) | BIT(1); // Channel 0 (P0_0) et Channel 1 (P0_1)

  LOG_INF("ADC24 initialized: CH0 (P0_0) and CH1 (P0_1)");
  LOG_INF("Continuous conversion mode - Reading every 1s");

  while (true)
  {
    // Toggle LED
    ret = gpio_pin_toggle_dt(&led);
    if (ret < 0)
    {
      LOG_ERR("Led toggle failed");
      return 0;
    }

    // Lecture ADC en mode continu
    ret = adc_read(adc_dev, &sequence);
    if (ret < 0)
    {
      LOG_ERR("Could not read ADC (%d)", ret);
    }
    else
    {
      // Afficher les valeurs des deux channels
      LOG_INF("CH0 (P0_0): %u | CH1 (P0_1): %u", buf[0], buf[1]);
    }

    // Attendre 1 seconde
    k_msleep(1000);
  }
}