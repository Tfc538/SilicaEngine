# Screenshot Module

The `Screenshot` module in SilicaEngine provides a comprehensive screenshot capture system with support for multiple image formats, region capture, automatic timestamping, and batch operations. It offers both simple one-shot captures and advanced features like sequence capture and automatic periodic capture.

## Key Features

*   **Multiple Format Support**: PNG, JPEG, BMP, TGA, PPM, and raw framebuffer data.
*   **Configurable Capture**: Region capture, format-specific options (JPEG quality, PNG compression).
*   **Automatic Timestamping**: Built-in timestamp generation for unique filenames.
*   **Memory Capture**: Capture to memory for post-processing or custom saving.
*   **Sequence Capture**: Support for animation/timelapse screenshot sequences.
*   **Batch Operations**: ScreenshotManager for automatic periodic capture and statistics.
*   **Thread Safety**: Thread-safe operations with proper synchronization.
*   **Error Handling**: Comprehensive error reporting and validation.

## Core Components

### ScreenshotFormat Enum

```cpp
enum class ScreenshotFormat {
    PNG,           // Portable Network Graphics (recommended)
    JPEG,          // JPEG with configurable quality
    BMP,           // Windows Bitmap
    TGA,           // Targa
    PPM,           // Portable Pixmap (simple text format)
    RAW            // Raw framebuffer data
};
```

### ScreenshotConfig Structure

```cpp
struct ScreenshotConfig {
    ScreenshotFormat format = ScreenshotFormat::PNG;
    int jpegQuality = 90;                    // JPEG quality (1-100)
    bool flipVertically = true;              // Flip image vertically (OpenGL uses bottom-left origin)
    bool includeAlpha = false;               // Include alpha channel (for formats that support it)
    int compressionLevel = 6;                // PNG compression level (0-9)
    
    // Region capture (0,0,0,0 means full screen)
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
    
    // Timestamp options
    bool addTimestamp = false;               // Add timestamp to filename
    std::string timestampFormat = "%Y%m%d_%H%M%S";  // strftime format
};
```

### ScreenshotResult Structure

```cpp
struct ScreenshotResult {
    bool success = false;
    std::string filename;
    std::string errorMessage;
    size_t fileSize = 0;
    int width = 0;
    int height = 0;
    int channels = 0;
    double captureTime = 0.0;               // Time taken to capture in seconds
};
```

### FramebufferData Structure

```cpp
struct FramebufferData {
    std::vector<unsigned char> pixels;
    int width = 0;
    int height = 0;
    int channels = 0;                       // 3 for RGB, 4 for RGBA
    bool flipVertically = true;
};
```

## Screenshot Class

The main `Screenshot` class provides static methods for capturing and saving screenshots:

### Core Capture Methods

```cpp
class Screenshot {
public:
    // Capture screenshot with specified configuration
    static ScreenshotResult Capture(const std::string& filename, 
                                   const ScreenshotConfig& config = ScreenshotConfig{});

    // Capture screenshot to memory (raw framebuffer data)
    static FramebufferData CaptureToMemory(const ScreenshotConfig& config = ScreenshotConfig{});

    // Save raw framebuffer data to file
    static ScreenshotResult SaveFramebufferData(const FramebufferData& data,
                                               const std::string& filename,
                                               ScreenshotFormat format = ScreenshotFormat::PNG,
                                               int jpegQuality = 90);

    // Quick capture with automatic filename generation
    static ScreenshotResult QuickCapture(ScreenshotFormat format = ScreenshotFormat::PNG,
                                        const std::string& prefix = "screenshot");

    // Capture screenshot sequence for animation/timelapse
    static ScreenshotResult CaptureSequence(const std::string& baseFilename,
                                           int frameNumber,
                                           const ScreenshotConfig& config = ScreenshotConfig{});
};
```

### Utility Methods

