# Caelis Build Scripts

**Author:** Tim Gatzke <post@tim-gatzke.de>

This directory contains cross-platform build scripts for the Caelis project.

## CI Environment Support

All scripts automatically detect and adapt to CI environments by checking standard environment variables:

### Supported CI Environment Variables

- `CI=1` or `CI=true` - General CI indicator
- `GITHUB_ACTIONS=true` - GitHub Actions
- `TRAVIS=true` - Travis CI
- `APPVEYOR=True` - AppVeyor
- `TF_BUILD=True` - Azure DevOps
- `CIRCLECI=true` - CircleCI
- `BUILDKITE=true` - Buildkite
- `JENKINS_URL` - Jenkins (when URL is set)

### CI Behavior Changes

When CI environment is detected, scripts will:

- **Disable interactive prompts** (no `pause` in Windows)
- **Enable verbose output** for better debugging
- **Skip GUI application execution** (Fractura won't run)
- **Use appropriate generators** (Unix Makefiles on macOS CI instead of Xcode)
- **Exit cleanly** without waiting for user input

## Platform-Specific Scripts

### Windows (`scripts/windows/`)

- `build.bat [Debug|Release]` - Build project
- `build-debug.bat` - Build debug configuration
- `build-release.bat` - Build release configuration
- `clean.bat` - Clean build artifacts
- `run-fractura.bat [Debug|Release]` - Run Fractura demo
- `install.bat` - Install project
- `setup-deps.bat` - Setup dependencies

### Linux (`scripts/linux/`)

- `build.sh [Debug|Release]` - Build project
- `build-debug.sh` - Build debug configuration
- `build-release.sh` - Build release configuration
- `clean.sh` - Clean build artifacts
- `run-fractura.sh [Debug|Release]` - Run Fractura demo
- `install.sh` - Install project
- `setup-deps.sh` - Setup dependencies

### macOS (`scripts/macos/`)

- `build.sh [Debug|Release]` - Build project
- `build-debug.sh` - Build debug configuration
- `build-release.sh` - Build release configuration
- `clean.sh` - Clean build artifacts
- `run-fractura.sh [Debug|Release]` - Run Fractura demo
- `install.sh` - Install project
- `setup-deps.sh` - Setup dependencies

## Usage Examples

### Local Development
```bash
# Build release version
./scripts/linux/build.sh Release

# Run Fractura demo
./scripts/linux/run-fractura.sh Release
```

### CI Environment
```bash
# Set CI environment
export CI=1

# Build with verbose output (no interactive prompts)
./scripts/linux/build.sh Release

# Verify build (Fractura execution will be skipped)
./scripts/linux/run-fractura.sh Release
```

### Windows
```batch
REM Build release version
scripts\windows\build.bat Release

REM Run Fractura demo (with pause for user)
scripts\windows\run-fractura.bat Release
```

### Windows CI
```batch
REM Set CI environment
set CI=1

REM Build with verbose output (no pauses)
scripts\windows\build.bat Release

REM Verify build (no GUI execution)
scripts\windows\run-fractura.bat Release
```

## Common Directories

- `scripts/common/` - Shared Python utilities for analysis and formatting

## Notes

- All scripts automatically detect the platform and adjust behavior accordingly
- Build scripts create executables in `build/bin/` directory
- Clean scripts remove both `build/` and `install/` directories
- CI detection ensures scripts work seamlessly in automated environments
- Scripts preserve original behavior for local development while adding CI compatibility