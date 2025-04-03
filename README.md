| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 | Linux |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | -------- | -------- | -------- | ----- |

# Hello World Example

Starts a FreeRTOS task to print "Hello World".

(See the README.md file in the upper level 'examples' directory for more information about examples.)

## How to use example

Follow detailed instructions provided specifically for this example.

Select the instructions depending on Espressif chip installed on your development board:

- [ESP32 Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/stable/get-started/index.html)
- [ESP32-S2 Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s2/get-started/index.html)


## Example folder contents

The project **hello_world** contains one source file in C language [hello_world_main.c](main/hello_world_main.c). The file is located in folder [main](main).

ESP-IDF projects are built using CMake. The project build configuration is contained in `CMakeLists.txt` files that provide set of directives and instructions describing the project's source files and targets (executable, library, or both).

Below is short explanation of remaining files in the project folder.

```
├── CMakeLists.txt
├── pytest_hello_world.py      Python script used for automated testing
├── main
│   ├── CMakeLists.txt
│   └── hello_world_main.c
└── README.md                  This is the file you are currently reading
```

For more information on structure and contents of ESP-IDF projects, please refer to Section [Build System](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/build-system.html) of the ESP-IDF Programming Guide.

## Troubleshooting

* Program upload failure

    * Hardware connection is not correct: run `idf.py -p PORT monitor`, and reboot your board to see if there are any output logs.
    * The baud rate for downloading is too high: lower your baud rate in the `menuconfig` menu, and try again.

## Technical support and feedback

Please use the following feedback channels:

* For technical queries, go to the [esp32.com](https://esp32.com/) forum
* For a feature request or bug report, create a [GitHub issue](https://github.com/espressif/esp-idf/issues)

We will get back to you as soon as possible.


python -m esptool --chip esp32 -b 460800 --before default_reset --after hard_reset  --port COM3 write_flash --flash_mode dio --flash_size detect --flash_freq 40m 0x1000 bootloader/bootloader.bin 0x10000 HelloWorld.bin 0x8000 partition_table/partition-table.bin 0x110000 ../ManufacturingData/bin/prefix-1.bin


#Run in container: dopo il clone, ho aperto un terminal ESP-IDF e ho seguito i due step seguenti:
1- Run the following CMake command to configure the project and generate the build.ninja file:
    cmake -S . -B build
2- After the configuration is complete, you can build the project using ninja:
    ninja -C build

Da quel momento ha iniziato a compilare perfettamente.

Da li in poi bisogna inoltrare la porta USB a WSL per poter flashare:
https://docs.espressif.com/projects/vscode-esp-idf-extension/en/latest/additionalfeatures/docker-container.html

praticamente se sei già a posto col wsl basta trovare i dispositivi usb. Nella powershell esegui:
usbipd list
e poi attaccare quello che hai tu (sostituisci 2-6). Sempre nella powershell:
usbipd attach --wsl --busid 2-6 --auto-attach
a quel punto devi configurare il dispositivo giusto dentro all'ide, nel mio caso deve chiamarsi /dev/ttyACM0. Esegui nel terminal di visual studio code in esecuzione in container:
dmesg | tail
dovrebbe apparire qualcosa del genere. Quello è il nome del tuo dispositivo usb:
[331222.483371] cdc_acm 1-1:1.0: ttyACM0: USB ACM device
quindi sempre in VS Code apri la cartella .vscode, file settings.json e modifica idf.port con il giusto nome:
  "idf.port": "/dev/ttyACM0",
A quel punto dovresti esserci. Se non si apre riavvia VS Code