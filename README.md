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

- `VSCode`. This is the main IDE.
- `gcc` compiler. 
    - For TM32G0B1 target: `gcc-arm-none-eabi-9-2020-q2-update` 
    - For x86_64 Linux CTests: `x86_64-linux-gnu-9`
- `cmake v3.22` to build the project. it is recommened to use VSCode with the `CMake tools` extension.
- `SEGGER JLink` to debug the device. You will need to purchase [JLink probe from SEGGER](https://www.segger.com/products/debug-probes/j-link/).  You also need to install [JLink SW and doc pack](https://www.segger.com/downloads/jlink/)
- `SEGGER RTT` to view printf output. JLinkRTTViewerExe is included in the JLink SW pack.
- `STM32CubeIDE v1.6.1` to generate the startup file, linker script and the HAL related library files. _This IDE is not used to compile the project._

## Installing the project

This project uses the `cpp_ssd1306` and `cpp_tlc5955` submodule projects.

After cloning, cd into the root directory and run `git submodule update --init --recursive`

## Updating the STM32CubeIDE generated code

STM32CubeIDE uses an Eclipse IDE that has some quirks(!)

To open the project

- open the workspace in the `stm32_workspace` directory
- Use `File > Import... > General > Existing project into Workspace` to import the project and update the paths. You should now see the `G0B1KET6N` project in the STM32CubeIDE Project Explorer.
- Double-click the `G0B1KET6N.ioc` file to open the Device Config Tool.
- After making device changes, use the generate icon in the toolbar to update the project code.

## Building the Project

The CMake configurations do not set the toolchain. This is done via the command line. These can be selected using the kit selection in the VSCode toolbar (if you have CMake Tools extension installed):

### Build the STM32 Target

- Select a kit that contains the target triplet
`gcc-arm-none-eabi`

    OR

- Configure the cmake environment with:

    `cmake --no-warn-unused-cli -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_BUILD_TYPE:STRING=Debug -DCMAKE_C_COMPILER:FILEPATH=/opt/gcc-arm-none-eabi-9-2020-q2-update/bin/arm-none-eabi-gcc-9.3.1 -H. -B./build -G Ninja`

- Build the project with:

    `cmake --build build --config Debug --clean-first --target build -j 10 --`

### Build the Linux Target (for CTest)


- Select a kit that contains the target triplet
`x86_64-linux-gnu`

    OR

- Configure the CMake environment with:

    `cmake --no-warn-unused-cli -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_BUILD_TYPE:STRING=Debug -DCMAKE_C_COMPILER:FILEPATH=/bin/x86_64-linux-gnu-gcc-9 -DCMAKE_CXX_COMPILER:FILEPATH=/bin/x86_64-linux-gnu-g++-9 -H. -B./build -G Ninja`

- Build the project with:

    `cmake --build build --config Debug --clean-first --target build -j 10 --`

- Run the CTest with the `Run CTest tests` toolbar button

    OR

- Run the CTest command:

    `pushd build; ctest -j10 -C Debug -T test --output-on-failure; popd;`


## Running the Project

VScode will allow debugging via the built-in IDE debugger with the `.vscode/launch.json` configuration file.

You will need to launch and connect JLinkRTTViewerEXE seperately.