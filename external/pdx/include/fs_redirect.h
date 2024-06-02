#pragma once

extern "C"{
#if defined _WIN32
#include "../src/windows/filesystem/fs_utils.h"
#define OS_SEP '\\'
#define OS_SSEP "\\"
#define FAKE_HOME "c\\users\\user"
#define NT_PREFIX_W L"\\??\\"
#define NT_PREFIX "\\??\\"
#else
#include "../src/linux/filesystem/fs_utils.h"
#define OS_SEP '/'
#define OS_SSEP "/"
#define FAKE_HOME "home/user"
#endif
}

int fs_redirect(char* in_abspath, int is_directory, int is_read, int is_write, int fail_if_exist, int fail_if_not_exist, char** redirected_path);

void enableFS_Redirect();
void disableFS_Redirect();