```cpp
class Screenshot {
public:
    // Format utilities
    static std::string GetFileExtension(ScreenshotFormat format);
    static std::string GetFormatDescription(ScreenshotFormat format);
    static std::vector<ScreenshotFormat> GetSupportedFormats();
    
    // Configuration validation
    static bool ValidateConfig(const ScreenshotConfig& config, std::string& errorMessage);
    
    // Directory management
    static void SetDefaultDirectory(const std::string& directory);
    static const std::string& GetDefaultDirectory();
    
    // Filename generation
    static std::string GenerateTimestampedFilename(const std::string& prefix,
                                                  ScreenshotFormat format,
                                                  const std::string& timestampFormat = "%Y%m%d_%H%M%S");
    
    static std::string GenerateSequenceFilename(const std::string& baseFilename,
                                               int frameNumber,
                                               ScreenshotFormat format,
                                               int padding = 4);
};
```

## ScreenshotManager Class

The `ScreenshotManager` class provides advanced features for batch operations and automatic capture:

```cpp
class ScreenshotManager {
public:
    // Configuration
    void SetConfig(const ScreenshotConfig& config);
    const ScreenshotConfig& GetConfig() const;
    
    // Automatic capture
    void SetAutoCaptureEnabled(bool enabled);
    bool IsAutoCaptureEnabled() const;
    void SetAutoCaptureInterval(double interval);
    double GetAutoCaptureInterval() const;
    
    // Main update method (call once per frame)
    void Update(double deltaTime);
    
    // Capture with current configuration
    ScreenshotResult Capture(const std::string& filename);
    
    // Sequence capture
    void StartSequenceCapture(const std::string& baseFilename);
    void StopSequenceCapture();
    bool IsCapturingSequence() const;
    
    // Statistics
    int GetTotalCaptureCount() const;
    int GetSuccessfulCaptureCount() const;
    double GetAverageCaptureTime() const;
    void ClearStatistics();
};
```

## Usage Examples

### Basic Screenshot Capture

```cpp
// Simple capture with default settings
auto result = Screenshot::Capture("screenshot.png");
if (result.success) {
    std::cout << "Screenshot saved: " << result.filename << std::endl;
    std::cout << "Size: " << result.width << "x" << result.height << std::endl;
    std::cout << "File size: " << result.fileSize << " bytes" << std::endl;
    std::cout << "Capture time: " << result.captureTime << " seconds" << std::endl;
} else {
    std::cout << "Screenshot failed: " << result.errorMessage << std::endl;
}
```

### Custom Configuration

```cpp
// Configure screenshot settings
ScreenshotConfig config;
config.format = ScreenshotFormat::JPEG;
config.jpegQuality = 85;
config.flipVertically = true;
config.includeAlpha = false;
config.x = 100;
config.y = 100;
config.width = 800;
config.height = 600;
config.addTimestamp = true;

auto result = Screenshot::Capture("game_screenshot.jpg", config);
```

### Quick Capture with Timestamping

```cpp
// Set default directory
Screenshot::SetDefaultDirectory("./screenshots/");

// Quick capture with automatic timestamp
auto result = Screenshot::QuickCapture(ScreenshotFormat::PNG, "game");
// Generates filename like: game_20231201_143022.png
```

### Memory Capture and Custom Processing

```cpp
// Capture to memory
FramebufferData data = Screenshot::CaptureToMemory();
if (!data.pixels.empty()) {
    // Process the image data
    for (size_t i = 0; i < data.pixels.size(); i += data.channels) {
        // Access RGB values
        unsigned char r = data.pixels[i];
        unsigned char g = data.pixels[i + 1];
        unsigned char b = data.pixels[i + 2];
        
        // Apply custom processing...
    }
    
    // Save processed data
    Screenshot::SaveFramebufferData(data, "processed_screenshot.png");
}
```

### Sequence Capture for Animation

```cpp
// Capture sequence for animation
for (int frame = 0; frame < 60; ++frame) {
    // Update game state
    game.Update(deltaTime);
    game.Render();
    
    // Capture frame
    auto result = Screenshot::CaptureSequence("animation", frame);
    if (result.success) {
        std::cout << "Captured frame " << frame << std::endl;
    }
}
```

### Using ScreenshotManager for Automatic Capture

