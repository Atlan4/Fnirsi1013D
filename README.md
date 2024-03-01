Fnirsi1013D
Oscilloscope  FNIRSI1013D alternative opensource firmware

No one is responsible for anything, you do everything at your own risk.

More information can be found in the manual.

Everything is described on the pages https://www.eevblog.com/forum/testgear/fnirsi-1013d-100mhz-tablet-oscilloscope/msg5172177/#msg5172177

The RTC module is not required for the function of the alternative software. The RTC module does not include an oscilloscope, it must be installed additionally.  And it is disabled by default.

In case of problems with the touch screen, see this: configuration_file.txt

To upload the firmware under Windows, download the LOADER.zip file, unzip it and run it. Turn on the oscilloscope, connect the USB cable, select the option to turn on USB in the oscilloscope menu. After about 10 seconds, windows will recognize the device and open a window with folders. Now in the loader program, select the BIN you want to load. Upload it to the device, touch the screen to turn off the usb, and turn off the oscilloscope.

https://www.youtube.com/watch?v=LVI45oJHTK8

CALIBRATION
Follow the instructions for calibration.  You must have the notification confirmation option turned on. Calibrate without usb and probes.  In the next step, when prompted, connect the required voltage to both channels at once.  After finishing the calibration, close the menu and turn off the oscilloscope to save the data.

Loader allows you to return the oscilloscope card to the previous firmware, the new firmware is stored only on the SD card, where the original is loaded in the flash of the device.
The safe solution is to use another SD card. Upload the firmware there, and insert the cards in the device.


Original source https://github.com/pecostm32/FNIRSI_1013D_Firmware
