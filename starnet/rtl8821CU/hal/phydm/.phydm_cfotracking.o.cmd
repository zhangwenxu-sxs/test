cmd_drivers/starnet/rtl8821CU/hal/phydm/phydm_cfotracking.o := aarch64-himix100-linux-gcc -Wp,-MD,drivers/starnet/rtl8821CU/hal/phydm/.phydm_cfotracking.o.d  -nostdinc -isystem /opt/hisi-linux/x86-arm/aarch64-himix100-linux/host_bin/../lib/gcc/aarch64-linux-gnu/6.3.0/include -I./arch/arm64/include -I./arch/arm64/include/generated/uapi -I./arch/arm64/include/generated  -I./include -I./arch/arm64/include/uapi -I./include/uapi -I./include/generated/uapi -include ./include/linux/kconfig.h -D__KERNEL__ -mlittle-endian -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -Wno-format-security -std=gnu89 -fno-PIE -mgeneral-regs-only -DCONFIG_AS_LSE=1 -fno-asynchronous-unwind-tables -mpc-relative-literal-loads -fno-delete-null-pointer-checks -Wno-frame-address -O2 --param=allow-store-data-races=0 -DCC_HAVE_ASM_GOTO -Wframe-larger-than=2048 -fstack-protector-strong -Wno-unused-but-set-variable -Wno-unused-const-variable -fno-omit-frame-pointer -fno-optimize-sibling-calls -fno-var-tracking-assignments -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fconserve-stack -Werror=implicit-int -Werror=strict-prototypes -Werror=date-time -Werror=incompatible-pointer-types -O1 -Wno-unused-variable -Wno-date-time -Idrivers/starnet/rtl8821CU/include -Idrivers/starnet/rtl8821CU/platform -Idrivers/starnet/rtl8821CU/hal/btc -DCONFIG_MP_INCLUDED -DCONFIG_BT_COEXIST -DCONFIG_EFUSE_CONFIG_FILE -DEFUSE_MAP_PATH=\"/system/etc/wifi/wifi_efuse_8821cu.map\" -DWIFIMAC_PATH=\"/data/wifimac.txt\" -DCONFIG_LOAD_PHY_PARA_FROM_FILE -DREALTEK_CONFIG_PATH=\"/lib/firmware/\" -DCONFIG_TXPWR_BY_RATE=1 -DCONFIG_TXPWR_BY_RATE_EN=1 -DCONFIG_TXPWR_LIMIT=1 -DCONFIG_TXPWR_LIMIT_EN=0 -DCONFIG_RTW_ADAPTIVITY_EN=0 -DCONFIG_RTW_ADAPTIVITY_MODE=0 -DCONFIG_IEEE80211W -DHIGH_ACTIVE_HST2DEV=0 -DCONFIG_BR_EXT '-DCONFIG_BR_EXT_BRNAME="'br0'"' -DCONFIG_RTW_NAPI -DCONFIG_RTW_GRO -DCONFIG_RTW_NETIF_SG -DCONFIG_RTW_DEBUG -DRTW_LOG_LEVEL=4 -DCONFIG_PROC_DEBUG -DCONFIG_RTW_UP_MAPPING_RULE=0 -DDM_ODM_SUPPORT_TYPE=0x04 -DCONFIG_PLATFORM_HISILICON -DCONFIG_PLATFORM_HISILICON_HI3559 -DCONFIG_LITTLE_ENDIAN -DCONFIG_CONCURRENT_MODE -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT -Idrivers/starnet/rtl8821CU/hal/phydm -DCONFIG_RTL8821C    -DKBUILD_BASENAME='"phydm_cfotracking"'  -DKBUILD_MODNAME='"8821cu"' -c -o drivers/starnet/rtl8821CU/hal/phydm/phydm_cfotracking.o drivers/starnet/rtl8821CU/hal/phydm/phydm_cfotracking.c

source_drivers/starnet/rtl8821CU/hal/phydm/phydm_cfotracking.o := drivers/starnet/rtl8821CU/hal/phydm/phydm_cfotracking.c

