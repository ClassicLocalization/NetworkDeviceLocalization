/* Wi-Fi CSI Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_wifi.h"
#include "driver/gpio.h"
#include "esp_console.h"

#include "lwip/inet.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"

#include "esp_radar.h"
#include "app_priv.h"

#define CONFIG_CSI_BUF_SIZE          50
#define CONFIG_GPIO_LED_MOVE_STATUS  GPIO_NUM_18

static const char *TAG  = "app_main";

float g_move_absolute_threshold = 0.3;
float g_move_relative_threshold = 1.5;

//for automatic sta init
#define WIFI_CONNECTED_BIT    BIT0
#define WIFI_DISCONNECTED_BIT BIT1

static bool s_reconnect = true;
static EventGroupHandle_t s_wifi_event_group;

static int num_sta_connected = 0;
static int counter = 0;
static int port = 50000;
static char ip_address[18] = "192.168.4.3";
static int external_ip = 0;

static int a = 1;
static int* cmd_ret = &a;

static int step = 0;

static void wifi_init(void)
{
    esp_err_t ret = nvs_flash_init();

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    ESP_ERROR_CHECK(ret);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_NULL));
    ESP_ERROR_CHECK(esp_wifi_start());
}

/* Event handler for catching system events */
static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        xEventGroupClearBits(s_wifi_event_group, WIFI_DISCONNECTED_BIT);
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_reconnect) {
            ESP_LOGI(TAG, "sta disconnect, s_reconnect...");
            esp_wifi_connect();
        } else {
            ESP_LOGI(TAG, "sta disconnect");
        }

        xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
        xEventGroupSetBits(s_wifi_event_group, WIFI_DISCONNECTED_BIT);
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_CONNECTED) {
        wifi_event_sta_connected_t *event = (wifi_event_sta_connected_t *)event_data;
        ESP_LOGI(TAG, "Connected to %s (bssid: "MACSTR", channel: %d)", event->ssid,
                 MAC2STR(event->bssid), event->channel);
    }  else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STACONNECTED) {
        ESP_LOGI(TAG, "STA Connecting to the AP again...");
    }
}

static int sta_setup(char *user_password, char *user_ssid)
{
    const char *ssid     = user_ssid;
    const char *password = user_password;
    wifi_config_t wifi_config = { 0 };
    static esp_netif_t *s_netif_sta = NULL;

    if(!s_wifi_event_group){
        s_wifi_event_group = xEventGroupCreate();
        ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
        ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL));
    }

    if (!s_netif_sta) {
        s_netif_sta = esp_netif_create_default_wifi_sta();
    }

    strlcpy((char *) wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid));

    if (password) {
        strlcpy((char *) wifi_config.sta.password, password, sizeof(wifi_config.sta.password));
    }

    ESP_LOGI(TAG, "sta connecting to '%s'", ssid);

    int bits = xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT, false, true, false);

    if (bits & WIFI_CONNECTED_BIT) {
        s_reconnect = false;
        xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
        ESP_ERROR_CHECK(esp_wifi_disconnect());
        xEventGroupWaitBits(s_wifi_event_group, WIFI_DISCONNECTED_BIT, false, true, portTICK_RATE_MS);
    }

    s_reconnect = true;

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    esp_wifi_connect();

    xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT, false, true, pdMS_TO_TICKS(5000));

    return ESP_OK;
}

void send_udp(char message[], char address_to_send[], int port_num) {
    ESP_LOGI(TAG, "%s", message);
    ESP_LOGI(TAG, "%s", address_to_send);
    struct sockaddr_in dest_addr;
    dest_addr.sin_addr.s_addr = inet_addr(address_to_send);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port_num);

    int addr_family = AF_INET;
    int ip_protocol = IPPROTO_IP;

    int sock = socket(addr_family, SOCK_DGRAM, ip_protocol);

    char *payload[100];
    strcpy(payload, message);

    int err = sendto(sock, message, strlen(message), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (err < 0) {
        ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
    }
    ESP_LOGI(TAG, "Message sent to %s", address_to_send);

    ESP_LOGE(TAG, "Shutting down socket sender...");
    shutdown(sock, 0);
    close(sock);
}

