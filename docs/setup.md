To fix the Insufficient permissions to communicate with X_LINK_UNBOOTED device error on Linux, you need to set up or reload the udev rules so your user account has permission to access the Luxonis USB device (Movidius / OAK camera) without needing sudo.

## 1. Install

```bash
echo 'SUBSYSTEM=="usb", ATTRS{idVendor}=="03e7", MODE="0666", GROUP="plugdev"' | sudo tee /etc/udev/rules.d/80-movidius.rules
```

## 2. Reload

```bash
sudo udevadm control --reload-rules
sudo udevadm trigger
```

## 3. Add Your User to the `plugdev` Group (If Needed)

Ensure your current user belongs to the `plugdev` group (standard for access to plugged-in hardware):

```bash
sudo usermod -aG plugdev $USER
```

## 4. Reconnect the Device

* Unplug your DepthAI / OAK camera from the USB port.
* Wait a few seconds, then plug it back in.
* **Log out and log back in** (or reboot your machine) so that group membership changes take effect.