deps_drivers/starnet/rtl8821CU/hal/phydm/phydm_cfotracking.o := \
    $(wildcard include/config/bt/coexist.h) \
  drivers/starnet/rtl8821CU/hal/phydm/mp_precomp.h \
  drivers/starnet/rtl8821CU/hal/phydm/phydm_precomp.h \
    $(wildcard include/config/rtl/triband/support.h) \
    $(wildcard include/config/usb/hci.h) \
    $(wildcard include/config/sfw/supported.h) \
  drivers/starnet/rtl8821CU/hal/phydm/phydm_types.h \
    $(wildcard include/config/pci/hci.h) \
    $(wildcard include/config/sdio/hci.h) \
    $(wildcard include/config/gspi/hci.h) \
    $(wildcard include/config/little/endian.h) \
    $(wildcard include/config/big/endian.h) \
  drivers/starnet/rtl8821CU/include/drv_types.h \
    $(wildcard include/config/arp/keep/alive.h) \
    $(wildcard include/config/80211n/ht.h) \
    $(wildcard include/config/80211ac/vht.h) \
    $(wildcard include/config/beamforming.h) \
    $(wildcard include/config/rtw/mesh.h) \
    $(wildcard include/config/prealloc/rx/skb/buffer.h) \
    $(wildcard include/config/tdls.h) \
    $(wildcard include/config/wapi/support.h) \
    $(wildcard include/config/mp/included.h) \
    $(wildcard include/config/br/ext.h) \
    $(wildcard include/config/iol.h) \
    $(wildcard include/config/mcc/mode.h) \
    $(wildcard include/config/rtw/repeater/son.h) \
    $(wildcard include/config/1t1r.h) \
    $(wildcard include/config/2t2r.h) \
    $(wildcard include/config/lps/1t1r.h) \
    $(wildcard include/config/wowlan.h) \
    $(wildcard include/config/wmmps/sta.h) \
    $(wildcard include/config/rtw/customer/str.h) \
    $(wildcard include/config/tx/early/mode.h) \
    $(wildcard include/config/narrowband/supporting.h) \
    $(wildcard include/config/tx/ac/lifetime.h) \
    $(wildcard include/config/ap/mode.h) \
    $(wildcard include/config/rtw/tx/npath/en.h) \
    $(wildcard include/config/rtw/path/div.h) \
    $(wildcard include/config/adaptor/info/caching/file.h) \
    $(wildcard include/config/layer2/roaming.h) \
    $(wildcard include/config/80211d.h) \
    $(wildcard include/config/txpwr/limit.h) \
    $(wildcard include/config/ieee80211/band/5ghz.h) \
    $(wildcard include/config/load/phy/para/from/file.h) \
    $(wildcard include/config/concurrent/mode.h) \
    $(wildcard include/config/p2p.h) \
    $(wildcard include/config/rtw/acs.h) \
    $(wildcard include/config/background/noise/monitor.h) \
    $(wildcard include/config/dfs/master.h) \
    $(wildcard include/config/rtw/napi.h) \
    $(wildcard include/config/rtw/napi/dynamic.h) \
    $(wildcard include/config/rtw/gro.h) \
    $(wildcard include/config/support/trx/shared.h) \
    $(wildcard include/config/advance/ota.h) \
    $(wildcard include/config/fw/offload/param/init.h) \
    $(wildcard include/config/dynamic/soml.h) \
    $(wildcard include/config/fw/handle/txbcn.h) \
    $(wildcard include/config/tdmadig.h) \
    $(wildcard include/config/dbg/counter.h) \
    $(wildcard include/config/client/port/cfg.h) \
    $(wildcard include/config/iface/number.h) \
    $(wildcard include/config/protsel/macsleep.h) \
    $(wildcard include/config/dfs.h) \
    $(wildcard include/config/dfs/slave/with/radar/detect.h) \
    $(wildcard include/config/mbssid/cam.h) \
    $(wildcard include/config/fw/multi/port/support.h) \
    $(wildcard include/config/hw/p0/tsf/sync.h) \
    $(wildcard include/config/rtl8814b.h) \
    $(wildcard include/config/sdio/indirect/access.h) \
    $(wildcard include/config/syson/indirect/access.h) \
    $(wildcard include/config/support/multi/bcn.h) \
    $(wildcard include/config/ioctl/cfg80211.h) \
    $(wildcard include/config/swtimer/based/txbcn.h) \
    $(wildcard include/config/rtw/wifi/hal.h) \
    $(wildcard include/config/usb/vendor/req/mutex.h) \
    $(wildcard include/config/usb/vendor/req/buffer/prealloc.h) \
    $(wildcard include/config/rtw/tpt/mode.h) \
    $(wildcard include/config/protsel/port.h) \
    $(wildcard include/config/protsel/atimdtim.h) \
    $(wildcard include/config/mac/loopback/driver.h) \
    $(wildcard include/config/rtw/80211k.h) \
    $(wildcard include/config/ieee80211w.h) \
    $(wildcard include/config/wfd.h) \
    $(wildcard include/config/bt/coexist/socket/trx.h) \
    $(wildcard include/config/gpio/api.h) \
    $(wildcard include/config/event/thread/mode.h) \
    $(wildcard include/config/xmit/thread/mode.h) \
    $(wildcard include/config/recv/thread/mode.h) \
    $(wildcard include/config/autosuspend.h) \
    $(wildcard include/config/support/fifo/dump.h) \
    $(wildcard include/config/tx/amsdu.h) \
    $(wildcard include/config/rtw/cfgvendor/random/mac/oui.h) \
    $(wildcard include/config/pno/support.h) \
    $(wildcard include/config/pno/set/debug.h) \
  drivers/starnet/rtl8821CU/include/drv_conf.h \
    $(wildcard include/config/rssi/priority.h) \
    $(wildcard include/config/ap.h) \
    $(wildcard include/config/rtw/repeater/son/id.h) \
    $(wildcard include/config/rtw/repeater/son/root.h) \
    $(wildcard include/config/layer2/roaming/active.h) \
    $(wildcard include/config/power/saving.h) \
    $(wildcard include/config/rtw/80211r.h) \
    $(wildcard include/config/android.h) \
    $(wildcard include/config/platform/android.h) \
    $(wildcard include/config/validate/ssid.h) \
    $(wildcard include/config/signal/display/dbm.h) \
    $(wildcard include/config/has/earlysuspend.h) \
    $(wildcard include/config/resume/in/workqueue.h) \
    $(wildcard include/config/android/power.h) \
    $(wildcard include/config/wakelock.h) \
    $(wildcard include/config/vendor/req/retry.h) \
    $(wildcard include/config/wifi/monitor.h) \
    $(wildcard include/config/monitor/mode/xmit.h) \
    $(wildcard include/config/customer/alibaba/general.h) \
    $(wildcard include/config/customer01/smart/antenna.h) \
    $(wildcard include/config/tx/mcast2uni.h) \
    $(wildcard include/config/rtw/mesh/acnode/prevent.h) \
    $(wildcard include/config/rtw/mesh/offch/cand.h) \
    $(wildcard include/config/rtw/mesh/peer/blacklist.h) \
    $(wildcard include/config/rtw/mesh/cto/mgate/blacklist.h) \
    $(wildcard include/config/rtw/mesh/cto/mgate/carrier.h) \
    $(wildcard include/config/rtw/mpm/tx/ies/sync/bss.h) \
    $(wildcard include/config/rtw/mesh/aek.h) \
    $(wildcard include/config/rtw/mesh/data/bmc/to/uc.h) \
    $(wildcard include/config/scan/backop.h) \
    $(wildcard include/config/tx/aclt/flags.h) \
    $(wildcard include/config/tx/aclt/conf/default.h) \
    $(wildcard include/config/tx/aclt/conf/ap/m2u.h) \
    $(wildcard include/config/tx/aclt/conf/mesh.h) \
    $(wildcard include/config/rtw/hiq/filter.h) \
    $(wildcard include/config/rtw/adaptivity/en.h) \
    $(wildcard include/config/rtw/adaptivity/mode.h) \
    $(wildcard include/config/rtw/adaptivity/th/l2h/ini.h) \
    $(wildcard include/config/rtw/adaptivity/th/edcca/hl/diff.h) \
    $(wildcard include/config/rtw/excl/chs.h) \
    $(wildcard include/config/rtw/dfs/region/domain.h) \
    $(wildcard include/config/txpwr/by/rate/en.h) \
    $(wildcard include/config/txpwr/limit/en.h) \
    $(wildcard include/config/rtw/chplan.h) \
    $(wildcard include/config/calibrate/tx/power/by/regulatory.h) \
    $(wildcard include/config/calibrate/tx/power/to/max.h) \
    $(wildcard include/config/rtw/ipcam/application.h) \
    $(wildcard include/config/rtw/customize/beedca.h) \
    $(wildcard include/config/rtw/customize/bwmode.h) \
    $(wildcard include/config/rtw/customize/rlsta.h) \
    $(wildcard include/config/rtl8192e.h) \
    $(wildcard include/config/rtl8192f.h) \
    $(wildcard include/config/rtl8822b.h) \
    $(wildcard include/config/extend/lowrate/txop.h) \
    $(wildcard include/config/rtw/rx/ampdu/sz/limit/1ss.h) \
    $(wildcard include/config/rtw/rx/ampdu/sz/limit/2ss.h) \
    $(wildcard include/config/rtw/rx/ampdu/sz/limit/3ss.h) \
    $(wildcard include/config/rtw/rx/ampdu/sz/limit/4ss.h) \
    $(wildcard include/config/rtw/target/tx/pwr/2g/a.h) \
    $(wildcard include/config/rtw/target/tx/pwr/2g/b.h) \
    $(wildcard include/config/rtw/target/tx/pwr/2g/c.h) \
    $(wildcard include/config/rtw/target/tx/pwr/2g/d.h) \
    $(wildcard include/config/rtw/target/tx/pwr/5g/a.h) \
    $(wildcard include/config/rtw/target/tx/pwr/5g/b.h) \
    $(wildcard include/config/rtw/target/tx/pwr/5g/c.h) \
    $(wildcard include/config/rtw/target/tx/pwr/5g/d.h) \
    $(wildcard include/config/rtw/amplifier/type/2g.h) \
    $(wildcard include/config/rtw/amplifier/type/5g.h) \
    $(wildcard include/config/rtw/rfe/type.h) \
    $(wildcard include/config/rtw/glna/type.h) \
    $(wildcard include/config/rtw/pll/ref/clk/sel.h) \
    $(wildcard include/config/rtl8188e.h) \
    $(wildcard include/config/rtl8188f.h) \
    $(wildcard include/config/rtl8188gtv.h) \
    $(wildcard include/config/rtl8812a.h) \
    $(wildcard include/config/rtl8821a.h) \
    $(wildcard include/config/rtl8710b.h) \
    $(wildcard include/config/rtl8723b.h) \
    $(wildcard include/config/rtl8703b.h) \
    $(wildcard include/config/rtl8723d.h) \
    $(wildcard include/config/hwmpcap/gen1.h) \
    $(wildcard include/config/rtl8821c.h) \
    $(wildcard include/config/rtl8822c.h) \
    $(wildcard include/config/rtl8814a.h) \
    $(wildcard include/config/hwmpcap/gen2.h) \
    $(wildcard include/config/hwmpcap/gen3.h) \
    $(wildcard include/config/mi/with/mbssid/cam.h) \
    $(wildcard include/config/runtime/port/switch.h) \
    $(wildcard include/config/limited/ap/num.h) \
    $(wildcard include/config/new/netdev/hdl.h) \
    $(wildcard include/config/wow/pattern/hw/cam.h) \
    $(wildcard include/config/tsf/update/pause/factor.h) \
    $(wildcard include/config/tsf/update/restore/factor.h) \
    $(wildcard include/config/deauth/before/connect.h) \
    $(wildcard include/config/wext/dont/join/byssid.h) \
    $(wildcard include/config/doscan/in/busytraffic.h) \
    $(wildcard include/config/phdym/fw/fixrate.h) \
    $(wildcard include/config/rtw/sdio/keep/irq.h) \
    $(wildcard include/config/usb/rx/aggregation.h) \
    $(wildcard include/config/rtw/hostapd/acs.h) \
    $(wildcard include/config/find/best/channel.h) \
    $(wildcard include/config/error/reset.h) \
    $(wildcard include/config/ips.h) \
    $(wildcard include/config/ips/mode.h) \
    $(wildcard include/config/lps/pg.h) \
    $(wildcard include/config/lps/pg/ddma.h) \
    $(wildcard include/config/lps/lclk.h) \
    $(wildcard include/config/lps.h) \
    $(wildcard include/config/lps/mode.h) \
    $(wildcard include/config/pci/bcn/polling.h) \
    $(wildcard include/config/bcn/icf.h) \
    $(wildcard include/config/pci/msi.h) \
    $(wildcard include/config/rtw/pci/msi/disable.h) \
    $(wildcard include/config/pci/dynamic/aspm/l1/latency.h) \
    $(wildcard include/config/pci/dynamic/aspm/link/ctrl.h) \
    $(wildcard include/config/pci/dynamic/aspm.h) \
  drivers/starnet/rtl8821CU/include/autoconf.h \
    $(wildcard include/config/disable/odm.h) \
    $(wildcard include/config/no/fw.h) \
    $(wildcard include/config/cfg80211/force/compatible/2/6/37/under.h) \
    $(wildcard include/config/debug/cfg80211.h) \
    $(wildcard include/config/drv/issue/prov/req.h) \
    $(wildcard include/config/set/scan/deny/timer.h) \
    $(wildcard include/config/h2clbk.h) \
    $(wildcard include/config/embedded/fwimg.h) \
    $(wildcard include/config/file/fwimg.h) \
    $(wildcard include/config/xmit/ack.h) \
    $(wildcard include/config/active/keep/alive/check.h) \
    $(wildcard include/config/recv/reordering/ctrl.h) \
    $(wildcard include/config/support/usb/int.h) \
    $(wildcard include/config/usb/interrupt/in/pipe.h) \
    $(wildcard include/config/ips/level/2.h) \
    $(wildcard include/config/ips/check/in/wd.h) \
    $(wildcard include/config/detect/cpwm/by/polling.h) \
    $(wildcard include/config/antenna/diversity.h) \
    $(wildcard include/config/hw/antenna/diversity.h) \
    $(wildcard include/config/interrupt/based/txbcn.h) \
    $(wildcard include/config/interrupt/based/txbcn/early/int.h) \
    $(wildcard include/config/interrupt/based/txbcn/bcn/ok/err.h) \
    $(wildcard include/config/nativeap/mlme.h) \
    $(wildcard include/config/hostapd/mlme.h) \
    $(wildcard include/config/p2p/remove/group/info.h) \
    $(wildcard include/config/dbg/p2p.h) \
    $(wildcard include/config/p2p/ps.h) \
    $(wildcard include/config/p2p/ips.h) \
    $(wildcard include/config/p2p/op/chk/social/ch.h) \
    $(wildcard include/config/cfg80211/onechannel/under/concurrent.h) \
    $(wildcard include/config/p2p/chk/invite/ch/list.h) \
    $(wildcard include/config/p2p/invite/iot.h) \
    $(wildcard include/config/tdls/driver/setup.h) \
    $(wildcard include/config/tdls/autosetup.h) \
    $(wildcard include/config/tdls/autocheckalive.h) \
    $(wildcard include/config/tdls/ch/sw.h) \
    $(wildcard include/config/skb/copy.h) \
    $(wildcard include/config/rtw/led.h) \
    $(wildcard include/config/rtw/sw/led.h) \
    $(wildcard include/config/rtw/led/handled/by/cmd/thread.h) \
    $(wildcard include/config/global/ui/pid.h) \
    $(wildcard include/config/layer2/roaming/resume.h) \
    $(wildcard include/config/long/delay/issue.h) \
    $(wildcard include/config/new/signal/stat/process.h) \
    $(wildcard include/config/minimal/memory/usage.h) \
    $(wildcard include/config/usb/tx/aggregation.h) \
    $(wildcard include/config/reduce/usb/tx/int.h) \
    $(wildcard include/config/easy/replacement.h) \
    $(wildcard include/config/use/usb/buffer/alloc/xx.h) \
    $(wildcard include/config/use/usb/buffer/alloc/tx.h) \
    $(wildcard include/config/use/usb/buffer/alloc/rx.h) \
    $(wildcard include/config/prealloc/recv/skb.h) \
    $(wildcard include/config/fix/nr/bulkin/buffer.h) \
    $(wildcard include/config/usb/vendor/req/buffer/dynamic/allocate.h) \
    $(wildcard include/config/usb/support/async/vdn/req.h) \
    $(wildcard include/config/gtk/ol.h) \
    $(wildcard include/config/gpio/wakeup.h) \
    $(wildcard include/config/rx/packet/append/fcs.h) \
    $(wildcard include/config/adhoc/workaround/setting.h) \
    $(wildcard include/config/mp/iwpriv/support.h) \
    $(wildcard include/config/platform/actions/atm702x.h) \
    $(wildcard include/config/error/detect.h) \
    $(wildcard include/config/error/detect/int.h) \
  drivers/starnet/rtl8821CU/include/hal_ic_cfg.h \
    $(wildcard include/config/multidrv.h) \
    $(wildcard include/config/txpwr/pg/with/pwr/idx.h) \
    $(wildcard include/config/fw/c2h/pkt.h) \
    $(wildcard include/config/beamformer/fw/ndpa.h) \
    $(wildcard include/config/rts/full/bw.h) \
    $(wildcard include/config/rtw/mac/hidden/rpt.h) \
    $(wildcard include/config/ampdu/pretx/cd.h) \
    $(wildcard include/config/p2p/ps/noa/use/macid/sleep.h) \
    $(wildcard include/config/nb/value.h) \
    $(wildcard include/config/none.h) \
    $(wildcard include/config/width/10.h) \
    $(wildcard include/config/width/5.h) \
    $(wildcard include/config/wow/pattern/in/txfifo.h) \
    $(wildcard include/config/fw/correct/bcn.h) \
    $(wildcard include/config/ap/port/swap.h) \
    $(wildcard include/config/tcp/csum/offload/tx.h) \
    $(wildcard include/config/rtw/netif/sg.h) \
    $(wildcard include/config/tcp/csum/offload/rx.h) \
    $(wildcard include/config/mcc/mode/v2.h) \
    $(wildcard include/config/mcc/phydm/offload.h) \
    $(wildcard include/config/tdls/ch/sw/v2.h) \
    $(wildcard include/config/bcn/recv/time.h) \
    $(wildcard include/config/lps/ack.h) \
    $(wildcard include/config/bt/efuse/mask.h) \
    $(wildcard include/config/txpwr/pg/with/tssi/offset.h) \
    $(wildcard include/config/phy/capability/query.h) \
    $(wildcard include/config/support/ap/port1.h) \
    $(wildcard include/config/has/hw/var/bcn/ctrl/addr.h) \
    $(wildcard include/config/has/hw/var/bcn/func.h) \
    $(wildcard include/config/has/hw/var/mlme/disconnect.h) \
    $(wildcard include/config/has/hw/var/mlme/join.h) \
    $(wildcard include/config/has/hw/var/correct/tsf.h) \
    $(wildcard include/config/has/tx/beacon/pause.h) \
  drivers/starnet/rtl8821CU/include/basic_types.h \
  include/generated/uapi/linux/version.h \
  include/linux/types.h \
    $(wildcard include/config/have/uid16.h) \
    $(wildcard include/config/uid16.h) \
    $(wildcard include/config/lbdaf.h) \
    $(wildcard include/config/arch/dma/addr/t/64bit.h) \
    $(wildcard include/config/phys/addr/t/64bit.h) \
    $(wildcard include/config/64bit.h) \
  include/uapi/linux/types.h \
  arch/arm64/include/generated/asm/types.h \
  include/uapi/asm-generic/types.h \
  include/asm-generic/int-ll64.h \
  include/uapi/asm-generic/int-ll64.h \
  arch/arm64/include/uapi/asm/bitsperlong.h \
  include/asm-generic/bitsperlong.h \
  include/uapi/asm-generic/bitsperlong.h \
  include/uapi/linux/posix_types.h \
  include/linux/stddef.h \
  include/uapi/linux/stddef.h \
  include/linux/compiler.h \
    $(wildcard include/config/sparse/rcu/pointer.h) \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/kasan.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
    $(wildcard include/config/kprobes.h) \
  include/linux/compiler-gcc.h \
    $(wildcard include/config/arch/supports/optimized/inlining.h) \
    $(wildcard include/config/optimize/inlining.h) \
    $(wildcard include/config/gcov/kernel.h) \
    $(wildcard include/config/arch/use/builtin/bswap.h) \
  arch/arm64/include/uapi/asm/posix_types.h \
  include/uapi/asm-generic/posix_types.h \
  include/linux/module.h \
    $(wildcard include/config/modules.h) \
    $(wildcard include/config/sysfs.h) \
    $(wildcard include/config/modules/tree/lookup.h) \
    $(wildcard include/config/livepatch.h) \
    $(wildcard include/config/unused/symbols.h) \
    $(wildcard include/config/module/sig.h) \
    $(wildcard include/config/generic/bug.h) \
    $(wildcard include/config/kallsyms.h) \
    $(wildcard include/config/smp.h) \
    $(wildcard include/config/tracepoints.h) \
    $(wildcard include/config/tracing.h) \
    $(wildcard include/config/event/tracing.h) \
    $(wildcard include/config/ftrace/mcount/record.h) \
    $(wildcard include/config/module/unload.h) \
    $(wildcard include/config/constructors.h) \
    $(wildcard include/config/debug/set/module/ronx.h) \
  include/linux/list.h \
    $(wildcard include/config/debug/list.h) \
  include/linux/poison.h \
    $(wildcard include/config/illegal/pointer/value.h) \
    $(wildcard include/config/page/poisoning/zero.h) \
  include/uapi/linux/const.h \
  include/linux/kernel.h \
    $(wildcard include/config/preempt/voluntary.h) \
    $(wildcard include/config/debug/atomic/sleep.h) \
    $(wildcard include/config/mmu.h) \
    $(wildcard include/config/prove/locking.h) \
    $(wildcard include/config/panic/timeout.h) \
  /opt/hisi-linux/x86-arm/aarch64-himix100-linux/lib/gcc/aarch64-linux-gnu/6.3.0/include/stdarg.h \
  include/linux/linkage.h \
  include/linux/stringify.h \
  include/linux/export.h \
    $(wildcard include/config/have/underscore/symbol/prefix.h) \
    $(wildcard include/config/modversions.h) \
    $(wildcard include/config/trim/unused/ksyms.h) \
  arch/arm64/include/asm/linkage.h \
  include/linux/bitops.h \
  arch/arm64/include/asm/bitops.h \
  arch/arm64/include/asm/barrier.h \
  include/asm-generic/barrier.h \
  include/asm-generic/bitops/builtin-__ffs.h \
  include/asm-generic/bitops/builtin-ffs.h \
  include/asm-generic/bitops/builtin-__fls.h \
  include/asm-generic/bitops/builtin-fls.h \
  include/asm-generic/bitops/ffz.h \
  include/asm-generic/bitops/fls64.h \
  include/asm-generic/bitops/find.h \
    $(wildcard include/config/generic/find/first/bit.h) \
  include/asm-generic/bitops/sched.h \
  include/asm-generic/bitops/hweight.h \
  include/asm-generic/bitops/arch_hweight.h \
  include/asm-generic/bitops/const_hweight.h \
  include/asm-generic/bitops/lock.h \
  include/asm-generic/bitops/non-atomic.h \
  include/asm-generic/bitops/le.h \
  arch/arm64/include/uapi/asm/byteorder.h \
  include/linux/byteorder/little_endian.h \
  include/uapi/linux/byteorder/little_endian.h \
  include/linux/swab.h \
  include/uapi/linux/swab.h \
  arch/arm64/include/generated/asm/swab.h \
  include/uapi/asm-generic/swab.h \
  include/linux/byteorder/generic.h \
  include/linux/log2.h \
    $(wildcard include/config/arch/has/ilog2/u32.h) \
    $(wildcard include/config/arch/has/ilog2/u64.h) \
  include/linux/typecheck.h \
  include/linux/printk.h \
    $(wildcard include/config/message/loglevel/default.h) \
    $(wildcard include/config/early/printk.h) \
    $(wildcard include/config/printk/nmi.h) \
    $(wildcard include/config/printk.h) \
    $(wildcard include/config/dynamic/debug.h) \
  include/linux/init.h \
    $(wildcard include/config/debug/rodata.h) \
  include/linux/kern_levels.h \
  include/linux/cache.h \
    $(wildcard include/config/arch/has/cache/line/size.h) \
  include/uapi/linux/kernel.h \
  include/uapi/linux/sysinfo.h \
  arch/arm64/include/asm/cache.h \
  arch/arm64/include/asm/cachetype.h \
  arch/arm64/include/asm/cputype.h \
  arch/arm64/include/asm/sysreg.h \
    $(wildcard include/config/arm64/4k/pages.h) \
    $(wildcard include/config/arm64/16k/pages.h) \
    $(wildcard include/config/arm64/64k/pages.h) \
  arch/arm64/include/asm/opcodes.h \
    $(wildcard include/config/cpu/big/endian.h) \
    $(wildcard include/config/cpu/endian/be8.h) \
  arch/arm64/include/../../arm/include/asm/opcodes.h \
    $(wildcard include/config/cpu/endian/be32.h) \
    $(wildcard include/config/thumb2/kernel.h) \
  include/linux/stat.h \
  arch/arm64/include/asm/stat.h \
    $(wildcard include/config/compat.h) \
  arch/arm64/include/uapi/asm/stat.h \
  include/uapi/asm-generic/stat.h \
  arch/arm64/include/asm/compat.h \
  include/linux/sched.h \
    $(wildcard include/config/no/hz/common.h) \
    $(wildcard include/config/sched/debug.h) \
    $(wildcard include/config/prove/rcu.h) \
    $(wildcard include/config/hotplug/cpu.h) \
    $(wildcard include/config/lockup/detector.h) \
    $(wildcard include/config/detect/hung/task.h) \
    $(wildcard include/config/core/dump/default/elf/headers.h) \
    $(wildcard include/config/virt/cpu/accounting/native.h) \
    $(wildcard include/config/no/hz/full.h) \
    $(wildcard include/config/sched/autogroup.h) \
    $(wildcard include/config/bsd/process/acct.h) \
    $(wildcard include/config/taskstats.h) \
    $(wildcard include/config/audit.h) \
    $(wildcard include/config/inotify/user.h) \
    $(wildcard include/config/fanotify.h) \
    $(wildcard include/config/epoll.h) \
    $(wildcard include/config/posix/mqueue.h) \
    $(wildcard include/config/keys.h) \
    $(wildcard include/config/perf/events.h) \
    $(wildcard include/config/bpf/syscall.h) \
    $(wildcard include/config/sched/info.h) \
    $(wildcard include/config/task/delay/acct.h) \
    $(wildcard include/config/schedstats.h) \
    $(wildcard include/config/sched/smt.h) \
    $(wildcard include/config/sched/mc.h) \
    $(wildcard include/config/numa.h) \
    $(wildcard include/config/fair/group/sched.h) \
    $(wildcard include/config/rt/group/sched.h) \
    $(wildcard include/config/thread/info/in/task.h) \
    $(wildcard include/config/cgroup/sched.h) \
    $(wildcard include/config/preempt/notifiers.h) \
    $(wildcard include/config/blk/dev/io/trace.h) \
    $(wildcard include/config/preempt/rcu.h) \
    $(wildcard include/config/tasks/rcu.h) \
    $(wildcard include/config/memcg.h) \
    $(wildcard include/config/slob.h) \
    $(wildcard include/config/compat/brk.h) \
    $(wildcard include/config/cgroups.h) \
    $(wildcard include/config/cc/stackprotector.h) \
    $(wildcard include/config/virt/cpu/accounting/gen.h) \
    $(wildcard include/config/sysvipc.h) \
    $(wildcard include/config/auditsyscall.h) \
    $(wildcard include/config/rt/mutexes.h) \
    $(wildcard include/config/debug/mutexes.h) \
    $(wildcard include/config/trace/irqflags.h) \
    $(wildcard include/config/lockdep.h) \
    $(wildcard include/config/ubsan.h) \
    $(wildcard include/config/block.h) \
    $(wildcard include/config/task/xacct.h) \
    $(wildcard include/config/cpusets.h) \
    $(wildcard include/config/futex.h) \
    $(wildcard include/config/debug/preempt.h) \
    $(wildcard include/config/numa/balancing.h) \
    $(wildcard include/config/arch/want/batched/unmap/tlb/flush.h) \
    $(wildcard include/config/fault/injection.h) \
    $(wildcard include/config/latencytop.h) \
    $(wildcard include/config/function/graph/tracer.h) \
    $(wildcard include/config/kcov.h) \
    $(wildcard include/config/uprobes.h) \
    $(wildcard include/config/bcache.h) \
    $(wildcard include/config/vmap/stack.h) \
    $(wildcard include/config/arch/wants/dynamic/task/struct.h) \
    $(wildcard include/config/have/unstable/sched/clock.h) \
    $(wildcard include/config/irq/time/accounting.h) \
    $(wildcard include/config/proc/fs.h) \
    $(wildcard include/config/stack/growsup.h) \
    $(wildcard include/config/have/copy/thread/tls.h) \
    $(wildcard include/config/have/exit/thread.h) \
    $(wildcard include/config/debug/stack/usage.h) \
    $(wildcard include/config/preempt.h) \
    $(wildcard include/config/cpu/freq.h) \
  include/uapi/linux/sched.h \
  include/linux/sched/prio.h \
  arch/arm64/include/uapi/asm/param.h \
  include/asm-generic/param.h \
    $(wildcard include/config/hz.h) \
  include/uapi/asm-generic/param.h \
  include/linux/capability.h \
    $(wildcard include/config/multiuser.h) \
  include/uapi/linux/capability.h \
  include/linux/threads.h \
    $(wildcard include/config/nr/cpus.h) \
    $(wildcard include/config/base/small.h) \
  include/linux/timex.h \
  include/uapi/linux/timex.h \
  include/linux/time.h \
    $(wildcard include/config/arch/uses/gettimeoffset.h) \
  include/linux/seqlock.h \
    $(wildcard include/config/debug/lock/alloc.h) \
  include/linux/spinlock.h \
    $(wildcard include/config/debug/spinlock.h) \
    $(wildcard include/config/generic/lockbreak.h) \
  include/linux/preempt.h \
    $(wildcard include/config/preempt/count.h) \
    $(wildcard include/config/preempt/tracer.h) \
  arch/arm64/include/generated/asm/preempt.h \
  include/asm-generic/preempt.h \
  include/linux/thread_info.h \
    $(wildcard include/config/have/arch/within/stack/frames.h) \
    $(wildcard include/config/hardened/usercopy.h) \
  include/linux/bug.h \
  arch/arm64/include/asm/bug.h \
    $(wildcard include/config/debug/bugverbose.h) \
  arch/arm64/include/asm/brk-imm.h \
  include/asm-generic/bug.h \
    $(wildcard include/config/bug.h) \
    $(wildcard include/config/generic/bug/relative/pointers.h) \
  arch/arm64/include/asm/thread_info.h \
    $(wildcard include/config/arm64/sw/ttbr0/pan.h) \
  include/linux/irqflags.h \
    $(wildcard include/config/irqsoff/tracer.h) \
    $(wildcard include/config/trace/irqflags/support.h) \
  arch/arm64/include/asm/irqflags.h \
  arch/arm64/include/asm/ptrace.h \
  arch/arm64/include/uapi/asm/ptrace.h \
  arch/arm64/include/asm/hwcap.h \
  arch/arm64/include/uapi/asm/hwcap.h \
  include/asm-generic/ptrace.h \
  include/linux/bottom_half.h \
  include/linux/spinlock_types.h \
  arch/arm64/include/asm/spinlock_types.h \
  include/linux/lockdep.h \
    $(wildcard include/config/lock/stat.h) \
  include/linux/rwlock_types.h \
  arch/arm64/include/asm/spinlock.h \
  arch/arm64/include/asm/lse.h \
    $(wildcard include/config/as/lse.h) \
    $(wildcard include/config/arm64/lse/atomics.h) \
  arch/arm64/include/asm/processor.h \
  include/linux/string.h \
    $(wildcard include/config/binary/printf.h) \
  include/uapi/linux/string.h \
  arch/arm64/include/asm/string.h \
  arch/arm64/include/asm/alternative.h \
    $(wildcard include/config/arm64/uao.h) \
    $(wildcard include/config/foo.h) \
  arch/arm64/include/asm/cpucaps.h \
  arch/arm64/include/asm/insn.h \
  arch/arm64/include/asm/fpsimd.h \
  arch/arm64/include/asm/hw_breakpoint.h \
    $(wildcard include/config/have/hw/breakpoint.h) \
  arch/arm64/include/asm/cpufeature.h \
  include/linux/jump_label.h \
    $(wildcard include/config/jump/label.h) \
  include/linux/atomic.h \
    $(wildcard include/config/generic/atomic64.h) \
  arch/arm64/include/asm/atomic.h \
  arch/arm64/include/asm/atomic_ll_sc.h \
  arch/arm64/include/asm/cmpxchg.h \
  include/asm-generic/atomic-long.h \
  arch/arm64/include/asm/virt.h \
    $(wildcard include/config/arm64/vhe.h) \
  arch/arm64/include/asm/sections.h \
  include/asm-generic/sections.h \
  arch/arm64/include/asm/pgtable-hwdef.h \
    $(wildcard include/config/pgtable/levels.h) \
  include/linux/rwlock.h \
  include/linux/spinlock_api_smp.h \
    $(wildcard include/config/inline/spin/lock.h) \
    $(wildcard include/config/inline/spin/lock/bh.h) \
    $(wildcard include/config/inline/spin/lock/irq.h) \
    $(wildcard include/config/inline/spin/lock/irqsave.h) \
    $(wildcard include/config/inline/spin/trylock.h) \
    $(wildcard include/config/inline/spin/trylock/bh.h) \
    $(wildcard include/config/uninline/spin/unlock.h) \
    $(wildcard include/config/inline/spin/unlock/bh.h) \
    $(wildcard include/config/inline/spin/unlock/irq.h) \
    $(wildcard include/config/inline/spin/unlock/irqrestore.h) \
  include/linux/rwlock_api_smp.h \
    $(wildcard include/config/inline/read/lock.h) \
    $(wildcard include/config/inline/write/lock.h) \
    $(wildcard include/config/inline/read/lock/bh.h) \
    $(wildcard include/config/inline/write/lock/bh.h) \
    $(wildcard include/config/inline/read/lock/irq.h) \
    $(wildcard include/config/inline/write/lock/irq.h) \
    $(wildcard include/config/inline/read/lock/irqsave.h) \
    $(wildcard include/config/inline/write/lock/irqsave.h) \
    $(wildcard include/config/inline/read/trylock.h) \
    $(wildcard include/config/inline/write/trylock.h) \
    $(wildcard include/config/inline/read/unlock.h) \
    $(wildcard include/config/inline/write/unlock.h) \
    $(wildcard include/config/inline/read/unlock/bh.h) \
    $(wildcard include/config/inline/write/unlock/bh.h) \
    $(wildcard include/config/inline/read/unlock/irq.h) \
    $(wildcard include/config/inline/write/unlock/irq.h) \
    $(wildcard include/config/inline/read/unlock/irqrestore.h) \
    $(wildcard include/config/inline/write/unlock/irqrestore.h) \
  include/linux/math64.h \
    $(wildcard include/config/arch/supports/int128.h) \
  arch/arm64/include/generated/asm/div64.h \
  include/asm-generic/div64.h \
  include/linux/time64.h \
  include/uapi/linux/time.h \
  include/uapi/linux/param.h \
  arch/arm64/include/asm/timex.h \
  arch/arm64/include/asm/arch_timer.h \
    $(wildcard include/config/fsl/erratum/a008585.h) \
  include/clocksource/arm_arch_timer.h \
    $(wildcard include/config/arm/arch/timer.h) \
  include/linux/timecounter.h \
  include/asm-generic/timex.h \
  include/linux/jiffies.h \
  include/generated/timeconst.h \
  include/linux/plist.h \
    $(wildcard include/config/debug/pi/list.h) \
  include/linux/rbtree.h \
  include/linux/rcupdate.h \
    $(wildcard include/config/tiny/rcu.h) \
    $(wildcard include/config/tree/rcu.h) \
    $(wildcard include/config/rcu/trace.h) \
    $(wildcard include/config/rcu/stall/common.h) \
    $(wildcard include/config/rcu/nocb/cpu.h) \
    $(wildcard include/config/debug/objects/rcu/head.h) \
    $(wildcard include/config/rcu/boost.h) \
    $(wildcard include/config/rcu/nocb/cpu/all.h) \
    $(wildcard include/config/no/hz/full/sysidle.h) \
  include/linux/cpumask.h \
    $(wildcard include/config/cpumask/offstack.h) \
    $(wildcard include/config/debug/per/cpu/maps.h) \
  include/linux/bitmap.h \
    $(wildcard include/config/s390.h) \
  include/linux/completion.h \
  include/linux/wait.h \
  arch/arm64/include/generated/asm/current.h \
  include/asm-generic/current.h \
  include/uapi/linux/wait.h \
  include/linux/debugobjects.h \
    $(wildcard include/config/debug/objects.h) \
    $(wildcard include/config/debug/objects/free.h) \
  include/linux/ktime.h \
  include/linux/timekeeping.h \
  include/linux/errno.h \
  include/uapi/linux/errno.h \
  arch/arm64/include/generated/asm/errno.h \
  include/uapi/asm-generic/errno.h \
  include/uapi/asm-generic/errno-base.h \
  include/linux/rcutree.h \
  include/linux/nodemask.h \
    $(wildcard include/config/highmem.h) \
    $(wildcard include/config/movable/node.h) \
  include/linux/numa.h \
    $(wildcard include/config/nodes/shift.h) \
  include/linux/percpu.h \
    $(wildcard include/config/need/per/cpu/embed/first/chunk.h) \
    $(wildcard include/config/need/per/cpu/page/first/chunk.h) \
    $(wildcard include/config/have/setup/per/cpu/area.h) \
  include/linux/mmdebug.h \
    $(wildcard include/config/debug/vm.h) \
    $(wildcard include/config/debug/virtual.h) \
    $(wildcard include/config/debug/vm/pgflags.h) \
  include/linux/smp.h \
    $(wildcard include/config/up/late/init.h) \
  include/linux/llist.h \
    $(wildcard include/config/arch/have/nmi/safe/cmpxchg.h) \
  arch/arm64/include/asm/smp.h \
    $(wildcard include/config/arm64/acpi/parking/protocol.h) \
  include/linux/pfn.h \
  arch/arm64/include/asm/percpu.h \
  include/asm-generic/percpu.h \
  include/linux/percpu-defs.h \
    $(wildcard include/config/debug/force/weak/per/cpu.h) \
  include/linux/mm_types.h \
    $(wildcard include/config/split/ptlock/cpus.h) \
    $(wildcard include/config/arch/enable/split/pmd/ptlock.h) \
    $(wildcard include/config/have/cmpxchg/double.h) \
    $(wildcard include/config/have/aligned/struct/page.h) \
    $(wildcard include/config/transparent/hugepage.h) \
    $(wildcard include/config/kmemcheck.h) \
    $(wildcard include/config/userfaultfd.h) \
    $(wildcard include/config/aio.h) \
    $(wildcard include/config/mmu/notifier.h) \
    $(wildcard include/config/compaction.h) \
    $(wildcard include/config/x86/intel/mpx.h) \
    $(wildcard include/config/hugetlb/page.h) \
  include/linux/auxvec.h \
  include/uapi/linux/auxvec.h \
  arch/arm64/include/uapi/asm/auxvec.h \
  include/linux/rwsem.h \
    $(wildcard include/config/rwsem/spin/on/owner.h) \
    $(wildcard include/config/rwsem/generic/spinlock.h) \
  include/linux/err.h \
  include/linux/osq_lock.h \
  arch/arm64/include/generated/asm/rwsem.h \
  include/asm-generic/rwsem.h \
  include/linux/uprobes.h \
  include/linux/page-flags-layout.h \
    $(wildcard include/config/sparsemem.h) \
    $(wildcard include/config/sparsemem/vmemmap.h) \
  include/generated/bounds.h \
  arch/arm64/include/asm/sparsemem.h \
  include/linux/workqueue.h \
    $(wildcard include/config/debug/objects/work.h) \
    $(wildcard include/config/freezer.h) \
    $(wildcard include/config/wq/watchdog.h) \
  include/linux/timer.h \
    $(wildcard include/config/timer/stats.h) \
    $(wildcard include/config/debug/objects/timers.h) \
  arch/arm64/include/asm/page.h \
    $(wildcard include/config/arm64/page/shift.h) \
    $(wildcard include/config/arm64/cont/shift.h) \
    $(wildcard include/config/have/arch/pfn/valid.h) \
  include/linux/personality.h \
  include/uapi/linux/personality.h \
  arch/arm64/include/asm/pgtable-types.h \
  include/asm-generic/pgtable-nopud.h \
  arch/arm64/include/asm/memory.h \
    $(wildcard include/config/arm64/va/bits.h) \
    $(wildcard include/config/blk/dev/initrd.h) \
  arch/arm64/include/generated/asm/sizes.h \
  include/asm-generic/sizes.h \
  include/linux/sizes.h \
  include/asm-generic/memory_model.h \
    $(wildcard include/config/flatmem.h) \
    $(wildcard include/config/discontigmem.h) \
  include/asm-generic/getorder.h \
  arch/arm64/include/asm/mmu.h \
    $(wildcard include/config/unmap/kernel/at/el0.h) \
    $(wildcard include/config/harden/branch/predictor.h) \
  include/linux/cputime.h \
  arch/arm64/include/generated/asm/cputime.h \
  include/asm-generic/cputime.h \
    $(wildcard include/config/virt/cpu/accounting.h) \
  include/asm-generic/cputime_jiffies.h \
  include/linux/sem.h \
  include/uapi/linux/sem.h \
  include/linux/ipc.h \
  include/linux/uidgid.h \
    $(wildcard include/config/user/ns.h) \
  include/linux/highuid.h \
  include/uapi/linux/ipc.h \
  arch/arm64/include/generated/asm/ipcbuf.h \
  include/uapi/asm-generic/ipcbuf.h \
  arch/arm64/include/generated/asm/sembuf.h \
  include/uapi/asm-generic/sembuf.h \
  include/linux/shm.h \
  include/uapi/linux/shm.h \
  arch/arm64/include/generated/asm/shmbuf.h \
  include/uapi/asm-generic/shmbuf.h \
  arch/arm64/include/asm/shmparam.h \
  include/uapi/asm-generic/shmparam.h \
  include/linux/signal.h \
    $(wildcard include/config/old/sigaction.h) \
  include/uapi/linux/signal.h \
  arch/arm64/include/uapi/asm/signal.h \
  include/asm-generic/signal.h \
  include/uapi/asm-generic/signal.h \
  include/uapi/asm-generic/signal-defs.h \
  arch/arm64/include/uapi/asm/sigcontext.h \
  arch/arm64/include/uapi/asm/siginfo.h \
  include/asm-generic/siginfo.h \
  include/uapi/asm-generic/siginfo.h \
  include/linux/pid.h \
  include/linux/topology.h \
    $(wildcard include/config/use/percpu/numa/node/id.h) \
    $(wildcard include/config/have/memoryless/nodes.h) \
  include/linux/mmzone.h \
    $(wildcard include/config/force/max/zoneorder.h) \
    $(wildcard include/config/cma.h) \
    $(wildcard include/config/memory/isolation.h) \
    $(wildcard include/config/zsmalloc.h) \
    $(wildcard include/config/zone/dma.h) \
    $(wildcard include/config/zone/dma32.h) \
    $(wildcard include/config/zone/device.h) \
    $(wildcard include/config/memory/hotplug.h) \
    $(wildcard include/config/flat/node/mem/map.h) \
    $(wildcard include/config/page/extension.h) \
    $(wildcard include/config/no/bootmem.h) \
    $(wildcard include/config/deferred/struct/page/init.h) \
    $(wildcard include/config/have/memory/present.h) \
    $(wildcard include/config/need/node/memmap/size.h) \
    $(wildcard include/config/have/memblock/node/map.h) \
    $(wildcard include/config/need/multiple/nodes.h) \
    $(wildcard include/config/have/arch/early/pfn/to/nid.h) \
    $(wildcard include/config/sparsemem/extreme.h) \
    $(wildcard include/config/holes/in/zone.h) \
    $(wildcard include/config/arch/has/holes/memorymodel.h) \
  include/linux/pageblock-flags.h \
    $(wildcard include/config/hugetlb/page/size/variable.h) \
  include/linux/memory_hotplug.h \
    $(wildcard include/config/memory/hotremove.h) \
    $(wildcard include/config/have/arch/nodedata/extension.h) \
    $(wildcard include/config/have/bootmem/info/node.h) \
  include/linux/notifier.h \
  include/linux/mutex.h \
    $(wildcard include/config/mutex/spin/on/owner.h) \
  include/linux/srcu.h \
  arch/arm64/include/asm/topology.h \
  include/asm-generic/topology.h \
  include/linux/seccomp.h \
    $(wildcard include/config/seccomp.h) \
    $(wildcard include/config/have/arch/seccomp/filter.h) \
    $(wildcard include/config/seccomp/filter.h) \
    $(wildcard include/config/checkpoint/restore.h) \
  include/uapi/linux/seccomp.h \
  include/linux/rculist.h \
  include/linux/rtmutex.h \
    $(wildcard include/config/debug/rt/mutexes.h) \
  include/linux/resource.h \
  include/uapi/linux/resource.h \
  arch/arm64/include/generated/asm/resource.h \
  include/asm-generic/resource.h \
  include/uapi/asm-generic/resource.h \
  include/linux/hrtimer.h \
    $(wildcard include/config/high/res/timers.h) \
    $(wildcard include/config/time/low/res.h) \
    $(wildcard include/config/timerfd.h) \
  include/linux/timerqueue.h \
  include/linux/kcov.h \
  include/uapi/linux/kcov.h \
  include/linux/task_io_accounting.h \
    $(wildcard include/config/task/io/accounting.h) \
  include/linux/latencytop.h \
  include/linux/cred.h \
    $(wildcard include/config/debug/credentials.h) \
    $(wildcard include/config/security.h) \
  include/linux/key.h \
    $(wildcard include/config/sysctl.h) \
  include/linux/sysctl.h \
  include/uapi/linux/sysctl.h \
  include/linux/assoc_array.h \
    $(wildcard include/config/associative/array.h) \
  include/linux/selinux.h \
    $(wildcard include/config/security/selinux.h) \
  include/linux/gfp.h \
    $(wildcard include/config/pm/sleep.h) \
  include/uapi/linux/magic.h \
  include/linux/cgroup-defs.h \
    $(wildcard include/config/sock/cgroup/data.h) \
  include/uapi/linux/limits.h \
  include/linux/idr.h \
  include/linux/percpu-refcount.h \
  include/linux/percpu-rwsem.h \
  include/linux/rcu_sync.h \
  include/uapi/linux/stat.h \
  include/linux/kmod.h \
  include/linux/elf.h \
  arch/arm64/include/asm/elf.h \
  arch/arm64/include/generated/asm/user.h \
  include/asm-generic/user.h \
  include/uapi/linux/elf.h \
  include/uapi/linux/elf-em.h \
  include/linux/kobject.h \
    $(wildcard include/config/uevent/helper.h) \
    $(wildcard include/config/debug/kobject/release.h) \
  include/linux/sysfs.h \
  include/linux/kernfs.h \
    $(wildcard include/config/kernfs.h) \
  include/linux/kobject_ns.h \
  include/linux/kref.h \
  include/linux/moduleparam.h \
    $(wildcard include/config/alpha.h) \
    $(wildcard include/config/ia64.h) \
    $(wildcard include/config/ppc64.h) \
  include/linux/extable.h \
  include/linux/rbtree_latch.h \
  arch/arm64/include/asm/module.h \
    $(wildcard include/config/arm64/module/plts.h) \
    $(wildcard include/config/randomize/base.h) \
  include/asm-generic/module.h \
    $(wildcard include/config/have/mod/arch/specific.h) \
    $(wildcard include/config/modules/use/elf/rel.h) \
    $(wildcard include/config/modules/use/elf/rela.h) \
  include/linux/utsname.h \
    $(wildcard include/config/uts/ns.h) \
    $(wildcard include/config/proc/sysctl.h) \
  include/linux/nsproxy.h \
  include/linux/ns_common.h \
  include/uapi/linux/utsname.h \
  drivers/starnet/rtl8821CU/include/osdep_service.h \
    $(wildcard include/config/use/vmalloc.h) \
    $(wildcard include/config/ap/wowlan.h) \
  drivers/starnet/rtl8821CU/include/osdep_service_linux.h \
    $(wildcard include/config/net/radio.h) \
    $(wildcard include/config/wireless/ext.h) \
    $(wildcard include/config/ieee80211/ht/addt/info.h) \
    $(wildcard include/config/efuse/config/file.h) \
    $(wildcard include/config/usb/suspend.h) \
  include/linux/slab.h \
    $(wildcard include/config/debug/slab.h) \
    $(wildcard include/config/failslab.h) \
    $(wildcard include/config/have/hardened/usercopy/allocator.h) \
    $(wildcard include/config/slab.h) \
    $(wildcard include/config/slub.h) \
  include/linux/kmemleak.h \
    $(wildcard include/config/debug/kmemleak.h) \
  include/linux/kasan.h \
  include/linux/namei.h \
  include/linux/path.h \
  include/linux/fcntl.h \
  include/uapi/linux/fcntl.h \
  arch/arm64/include/uapi/asm/fcntl.h \
  include/uapi/asm-generic/fcntl.h \
  include/linux/netdevice.h \
    $(wildcard include/config/dcb.h) \
    $(wildcard include/config/hyperv/net.h) \
    $(wildcard include/config/wlan.h) \
    $(wildcard include/config/ax25.h) \
    $(wildcard include/config/mac80211/mesh.h) \
    $(wildcard include/config/net/ipip.h) \
    $(wildcard include/config/net/ipgre.h) \
    $(wildcard include/config/ipv6/sit.h) \
    $(wildcard include/config/ipv6/tunnel.h) \
    $(wildcard include/config/rps.h) \
    $(wildcard include/config/netpoll.h) \
    $(wildcard include/config/net/rx/busy/poll.h) \
    $(wildcard include/config/xps.h) \
    $(wildcard include/config/bql.h) \
    $(wildcard include/config/rfs/accel.h) \
    $(wildcard include/config/fcoe.h) \
    $(wildcard include/config/net/poll/controller.h) \
    $(wildcard include/config/libfcoe.h) \
    $(wildcard include/config/net/switchdev.h) \
    $(wildcard include/config/net/l3/master/dev.h) \
    $(wildcard include/config/ipv6.h) \
    $(wildcard include/config/vlan/8021q.h) \
    $(wildcard include/config/net/dsa.h) \
    $(wildcard include/config/tipc.h) \
    $(wildcard include/config/mpls/routing.h) \
    $(wildcard include/config/net/cls/act.h) \
    $(wildcard include/config/netfilter/ingress.h) \
    $(wildcard include/config/net/sched.h) \
    $(wildcard include/config/cgroup/net/prio.h) \
    $(wildcard include/config/net/flow/limit.h) \
  include/linux/delay.h \
  arch/arm64/include/generated/asm/delay.h \
  include/asm-generic/delay.h \
  include/linux/prefetch.h \
  include/linux/dmaengine.h \
    $(wildcard include/config/async/tx/enable/channel/switch.h) \
    $(wildcard include/config/dma/engine.h) \
    $(wildcard include/config/rapidio/dma/engine.h) \
    $(wildcard include/config/async/tx/dma.h) \
  include/linux/device.h \
    $(wildcard include/config/debug/devres.h) \
    $(wildcard include/config/generic/msi/irq/domain.h) \
    $(wildcard include/config/pinctrl.h) \
    $(wildcard include/config/generic/msi/irq.h) \
    $(wildcard include/config/dma/cma.h) \
    $(wildcard include/config/of.h) \
    $(wildcard include/config/devtmpfs.h) \
    $(wildcard include/config/sysfs/deprecated.h) \
  include/linux/ioport.h \
  include/linux/klist.h \
  include/linux/pinctrl/devinfo.h \
    $(wildcard include/config/pm.h) \
  include/linux/pinctrl/consumer.h \
  include/linux/seq_file.h \
  include/linux/fs.h \
    $(wildcard include/config/fs/posix/acl.h) \
    $(wildcard include/config/cgroup/writeback.h) \
    $(wildcard include/config/ima.h) \
    $(wildcard include/config/fsnotify.h) \
    $(wildcard include/config/fs/encryption.h) \
    $(wildcard include/config/file/locking.h) \
    $(wildcard include/config/quota.h) \
    $(wildcard include/config/fs/dax.h) \
    $(wildcard include/config/mandatory/file/locking.h) \
    $(wildcard include/config/migration.h) \
  include/linux/kdev_t.h \
  include/uapi/linux/kdev_t.h \
  include/linux/dcache.h \
  include/linux/rculist_bl.h \
  include/linux/list_bl.h \
  include/linux/bit_spinlock.h \
  include/linux/lockref.h \
    $(wildcard include/config/arch/use/cmpxchg/lockref.h) \
  include/linux/stringhash.h \
    $(wildcard include/config/dcache/word/access.h) \
  include/linux/hash.h \
    $(wildcard include/config/have/arch/hash.h) \
  include/linux/list_lru.h \
  include/linux/shrinker.h \
  include/linux/radix-tree.h \
    $(wildcard include/config/radix/tree/multiorder.h) \
  include/linux/semaphore.h \
  include/uapi/linux/fiemap.h \
  include/linux/migrate_mode.h \
  include/linux/blk_types.h \
    $(wildcard include/config/blk/cgroup.h) \
    $(wildcard include/config/blk/dev/integrity.h) \
  include/linux/bvec.h \
  include/linux/delayed_call.h \
  include/uapi/linux/fs.h \
  include/uapi/linux/ioctl.h \
  arch/arm64/include/generated/asm/ioctl.h \
  include/asm-generic/ioctl.h \
  include/uapi/asm-generic/ioctl.h \
  include/linux/quota.h \
    $(wildcard include/config/quota/netlink/interface.h) \
  include/linux/percpu_counter.h \
  include/uapi/linux/dqblk_xfs.h \
  include/linux/dqblk_v1.h \
  include/linux/dqblk_v2.h \
  include/linux/dqblk_qtree.h \
  include/linux/projid.h \
  include/uapi/linux/quota.h \
  include/linux/nfs_fs_i.h \
  include/linux/pinctrl/pinctrl-state.h \
  include/linux/pm.h \
    $(wildcard include/config/vt/console/sleep.h) \
    $(wildcard include/config/pm/clk.h) \
    $(wildcard include/config/pm/generic/domains.h) \
  include/linux/ratelimit.h \
  arch/arm64/include/asm/device.h \
    $(wildcard include/config/iommu/api.h) \
  include/linux/pm_wakeup.h \
  include/linux/uio.h \
  include/uapi/linux/uio.h \
  include/linux/scatterlist.h \
    $(wildcard include/config/debug/sg.h) \
    $(wildcard include/config/need/sg/dma/length.h) \
    $(wildcard include/config/arch/has/sg/chain.h) \
    $(wildcard include/config/sg/pool.h) \
  include/linux/mm.h \
    $(wildcard include/config/have/arch/mmap/rnd/bits.h) \
    $(wildcard include/config/have/arch/mmap/rnd/compat/bits.h) \
    $(wildcard include/config/mem/soft/dirty.h) \
    $(wildcard include/config/arch/uses/high/vma/flags.h) \
    $(wildcard include/config/x86.h) \
    $(wildcard include/config/x86/intel/memory/protection/keys.h) \
    $(wildcard include/config/ppc.h) \
    $(wildcard include/config/parisc.h) \
    $(wildcard include/config/metag.h) \
    $(wildcard include/config/shmem.h) \
    $(wildcard include/config/debug/vm/rb.h) \
    $(wildcard include/config/page/poisoning.h) \
    $(wildcard include/config/debug/pagealloc.h) \
    $(wildcard include/config/hibernation.h) \
    $(wildcard include/config/hugetlbfs.h) \
  include/linux/debug_locks.h \
    $(wildcard include/config/debug/locking/api/selftests.h) \
  include/linux/range.h \
  include/linux/page_ext.h \
    $(wildcard include/config/idle/page/tracking.h) \
  include/linux/stacktrace.h \
    $(wildcard include/config/stacktrace.h) \
    $(wildcard include/config/user/stacktrace/support.h) \
  include/linux/stackdepot.h \
  include/linux/page_ref.h \
    $(wildcard include/config/debug/page/ref.h) \
  include/linux/page-flags.h \
    $(wildcard include/config/arch/uses/pg/uncached.h) \
    $(wildcard include/config/memory/failure.h) \
    $(wildcard include/config/swap.h) \
    $(wildcard include/config/ksm.h) \
  include/linux/tracepoint-defs.h \
  include/linux/static_key.h \
  arch/arm64/include/asm/pgtable.h \
    $(wildcard include/config/arm64/hw/afdbm.h) \
  arch/arm64/include/asm/proc-fns.h \
  arch/arm64/include/asm/pgtable-prot.h \
  arch/arm64/include/asm/fixmap.h \
  arch/arm64/include/asm/boot.h \
  include/asm-generic/fixmap.h \
  include/asm-generic/pgtable.h \
    $(wildcard include/config/have/arch/soft/dirty.h) \
    $(wildcard include/config/have/arch/huge/vmap.h) \
  include/linux/huge_mm.h \
  include/linux/vmstat.h \
    $(wildcard include/config/vm/event/counters.h) \
    $(wildcard include/config/debug/tlbflush.h) \
    $(wildcard include/config/debug/vm/vmacache.h) \
  include/linux/vm_event_item.h \
    $(wildcard include/config/memory/balloon.h) \
    $(wildcard include/config/balloon/compaction.h) \
  arch/arm64/include/asm/io.h \
  arch/arm64/include/generated/asm/early_ioremap.h \
  include/asm-generic/early_ioremap.h \
    $(wildcard include/config/generic/early/ioremap.h) \
  include/xen/xen.h \
    $(wildcard include/config/xen.h) \
    $(wildcard include/config/xen/dom0.h) \
    $(wildcard include/config/xen/pvh.h) \
  include/asm-generic/io.h \
    $(wildcard include/config/generic/iomap.h) \
    $(wildcard include/config/has/ioport/map.h) \
    $(wildcard include/config/virt/to/bus.h) \
  include/asm-generic/pci_iomap.h \
    $(wildcard include/config/pci.h) \
    $(wildcard include/config/no/generic/pci/ioport/map.h) \
    $(wildcard include/config/generic/pci/iomap.h) \
  include/linux/vmalloc.h \
  include/linux/dynamic_queue_limits.h \
  include/linux/ethtool.h \
  include/linux/compat.h \
    $(wildcard include/config/compat/old/sigaction.h) \
    $(wildcard include/config/odd/rt/sigaction.h) \
  include/linux/socket.h \
  arch/arm64/include/generated/asm/socket.h \
  include/uapi/asm-generic/socket.h \
  arch/arm64/include/generated/asm/sockios.h \
  include/uapi/asm-generic/sockios.h \
  include/uapi/linux/sockios.h \
  include/uapi/linux/socket.h \
  include/uapi/linux/if.h \
  include/uapi/linux/libc-compat.h \
    $(wildcard include/config/data.h) \
  include/uapi/linux/hdlc/ioctl.h \
  include/uapi/linux/aio_abi.h \
  include/uapi/linux/unistd.h \
  arch/arm64/include/asm/unistd.h \
  arch/arm64/include/uapi/asm/unistd.h \
  include/asm-generic/unistd.h \
  include/uapi/asm-generic/unistd.h \
  include/uapi/linux/ethtool.h \
  include/linux/if_ether.h \
  include/linux/skbuff.h \
    $(wildcard include/config/nf/conntrack.h) \
    $(wildcard include/config/bridge/netfilter.h) \
    $(wildcard include/config/xfrm.h) \
    $(wildcard include/config/ipv6/ndisc/nodetype.h) \
    $(wildcard include/config/network/secmark.h) \
    $(wildcard include/config/network/phy/timestamping.h) \
    $(wildcard include/config/netfilter/xt/target/trace.h) \
    $(wildcard include/config/nf/tables.h) \
  include/linux/kmemcheck.h \
  include/linux/net.h \
  include/linux/random.h \
    $(wildcard include/config/gcc/plugin/latent/entropy.h) \
    $(wildcard include/config/arch/random.h) \
  include/linux/once.h \
  include/uapi/linux/random.h \
  include/linux/irqnr.h \
  include/uapi/linux/irqnr.h \
  include/uapi/linux/net.h \
  include/linux/textsearch.h \
  include/net/checksum.h \
  arch/arm64/include/asm/uaccess.h \
    $(wildcard include/config/arm64/pan.h) \
  arch/arm64/include/asm/kernel-pgtable.h \
  include/linux/kasan-checks.h \
  arch/arm64/include/asm/compiler.h \
  arch/arm64/include/asm/checksum.h \
  include/asm-generic/checksum.h \
  include/linux/dma-mapping.h \
    $(wildcard include/config/have/generic/dma/coherent.h) \
    $(wildcard include/config/has/dma.h) \
    $(wildcard include/config/arch/has/dma/set/coherent/mask.h) \
    $(wildcard include/config/need/dma/map/state.h) \
    $(wildcard include/config/dma/api/debug.h) \
  include/linux/dma-debug.h \
  include/linux/dma-direction.h \
  arch/arm64/include/asm/dma-mapping.h \
    $(wildcard include/config/iommu/dma.h) \
  arch/arm64/include/asm/xen/hypervisor.h \
  arch/arm64/include/../../arm/include/asm/xen/hypervisor.h \
  include/linux/netdev_features.h \
  include/net/flow_dissector.h \
  include/linux/in6.h \
  include/uapi/linux/in6.h \
  include/uapi/linux/if_ether.h \
  include/linux/splice.h \
  include/linux/pipe_fs_i.h \
  include/uapi/linux/if_packet.h \
  include/net/flow.h \
  include/net/net_namespace.h \
    $(wildcard include/config/ieee802154/6lowpan.h) \
    $(wildcard include/config/ip/sctp.h) \
    $(wildcard include/config/ip/dccp.h) \
    $(wildcard include/config/netfilter.h) \
    $(wildcard include/config/nf/defrag/ipv6.h) \
    $(wildcard include/config/netfilter/netlink/acct.h) \
    $(wildcard include/config/nf/ct/netlink/timeout.h) \
    $(wildcard include/config/wext/core.h) \
    $(wildcard include/config/ip/vs.h) \
    $(wildcard include/config/mpls.h) \
    $(wildcard include/config/net/ns.h) \
  include/net/netns/core.h \
  include/net/netns/mib.h \
    $(wildcard include/config/xfrm/statistics.h) \
  include/net/snmp.h \
  include/uapi/linux/snmp.h \
  include/linux/u64_stats_sync.h \
  include/net/netns/unix.h \
  include/net/netns/packet.h \
  include/net/netns/ipv4.h \
    $(wildcard include/config/ip/multiple/tables.h) \
    $(wildcard include/config/ip/route/classid.h) \
    $(wildcard include/config/ip/mroute.h) \
    $(wildcard include/config/ip/mroute/multiple/tables.h) \
    $(wildcard include/config/ip/route/multipath.h) \
  include/net/inet_frag.h \
  include/net/netns/ipv6.h \
    $(wildcard include/config/ipv6/multiple/tables.h) \
    $(wildcard include/config/ipv6/mroute.h) \
    $(wildcard include/config/ipv6/mroute/multiple/tables.h) \
  include/net/dst_ops.h \
  include/net/netns/ieee802154_6lowpan.h \
  include/net/netns/sctp.h \
  include/net/netns/dccp.h \
  include/net/netns/netfilter.h \
  include/linux/netfilter_defs.h \
  include/uapi/linux/netfilter.h \
  include/linux/in.h \
  include/uapi/linux/in.h \
  include/net/netns/x_tables.h \
    $(wildcard include/config/bridge/nf/ebtables.h) \
  include/net/netns/conntrack.h \
    $(wildcard include/config/nf/conntrack/events.h) \
    $(wildcard include/config/nf/conntrack/labels.h) \
  include/linux/list_nulls.h \
  include/linux/netfilter/nf_conntrack_tcp.h \
  include/uapi/linux/netfilter/nf_conntrack_tcp.h \
  include/net/netns/nftables.h \
  include/net/netns/xfrm.h \
  include/uapi/linux/xfrm.h \
  include/net/flowcache.h \
  include/linux/interrupt.h \
    $(wildcard include/config/irq/forced/threading.h) \
    $(wildcard include/config/generic/irq/probe.h) \
  include/linux/irqreturn.h \
  include/linux/hardirq.h \
  include/linux/ftrace_irq.h \
    $(wildcard include/config/ftrace/nmi/enter.h) \
    $(wildcard include/config/hwlat/tracer.h) \
  include/linux/vtime.h \
  include/linux/context_tracking_state.h \
    $(wildcard include/config/context/tracking.h) \
  arch/arm64/include/asm/hardirq.h \
  arch/arm64/include/asm/irq.h \
  include/asm-generic/irq.h \
  include/linux/irq_cpustat.h \
  include/net/netns/mpls.h \
  include/linux/seq_file_net.h \
  include/net/dsa.h \
    $(wildcard include/config/net/dsa/hwmon.h) \
  include/linux/of.h \
    $(wildcard include/config/sparc.h) \
    $(wildcard include/config/of/dynamic.h) \
    $(wildcard include/config/attach/node.h) \
    $(wildcard include/config/detach/node.h) \
    $(wildcard include/config/add/property.h) \
    $(wildcard include/config/remove/property.h) \
    $(wildcard include/config/update/property.h) \
    $(wildcard include/config/of/numa.h) \
    $(wildcard include/config/no/change.h) \
    $(wildcard include/config/change/add.h) \
    $(wildcard include/config/change/remove.h) \
    $(wildcard include/config/of/resolve.h) \
    $(wildcard include/config/of/overlay.h) \
  include/linux/mod_devicetable.h \
  include/linux/uuid.h \
  include/uapi/linux/uuid.h \
  include/linux/property.h \
  include/linux/fwnode.h \
  include/linux/phy.h \
  include/linux/mdio.h \
  include/uapi/linux/mdio.h \
  include/linux/mii.h \
  include/uapi/linux/mii.h \
  include/linux/phy_fixed.h \
    $(wildcard include/config/fixed/phy.h) \
  include/net/netprio_cgroup.h \
  include/linux/cgroup.h \
    $(wildcard include/config/cgroup/net/classid.h) \
    $(wildcard include/config/cgroup/data.h) \
  include/uapi/linux/cgroupstats.h \
  include/uapi/linux/taskstats.h \
  include/linux/user_namespace.h \
    $(wildcard include/config/persistent/keyrings.h) \
  include/uapi/linux/neighbour.h \
  include/linux/netlink.h \
  include/net/scm.h \
    $(wildcard include/config/security/network.h) \
  include/linux/security.h \
    $(wildcard include/config/security/network/xfrm.h) \
    $(wildcard include/config/security/path.h) \
    $(wildcard include/config/securityfs.h) \
  include/uapi/linux/netlink.h \
  include/uapi/linux/netdevice.h \
  include/linux/if_link.h \
  include/uapi/linux/if_link.h \
    $(wildcard include/config/pending.h) \
  include/uapi/linux/if_bonding.h \
  include/uapi/linux/pkt_cls.h \
    $(wildcard include/config/net/cls/ind.h) \
  include/uapi/linux/pkt_sched.h \
  include/linux/hashtable.h \
  include/linux/inetdevice.h \
  include/linux/ip.h \
  include/uapi/linux/ip.h \
  include/linux/rtnetlink.h \
    $(wildcard include/config/net/ingress.h) \
    $(wildcard include/config/net/egress.h) \
  include/uapi/linux/rtnetlink.h \
  include/uapi/linux/if_addr.h \
  include/linux/circ_buf.h \
  include/linux/etherdevice.h \
    $(wildcard include/config/have/efficient/unaligned/access.h) \
  arch/arm64/include/generated/asm/unaligned.h \
  include/asm-generic/unaligned.h \
  include/linux/unaligned/access_ok.h \
  include/linux/unaligned/generic.h \
  include/linux/wireless.h \
  include/uapi/linux/wireless.h \
  include/net/iw_handler.h \
    $(wildcard include/config/wext/priv.h) \
  include/net/addrconf.h \
    $(wildcard include/config/ipv6/mip6.h) \
  include/net/if_inet6.h \
  include/linux/ipv6.h \
    $(wildcard include/config/ipv6/router/pref.h) \
    $(wildcard include/config/ipv6/route/info.h) \
    $(wildcard include/config/ipv6/optimistic/dad.h) \
    $(wildcard include/config/ipv6/subtrees.h) \
  include/uapi/linux/ipv6.h \
  include/linux/icmpv6.h \
  include/uapi/linux/icmpv6.h \
  include/linux/tcp.h \
    $(wildcard include/config/tcp/md5sig.h) \
  include/linux/win_minmax.h \
  include/net/sock.h \
    $(wildcard include/config/net.h) \
    $(wildcard include/config/inet.h) \
  include/linux/uaccess.h \
  include/linux/page_counter.h \
  include/linux/memcontrol.h \
    $(wildcard include/config/memcg/swap.h) \
  include/linux/vmpressure.h \
  include/linux/eventfd.h \
    $(wildcard include/config/eventfd.h) \
  include/linux/writeback.h \
  include/linux/flex_proportions.h \
  include/linux/backing-dev-defs.h \
    $(wildcard include/config/debug/fs.h) \
  include/linux/filter.h \
    $(wildcard include/config/bpf/jit.h) \
    $(wildcard include/config/have/ebpf/jit.h) \
  include/net/sch_generic.h \
  include/uapi/linux/pkt_cls.h \
  include/net/gen_stats.h \
  include/uapi/linux/gen_stats.h \
  include/net/rtnetlink.h \
  include/net/netlink.h \
  arch/arm64/include/asm/cacheflush.h \
  include/uapi/linux/filter.h \
  include/uapi/linux/bpf_common.h \
  include/uapi/linux/bpf.h \
  include/linux/rculist_nulls.h \
  include/linux/poll.h \
  include/uapi/linux/poll.h \
  arch/arm64/include/generated/asm/poll.h \
  include/uapi/asm-generic/poll.h \
  include/net/dst.h \
  include/net/neighbour.h \
  include/net/tcp_states.h \
  include/uapi/linux/net_tstamp.h \
  include/net/inet_connection_sock.h \
  include/net/inet_sock.h \
  include/linux/jhash.h \
  include/linux/unaligned/packed_struct.h \
  include/net/request_sock.h \
  include/net/netns/hash.h \
  include/net/l3mdev.h \
  include/net/fib_rules.h \
  include/uapi/linux/fib_rules.h \
  include/net/inet_timewait_sock.h \
  include/net/timewait_sock.h \
  include/uapi/linux/tcp.h \
  include/linux/udp.h \
  include/uapi/linux/udp.h \
  include/net/ipv6.h \
  include/net/ndisc.h \
  include/linux/if_arp.h \
    $(wildcard include/config/firewire/net.h) \
  include/uapi/linux/if_arp.h \
  include/linux/kthread.h \
  include/uapi/linux/limits.h \
  include/net/ieee80211_radiotap.h \
  include/linux/ieee80211.h \
    $(wildcard include/config/timeout.h) \
  include/net/cfg80211.h \
    $(wildcard include/config/cfg80211.h) \
    $(wildcard include/config/nl80211/testmode.h) \
    $(wildcard include/config/cfg80211/wext.h) \
  include/linux/debugfs.h \
  include/uapi/linux/nl80211.h \
  include/net/regulatory.h \
  include/linux/usb.h \
    $(wildcard include/config/usb/mon.h) \
    $(wildcard include/config/acpi.h) \
    $(wildcard include/config/usb/led/trig.h) \
  include/linux/usb/ch9.h \
  include/uapi/linux/usb/ch9.h \
    $(wildcard include/config/size.h) \
    $(wildcard include/config/att/one.h) \
    $(wildcard include/config/att/selfpower.h) \
    $(wildcard include/config/att/wakeup.h) \
    $(wildcard include/config/att/battery.h) \
  include/linux/pm_runtime.h \
  drivers/starnet/rtl8821CU/include/../os_dep/linux/rtw_rhashtable.h \
  drivers/starnet/rtl8821CU/include/drv_types_linux.h \
  drivers/starnet/rtl8821CU/include/rtw_byteorder.h \
    $(wildcard include/config/platform/mstar389.h) \
  drivers/starnet/rtl8821CU/include/byteorder/little_endian.h \
  drivers/starnet/rtl8821CU/include/wlan_bssdef.h \
  drivers/starnet/rtl8821CU/include/wifi.h \
    $(wildcard include/config/append/vendor/ie/enable.h) \
    $(wildcard include/config/error.h) \
    $(wildcard include/config/method/flash.h) \
    $(wildcard include/config/method/ethernet.h) \
    $(wildcard include/config/method/label.h) \
    $(wildcard include/config/method/display.h) \
    $(wildcard include/config/method/e/nfc.h) \
    $(wildcard include/config/method/i/nfc.h) \
    $(wildcard include/config/method/nfc.h) \
    $(wildcard include/config/method/pbc.h) \
    $(wildcard include/config/method/keypad.h) \
    $(wildcard include/config/method/vpbc.h) \
    $(wildcard include/config/method/ppbc.h) \
    $(wildcard include/config/method/vdisplay.h) \
    $(wildcard include/config/method/pdisplay.h) \
  drivers/starnet/rtl8821CU/include/ieee80211.h \
    $(wildcard include/config/rtw/wnm.h) \
  drivers/starnet/rtl8821CU/include/rtw_debug.h \
    $(wildcard include/config/rtw/debug.h) \
    $(wildcard include/config/proc/debug.h) \
    $(wildcard include/config/huawei/proc.h) \
    $(wildcard include/config/rf4ce/coexist.h) \
    $(wildcard include/config/p2p/wowlan.h) \
    $(wildcard include/config/dbg/rf/cal.h) \
    $(wildcard include/config/ctrl/txss/by/tp.h) \
    $(wildcard include/config/lps/chk/by/tp.h) \
    $(wildcard include/config/support/static/smps.h) \
  drivers/starnet/rtl8821CU/include/cmn_info/rtw_sta_info.h \
  drivers/starnet/rtl8821CU/include/rtw_rf.h \
  drivers/starnet/rtl8821CU/include/../core/rtw_chplan.h \
  drivers/starnet/rtl8821CU/include/rtw_ht.h \
  drivers/starnet/rtl8821CU/include/rtw_vht.h \
  drivers/starnet/rtl8821CU/include/rtw_cmd.h \
    $(wildcard include/config/fw/c2h/reg.h) \
    $(wildcard include/config/c2h/wk.h) \
  drivers/starnet/rtl8821CU/include/cmd_osdep.h \
  drivers/starnet/rtl8821CU/include/rtw_security.h \
  drivers/starnet/rtl8821CU/include/rtw_xmit.h \
    $(wildcard include/config/tx/aggregation.h) \
    $(wildcard include/config/sdio/tx/format/dummy/auto.h) \
    $(wildcard include/config/platform/arm/sunxi.h) \
    $(wildcard include/config/platform/arm/sun6i.h) \
    $(wildcard include/config/platform/arm/sun7i.h) \
    $(wildcard include/config/platform/arm/sun8i.h) \
    $(wildcard include/config/platform/arm/sun50iw1p1.h) \
    $(wildcard include/config/platform/mstar.h) \
    $(wildcard include/config/single/xmit/buf.h) \
    $(wildcard include/config/pci/tx/polling.h) \
    $(wildcard include/config/lps/poff.h) \
    $(wildcard include/config/trx/bd/arch.h) \
    $(wildcard include/config/64bit/dma.h) \
    $(wildcard include/config/sdio/tx/tasklet.h) \
  drivers/starnet/rtl8821CU/include/xmit_osdep.h \
  drivers/starnet/rtl8821CU/include/rtw_recv.h \
    $(wildcard include/config/single/recv/buf.h) \
    $(wildcard include/config/platform/rtk390x.h) \
    $(wildcard include/config/rx/indicate/queue.h) \
  drivers/starnet/rtl8821CU/include/cmn_info/rtw_sta_info.h \
  drivers/starnet/rtl8821CU/include/rtw_rm.h \
  drivers/starnet/rtl8821CU/include/recv_osdep.h \
  drivers/starnet/rtl8821CU/include/rtw_efuse.h \
  drivers/starnet/rtl8821CU/include/rtw_sreset.h \
  drivers/starnet/rtl8821CU/include/hal_intf.h \
    $(wildcard include/config/lps/lclk/wd/timer.h) \
    $(wildcard include/config/rfkill/poll.h) \
  drivers/starnet/rtl8821CU/include/hal_com.h \
    $(wildcard include/config/rf/power/trim.h) \
    $(wildcard include/config/tsf/reset/offload.h) \
    $(wildcard include/config/bcn/recovery.h) \
    $(wildcard include/config/bcn/xmit/protect.h) \
  drivers/starnet/rtl8821CU/include/HalVerDef.h \
  drivers/starnet/rtl8821CU/include/hal_pg.h \
  drivers/starnet/rtl8821CU/include/hal_phy.h \
    $(wildcard include/config/rf/shadow/rw.h) \
  drivers/starnet/rtl8821CU/include/hal_phy_reg.h \
    $(wildcard include/config/ant/a.h) \
    $(wildcard include/config/ant/b.h) \
    $(wildcard include/config/pmpd/ant/a.h) \
    $(wildcard include/config/pmpd/ant/b.h) \
  drivers/starnet/rtl8821CU/include/hal_com_reg.h \
    $(wildcard include/config/usedk.h) \
    $(wildcard include/config/no/usedk.h) \
  drivers/starnet/rtl8821CU/include/hal_com_phycfg.h \
  drivers/starnet/rtl8821CU/include/../hal/hal_com_c2h.h \
  drivers/starnet/rtl8821CU/include/hal_com_h2c.h \
    $(wildcard include/config/ra/dbg/cmd.h) \
  drivers/starnet/rtl8821CU/include/hal_com_led.h \
    $(wildcard include/config/rtw/sw/led/trx/da/classify.h) \
  drivers/starnet/rtl8821CU/include/../hal/hal_dm.h \
    $(wildcard include/config/support/dynamic/txpwr.h) \
    $(wildcard include/config/lps/pwr/tracking.h) \
  drivers/starnet/rtl8821CU/include/rtw_qos.h \
  drivers/starnet/rtl8821CU/include/rtw_pwrctrl.h \
    $(wildcard include/config/platform/android/intel/x86.h) \
    $(wildcard include/config/lps/rpwm/timer.h) \
    $(wildcard include/config/rtw/cfgvednor/llstats.h) \
    $(wildcard include/config/check/leave/lps.h) \
  drivers/starnet/rtl8821CU/include/rtw_mlme.h \
    $(wildcard include/config/bcn/cnt/confirm/hdl.h) \
  drivers/starnet/rtl8821CU/include/mlme_osdep.h \
  drivers/starnet/rtl8821CU/include/rtw_io.h \
  drivers/starnet/rtl8821CU/include/rtw_ioctl.h \
  drivers/starnet/rtl8821CU/include/rtw_ioctl_set.h \
  drivers/starnet/rtl8821CU/include/rtw_ioctl_query.h \
  drivers/starnet/rtl8821CU/include/osdep_intf.h \
  drivers/starnet/rtl8821CU/include/../os_dep/linux/rtw_proc.h \
  include/linux/proc_fs.h \
  drivers/starnet/rtl8821CU/include/../os_dep/linux/ioctl_cfg80211.h \
    $(wildcard include/config/rtw/dynamic/ndev.h) \
    $(wildcard include/config/radio/work.h) \
    $(wildcard include/config/rtw/cfgvednor/rssimonitor.h) \
  drivers/starnet/rtl8821CU/include/../os_dep/linux/rtw_cfgvendor.h \
    $(wildcard include/config/nd/offload.h) \
  drivers/starnet/rtl8821CU/include/rtw_eeprom.h \
  drivers/starnet/rtl8821CU/include/sta_info.h \
    $(wildcard include/config/rtw/macaddr/acl.h) \
    $(wildcard include/config/rtw/pre/link/sta.h) \
    $(wildcard include/config/atmel/rc/patch.h) \
    $(wildcard include/config/auto/ap/mode.h) \
  drivers/starnet/rtl8821CU/include/rtw_event.h \
  drivers/starnet/rtl8821CU/include/rtw_mlme_ext.h \
  drivers/starnet/rtl8821CU/include/rtw_mi.h \
  drivers/starnet/rtl8821CU/include/rtw_ap.h \
    $(wildcard include/config/bmc/tx/rate/select.h) \
  drivers/starnet/rtl8821CU/include/rtw_version.h \
  drivers/starnet/rtl8821CU/include/rtw_odm.h \
  drivers/starnet/rtl8821CU/include/../hal/phydm/phydm_types.h \
  drivers/starnet/rtl8821CU/include/rtw_p2p.h \
  drivers/starnet/rtl8821CU/include/rtw_mp.h \
    $(wildcard include/config/txt.h) \
  drivers/starnet/rtl8821CU/include/rtw_br_ext.h \
  drivers/starnet/rtl8821CU/include/ip.h \
  drivers/starnet/rtl8821CU/include/if_ether.h \
  drivers/starnet/rtl8821CU/include/ethernet.h \
  drivers/starnet/rtl8821CU/include/circ_buf.h \
  drivers/starnet/rtl8821CU/include/rtw_android.h \
    $(wildcard include/config/platform/intel/byt.h) \
  drivers/starnet/rtl8821CU/include/rtw_btcoex_wifionly.h \
  drivers/starnet/rtl8821CU/include/rtw_btcoex.h \
  drivers/starnet/rtl8821CU/include/usb_osintf.h \
  drivers/starnet/rtl8821CU/include/usb_vendor_req.h \
  drivers/starnet/rtl8821CU/include/usb_ops.h \
    $(wildcard include/config/rtl8814.h) \
  drivers/starnet/rtl8821CU/include/usb_ops_linux.h \
  drivers/starnet/rtl8821CU/include/usb_hal.h \
  drivers/starnet/rtl8821CU/hal/phydm/halrf/halrf_features.h \
    $(wildcard include/config/halrf/powertracking.h) \
  drivers/starnet/rtl8821CU/hal/phydm/phydm.h \
    $(wildcard include/config/path/diversity.h) \
    $(wildcard include/config/phydm/antenna/diversity.h) \
    $(wildcard include/config/smart/antenna.h) \
    $(wildcard include/config/ant/detection.h) \
    $(wildcard include/config/dynamic/tx/twr.h) \
    $(wildcard include/config/psd/tool.h) \
    $(wildcard include/config/directional/bf.h) \
    $(wildcard include/config/bb/rf.h) \
    $(wildcard include/config/mcc/dm.h) \
    $(wildcard include/config/phydm/dfs/master.h) \
    $(wildcard include/config/adaptive/soml.h) \
    $(wildcard include/config/hl/smart/antenna.h) \
    $(wildcard include/config/bb/phy/reg.h) \
    $(wildcard include/config/bb/agc/tab.h) \
    $(wildcard include/config/bb/agc/tab/2g.h) \
    $(wildcard include/config/bb/agc/tab/5g.h) \
    $(wildcard include/config/bb/phy/reg/pg.h) \
    $(wildcard include/config/bb/phy/reg/mp.h) \
    $(wildcard include/config/bb/agc/tab/diff.h) \
    $(wildcard include/config/bb/rf/cal/init.h) \
    $(wildcard include/config/rf/radio.h) \
    $(wildcard include/config/rf/txpwr/lmt.h) \
    $(wildcard include/config/rf/syn/radio.h) \
    $(wildcard include/config/fw/nic.h) \
    $(wildcard include/config/fw/nic/2.h) \
    $(wildcard include/config/fw/ap.h) \
    $(wildcard include/config/fw/ap/2.h) \
    $(wildcard include/config/fw/mp.h) \
    $(wildcard include/config/fw/wowlan.h) \
    $(wildcard include/config/fw/wowlan/2.h) \
    $(wildcard include/config/fw/ap/wowlan.h) \
    $(wildcard include/config/fw/bt.h) \
  drivers/starnet/rtl8821CU/hal/phydm/phydm_pre_define.h \
    $(wildcard include/config/mu/jaguar/2.h) \
    $(wildcard include/config/mu/jaguar/3.h) \
    $(wildcard include/config/rfe/by/hw/info.h) \
  drivers/starnet/rtl8821CU/hal/phydm/phydm_features.h \
    $(wildcard include/config/run/in/drv.h) \
    $(wildcard include/config/powersaving.h) \
  drivers/starnet/rtl8821CU/hal/phydm/phydm_features_ce.h \
    $(wildcard include/config/receiver/blocking.h) \
    $(wildcard include/config/8912f/spur/calibration.h) \
    $(wildcard include/config/8822b/spur/calibration.h) \
    $(wildcard include/config/s0s1/sw/antenna/diversity.h) \
    $(wildcard include/config/hl/smart/antenna/type1.h) \
    $(wildcard include/config/hl/smart/antenna/type2.h) \
    $(wildcard include/config/cumitek/smart/antenna.h) \
    $(wildcard include/config/ra/fw/dbg/code.h) \
    $(wildcard include/config/ra/dynamic/rty/limit.h) \
    $(wildcard include/config/bb/txbf/api.h) \
    $(wildcard include/config/phydm/debug/function.h) \
    $(wildcard include/config/dynamic/bypass/mode.h) \
    $(wildcard include/config/dynamic/bypass.h) \
  drivers/starnet/rtl8821CU/hal/phydm/phydm_dig.h \
  drivers/starnet/rtl8821CU/hal/phydm/phydm_soml.h \
  drivers/starnet/rtl8821CU/hal/phydm/phydm_rainfo.h \
  drivers/starnet/rtl8821CU/hal/phydm/phydm_cfotracking.h \
  drivers/starnet/rtl8821CU/hal/phydm/phydm_adaptivity.h \
  drivers/starnet/rtl8821CU/hal/phydm/phydm_dfs.h \
  drivers/starnet/rtl8821CU/hal/phydm/phydm_ccx.h \
  drivers/starnet/rtl8821CU/hal/phydm/txbf/phydm_hal_txbf_api.h \
  drivers/starnet/rtl8821CU/hal/phydm/phydm_adc_sampling.h \
  drivers/starnet/rtl8821CU/hal/phydm/phydm_psd.h \
  drivers/starnet/rtl8821CU/hal/phydm/phydm_cck_pd.h \
  drivers/starnet/rtl8821CU/hal/phydm/phydm_rssi_monitor.h \
  drivers/starnet/rtl8821CU/hal/phydm/phydm_auto_dbg.h \
  drivers/starnet/rtl8821CU/hal/phydm/phydm_math_lib.h \
  drivers/starnet/rtl8821CU/hal/phydm/phydm_noisemonitor.h \
  drivers/starnet/rtl8821CU/hal/phydm/phydm_api.h \
  drivers/starnet/rtl8821CU/hal/phydm/phydm_beamforming.h \
  drivers/starnet/rtl8821CU/hal/phydm/txbf/halcomtxbf.h \
  drivers/starnet/rtl8821CU/hal/phydm/txbf/haltxbfjaguar.h \
  drivers/starnet/rtl8821CU/hal/phydm/txbf/haltxbf8192e.h \
  drivers/starnet/rtl8821CU/hal/phydm/txbf/haltxbf8814a.h \
  drivers/starnet/rtl8821CU/hal/phydm/txbf/haltxbf8822b.h \
  drivers/starnet/rtl8821CU/hal/phydm/txbf/haltxbfinterface.h \
  drivers/starnet/rtl8821CU/hal/phydm/phydm_regtable.h \
  drivers/starnet/rtl8821CU/hal/phydm/halrf/halrf_iqk.h \
  drivers/starnet/rtl8821CU/hal/phydm/halrf/halrf_dpk.h \
  drivers/starnet/rtl8821CU/hal/phydm/halrf/halrf.h \
    $(wildcard include/config/2g/band/shift.h) \
  drivers/starnet/rtl8821CU/hal/phydm/halrf/halrf_psd.h \
  drivers/starnet/rtl8821CU/hal/phydm/halrf/halrf_powertracking.h \
  drivers/starnet/rtl8821CU/hal/phydm/halrf/halphyrf_ce.h \
  drivers/starnet/rtl8821CU/hal/phydm/halrf/halrf_kfree.h \
  drivers/starnet/rtl8821CU/hal/phydm/halrf/rtl8821c/halrf_iqk_8821c.h \
  drivers/starnet/rtl8821CU/hal/phydm/halrf/halrf_powertracking_ce.h \
  drivers/starnet/rtl8821CU/hal/phydm/phydm_hwconfig.h \
    $(wildcard include/config/mp.h) \
    $(wildcard include/config/tc.h) \
  drivers/starnet/rtl8821CU/hal/phydm/phydm_phystatus.h \
  drivers/starnet/rtl8821CU/hal/phydm/phydm_debug.h \
  drivers/starnet/rtl8821CU/hal/phydm/phydm_regdefine11ac.h \
  drivers/starnet/rtl8821CU/hal/phydm/phydm_regdefine11n.h \
    $(wildcard include/config/anta/11n.h) \
  drivers/starnet/rtl8821CU/hal/phydm/phydm_interface.h \
  drivers/starnet/rtl8821CU/hal/phydm/phydm_reg.h \
    $(wildcard include/config/wlan/hal/8814ae.h) \
    $(wildcard include/config/wlan/hal/8198f.h) \
    $(wildcard include/config/wlan/hal/8822ce.h) \
    $(wildcard include/config/wlan/hal/8814be.h) \
    $(wildcard include/config/wlan/hal/8812fe.h) \
    $(wildcard include/config/wlan/hal/8197g.h) \
  drivers/starnet/rtl8821CU/hal/phydm/halrf/halrf_debug.h \
  drivers/starnet/rtl8821CU/hal/phydm/rtl8821c/phydm_hal_api8821c.h \
    $(wildcard include/config/version/8821c.h) \
  drivers/starnet/rtl8821CU/hal/phydm/rtl8821c/halhwimg8821c_mac.h \
  drivers/starnet/rtl8821CU/hal/phydm/rtl8821c/halhwimg8821c_bb.h \
  drivers/starnet/rtl8821CU/hal/phydm/rtl8821c/phydm_regconfig8821c.h \
    $(wildcard include/config/h/8821c.h) \
  drivers/starnet/rtl8821CU/hal/phydm/halrf/rtl8821c/halrf_8821c.h \
  drivers/starnet/rtl8821CU/hal/phydm/halrf/rtl8821c/halhwimg8821c_rf.h \
    $(wildcard include/config/xxx/drv/dis.h) \
    $(wildcard include/config/8821c/drv/dis.h) \
    $(wildcard include/config/8821c/type0x20/drv/dis.h) \
    $(wildcard include/config/8821c/type0x28/drv/dis.h) \
    $(wildcard include/config/8821c/fccsar/drv/dis.h) \
    $(wildcard include/config/8821c.h) \
    $(wildcard include/config/8821c/type0x20.h) \
    $(wildcard include/config/8821c/type0x28.h) \
    $(wildcard include/config/8821c/fccsar.h) \
  drivers/starnet/rtl8821CU/hal/phydm/halrf/rtl8821c/version_rtl8821c_rf.h \
  drivers/starnet/rtl8821CU/hal/phydm/rtl8821c/version_rtl8821c.h \
  drivers/starnet/rtl8821CU/include/rtl8821c_hal.h \
  drivers/starnet/rtl8821CU/include/../hal/halmac/halmac_api.h \
  drivers/starnet/rtl8821CU/include/../hal/halmac/halmac_2_platform.h \
  drivers/starnet/rtl8821CU/include/../hal/halmac/halmac_type.h \
  drivers/starnet/rtl8821CU/include/../hal/halmac/halmac_hw_cfg.h \
    $(wildcard include/config/rtl8723a.h) \
    $(wildcard include/config/rtl8881a.h) \
    $(wildcard include/config/rtl8821b.h) \
    $(wildcard include/config/rtl8821bmp.h) \
    $(wildcard include/config/rtl8814amp.h) \
    $(wildcard include/config/rtl8195a.h) \
    $(wildcard include/config/rtl8196f.h) \
    $(wildcard include/config/rtl8197f.h) \
    $(wildcard include/config/rtl8198f.h) \
    $(wildcard include/config/rtl8197g.h) \
    $(wildcard include/config/rtl8812f.h) \
  drivers/starnet/rtl8821CU/include/../hal/halmac/halmac_fw_info.h \
  drivers/starnet/rtl8821CU/include/../hal/halmac/halmac_intf_phy_cmd.h \
  drivers/starnet/rtl8821CU/include/../hal/halmac/halmac_state_machine.h \
  drivers/starnet/rtl8821CU/include/../hal/halmac/halmac_usb_reg.h \
  drivers/starnet/rtl8821CU/include/../hal/halmac/halmac_sdio_reg.h \
  drivers/starnet/rtl8821CU/include/../hal/halmac/halmac_pcie_reg.h \
  drivers/starnet/rtl8821CU/include/../hal/halmac/halmac_bit2.h \
  drivers/starnet/rtl8821CU/include/../hal/halmac/halmac_reg2.h \
  drivers/starnet/rtl8821CU/include/../hal/halmac/halmac_reg_8821c.h \
  drivers/starnet/rtl8821CU/include/../hal/halmac/halmac_bit_8821c.h \
  drivers/starnet/rtl8821CU/include/../hal/halmac/halmac_tx_desc_nic.h \
  drivers/starnet/rtl8821CU/include/../hal/halmac/halmac_tx_desc_buffer_nic.h \
  drivers/starnet/rtl8821CU/include/../hal/halmac/halmac_tx_desc_ie_nic.h \
  drivers/starnet/rtl8821CU/include/../hal/halmac/halmac_rx_desc_nic.h \
  drivers/starnet/rtl8821CU/include/../hal/halmac/halmac_tx_bd_nic.h \
  drivers/starnet/rtl8821CU/include/../hal/halmac/halmac_rx_bd_nic.h \
  drivers/starnet/rtl8821CU/include/../hal/halmac/halmac_fw_offload_c2h_nic.h \
  drivers/starnet/rtl8821CU/include/../hal/halmac/halmac_fw_offload_h2c_nic.h \
  drivers/starnet/rtl8821CU/include/../hal/halmac/halmac_h2c_extra_info_nic.h \
  drivers/starnet/rtl8821CU/include/../hal/halmac/halmac_original_c2h_nic.h \
  drivers/starnet/rtl8821CU/include/../hal/halmac/halmac_original_h2c_nic.h \
  drivers/starnet/rtl8821CU/include/../hal/halmac/halmac_tx_desc_chip.h \
  drivers/starnet/rtl8821CU/include/../hal/halmac/halmac_rx_desc_chip.h \
  drivers/starnet/rtl8821CU/include/../hal/halmac/halmac_tx_desc_buffer_chip.h \
  drivers/starnet/rtl8821CU/include/../hal/halmac/halmac_tx_desc_ie_chip.h \
  drivers/starnet/rtl8821CU/include/hal_data.h \
    $(wildcard include/config/single/img.h) \
    $(wildcard include/config/sdio/tx/enable/aval/int.h) \
  drivers/starnet/rtl8821CU/include/../hal/phydm/phydm_precomp.h \
  drivers/starnet/rtl8821CU/include/hal_btcoex.h \
  drivers/starnet/rtl8821CU/include/hal_btcoex_wifionly.h \
    $(wildcard include/config/btcoex/support/wifi/only/cfg.h) \
    $(wildcard include/config/btcoex/support/btc/cmn.h) \
  drivers/starnet/rtl8821CU/include/hal_data.h \
  drivers/starnet/rtl8821CU/include/rtl8821c_spec.h \
  drivers/starnet/rtl8821CU/include/../hal/rtl8821c/hal8821c_fw.h \
  drivers/starnet/rtl8821CU/include/rtl8821cu_hal.h \

drivers/starnet/rtl8821CU/hal/phydm/phydm_cfotracking.o: $(deps_drivers/starnet/rtl8821CU/hal/phydm/phydm_cfotracking.o)

$(deps_drivers/starnet/rtl8821CU/hal/phydm/phydm_cfotracking.o):
