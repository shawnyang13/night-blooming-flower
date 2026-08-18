# night-blooming-flower
Auto light charging night-blooming flower shines in the darkness...

## 多模式 UV 花灯

当前默认固件是 `Firmware/flower_lamp_mic`：

- MAX9814 麦克风：`OUT -> GPIO34`
- CN5711 UV 灯驱动：`GPIO23 -> CE`
- 4 脚复位模式按钮：`GPIO27 -> 按钮 -> GND`
- 开机静音 2 秒做底噪校准
- 通过模式按钮切换不同灯效，其中声控律动模式会让 UV 灯亮度跟随声音强度变化
- 重要参数在 `Firmware/flower_lamp_mic/lamp_config.conf` 中配置，运行 `bash burst/flash.sh` 或 `bash burst/flash.sh -c` 编译时会自动生成固件常量

接线图：

- `Docs/flower_lamp_mic_cn5711_switch_wiring.svg`

### 模式按钮

模式按钮只负责给 ESP32 切换程序模式，电源开关单独控制灯电源。

每按一次 `GPIO27` 上的复位按钮，模式按下面顺序循环，并保存到 ESP32：

```text
声控律动 -> 固定呼吸 -> 常亮低亮 -> 关灯 -> 声控律动
```

模式说明：

- 声控律动：麦克风采集到的相对声压越大，UV 灯 PWM 亮度越高；安静时缓慢回落到底亮
- 固定呼吸：不看麦克风，按固定节奏柔和明暗变化
- 常亮低亮：保持低亮度常亮
- 关灯：PWM 输出 0，UV 灯关闭

4 脚复位按钮只需要接两根线：

```text
GPIO27 -> 按钮一侧任意一个脚
GND    -> 按钮另一侧任意一个脚
```

用万用表确认这两个脚满足：

```text
不按：不通
按下：导通
```

不要把按钮两侧额外短接；否则会变成常通，ESP32 会一直认为按钮被按下。

编译并烧录：

```bash
bash burst/flash.sh
```

只编译不烧录：

```bash
bash burst/flash.sh -c
```
