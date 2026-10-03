# YouTube Downloader

A Windows desktop application for downloading YouTube videos, playlists, and audio files. The application is written in C++ with Qt 6 and uses [yt-dlp](https://github.com/yt-dlp/yt-dlp) and FFmpeg to handle downloads and media conversion.

> **Important:** Only download content that you have permission to download. Respect YouTube's Terms of Service, copyright laws, and the rights of content creators.

## Features

- Download individual YouTube videos or complete playlists.
- Choose video formats including MP4, MKV, and WebM.
- Extract audio as MP3 or MPA.
- Select video resolution up to 2160p, when available.
- Select the preferred frame rate.
- Download subtitles and automatically convert them to SRT.
- Configure the download directory.
- Configure download thread count for improved performance.
- Pause or cancel active download tasks.
- Display download progress, speed, and status information.
- Save application preferences between sessions.
- Use an optional `aria2c.exe` installation as an external downloader.

## Requirements

- Windows 10 or later, 64-bit.
- [Qt 6](https://www.qt.io/), including the Core, Gui, and Widgets components.
- [CMake 3.20](https://cmake.org/) or later.
- A C++17-compatible compiler, such as Visual Studio 2022.
- PowerShell, used by the application to download and extract dependencies.
- An internet connection.

The application can download `yt-dlp.exe` and FFmpeg automatically on first use. They are placed alongside the application executable.

## Building from Source

### Visual Studio and CMake

1. Install Qt 6 and make sure CMake can find the Qt installation.
2. Clone the repository:

   ```powershell
   git clone https://github.com/neil289506-commits/YouTubedownloader.git
   cd YouTubedownloader
   ```

3. Configure a 64-bit build. Set `CMAKE_PREFIX_PATH` to your Qt installation if Qt is not already available in your environment:

   ```powershell
   cmake -S . -B build -A x64 -DCMAKE_PREFIX_PATH="C:\Qt\6.x.x\msvc2022_64"
   ```

4. Build the Release configuration:

   ```powershell
   cmake --build build --config Release
   ```

5. Run the generated executable from the `build/bin/Release` directory, or from the corresponding CMake output directory for your generator.

> Replace `C:\Qt\6.x.x\msvc2022_64` with the actual path to your Qt installation.

## Usage

1. Launch `YTDownloader.exe`.
2. Enter a YouTube video or playlist URL.
3. Click **Add** to add the URL to the download list.
4. Select the download directory, format, resolution, and frame rate.
5. Enable playlist or subtitle options if needed.
6. Click **Start Download**.

On the first run, the application prepares the required `yt-dlp.exe` and FFmpeg components. A working internet connection and permission to write to the application directory are required.

## Project Structure

- `main.cpp` - Application entry point and theme setup.
- `mainwindow.cpp` / `mainwindow.h` - Main Qt user interface.
- `downloadworker.cpp` / `downloadworker.h` - Dependency setup and download execution.
- `settingsdialog.cpp` / `settingsdialog.h` - Application settings dialog.
- `CMakeLists.txt` - CMake build configuration.
- `resources.qrc` - Embedded application resources.

## Dependencies

This project relies on the following external software:

- [yt-dlp](https://github.com/yt-dlp/yt-dlp) for retrieving video and audio streams.
- [FFmpeg](https://ffmpeg.org/) for media merging and audio conversion.
- [Qt](https://www.qt.io/) for the graphical user interface.

Please review and comply with the licenses of all dependencies before distributing the application.

## License

No license file is currently included in this repository. Unless a license is added, all rights are reserved by the copyright holder. Add a license before redistributing or contributing the project under open-source terms.
