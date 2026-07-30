# night-blooming-flower
Auto light charging night-blooming flower shines in the darkness...

## 声控呼吸灯

当前默认固件是 `Firmware/flower_lamp_mic`：

- MAX9814 麦克风：`OUT -> GPIO34`
- CN5711 UV 灯驱动：`GPIO23 -> CE`
- 开机静音 2 秒做底噪校准
- 麦克风采集到的相对声压越大，UV 灯 PWM 亮度越高；安静时缓慢回落到底亮

编译并烧录：

```bash
bash burst/flash.sh
```

只编译不烧录：

```bash
bash burst/flash.sh -c
```