void wifi_csi_raw_cb(void *ctx, wifi_csi_info_t *info)
{
    static char buff[2048];
    size_t len = 0;
    wifi_pkt_rx_ctrl_t *rx_ctrl = &info->rx_ctrl;
    static uint32_t s_count = 0;

    if (!s_count) {
        // ets_printf("type,id,mac,rssi,rate,sig_mode,mcs,bandwidth,smoothing,not_sounding,aggregation,stbc,fec_coding,sgi,noise_floor,ampdu_cnt,channel,secondary_channel,local_timestamp,ant,sig_len,rx_state,len,first_word,data\n");
        len += snprintf(buff, sizeof(buff),"type,id,mac,rssi,rate,sig_mode,mcs,bandwidth,smoothing,not_sounding,aggregation,stbc,fec_coding,sgi,noise_floor,ampdu_cnt,channel,secondary_channel,local_timestamp,ant,sig_len,rx_state,len,first_word,data\n");
    }

    len += snprintf(buff + len, sizeof(buff) - len,"CSI_DATA,%d," MACSTR ",%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
               s_count++, MAC2STR(info->mac), rx_ctrl->rssi, rx_ctrl->rate, rx_ctrl->sig_mode,
               rx_ctrl->mcs, rx_ctrl->cwb, rx_ctrl->smoothing, rx_ctrl->not_sounding,
               rx_ctrl->aggregation, rx_ctrl->stbc, rx_ctrl->fec_coding, rx_ctrl->sgi,
               rx_ctrl->noise_floor, rx_ctrl->ampdu_cnt, rx_ctrl->channel, rx_ctrl->secondary_channel,
               rx_ctrl->timestamp, rx_ctrl->ant, rx_ctrl->sig_len, rx_ctrl->rx_state);

    len += snprintf(buff + len, sizeof(buff) - len, ",%d,%d,\"[", info->len, info->first_word_invalid);

    int i = 0;
    for (; i < info->len - 1; i++) {
        len += snprintf(buff + len, sizeof(buff) - len, "%d,", info->buf[i]);
    }
    len += snprintf(buff + len, sizeof(buff) - len, "%d",info->buf[i]);

    len += snprintf(buff + len, sizeof(buff) - len, "]\"\n");
    ets_printf("%s",buff);

    char *payload[256];
    strcpy(payload, "");

    char *data[8];
    sprintf(data, "%d;%d;%d", rx_ctrl->rssi, rx_ctrl->noise_floor, rx_ctrl->sig_len);
    strcat(payload, data);

    for(int i = 32; i < 63; i++) {
        sprintf(data, ";%d", info->buf[i]);
        strcat(payload, data);
    }

    send_udp(payload, ip_address, port);

    counter++;
    if(counter==50) {
        esp_console_run("reset", cmd_ret);
    }
    
}

static bool valid_message(char message[]) 
{
    step = message[0] - '0';

    if(strlen(message) == 4) {
        external_ip = message[3] - '0';
    }else if(strlen(message) == 5) {
        char storage[5] = "";
        storage[0] = message[3];
        storage[1] = message[4];
        external_ip = atoi(storage);
    }else if(strlen(message) == 12) {
        esp_console_run("reset", cmd_ret);
    }else {
        return false;
    }

    return true;
}

static void start_csi_exchange(void) 
{
    if(step == 1) {
        //esp_console_run("csi -l 384 -m 42:29:23:38:a4:be", cmd_ret);
        esp_console_run("csi -l 384 -m a8:03:2a:e1:11:b5", cmd_ret);
        esp_console_run("ping 192.168.4.1 -c 1000 -i 0.01", cmd_ret);
    }else if(step == 2) {
        esp_console_run("csi -l 384 -m a8:03:2a:e1:11:bc", cmd_ret);
        esp_console_run("ping 192.168.4.2 -c 9 -i 0.001", cmd_ret);
    }else if(step == 3) {
        esp_console_run("csi -l 384 -m a8:03:2a:e1:11:90", cmd_ret);
        esp_console_run("ping 192.168.4.3 -c 9 -i 0.001", cmd_ret);
    }else if(step == 4) {
        esp_console_run("csi -l 384 -m a8:03:2a:e1:0f:b8", cmd_ret);
        esp_console_run("ping 192.168.4.4 -c 9 -i 0.001", cmd_ret);
    }else if(step == 5) {
        esp_console_run("csi -l 384 -m a8:03:2a:e1:0f:c4", cmd_ret);
        esp_console_run("ping 192.168.4.5 -c 9 -i 0.001", cmd_ret);
    }else if(step == 6) {
        esp_console_run("csi -l 384 -m a8:03:2a:e1:0f:b0", cmd_ret);
        esp_console_run("ping 192.168.4.5 -c 9 -i 0.001", cmd_ret);
    }
}

