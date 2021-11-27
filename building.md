# StepSequencer Software Project


## Building the Project

Use the vscode command palette:

1. CMake: select a kit

    - `x86_64-linux-gnu`
    - `arm-none-eabi`

2. CMake: Delete Cache and Reconfigure
3. CMake: Clean Rebuild

## Running the Catch2 unit tests

1. Build the `x86_64-linux-gnu` target 
2. Select the `Host Debug` debug config
3. Uncomment an argument in launch.json to run a specific test
4. Press F5 to start debugging.

## Running GCOVR and displaying code coverage report

1. Build the `x86_64-linux-gnu` target
2. Select the `Host Debug` debug config
3. Using the command palette, run the Task: `coverage report`

Latest code coverage report can be found [here](coverage/code.pdf)

## Debuggin/Downloading to STM32 target

1. Build the `arm-none-eabi` target 
2. Select the `Cortex-Debug` debug config
3. Connect the device to your PC via SEGGER JLink probe.
4. Press F5 to start debugging.