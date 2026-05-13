## This is a Fork of [Native File Dialog](https://github.com/mlabbe/nativefiledialog), which replaces the build system with CMake, and bumps the GTK version to 4.

# Native File Dialog #

A tiny, neat C library that portably invokes native file open, folder select and save dialogs.  Write dialog code once and have it pop up native dialogs on all supported platforms.  Avoid linking large dependencies like wxWidgets and qt.

Features:

 - Lean C API, static library -- no ObjC, no C++, no STL.
 - Zlib licensed.
 - Consistent UTF-8 support on all platforms.
 - Simple universal file filter syntax.
 - Paid support available.
 - Multiple file selection support.
 - 64-bit and 32-bit friendly.
 - GCC, Clang, Xcode, Mingw and Visual Studio supported.
 - No third party dependencies for building or linking.
 - Support for Vista's modern `IFileDialog` on Windows.
 - Support for non-deprecated Cocoa APIs on OS X.
 - GTK4 dialog on Linux.
 - Optional Zenity support on Linux to avoid linking GTK.
 - Tested, works alongside [http://www.libsdl.org](SDL2) on all platforms, for the game developers out there.

# Example Usage #

```C
#include <nfd.h>
#include <stdio.h>
#include <stdlib.h>

int main( void )
{
    nfdchar_t *outPath = NULL;
    nfdresult_t result = NFD_OpenDialog( NULL, NULL, &outPath );
        
    if ( result == NFD_OKAY ) {
        puts("Success!");
        puts(outPath);
        free(outPath);
    }
    else if ( result == NFD_CANCEL ) {
        puts("User pressed cancel.");
    }
    else {
        printf("Error: %s\n", NFD_GetError() );
    }

    return 0;
}
```

See self-documenting API [NFD.h](src/include/nfd.h) for more options.

# Screenshots #

![Windows rendering a dialog](screens/open_win.png?raw=true)
![GTK4 on Linux rendering a dialog](screens/open_gtk4.png?raw=true)
![Cocoa on MacOS rendering a dialog](screens/open_cocoa.png?raw=true)

## Changelog ##

 - **Major** version increments denote API or ABI departure.
 - **Minor** version increments denote build or trivial departures.
 - **Micro** version increments just recompile and drop-in.

release | what's new                  | date
--------|-----------------------------|---------
1.0.0   | initial                     | oct 2014
1.1.0   | premake5; scons deprecated  | aug 2016
1.1.1   | mingw support, build fixes  | aug 2016
1.1.2   | test_pickfolder() added     | aug 2016
1.1.3   | zenity linux backend added  | nov 2017
<i></i> | fix char type in decls      | nov 2017
1.1.4   | fix win32 memleaks          | dec 2018
<i></i> | improve win32 errorhandling | dec 2018
<i></i> | macos fix focus bug         | dec 2018
1.1.5   | win32 fix com reinitialize  | aug 2019
1.1.6   | fix osx filter bug          | aug 2019
<i></i> | remove deprecated scons     | aug 2019
<i></i> | fix mingw compilation       | aug 2019
<i></i> | -Wextra warning cleanup     | aug 2019

## Building ##

NFD uses CMake to build.

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### Compiling Your Programs ###

 1. Add `src/include` to your include search path.
 2. Add `NativeFileDialog` to the list of static libraries to link against.

#### Linux GTK ####

`apt-get libgtk-4-dev` installs the gtk dependency for library compilation.

On Linux, you have the option of compiling and linking against GTK.  If you use it, the recommended way to compile is to include the arguments of `pkg-config --cflags --libs gtk4`.

#### MacOS ####

On Mac OS, add `AppKit` to the list of frameworks.

#### Windows ####

On Windows, ensure you are linking against `comctl32.lib`.

## Usage ##

See `NFD.h` for API calls.  See `tests/*.c` for example code.

## File Filter Syntax ##

There is a form of file filtering in every file dialog API, but no consistent means of supporting it.  NFD provides support for filtering files by groups of extensions, providing its own descriptions (where applicable) for the extensions.

A wildcard filter is always added to every dialog.

### Separators ###

 - `;` Begin a new filter.
 - `,` Add a separate type to the filter.

#### Examples ####

`txt` The default filter is for text files.  There is a wildcard option in a dropdown.

`png,jpg;psd` The default filter is for png and jpg files.  A second filter is available for psd files.  There is a wildcard option in a dropdown.

`NULL` Wildcard only.

## Iterating Over PathSets ##

See [test_opendialogmultiple.c](test/test_opendialogmultiple.c).

# Known Limitations #

I accept quality code patches, or will resolve these and other matters through support.  See [contributing](docs/contributing.md) for details.

 - No support for Windows XP's legacy dialogs such as `GetOpenFileName`.
 - No support for file filter names -- ex: "Image Files" (*.png, *.jpg).  Nameless filters are supported, however.
 - GTK Zenity implementation's process exec error handling does not gracefully handle numerous error cases, choosing to abort rather than cleanup and return.
 - GTK 4 spams one warning per dialog created.

# Copyright and Credit #

Copyright &copy; 2014-2019 [Frogtoss Games](http://www.frogtoss.com), Inc.
File [LICENSE](LICENSE) covers all files in this repo.

Native File Dialog by Michael Labbe
<mike@frogtoss.com>

Tomasz Konojacki for [microutf8](http://puszcza.gnu.org.ua/software/microutf8/)

[Denis Kolodin](https://github.com/DenisKolodin) for mingw support.

[Tom Mason](https://github.com/wheybags) for Zenity support.

## Support ##

Directed support for this work is available from the original author under a paid agreement.

[Contact Frogtoss Games](http://www.frogtoss.com/pages/contact.html).
