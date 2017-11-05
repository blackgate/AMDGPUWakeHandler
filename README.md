#  AMDGPUWakeHandler

This kernel extension disables the AMD GPU after waking up from sleep. It is intended to be used on a 2011 MacBook Pro with a failed AMD GPU.

**NOTE: this should only be used with the grub solution (found here) or simillar that prevents the AMD kexts from being loaded, otherwise it will kernel panic.**

## Installation

After building using Xcode, copy the kext to `/Library/Extensions` and run the following commands from terminal:

```
sudo chown -R root:wheel /Library/Extensions/AMDGPUWakeHandler.kext
touch /Libary/Extensions
```

Reboot.

## Manual loading

If you prefer to load the kext manually, then after building the kext, copy it to a location of your choice and the run the following command to change its permissions:

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