static void wait_csi_enabling(void)
{
    char rx_buffer[128];
    char addr_str[128];
    int addr_family = AF_INET;
    int ip_protocol = 0;
    struct sockaddr_in6 dest_addr;

    while (1) {
        struct sockaddr_in *dest_addr_ip4 = (struct sockaddr_in *)&dest_addr;
        dest_addr_ip4->sin_family = AF_INET;
        dest_addr_ip4->sin_port = htons(port);
        ip_protocol = IPPROTO_IP;

        int sock = socket(addr_family, SOCK_DGRAM, ip_protocol);
        if (sock < 0) {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Socket created");

        #if defined(CONFIG_EXAMPLE_IPV4) && defined(CONFIG_EXAMPLE_IPV6)
            if (addr_family == AF_INET6) {
                // Note that by default IPV6 binds to both protocols, it is must be disabled
                // if both protocols used at the same time (used in CI)
                int opt = 1;
                setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
                setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, &opt, sizeof(opt));
            }
        #endif

        int err = bind(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (err < 0) {
            ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
        }
        ESP_LOGI(TAG, "Socket bound, port %d", port);

        while (1) {
            ESP_LOGI(TAG, "Waiting for activation procedure");
            struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
            socklen_t socklen = sizeof(source_addr);
            int len = recvfrom(sock, rx_buffer, sizeof(rx_buffer) - 1, 0, (struct sockaddr *)&source_addr, &socklen);

            // Error occurred during receiving
            if (len < 0) {
                ESP_LOGE(TAG, "recvfrom failed: errno %d", errno);
                break;
            }
            // Data received
            else {
                // Get the sender's ip address as string
                if (source_addr.ss_family == PF_INET) {
                    inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr, addr_str, sizeof(addr_str) - 1);
                } else if (source_addr.ss_family == PF_INET6) {
                    inet6_ntoa_r(((struct sockaddr_in6 *)&source_addr)->sin6_addr, addr_str, sizeof(addr_str) - 1);
                }

                rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string...
                ESP_LOGI(TAG, "Received %d bytes from %s:", len, addr_str);
                ESP_LOGI(TAG, "%s", rx_buffer);
                
                if (valid_message(rx_buffer)) {
                    ESP_LOGI(TAG, "Starting csi procedure");
                    counter = 0;
                    start_csi_exchange();
                    
                } else {
                    ESP_LOGI(TAG, "Command unknown");
                }
                if (err < 0) {
                    ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
                    break;
                }
            }
        }

        if (sock != -1) {
            ESP_LOGE(TAG, "Shutting down socket listener and restarting...");
            shutdown(sock, 0);
            close(sock);
        }
    }
    vTaskDelete(NULL);
}


static void wifi_radar_cb(const wifi_radar_info_t *info, void *ctx)
{
    static int s_count = 0;
    wifi_radar_config_t radar_config = {0};
    static float s_amplitude_std_list[CONFIG_CSI_BUF_SIZE];
    bool trigger_relative_flag = false;

    esp_wifi_radar_get_config(&radar_config);

    float amplitude_std  = avg(info->amplitude_std, radar_config.filter_len / 128);
    float amplitude_corr = avg(info->amplitude_corr, radar_config.filter_len / 128);
    float amplitude_std_max = 0;
    float amplitude_std_avg = 0;
    s_amplitude_std_list[s_count % CONFIG_CSI_BUF_SIZE] = amplitude_std;

    s_count++;

    if (s_count > CONFIG_CSI_BUF_SIZE) {
        amplitude_std_max = max(s_amplitude_std_list, CONFIG_CSI_BUF_SIZE, 0.10);
        amplitude_std_avg = trimmean(s_amplitude_std_list, CONFIG_CSI_BUF_SIZE, 0.10);

        for (int i = 1, count = 0; i < 6; ++i) {
            if (s_amplitude_std_list[(s_count - i) % CONFIG_CSI_BUF_SIZE] > amplitude_std_avg * g_move_relative_threshold
                    || s_amplitude_std_list[(s_count - i) % CONFIG_CSI_BUF_SIZE] > amplitude_std_max) {
                if (++count > 2) {
                    trigger_relative_flag = true;
                    break;
                }
            }
        }
    }

    static uint32_t s_last_move_time = 0;

    if (amplitude_std > g_move_absolute_threshold || trigger_relative_flag) {
        gpio_set_level(CONFIG_GPIO_LED_MOVE_STATUS, 1);
        s_last_move_time = xTaskGetTickCount() * (1000 / configTICK_RATE_HZ);;
        ESP_LOGW(TAG, "Someone is moving");
    }

    if (s_last_move_time && xTaskGetTickCount() * (1000 / configTICK_RATE_HZ) - s_last_move_time > 3 * 1000){
        s_last_move_time  = 0;
        gpio_set_level(CONFIG_GPIO_LED_MOVE_STATUS, 0);
    }
    ESP_LOGI(TAG, "<%d> time: %u ms, rssi: %d, corr: %.3f, std: %.3f, std_avg: %.3f, std_max: %.3f, threshold: %.3f/%.3f, trigger: %d/%d, free_heap: %u/%u",
             s_count, info->time_end - info->time_start, info->rssi_avg,
             amplitude_corr, amplitude_std, amplitude_std_avg, amplitude_std_max,
             g_move_absolute_threshold, g_move_relative_threshold,
             amplitude_std > g_move_absolute_threshold, trigger_relative_flag,
             esp_get_minimum_free_heap_size(), esp_get_free_heap_size());

    counter++;
    //send csi data to external device (pc, cellphone, etc.)

    char *payload[100];
    strcpy(payload, ";");
    char *data[10];

    sprintf(data, "%f;", amplitude_std);
    strcat(payload, data);
    sprintf(data, "%d;", info->rssi_avg);
    strcat(payload, data);
    sprintf(data, "%f;", amplitude_corr);
    strcat(payload, data);
    sprintf(data, "%f;", amplitude_std_avg);
    strcat(payload, data);
    sprintf(data, "%f", amplitude_std_max);
    strcat(payload, data);

    send_udp(payload, ip_address, port);

    if(counter == 100) {
       esp_console_run("reset", cmd_ret);
    }
}

