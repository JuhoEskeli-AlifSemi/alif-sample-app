#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>

LOG_MODULE_REGISTER(app);

#define GREEN_LED_NODE DT_ALIAS(led0)
#define DEVICE_NAME "TEMPLATE_HE"
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(GREEN_LED_NODE, gpios);

const struct bt_le_adv_param *adv_param =
    BT_LE_ADV_PARAM((BT_LE_ADV_OPT_CONN | BT_LE_ADV_OPT_USE_IDENTITY), 800, /* Min Advertising Interval 500ms
                                                                                                               (800*0.625ms) */
                    801,                                                    /* Max Advertising Interval 500.625ms (801*0.625ms) */
                    NULL);                                                  /* Set to NULL for undirected advertising */

static struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),

};

static struct bt_data sd[] = {
    BT_DATA_BYTES(BT_DATA_UUID128_ALL, 0x84, 0xd1, 0x26, 0xd9, 0x92, 0x41, 0x22, 0xb6, 0x05, 0x42, 0x2f, 0xbf, 0xc1, 0x38, 0x25, 0x67),
};

// Callback appelé quand un client se connecte
static void onConnected(struct bt_conn *conn, uint8_t err)
{
  if (err)
  {
    LOG_ERR("Connection failed (err %u)", err);
    return;
  }

  char addr[BT_ADDR_LE_STR_LEN];
  bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
  LOG_INF("Connected: %s", addr);
}

// Callback appelé quand un client se déconnecte
static void onDisconnected(struct bt_conn *conn, uint8_t reason)
{
  char addr[BT_ADDR_LE_STR_LEN];
  bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
  LOG_INF("Disconnected: %s (reason %u)", addr, reason);
}

static struct bt_conn_cb connectionCallbacks = {
    .connected = onConnected,
    .disconnected = onDisconnected,
};

auto main() -> int
{
  int ret;
  int err;

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

  LOG_INF("Initializing Bluetooth...");
  LOG_INF("This may take a few seconds...");

  bt_conn_cb_register(&connectionCallbacks);

  err = bt_enable(NULL);
  if (err)
  {
    LOG_ERR("Bluetooth init failed (err %d)", err);
    LOG_ERR("Possible causes:");
    LOG_ERR("  - BT module not powered on");
    LOG_ERR("  - Wrong UART configuration");
    LOG_ERR("  - Wrong GPIO for bt-reg-on");
    LOG_ERR("  - Hardware flow control issue");
    return 0;
  }
  LOG_INF("Bluetooth initialized successfully");

  LOG_INF("Starting advertising...");

  err = bt_le_adv_start(adv_param, ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));
  if (err)
  {
    LOG_ERR("Advertising failed to start (err %d)", err);
    return 0;
  }
  LOG_INF("Advertising started successfully");
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