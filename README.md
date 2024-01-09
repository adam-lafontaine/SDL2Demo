# SDL2 Demo

A starter application using SDL2
* Detects keyboard, mouse and game controller input
* Plays music and sound effects.

## Setup Instructions

Set the `ROOT` constant in `app.hpp` to where you cloned the repository

```
namespace config
{
    constexpr auto APP_TITLE = "SDL2 Demo";


#ifdef _WIN32
    constexpr auto ROOT = "C:/D_Data/Repos";
#else
    constexpr auto ROOT = "/home/adam/Repos";
#endif
}
```

### Linux (Ubuntu)

Install SDL2  

```
apt-get install libsdl2-dev
apt-get install libsdl2-mixer-dev
```

Create the build directory

```
cd SDL2Demo/src/sdl
make setup
```

Build the application

```
make build
```

Run the application

```
make run
```

### Windows

Install SDL2

```
.\vcpkg.exe install sdl2:x64-windows
.\vcpkg.exe install sdl2-mixer:x64-windows
```

Run the Visual Studio solution in /VS/SDL2Demo