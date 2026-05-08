# Example Configuration and Styling for ozhium-ollium

This directory contains example configuration and CSS files for customizing ozhium-ollium's on-screen display (OSD).

## Files

- `ozhium-ollium.conf`: Example configuration file for OSD settings, icons, and slider/text behavior
- `style.css`: Example GTK4 CSS stylesheet for customizing the OSD's visual appearance

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
- `wifi`, `ethernet`, `bluetooth`

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
