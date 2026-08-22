#include "ti_kernel.h"

#include <stdio.h>
#include <wchar.h>

static void print_usage(void)
{
    fwprintf(stderr,
        L"TrustedInstaller CLI\n\n"
        L"用法:\n"
        L"  ti.exe                  打开 TI 权限 CMD 终端\n"
        L"  ti.exe -p               打开 TI 权限 PowerShell\n"
        L"  ti.exe run <命令...>    以 TI 身份执行命令\n"
        L"  ti.exe help             显示帮助\n");
}

static void print_error(int code)
{
    fwprintf(stderr, L"错误: %s", ti_strerror(code));
    if (code == TI_ERR_WIN32)
        fwprintf(stderr, L" (%lu)", (unsigned long)GetLastError());
    fputws(L"\n", stderr);
}

int wmain(int argc, wchar_t **argv)
{
    int powershell = 0;
    int r;

    if (argc >= 2) {
        if (wcscmp(argv[1], L"help") == 0 || wcscmp(argv[1], L"-h") == 0 || wcscmp(argv[1], L"--help") == 0) {
            print_usage();
            return 0;
        }
        if (wcscmp(argv[1], L"-p") == 0 || wcscmp(argv[1], L"--powershell") == 0) {
            powershell = 1;
            r = ti_start_shell(1);
            if (r == TI_OK) {
                fwprintf(stdout, L"已启动 TI 权限 CLI\n");
                return 0;
            }
            print_error(r);
            return 1;
        }
        if (wcscmp(argv[1], L"run") == 0) {
            wchar_t cmdline[1024] = L"";
            ti_process_result result = { 0 };

            if (argc < 3) {
                fwprintf(stderr, L"错误: run 需要指定命令\n");
                return 1;
            }
            for (int i = 2; i < argc; i++) {
                if (i > 2) wcsncat(cmdline, L" ", TI_COUNT(cmdline) - wcslen(cmdline) - 1);
                wcsncat(cmdline, argv[i], TI_COUNT(cmdline) - wcslen(cmdline) - 1);
            }
            r = ti_run_command(cmdline, 1, &result);
            if (r == TI_OK) {
                fwprintf(stdout, L"已启动 TI 进程，PID: %lu\n", (unsigned long)result.process_id);
                return 0;
            }
            print_error(r);
            return 1;
        }
    }

    r = ti_start_shell(powershell);
    if (r == TI_OK) {
        fwprintf(stdout, L"已启动 TI 权限 CLI\n");
        return 0;
    }
    print_error(r);
    return 1;
}
