# Struggle Logs:
Yeah, basically me struggling to obtain minimum system stability with somewhat-decent feature population and without kernel paniking too often...
## Uboot bootargs for fixed pointing to eeprom
bootargs root=/dev/mmcbl0p7 console=ttyS0,115200n1

## Main pkgs and kmodules:
#### Root Deps:
- kmod-fs-ext4
- kmod-mtd-rw
- kmod-loop
- kmod-mt7622 (cross compiled .dts from LEDE 4.4)
#### Mtks:
- mtk_loop -> Disabled
- mtk_block2mtd -> Disabled
- mtk_vfat
#### Utility Packages:
- e2fsprogs
- gdisk

	

## Steps and obs:
### Enabling wlan interface (default removed):
IN` $(master)/build_dir/target-aarch64_cortex.../linux-mediatek_mt7622/linux-$(kernel_ver)/arch/arm64/boot/dts/mediatek/$(target).dts`:
	
	&wmac {
		mediatek,rf-conf = "mt7622_rf_conf.bin"; //maybe useless
		status="okay";
	};
### Kernel configs && menuconfigs:
mtk_loop -> DISABLE

mtk_block2mtd -> DISSABLE

**Without these mmc will not be mounted:**

`cannot open blockdev (err -6)`
- MTD_BLOCK2MTD 
- MMC_BLOCK
- MTD_CMDLINE_PARTS.(Kernel) 

**To be able to mount squashfs file and derivates**
- mtk_vfat package
- kmod-mtd-rw

**Enabling fs for loopback overlay**

`mount -t ext4 loop0: no such device`
- kmod-fs-ext4 

### Wifi Tx power fixed to 6dBm:
* **OBSERVATIONS & ANNOTATIONS:**
	- WiFi Driver does not log any error o warning whatsoever (mt7622.tk)
	- Factory partition been successfully loaded 
	- Host system: squashfs + ext4 overlay 
* **POSSIBLE CAUSES:**
	- Eeprom with calibration data is ignored and not loaded during kernel boot routine
	- 6dBm seems to be a fallback fixed value for a pseudo-failsafe mode
	- Observed multiple case scenario in different enviroments where this value persist when something goes wrong with Factory part.
	- Presuming 6dBm is hardcoded in the driver interface.
* **FIX:**
	- Inject `mt76_get_eeprom_file(dev,len)` function to `$openwrtRoot/build_dir/target-aarch64_cortex.../linux-mediatek_mt7622/mt76-$snapshot_id/eeprom.c`
	- Recompile the modified target
	- Compile the mt76 driver or copy a ready .bin with calibration data
	- Copy the calibration '.bin' data to `/lib/firmware/mediatek/$($(driver_name)+"_rf.bin")` (i.e: 'mt7622_wmac_rf.bin') onto the host system
	- Reboot (and prey).
	
### Wifi Disconnection & wifi driver crash
* **OBS & ANN**
	- See `LASTDUMP${N}.log` files for entire report
	- WiFi every ~12h. crash during normal routine
	- Crash randomly
	- Kernel spit out time out error for wlan int.
	- Sometimes in the worse case scenario the system HALT completely - soft reboot switch needed
* **P.C**
	- dk mate
* **FIX:**
	- Seems that changing the auth protocol form TKIP to CCMP solved the issue (UP 6 days without any error)
	- BUT CCMP seems to slow down the overall performance - possible latency issue with driver?
