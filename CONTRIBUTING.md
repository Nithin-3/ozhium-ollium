# Contributing to ozhium-ollium

Thank you for your interest in contributing to ozhium-ollium!

## Table of Contents
- [Getting Started](#getting-started)
- [Development Setup](#development-setup)
- [Building the Project](#building-the-project)
- [Code Style](#code-style)
- [Making Changes](#making-changes)
- [Pull Request Process](#pull-request-process)
- [Reporting Issues](#reporting-issues)
- [License](#license)

## Getting Started

### Prerequisites

**Build Dependencies:**
- GCC or Clang compiler
- pkg-config
- make
- clang-format (for code style)

**Runtime Dependencies:**
- libpulse (PulseAudio library)
- gtk4 (GTK+ 4.0)
- gtk4-layer-shell

### Installing Dependencies

**Debian/Ubuntu:**
```bash
sudo apt install build-essential pkg-config libpulse-dev libgtk-4-dev libgtk4-layer-shell-dev clang-format
```

**Fedora/RHEL:**
```bash
sudo dnf install gcc make pkg-config pulseaudio-libs-devel gtk4-devel gtk4-layer-shell-devel clang-format
```

**Arch Linux:**
```bash
sudo pacman -S base-devel pkgconf pulseaudio gtk4 gtk4-layer-shell clang
```

## Building the Project

```bash
# Clone the repository
git clone https://github.com/Nithin-3/ozhium-ollium.git
cd ozhium-ollium

# Build both daemon and UI
make

# Clean build artifacts
make clean
```

## Code Style

This project uses a specific code style defined in `.clang-format`. Please ensure your changes conform to this style.

### Key Style Points:
- **Indentation:** Tabs with width 8
- **Brace style:** K&R (attach opening brace on same line)
- **Pointer alignment:** Right (`type *var`)
- **Spaces:** Before control statement parentheses (`if (`, `for (`)
- **Column limit:** None (no forced line wrapping)

### Formatting Your Code

```bash
# Format all source files
make format

# Check if code is properly formatted (useful for CI)
make check-format
```

### Naming Conventions

This project uses consistent naming across code and configuration:

- **Global Variables & Functions:** `snake_case` & `camelCase` (e.g., `netlink_fd=0`, `initNetlink()`)
- **Config keys:** `dash-style` (e.g., `audio-mute`, `bat-full`, not `audio_mute`)

### Project Structure

```
src/
├── daemon/          # Daemon source code
│   ├── monitors/    # Monitoring modules (inotify, pulse, netlink)
│   ├── utils/       # Utility modules (battery, backlight, tool)
│   ├── invoke.c     # UI invocation
│   ├── config.c     # Configuration handling
│   └── ozhium-ollium.c  # Main entry point
├── ui/              # UI source code (GTK4)
└── shared/          # Shared code (common types, utilities)

include/
├── daemon/          # Daemon header files
│   ├── monitors/
│   └── utils/
└── shared/          # Shared header files
```

## Making Changes

1. **Fork the repository** on GitHub
2. **Create a new branch** for your feature or bugfix:
   ```bash
   git checkout -b feature/your-feature-name
   # or
   git checkout -b fix/your-bugfix-name
   ```
3. **Make your changes** following the code style guidelines
4. **Test your changes** by building and running:
   ```bash
   make clean
   make
   ./ozhium-ollium
   ```
5. **Format your code:**
   ```bash
   make format
   ```
6. **Commit your changes** with a clear commit message:
   ```bash
   git add .
   git commit -m "feat: add new feature"
   # or
   git commit -m "fix: resolve issue with backlight monitoring"
   ```

   We follow [Conventional Commits](https://www.conventionalcommits.org/):
   - `feat:` for new features
   - `fix:` for bug fixes
   - `refactor:` for code refactoring
   - `style:` for formatting changes
   - `docs:` for documentation updates
   - `test:` for adding tests

## Pull Request Process

1. **Push your branch** to your fork:
   ```bash
   git push origin feature/your-feature-name
   ```
2. **Open a Pull Request** against the main branch
3. **Describe your changes** clearly in the PR description
4. **Reference any related issues** (e.g., "Closes #123")
5. **Ensure the build passes** (if CI is configured)
6. **Respond to review feedback** promptly

### PR Guidelines
- Keep PRs focused on a single feature or fix
- Include tests if applicable
- Update documentation if needed
- Ensure code is properly formatted

## Reporting Issues

When reporting issues, please include:

1. **Clear description** of the problem
2. **Steps to reproduce** the issue
3. **Expected behavior** vs **actual behavior**
4. **Your environment:**
   - OS and version
   - Kernel version
   - PulseAudio version
   - GTK4 version
5. **Relevant logs** or error messages

### Issue Types
- **Bug report:** Something isn't working
- **Feature request:** Suggest a new feature
- **Documentation:** Improve or clarify docs
- **Question:** Ask about usage or implementation

## License

By contributing to this project, you agree that your contributions will be licensed under the same BSD 3-Clause License as the project. See [LICENSE](LICENSE) for details.

When submitting contributions, you must:
- Retain the original copyright notice
- Attribute this project (ozhium-ollium) when using or modifying the code

## Questions?

Feel free to open an issue for any questions about contributing!

---

Thank you for contributing to ozhium-ollium!
