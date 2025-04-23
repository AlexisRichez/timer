<img src="./sliders_logo.png">

## Table of Contents
1. [Introduction](#introduction)
2. [Schematics](#schematics)
3. [PCB](#pcb)
4. [Power Supply](#power-supply)
5. [Hardware](#hardware)
6. [Sources](#sources)
7. [Remaining tasks](#remaining-tasks)

## Introduction
### About the story...
Sliders is a 1995 sci-fi television series :tv: that follows Quinn Mallory, a young genius who invents a device called the "Timer" that allows him and a small group of friends—Wade, Rembrandt, and Professor Arturo—to travel between parallel universes. Each universe presents a different version of Earth with unique cultures, histories, and technologies. The group "slides" from one world to the next, searching for their way back home while facing various challenges along the way.

The "Timer" is a handheld device that controls the sliding process, dictating how long the group must stay in each universe before they can slide again. It opens a wormhole :cyclone: at set intervals, and if the group misses their window, they are stranded in the current universe. The Timer becomes central to the story, especially when it malfunctions or is damaged, leading to unpredictable slides or dangerous situations.

### This is Open Source !
<b>Source Code:</b> You can download and modify the entire source code to suit your requirements. Whether you want to fix a bug, add new features, or create your own version, the choice is yours!

<b>3D Models:</b> All 3D models I use are accessible and can be adjusted. If you want to tweak the designs or use them for your own projects, you are free to do so.

<b>Vector Files:</b> Any vector graphic files I provide can also be edited. This includes things like logos, UI components, and other design assets that you can adapt for personal use.

> [!IMPORTANT]
> Feel free to fork this project for your own needs, or contribute directly by making a pull request :blush: :heart:

## Schematics
<img src="05 - schematics/timer_schematics.png" width="50%" height="50%"><br/>
## PCB
Size : 137mm x 50mm
## Power supply
### LiPo 3,7V 3000mAh 1S 1C
https://www.amazon.fr/dp/B08V11Z88Q
### Wemos D1 Battery Shield
The Wemos D1 Battery Shield is an expansion board designed for the Wemos D1 Mini series, allowing it to run on a lithium-ion (Li-Ion) or lithium-polymer (LiPo) battery. It features a TP4056 charging circuit for safe battery charging via micro-USB and includes a boost converter to provide a stable 5V output from the battery. Additionally, it has built-in overcharge and discharge protection to extend battery life and ensure safe operation. This shield is ideal for portable IoT projects, wireless sensor nodes, and other battery-powered applications.

<img src="06 - images/references/battery_shield.jpg" width="15%" height="15%"><br/>

https://www.wemos.cc/en/latest/d1_mini_shield/battery.html
## Hardware
### Bill of materials
| Component                                             | Link |
|-------------------------------------------------------|-------------|
| 7 segment red LED digit - 0.36 inch cathode (HH:MM:SS) | https://fr.aliexpress.com/item/1005002680227414.html |
| 7 segment red LED digit - 0.3 inch cathode (DDD)       | https://fr.aliexpress.com/item/32726538415.html |
| 10 green LEDs bargraph                                 | https://fr.aliexpress.com/item/1005006697608767.html      |
| Yellow, red, green LEDs (TAU, DELTA, ZETA)             | https://fr.aliexpress.com/item/1005005678632501.html |
| Red LEDs (between HH:MM:SS)                            | https://fr.aliexpress.com/item/1005005678632501.html |
| Light LEDs (top laser) | https://fr.aliexpress.com/item/32468733055.html |
| Buzzer | https://fr.aliexpress.com/item/1005006209077202.html |
| Potentiometer 10k                                      | https://fr.aliexpress.com/item/1005005671238738.html |
| Red acrylic sheets                                     | [https://fr.aliexpress.com/item/4000283087852.html](https://fr.aliexpress.com/item/4000283087852.html) |
| Translucent acrylic sheets                                     | [https://fr.aliexpress.com/item/4000283087852.html](https://fr.aliexpress.com/item/1005004925861639.html) |
| Arduino Nano V3 | [https://www.amazon.fr/dp/B0722YYBSS](https://www.amazon.fr/dp/B0722YYBSS) |

### Component Dimensions
> [!TIP]
> These are the dimensions used for the project. You should be able to use any component that adheres to these dimensions without needing to adapt anything on the PCB.

| Component                                             | Length (mm) | Width (mm) | Height (mm) |
|-------------------------------------------------------|-------------|------------|-------------|
| 7 segment red LED digit - 0.36 inch cathode (HH:MM:SS) | 14.00       | 7.40       | 7.20        |
| 7 segment red LED digit - 0.3 inch cathode (DDD)       | 12.70       | 7.60       | 7.00        |
| 10 green LEDs bargraph                                 | 25.40       | 10.10      | 7.90        |
| Yellow, red, green LEDs (TAU, DELTA, ZETA)             | 5.00        | 2.00       | 7.00        |
| Red LEDs (between HH:MM:SS)                            | 3.00        | 2.00       | 4.00        |
| Potentiometer 10k                                      | -           | -          | -           |

## Assembly tips
The holes are designed for use with M2 or M3 inserts. Depending on the 3D printing technology used, the hole dimensions may need to be adjusted to ensure a precise fit:

- For filament prints :<br/>
<img src="06 - images/references/M3_inserts.jpg" width="150">

- For resin prints :<br/>
<img src="06 - images/references/M3_inserts_resin.jpg" width="150">

## 3D models
Timer 3D model preview : https://3dviewer.net/#model=https://raw.githubusercontent.com/AlexisRichez/timer/main/03%20-%20fusion360/Timer.fbx<br/>
PCB 3D model preview : https://3dviewer.net/#model=https://raw.githubusercontent.com/AlexisRichez/timer/main/01%20-%20proteus/pcb.3DS

<div style="float:left">
<img src="06 - images/3d models/Timer.png" width="150">
<img src="06 - images/3d models/Main body.png" width="150">
<img src="06 - images/3d models/Top body.png" width="150">
<img src="06 - images/3d models/Bottom body.png" width="150">
<img src="06 - images/3d models/Battery box.png" width="150">
<img src="06 - images/3d models/Battery case.png" width="150">
<img src="06 - images/3d models/Pcb front.png" width="75" height="160">
<img src="06 - images/3d models/Pcb back.png" width="75" height="160">
</div>
<hr style="clear:both"/>

## Sources
+ [https://www.therpf.com/forums/threads/how-to-build-sliders-timer-electronics.104308/](https://www.therpf.com/forums/threads/how-to-build-sliders-timer-electronics.104308/)
+ https://github.com/kenny-caldieraro/Sliders-timer-replica

## Remaining tasks
:tada: Issues and improvments are available here : https://github.com/AlexisRichez/timer/issues<br/>

## Pictures :camera:
<div style="float:left">
<img src="06 - images/001.jpg" width="150" height="150">
<img src="06 - images/002.jpg" width="150" height="150">
<img src="06 - images/003.jpg" width="150" height="150">
<img src="06 - images/004.jpg" width="150" height="150">
<img src="06 - images/005.jpg" width="150" height="150">
<img src="06 - images/010.jpg" width="150" height="150">
<img src="06 - images/011.jpg" width="150" height="150">
<img src="06 - images/020.jpg" width="150" height="150">
<img src="06 - images/021.jpg" width="150" height="150">
<img src="06 - images/022.jpg" width="150" height="150">
<img src="06 - images/030.jpg" width="150" height="150">
<img src="06 - images/031.jpg" width="150" height="150">
<img src="06 - images/032.jpg" width="150" height="150">
<img src="06 - images/033.jpg" width="150" height="150">
<img src="06 - images/034.jpg" width="150" height="150">
<img src="06 - images/035.jpg" width="150" height="150">
<img src="06 - images/040.jpg" width="150" height="150">
<img src="06 - images/041.jpg" width="150" height="150">
</div>
