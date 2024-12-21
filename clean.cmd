@echo off
rem This script cleans the test files. It does not delete any build artifacts.

rem Delete file-like items.
del symlink-file-w
del target-file-w
del symlink-file-a
del target-file-a

rem Delete directory-like items. (This is done with "rmdir" even for symlinks.)
del /q symlink-dir-w
del /q target-dir-w
del /q symlink-dir-a
del /q target-dir-a
