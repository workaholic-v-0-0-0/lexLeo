# Installation {#installation}

LexLeo is intended to be distributed through native platform packages.

The installation methods available depend on the target platform.

## Linux

### Debian package (.deb)

When a Debian package is available, install it with:

    sudo dpkg -i lexleo_<version>_<arch>.deb

If additional dependencies must be resolved:

    sudo apt-get install -f

### Run

After installation, launch LexLeo with:

    lexleo

### Manual page

If the manual page is installed on the system, it can be opened with:

    man lexleo

If the manual database must be refreshed manually:

    sudo mandb

### List installed files

To inspect the files installed by the package:

    dpkg -L lexleo

### Remove package

To remove the application while keeping system configuration files:

    sudo dpkg -r lexleo

### Purge package

To remove the application and associated system configuration files:

    sudo dpkg --purge lexleo

### Notes

The exact package filename depends on:
- project version,
- target architecture,
- package generator configuration.

## Windows

### Native installer

When a Windows package is available, open a terminal in the directory
containing the installer.

Example:

    cd build\dist

List available files:

    dir

Run the installer:

    .\LexLeo-<version>-win64.exe

Follow the installation steps displayed by the installer.

The installer copies the packaged files into the selected installation
directory.

Typical installed contents include:

    bin\lexleo.exe
    config\lexleo.ini
    doc\README.txt
    doc\LICENSE.txt

### Run

After installation, LexLeo can be launched:

- from the installation directory,
- from a shortcut created by the installer, if provided,
- or from a terminal if the executable directory is available in `PATH`.

Typical executable:

    lexleo.exe

### Configuration

If a configuration file is installed with the package, it is typically
located at:

    config\lexleo.ini

### Documentation

Package documentation files are typically installed at:

    doc\README.txt
    doc\LICENSE.txt

### Uninstall

LexLeo can be removed through the standard Windows uninstall workflow.

Typical locations:

- Settings → Apps
- Control Panel → Programs and Features

### Notes

The exact installation flow depends on:
- package version,
- installer configuration,
- selected installation directory,
- selected install scope.

## macOS

todo
