# StepSequencer Software Project

## Table of Contents
1. [Respository Contents](#Repository-Contents)

2. [Requirements](#Requirements)

3. [Submodules](#Submodules)

4. [STM32CubeIDE](#STM32CubeIDE)



## Repository Contents

<pre><font color="#3465A4"><b>.</b></font> 
├── <font color="#3465A4"><b>source</b></font>
│   ├── <font color="#3465A4"><b>cpp_ssd1306</b></font> - oled display library
│   ├── <font color="#3465A4"><b>cpp_tlc5955</b></font> - led driver library
│   ├── <font color="#06989A"><b>cube_gen_src</b></font> - code gen from STM32CubeIDE
│   ├── <font color="#06989A"><b>hal_drivers</b></font> - HAL gen from STM32CubeIDE
│   ├── <font color="#3465A4"><b>main_app</b></font> - Application
│   └── <font color="#3465A4"><b>segger_rtt</b></font> - debug printf support
└── <font color="#3465A4"><b>stm32cube_workspace</b></font> - STM32CubeIDE project

</pre>

## Requirements

This project uses:

- `gcc` compiler. Recommended to use `gcc-arm-none-eabi-9-2020-q2-update` with STM32G0B1 devices.
- `cmake v3.22` to build the project. it is recommened to use VSCode with the `CMake tools` extension.
- `SEGGER JLink` to debug the device. You will need to purchase [JLink probe from SEGGER](https://www.segger.com/products/debug-probes/j-link/).  You also need to install [JLink SW and doc pack](https://www.segger.com/downloads/jlink/)
- `SEGGER RTT` to view printf output. JLinkRTTViewerExe is included in the JLink SW pack.
- `STM32CubeIDE v1.6.1` to generate the startup file, linker script and the HAL related library files. These are symlinked to the files in the stm32_workspace. 

## Submodules

This project uses the `cpp_ssd1306` and `cpp_tlc5955` submodule projects.

After cloning, cd into the root directory and run `git submodule update --init --recursive`

## STM32CubeIDE workspace

STM32CubeIDE uses an Eclipse IDE that has some quirks(!)

To open the project

- open the workspace in the `stm32_workspace` directory
- Use `File > Import... > General > Existing project into Workspace` to import the project and update the paths. You should now see the `G0B1KET6N` project in the STM32CubeIDE Project Explorer.
- Double-click the `G0B1KET6N.ioc` file to open the Device Config Tool.
- After making device changes, use the generate icon in the toolbar to update the project code.
