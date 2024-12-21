// Demonstrate symbolc link creation on Windows.
//
// This shows CreateSymbolicLinkW and CreateSymbolicLinkA explicitly, creating
// both file and directory symbolic links and reporting any errors.

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

static const wchar_t *const symlink_file_w = L"symlink-file-w";
static const wchar_t *const symlink_dir_w = L"symlink-dir-w";
static const wchar_t *const target_file_w = L"target-file-w";
static const wchar_t *const target_dir_w = L"target-dir-w";
static const char *const symlink_file_a = "symlink-file-a";
static const char *const symlink_dir_a = "symlink-dir-a";
static const char *const target_file_a = "target-file-a";
static const char *const target_dir_a = "target-dir-a";

enum status {
    status_success = 0,
    status_symlink_creation_failed = 1,
    status_error_setting_up_experiment = 2,
};

__declspec(noreturn) static void wdie(wchar_t *__restrict format, ...)
{
    fputws(L"fatal: ", stderr);

    va_list args;
    va_start(args, format);
    vfwprintf(stderr, format, args);
    va_end(args);

    fputwc(L'\n', stderr);

    exit(status_error_setting_up_experiment);
}

__declspec(noreturn) static void die(char *__restrict format, ...)
{
    fputs("fatal: ", stderr);

    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    fputc('\n', stderr);

    exit(status_error_setting_up_experiment);
}

static void wmsg(wchar_t *__restrict format, ...)
{
    fputws(L"note: ", stderr);

    va_list args;
    va_start(args, format);
    vfwprintf(stderr, format, args);
    va_end(args);

    fputwc(L'\n', stderr);
}

static void msg(char *__restrict format, ...)
{
    fputs("note: ", stderr);

    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    fputc('\n', stderr);
}

static inline enum status maxval(enum status lhs, enum status rhs)
{
    return lhs < rhs ? rhs : lhs;
}

static void check_clean_w(void)
{
    const wchar_t *wide_paths[] = {
        symlink_file_w,
        symlink_dir_w,
        target_file_w,
        target_dir_w,
    };
    for (int i = 0; i < _countof(wide_paths); ++i) {
        if (GetFileAttributesW(wide_paths[i]) != INVALID_FILE_ATTRIBUTES
                || GetLastError() != ERROR_FILE_NOT_FOUND) {
            wdie(L"workspace not clean: \"%s\" may exist", wide_paths[i]);
        }
    }
}

static void check_clean_a(void)
{
    const char *narrow_paths[] = {
        symlink_file_a,
        symlink_dir_a,
        target_file_a,
        target_dir_a,
    };
    for (int i = 0; i < _countof(narrow_paths); ++i) {
        if (GetFileAttributesA(narrow_paths[i]) != INVALID_FILE_ATTRIBUTES
                || GetLastError() != ERROR_FILE_NOT_FOUND) {
            die("workspace not clean: \"%s\" may exist", narrow_paths[i]);
        }
    }
}

// Make sure none of the files or directories exist yet. This catches the most
// likely "user error," giving clear messages. Files and directories do not
// come in two flavors--they are all Unicode (UTF-16LE) really--but using the
// same data types that will be passed to encoding-specific functions in the
// test makes it easier to understand what is going on and to inspect the test.
static void check_clean(void)
{
    check_clean_w();
    check_clean_a();
}

static void create_target_file_w(void)
{
    HANDLE tfwh = CreateFileW(
        target_file_w,
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_NEW,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
    if (tfwh == INVALID_HANDLE_VALUE) {
        wdie(L"can't create \"%s\" (error %d)", target_file_w, GetLastError());
    }
    if (!CloseHandle(tfwh)) {
        wdie(L"BUG: can't close handle after creating \"%s\" (error %d)",
             target_file_w, GetLastError());
    }
}

static void create_target_dir_w(void)
{
    if (!CreateDirectoryW(target_dir_w, NULL)) {
        wdie(L"can't create \"%s\" (error %d)", target_dir_w, GetLastError());
    }
}

static void create_target_file_a(void)
{
    HANDLE tfah = CreateFileA(
        target_file_a,
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_NEW,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
    if (tfah == INVALID_HANDLE_VALUE) {
        die("can't create \"%s\" (error %d)", target_file_a, GetLastError());
    }
    if (!CloseHandle(tfah)) {
        die("BUG: can't close handle after creating \"%s\" (error %d)",
            target_file_a, GetLastError());
    }
}

static void create_target_dir_a(void)
{
    if (!CreateDirectoryA(target_dir_a, NULL)) {
        die("can't create \"%s\" (error %d)", target_dir_a, GetLastError());
    }
}

// Dangling symlinks could be created, but would not test or demonstrate ordinary
// usage. So we create the target files and directories.
static void create_targets(void)
{
    create_target_file_w();
    create_target_dir_w();
    create_target_file_a();
    create_target_dir_a();
}

static enum status create_symlink_file_w(void)
{
    if (CreateSymbolicLinkW(
        symlink_file_w,
        target_file_w,
        SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE
    )) {
        wmsg(L"created file symlink \"%s\" to \"%s\"",
             symlink_file_w, target_file_w);
        return status_success;
    }
    wmsg(L"can't create file symlink \"%s\" to \"%s\" (error %d)",
         symlink_file_w, target_file_w, GetLastError());
    return status_symlink_creation_failed;
}

static enum status create_symlink_dir_w(void)
{
    if (CreateSymbolicLinkW(
        symlink_dir_w,
        target_dir_w,
        SYMBOLIC_LINK_FLAG_DIRECTORY
        | SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE
    )) {
        wmsg(L"created directory symlink \"%s\" to \"%s\"",
             symlink_dir_w, target_dir_w);
        return status_success;
    }
    wmsg(L"can't create directory symlink \"%s\" to \"%s\" (error %d)",
         symlink_dir_w, target_dir_w, GetLastError());
    return status_symlink_creation_failed;
}

static enum status create_symlink_file_a(void)
{
    if (CreateSymbolicLinkA(
        symlink_file_a,
        target_file_a,
        SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE
    )) {
        msg("created file symlink \"%s\" to \"%s\"",
            symlink_file_a, target_file_a);
        return status_success;
    }
    msg("can't create file symlink \"%s\" to \"%s\" (error %d)",
        symlink_file_a, target_file_a, GetLastError());
    return status_symlink_creation_failed;
}

static enum status create_symlink_dir_a(void)
{
    if (CreateSymbolicLinkA(
        symlink_dir_a,
        target_dir_a,
        SYMBOLIC_LINK_FLAG_DIRECTORY
        | SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE
    )) {
        msg(L"created directory symlink \"%s\" to \"%s\"",
            symlink_dir_a, target_dir_a);
        return status_success;
    }
    msg("can't create directory symlink \"%s\" to \"%s\" (error %d)",
        symlink_dir_a, target_dir_a, GetLastError());
    return status_symlink_creation_failed;
}

static enum status create_symlinks(void)
{
    enum status status = status_success;
    status = maxval(status, create_symlink_file_w());
    status = maxval(status, create_symlink_dir_w());
    status = maxval(status, create_symlink_file_a());
    status = maxval(status, create_symlink_dir_a());
    return status;
}

int wmain(void)
{
    check_clean();
    create_targets();
    return create_symlinks();
}
