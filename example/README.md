# Example Configuration and Styling for ozhium-ollium

This directory contains example configuration and CSS files for customizing ozhium-ollium's on-screen display (OSD).

## Files

- `ozhium-ollium.conf`: Example configuration file for OSD settings, icons, and slider/text behavior
- `ozhium-ollium.qml`: Qt6 QML template for the OSD user interface (used by the Qt backend)
- `style.css`: Example GTK4 CSS stylesheet for customizing the OSD's visual appearance (GTK backend only)

---

## Configuration File (`ozhium-ollium.conf`)

The configuration file is divided into sections controlling different aspects of the OSD. Application defaults are noted where applicable.

### `[osd]` Section
Controls OSD window position and behavior:
- `vertical-align`: Vertical alignment (`top`, `center`, `bottom`). Default: `top`
- `horizontal-align`: Horizontal alignment (`left`, `center`, `right`). Default: `center`
- `orientation`: Slider orientation (`vertical`, `horizontal`). Default: `horizontal`
- `timeout`: Seconds before OSD hides automatically. Default: `3`
- `x`, `y`: Manual position offsets (set to `-1` for automatic alignment). Default: `-1`
- `margin`: Margin around OSD window. Default: `0`

### `[slider]` Section
Customizes volume/backlight slider display:
- `invert-dir`: Invert slider direction (`true`/`false`). Default: `false`
- `label1`: First label format (supports `#ico#` for icon, `#val#` for value). Default: `#ico#`
- `label2`: Second label format. Default: `#val#`

### `[text]` Section
Customizes text-only OSD displays (battery, network, etc.):
- `label`: Text format (supports `#ico#` for icon, `#val#` for value). Default: `#ico# #val#`

### `[icons]` Section
Override default Nerd Font icons for system states. Available keys:
- `backlight`, `volume`, `volume-mute`
- `mic`, `mic-mute`
- `bat-low`, `bat-full`, `bat-charge`, `bat-discharge`, `bat-idel`, `bat-crit`
- `wifi`, `ethernet`, `bluetooth`, `vpn`

---

## Styling File (`style.css`)

GTK4-compatible CSS for customizing the OSD's visual appearance. The application loads this from the same config paths as `ozhium-ollium.conf`.

### Key Selectors Used by ozhium-ollium-ui
- `window`: Top-level OSD window
- `#slider`: Volume/backlight slider widget
- `#slider-box`: Container around the slider
- `#text-box`: Container for text-only OSD
- `#slider > trough`: Slider track background
- `#slider > trough > highlight`: Filled portion of the slider
- `label`: Default text styles
- `label.title`, `label.icon`: Specialized text styles

Styles follow GTK4 CSS conventions. See the [GTK4 CSS documentation](https://docs.gtk.org/gtk4/css-overview.html) for more customization options.

---

## QML Template (`ozhium-ollium.qml`)

The QML template renders the OSD using the Qt6 Quick backend (`make UI_BACKEND=qt`). It receives values from the C++ process via QML context properties.

### Context Properties

| Property | Type | Description |
|----------|------|-------------|
| `argsElement` | string | `"slider"` or `"text"` — selects which layout to show |
| `argsAction` | string | Action enum value (`"0"` … `"16"`) — mapped to an icon |
| `argsMin` | double | Slider minimum value |
| `argsMax` | double | Slider maximum value |
| `argsCurrent` | double | Current slider position |
| `argsText` | string | Text displayed in text mode |

### Hardcoded Action → Icon Map

The icons are Nerd Font glyphs matching the `[icons]` section of `ozhium-ollium.conf`:

| Action | Config key | QML escape |
|--------|-----------|------------|
| 0 | `backlight` | `\uEA61` |
| 1 | `volume` | `\uF028` |
| 2 | `volume-mute` | `\uDB81\uDD81` |
| 3 | `mic` | `\uDB80\uDF6C` |
| 4 | `mic-mute` | `\uF131` |
| 5 | `bat-crit` | `\uDB80\uDC83` |
| 6 | `bat-low` | `\uDB84\uDEA1` |
| 7 | `bat-full` | `\uF240` |
| 8 | `bat-charge` | `\uDB80\uDC84` |
| 9 | `bat-discharge` | `\uDB80\uDC7E` |
| 10 | `bat-idel` | `\uDB80\uDC79` |
| 11 | `wifi` | `\uF1EB` |
| 12 | `ethernet` | `\uDB80\uDE00` |
| 13 | `bluetooth` | `\uF293` |
| 14 | `block` | `\uDB84\uDDF0` |
| 15 | `hid` | `\uDB85\uDD81` |
| 16 | `vpn` | `\uDB85\uDD4A` |

To use the example QML file:

```bash
mkdir -p ~/.config/ozhium-ollium
cp example/ozhium-ollium.qml ~/.config/ozhium-ollium/
```

---

## Usage

The application searches for config files in these paths (in order):
1. `~/.config/ozhium-ollium/` (user local config)
2. `/usr/share/ozhium-ollium/` (system-wide config)

To use these example files:
```bash
# Create user config directory
mkdir -p ~/.config/ozhium-ollium

# Copy example files
cp example/ozhium-ollium.conf ~/.config/ozhium-ollium/
cp example/style.css ~/.config/ozhium-ollium/

# Modify to your preferences, then run ozhium-ollium as usual
./ozhium-ollium
```
