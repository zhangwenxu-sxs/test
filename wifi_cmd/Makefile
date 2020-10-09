#wpa_supplicant hostapd依赖netlink库
export LIBNL := $(shell pwd)/lib/libnl
export CONFIG_LIBNL20 := y
export CONFIG_LIBNL32 := y

export CFLAGS += -I$(LIBNL)/include/libnl3
export LIBS += -L$(LIBNL)/lib

#指定wpa_supplicant源码目录
WPA_SUPPLICANT_SRC := $(shell pwd)/wpa_supplicant_8_kk_4.4_rtw_r25669.20171213/wpa_supplicant
#指定hostapd源码目录
HOSTAPD_SRC := $(shell pwd)/wpa_supplicant_8_kk_4.4_rtw_r25669.20171213/hostapd

all:
	pushd $(HOSTAPD_SRC);make clean;make;popd;
	pushd $(WPA_SUPPLICANT_SRC);make clean;make;popd;

clean:
	pushd $(WPA_SUPPLICANT_SRC);make clean;popd;
	pushd $(HOSTAPD_SRC);make clean;popd;

