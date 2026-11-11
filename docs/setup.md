# Connecting to the Camera
To fix the Insufficient permissions to communicate with X_LINK_UNBOOTED device error on Linux, you need to set up or reload the udev rules so your user account has permission to access the Luxonis USB device (Movidius / OAK camera) without needing sudo.

### 1. Install

```bash
echo 'SUBSYSTEM=="usb", ATTRS{idVendor}=="03e7", MODE="0666", GROUP="plugdev"' | sudo tee /etc/udev/rules.d/80-movidius.rules
```

### 2. Reload

```bash
sudo udevadm control --reload-rules
sudo udevadm trigger
```

### 3. Add Your User to the `plugdev` Group (If Needed)

Ensure your current user belongs to the `plugdev` group (standard for access to plugged-in hardware):

```bash
sudo usermod -aG plugdev $USER
```

### 4. Reconnect the Device

* Unplug your DepthAI / OAK camera from the USB port.
* Wait a few seconds, then plug it back in.
* **Log out and log back in** (or reboot your machine) so that group membership changes take effect.


# Installing the DepthAI API
Build the DepthAI C++ library once and install it to `~/.local/depthai`, which this project links against.

### 1. Install System Packages

```bash
sudo apt install cmake g++ git curl zip unzip tar pkg-config autoconf automake libtool libudev-dev libopencv-dev
```

### 2. Clone depthai-core

Use `depthai==3.12.1`

```bash
git clone git@github.com:luxonis/depthai-core.git
cd depthai-core
git checkout v3.12.1
git submodule update --init --recursive
```

### 3. Configure

The last two flags save disk space by building dependencies in Release only and deleting their build files when done.

```bash
cmake -S . -B build -D BUILD_SHARED_LIBS=ON -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=$HOME/.local/depthai -D VCPKG_OVERLAY_TRIPLETS=$HOME/depthai-core/cmake/triplets/release -D VCPKG_INSTALL_OPTIONS=--clean-after-build
```

### 4. Build and Install into `~/.local/depthai`

```bash
cmake --build build --target install --parallel $(nproc)
```

### 5. Build This Project

```bash
cd ~/visual-inertial-slam
cmake -S . -B build -D CMAKE_PREFIX_PATH=$HOME/.local/depthai
cmake --build build
```

After code changes, only `cmake --build build` needs to be rerun.

### 6. Run

```bash
./build/camera
```

### 7. Clean Up (Optional)

The clone isn't needed after installing. Move it aside, then rebuild and run without it:

```bash
mv ~/depthai-core ~/depthai-core.bak
rm -rf build && cmake -S . -B build -D CMAKE_PREFIX_PATH=$HOME/.local/depthai && cmake --build build && ./build/camera
```

If the camera loads, delete the backup:

```bash
rm -rf ~/depthai-core.bak
```
