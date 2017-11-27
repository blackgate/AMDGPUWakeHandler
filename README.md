#  AMDGPUWakeHandler

This kernel extension disables the AMD GPU after waking up from sleep. It is intended to be used on a 2011 MacBook Pro with a failed AMD GPU.

**NOTE: this should only be used with the grub solution ([found here](https://gist.github.com/blackgate/17ac402e35d2f7e0f1c9708db3dc7a44)) or similar that powers down the discrete GPU before booting macOS, otherwise it will fail to load.**

## Installation

After building using Xcode, copy the kext to `/Library/Extensions` and run the following commands from terminal:

```
sudo chown -R root:wheel /Library/Extensions/AMDGPUWakeHandler.kext
sudo touch /Library/Extensions
```

Reboot.

## Manual loading

If you prefer to load the kext manually, then after building the kext, copy it to a location of your choice and run the following command to change its permissions:

```
sudo chown -R root:wheel /path/to/AMDGPUWakeHandler.kext
```

Then when you want to load the kext you can simply run the command:

```
sudo kextload /path/to/AMDGPUWakeHandler.kext
```

To unload:

```
sudo kextunload /path/to/AMDGPUWakeHandler.kext
```

## View logs

To view the logs for the last 24 hours run the following on the terminal:
```
log show --last 24h --predicate 'senderImagePath contains "AMDGPUWakeHandler"'
```