void app_main(void)
{
    wifi_radar_config_t radar_config = {
        .wifi_radar_cb = wifi_radar_cb,
        // .filter_len = 384,
        // .filter_mac = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
        // .wifi_csi_raw_cb = wifi_csi_raw_cb,
    };

    /**
     * @brief Used to show whether someone is moving
     */
    gpio_reset_pin(CONFIG_GPIO_LED_MOVE_STATUS);
    gpio_set_direction(CONFIG_GPIO_LED_MOVE_STATUS, GPIO_MODE_OUTPUT);
    gpio_set_level(CONFIG_GPIO_LED_MOVE_STATUS, 0);

    /**
     * @brief Initialize Wi-Fi radar
     */
    wifi_init();
    esp_wifi_radar_init();
    esp_wifi_radar_set_config(&radar_config);
    esp_wifi_radar_start();

    /**
     * @brief Register serial command
     */
    esp_console_repl_t *repl = NULL;
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    esp_console_dev_uart_config_t uart_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    repl_config.prompt = "csi>";
    ESP_ERROR_CHECK(esp_console_new_repl_uart(&uart_config, &repl_config, &repl));

    cmd_register_system();
    cmd_register_wifi();
    cmd_register_ping();
    cmd_register_csi();
    cmd_register_detect();

    printf("\n");
    printf(" =======================================================================\n");
    printf(" |                    Steps to test CSI                                |\n");
    printf(" |                                                                     |\n");
    printf(" |  1. Print 'help' to gain overview of commands                       |\n");
    printf(" |     "LOG_COLOR_I"csi>"LOG_RESET_COLOR" help                                                       |\n");
    printf(" |  2. Start SoftAP or Sta on another ESP32/ESP32S2/ESP32C3            |\n");
    printf(" |     "LOG_COLOR_I"csi>"LOG_RESET_COLOR" sta <ssid> <pwd>                                           |\n");
    printf(" |     "LOG_COLOR_I"csi>"LOG_RESET_COLOR" ap <ssid> <pwd>                                            |\n");
    printf(" |  3. Run ping to test                                                |\n");
    printf(" |     "LOG_COLOR_I"csi>"LOG_RESET_COLOR" ping <ip> -c <count> -i <interval>                         |\n");
    printf(" |  4. Configure CSI parameters                                        |\n");
    printf(" |     "LOG_COLOR_I"csi>"LOG_RESET_COLOR" csi -m <mac(xx:xx:xx:xx:xx:xx)> -l <len>                   |\n");
    printf(" |  5. Configure CSI parameters                                        |\n");
    printf(" |     "LOG_COLOR_I"csi>"LOG_RESET_COLOR" detect -a <absolute_threshold> -r <relative_threshold>     |\n");
    printf(" |                                                                     |\n");
    printf(" ======================================================================\n\n");

    ESP_ERROR_CHECK(esp_console_start_repl(repl));

    esp_console_run("sta csi_softap 123456789", cmd_ret);

    wait_csi_enabling();
}
