# ozhium-ollium

ozhium-ollium is a Linux daemon that monitors system events and displays a configurable GTK4 overlay UI for real-time feedback.
The project originally began as a simple utility focused only on brightness and audio level changes — reflected in its name: “ozhium” (Tamil-inspired for light) and “ollium” (Tamil-inspired for sound). As the project evolved, support expanded to battery, network, Bluetooth, and other system events, while retaining the original name that represented its beginnings.

## Features

- Real-time backlight brightness monitoring and adjustment
- Volume level monitoring and control via PulseAudio
- Network connection monitoring (Wi-Fi, Ethernet, VPN)
- Bluetooth device monitoring
- On-screen display (OSD) for visual feedback
- VPN interface detection via kernel link type
- System daemon for background operation
- Lightweight and efficient implementation
- Support for multiple audio devices
- execute your own commend on state change

## System Requirements

- Linux operating system with kernel support for:
  - inotify (file system event notification)
  - netlink (network, Bluetooth, and VPN event notification)
- PulseAudio sound server
- Wayland display server

## Dependencies

Before building, install the required dependencies for your distribution.

### Ubuntu/Debian
```bash
sudo apt-get install build-essential pkg-config libpulse-dev libudev-dev libgtk-4-dev libgtk-layer-shell-0-dev
```

### Fedora/RHEL
```bash
sudo dnf install gcc pkg-config pulseaudio-libs-devel systemd-devel gtk4-devel gtk4-layer-shell-devel
```

### Arch Linux
```bash
sudo pacman -S base-devel pulseaudio gtk4 gtk4-layer-shell
```

For more details, see the [DEPENDENCIES](DEPENDENCIES) file.

## Installation

1. Clone the repository:
```bash
git clone https://github.com/Nithin-3/ozhium-ollium.git
cd ozhium-ollium
```

2. Install dependencies (see Dependencies section above)

3. Build the project:
```bash
make
```

4. Install (optional):
```bash
make install                    # installs to /usr/local
make install PREFIX=/usr       # installs to /usr
make install DESTDIR=/tmp/pkg PREFIX=/usr  # staging for packaging
```

## Usage

Run the daemon:
```bash
./ozhium-ollium
```

## How It Works

The project consists of two main components:

**Daemon (ozhium-ollium)**
- Monitors backlight changes via inotify on sysfs
- Monitors volume changes via PulseAudio
- Detects VPN connections by parsing IFLA_INFO_KIND (tun, wireguard, openvpn, etc.)
- Tracks battery status
- Runs in the background as a system service
- Uses udev for hardware event detection

**UI (ozhium-ollium-ui)**
- Displays visual feedback for brightness and volume changes
- Renders on-screen display (OSD) notifications
- Uses GTK4 and gtk4-layer-shell for window rendering
- Receives callbacks from the daemon

## It Is Not
- change value (backlight,audio,mic)
- need root permission
- listen keylog
- open port

## Building

To build both the daemon and UI:
```bash
make
```

Install system-wide (defaults to `/usr/local`):
```bash
make install
```

Override installation prefix:
```bash
make install PREFIX=/usr          # install to /usr
make install DESTDIR=/tmp/pkg PREFIX=/usr  # staging directory for packaging
```

- **`PREFIX`** — installation root (default: `/usr/local`). Binaries go to `$PREFIX/bin`, data to `$PREFIX/share/ozhium-ollium/`.
- **`DESTDIR`** — staging directory prepended to all paths. Used when building packages (`.deb`/`.rpm`).

To clean build artifacts:
```bash
make clean
```

## License

This project is licensed under the BSD 3-Clause License. See the [LICENSE](LICENSE) file for details.

When using or modifying this code, you must include the original license and attribute this project (ozhium-ollium).

## Contributing

Contributions are welcome! Please feel free to submit pull requests or open issues for bugs and feature requests.

## Third Party Libraries

- [inih](https://github.com/benhoyt/inih) — INI file parser (BSD 3-Clause)
- [GTK4](https://gtk.org/) — UI toolkit (LGPL-2.1+)
- [Qt6](https://www.qt.io/) — UI toolkit (GPL-3.0 / LGPL-3.0)
