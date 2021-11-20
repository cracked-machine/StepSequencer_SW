## StepSequencer Software Project

### Requirements

This project uses 

- `cmake` to build the project. it is recommened to use VSCode with the `CMake tools` extension.
- `SEGGER JLink` to debug the device. You should install [JLink SW and doc pack](https://www.segger.com/downloads/jlink/)
- `SEGGER RTT` to view printf output. JLinkRTTViewerExe is included in the JLink SW pack.
- `STM32CubeIDE` to generate the startup file, linker script and the HAL related library files. These are symlinked to the files in the stm32_workspace. 

### Submodules

This project uses the `cpp_ssd1306` and `cpp_tlc5955` submodule projects.

After cloning, cd into the root directory and run `git submodule update --init --recursive`

#### STM32CubeIDE

This is an Eclipse IDE that has some quirks(!)

To open the project

- open the workspace in the `stm32_workspace` directory
- Use `File > Import... > General > Existing project into Workspace` to import the project and update the paths. You should now see the `G0B1KET6N` project in the STM32CubeIDE Project Explorer.
- Double-click the `G0B1KET6N.ioc` file to open the Device Config Tool.
- After making device changes, use the generate icon in the toolbar to update the project code.
