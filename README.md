# Black80211

This project aims to be used like a simulator of IEEE80211 stack for MacOS.

## Try

Download latest kext for your system in releases. Execute following commands:

```
sudo chown -R root:wheel Black80211.kext
sudo kextutil Black80211.kext
```

## Warning

Project currently in alpha stage. Do not install kext in your `/Library/Extensions` or `/System/Library/Extensions`
folders, because it may break whole your system.

## Known issues

- Unable to unload kext
