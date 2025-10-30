set(HP_APP hp_app)
set(HP_APP_SRC_DIR ./../project/${HP_APP})
externalzephyrproject_add(APPLICATION ${HP_APP} SOURCE_DIR ${HP_APP_SRC_DIR} BOARD alif_e7_dk_rtss_hp)
