#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>

#define GREEN_LED_NODE DT_ALIAS(led0)

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(GREEN_LED_NODE, gpios);

LOG_MODULE_REGISTER(app);

/* Test IRQ on P6.1 */
#if DT_NODE_HAS_PROP(DT_PATH(zephyr_user), test_irq_gpios)
static const struct gpio_dt_spec test_irq_pin = GPIO_DT_SPEC_GET(DT_PATH(zephyr_user), test_irq_gpios);
static struct gpio_callback test_irq_cb_data;

static void test_irq_callback(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
  int val = gpio_pin_get_dt(&test_irq_pin);
  LOG_INF("%s edge detected on P6.1 (HE)", val ? "Rising" : "Falling");
}
#endif

auto main() -> int
{
  int ret;

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

  /* Test IRQ on P6.1 : rising edge */
#if DT_NODE_HAS_PROP(DT_PATH(zephyr_user), test_irq_gpios)
  if (!gpio_is_ready_dt(&test_irq_pin))
  {
    LOG_ERR("GPIO IRQ device not ready");
  }
  else
  {
    ret = gpio_pin_configure_dt(&test_irq_pin, GPIO_INPUT);
    if (ret < 0)
    {
      LOG_ERR("GPIO IRQ configure failed: %d", ret);
    }
    else
    {
      gpio_init_callback(&test_irq_cb_data, test_irq_callback, BIT(test_irq_pin.pin));
      gpio_add_callback(test_irq_pin.port, &test_irq_cb_data);
      ret = gpio_pin_interrupt_configure_dt(&test_irq_pin, GPIO_INT_EDGE_BOTH);
      if (ret < 0)
      {
        LOG_ERR("GPIO IRQ interrupt configure failed: %d", ret);
      }
      else
      {
        LOG_INF("IRQ configured on P6.1 (both edges)");
      }
    }
  }
#endif

  while (true)
  {
    // LOG_INF("Blink from HE!");
    ret = gpio_pin_toggle_dt(&led);
    if (ret < 0)
    {
      LOG_ERR("Led toggle failed");
      return 0;
    }

    k_msleep(500);
  }
}