```cpp
// Create screenshot manager
ScreenshotManager manager;

// Configure automatic capture
ScreenshotConfig config;
config.format = ScreenshotFormat::PNG;
config.compressionLevel = 9;  // Maximum compression
manager.SetConfig(config);

// Enable automatic capture every 5 seconds
manager.SetAutoCaptureEnabled(true);
manager.SetAutoCaptureInterval(5.0);

// In game loop
while (game.IsRunning()) {
    game.Update(deltaTime);
    game.Render();
    
    // Update screenshot manager
    manager.Update(deltaTime);
    
    // Check statistics
    if (manager.GetTotalCaptureCount() % 10 == 0) {
        std::cout << "Average capture time: " << manager.GetAverageCaptureTime() << "s" << std::endl;
    }
}
```

### Sequence Capture with Manager

```cpp
ScreenshotManager manager;

// Start sequence capture
manager.StartSequenceCapture("timelapse");

// In game loop
while (game.IsRunning()) {
    game.Update(deltaTime);
    game.Render();
    
    // Capture frame
    if (manager.IsCapturingSequence()) {
        std::string filename = Screenshot::GenerateSequenceFilename(
            "timelapse", frameNumber, ScreenshotFormat::PNG);
        manager.Capture(filename);
        frameNumber++;
    }
    
    // Stop after 100 frames
    if (frameNumber >= 100) {
        manager.StopSequenceCapture();
        break;
    }
}
```

### Format-Specific Examples

```cpp
// High-quality PNG
ScreenshotConfig pngConfig;
pngConfig.format = ScreenshotFormat::PNG;
pngConfig.compressionLevel = 0;  // No compression for speed
pngConfig.includeAlpha = true;
Screenshot::Capture("high_quality.png", pngConfig);

// Compressed JPEG
ScreenshotConfig jpegConfig;
jpegConfig.format = ScreenshotFormat::JPEG;
jpegConfig.jpegQuality = 95;  // High quality
jpegConfig.includeAlpha = false;  // JPEG doesn't support alpha
Screenshot::Capture("compressed.jpg", jpegConfig);

// Raw framebuffer data
ScreenshotConfig rawConfig;
rawConfig.format = ScreenshotFormat::RAW;
rawConfig.includeAlpha = true;
Screenshot::Capture("framebuffer.raw", rawConfig);
```

### Error Handling

```cpp
// Validate configuration before capture
ScreenshotConfig config;
config.jpegQuality = 150;  // Invalid quality
config.compressionLevel = 15;  // Invalid compression level

std::string errorMessage;
if (!Screenshot::ValidateConfig(config, errorMessage)) {
    std::cout << "Invalid configuration: " << errorMessage << std::endl;
    return;
}

// Capture with error handling
auto result = Screenshot::Capture("screenshot.png", config);
if (!result.success) {
    SE_ERROR("Screenshot capture failed: {}", result.errorMessage);
    
    // Try with default configuration
    ScreenshotConfig defaultConfig;
    result = Screenshot::Capture("screenshot_fallback.png", defaultConfig);
}
```

## Implementation Details

### Thread Safety

The screenshot system uses mutex protection for shared resources like the default directory setting. The `ScreenshotManager` is thread-safe for concurrent access.

### Memory Management

The system uses `std::vector<unsigned char>` for pixel data storage with automatic memory management. Large captures are handled efficiently with proper error checking for memory allocation failures.

### OpenGL Integration

The system reads directly from the OpenGL framebuffer using `glReadPixels()`. It automatically handles the OpenGL coordinate system (bottom-left origin) by optionally flipping the image vertically.

### File Format Support

*   **PNG**: Uses STB Image Write library with configurable compression levels.
*   **JPEG**: Uses STB Image Write with quality settings (1-100).
*   **BMP**: Simple Windows Bitmap format.
*   **TGA**: Targa format support.
*   **PPM**: Simple text-based format for debugging.
*   **RAW**: Custom binary format with header information.

### Performance Considerations

*   **Memory Allocation**: Efficient memory allocation with error handling for large captures.
*   **File I/O**: Asynchronous file operations could be added for better performance.
*   **Compression**: PNG compression levels affect file size vs. save time trade-off.
*   **Region Capture**: Only captures specified region, reducing memory usage and processing time.

### Error Handling

The system provides comprehensive error handling:
*   Configuration validation before capture
*   OpenGL error checking during framebuffer reading
*   File system error handling
*   Memory allocation failure recovery
*   Detailed error messages for debugging

The screenshot system is designed to be robust and efficient, suitable for both development debugging and production use cases. 