目录说明:
    config_sample: 存放wpa_supplicant、hostapd应用的配置文件示例
    document: 存放RTL8821CU相关的文档
    lib: 存放依赖的库
    rtl8821CU_WiFi_linux_v5.8.1.1_34311.20190729_COEX20190509-4141: RTL8821CU驱动源码
    wpa_supplicant_8_kk_4.4_rtw_r25669.20171213: wpa_supplicant、 hostapd应用源码

编译说明:
    直接使用当前目录的Makefile编译时, 需要根据自己本机的环境配置Makefile中的PUB_ROOTFS、
    OSDRV_DIR两个环境变量。最后make, 就能编译并安装相关的ko、库、以及可执行程序。

