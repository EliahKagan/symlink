# symlink - Testing `SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE` on Windows

This program tests the ability to create file and directory symlinks on Windows
with `SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE`, using both of the Windows
API functions `CreateSymbolicLinkW` and `CreateSymbolicLinkA`.

The "W" and "A" functions are probably equivalent with respect to all aspects
not involving character encodings, because in the Windows API, "A" functions
are implemented as wrapper for the corresponding "W" functions.

However, as of this writing, `CreateSymbolicLinkW` and `CreateSymbolicLinkA`
are documented differently, such that it is not clear if the documentation
intends to specify that they differ beyond the usual "W" vs. "A" distinction.

As noted by [**@RivenSkaye**](https://github.com/RivenSkaye) in
[GitoxideLabs/gitoxide#1374 (comment)](https://github.com/GitoxideLabs/gitoxide/pull/1374#issuecomment-2124574975),
the `CreateSymbolicLinkA` function is documented in a way that suggests
[Developer Mode](https://learn.microsoft.com/en-us/windows/uwp/get-started/enable-your-device-for-development)
only needs to be enabled to create symlinks if they are created by a UWP
application.

`SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE` is documented for
[`CreateSymbolicLinkW`](https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-createsymboliclinkw)
to mean:

> Specify this flag to allow creation of symbolic links when the process is not
elevated.
[Developer Mode](https://learn.microsoft.com/en-us/windows/uwp/get-started/enable-your-device-for-development)
must first be enabled on the machine before this option will function.

In contrast, that flag is documented for
[`CreateSymbolicLinkA`](https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-createsymboliclinka)
to mean:

> Specify this flag to allow creation of symbolic links when the process is
not elevated. In UWP,
[Developer Mode](https://learn.microsoft.com/en-us/windows/uwp/get-started/enable-your-device-for-development)
must first be enabled on the machine before this option will function. Under
MSIX, developer mode is not required to be enabled for this flag.

This program can be built in various ways and tested on various systems to
example this, or otherwise to test the ability to create symlinks.

When UAC is enabled, the ability to create symlinks may vary by whether the
program is run with elevation.

## Usage

Build the program and run `symlink.exe`, either unelevated or elevated.

To avoid confusion and to test a scenario that resembles common real-world use
of symlinks, this program creates targets for all symlinks first. It will fail
with a hard error if any target or symlink it plans to create already exists,
of it it encounters any errors creating the targets.

The `clean.ps1` script can be run to delete any symlinks and targets that a
previous run (successful or not) of the program has created. The script tries
to delete all the files that `symlink.exe` might create, and reports failure
if any of those files cannot be deleted, even when the reason is that they do
not exist. But it still proceeds to (attempt to) delete the remaining files.

## Results

I ran this as a user with administrative privileges (i.e. a member of the
`Administrators` group) but with UAC enabled on a Windows 11 machine, *where
developer mode was **not** enabled*, for which:

```text
> (Get-ComputerInfo).WindowsBuildLabEx
22621.1.amd64fre.ni_release.220506-1250
```

Without elevation, all symlink creation operations failed, reporting failure
and not creating any of the symlinks:

```text
> x64/Debug/symlink.exe
note: can't create file symlink "symlink-file-w" to "target-file-w" (error 1314)
note: can't create directory symlink "symlink-dir-w" to "target-dir-w" (error 1314)
note: can't create file symlink "symlink-file-a" to "target-file-a" (error 1314)
note: can't create directory symlink "symlink-dir-a" to "target-dir-a" (error 1314)
```

Error 1314 is
[`ERROR_PRIVILEGE_NOT_HELD`](https://learn.microsoft.com/en-us/windows/win32/debug/system-error-codes--1300-1699-#ERROR_PRIVILEGE_NOT_HELD):

> A required privilege is not held by the client.

With elevation (via [psutils](https://github.com/lukesampson/psutils) `sudo`,
though an elevated console would also work fine), symlink creation operations
all succeeded as expected, reporting success and creating all the symlinks:

```text
> sudo x64/Debug/symlink.exe
note: created file symlink "symlink-file-w" to "target-file-w"
note: created directory symlink "symlink-dir-w" to "target-dir-w"
note: created file symlink "symlink-file-a" to "target-file-a"
note: created directory symlink "symlink-dir-a" to "target-dir-a"
```

(In between the runs, I ran `./clean.ps1`.)

## License

[0BSD](LICENSE)
