# ESP32 烧录操作手册

本目录用于在 WSL 里编译并烧录 ESP32 固件。

## 1. 准备烧录工具

在项目根目录运行：

```bash
bash burst/prepare.sh
```

这个脚本会把 `arduino-cli` 安装到：

```text
burst/tools/bin/arduino-cli
```

并安装 ESP32 Arduino core。

查看准备脚本说明：

```bash
bash burst/prepare.sh --help
```

常用参数：

```bash
bash burst/prepare.sh -S
bash burst/prepare.sh -a ~/.local/bin/arduino-cli
bash burst/prepare.sh --skip-core-install
```

## 2. 把 ESP32 USB 挂载到 WSL

在 Windows PowerShell 里运行：

```powershell
usbipd list
usbipd attach --wsl --busid <ESP32_BUSID>
```

然后在 WSL 里检查：

```bash
lsusb
ls /dev/ttyUSB*
```

正常会看到 CH340 设备和 `/dev/ttyUSB0`。

## 3. 烧录固件

在项目根目录运行：

```bash
bash burst/flash.sh
```

默认烧录：

```text
Firmware/flower_lamp_timer
```

默认串口：

```text
/dev/ttyUSB0
```

如果串口不是 `/dev/ttyUSB0`，可以指定：

```bash
bash burst/flash.sh -p /dev/ttyUSB1
```

查看烧录脚本说明：

```bash
bash burst/flash.sh --help
```

常用参数：

```bash
bash burst/flash.sh -p /dev/ttyUSB0
bash burst/flash.sh -c
bash burst/flash.sh -u
bash burst/flash.sh -s Firmware/flower_lamp_timer -p /dev/ttyUSB0
bash burst/flash.sh --sketch Firmware/flower_lamp_timer --port /dev/ttyUSB0
```

## 4. 当前固件效果

```text
GPIO23 输出 PWM
每 1 分钟亮一次
亮 20 秒
灭 40 秒
亮灯时呼吸效果
最大亮度 50%
```

## 5. 常见问题

如果提示找不到串口，先重新在 Windows PowerShell 里执行 `usbipd attach`。

如果提示没有串口权限，`flash.sh` 会自动尝试：

```bash
wsl.exe -u root -- chmod a+rw /dev/ttyUSB0
```

如果上传时一直 `Connecting...`，按住开发板 `BOOT` 键，再重新运行烧录脚本。

## 6. 声控呼吸灯固件（麦克风版）

`Firmware/flower_lamp_mic` 是另一套固件，灯的亮度跟随麦克风采集到的音乐音量包络呼吸（响的时候亮、静的时候暗），接线见 `Docs/mic_max9814_wiring.svg`。

烧录这套固件：

```bash
bash burst/flash.sh -s Firmware/flower_lamp_mic
```
