/**
 * @file main.c
 * @version 0.3
 * @author Junde Yhi <lmy441900@gmail.com>
 * @copyright (C) 2012-2016 Anthon Open Source Community Development Hub
 * @copyright This program is licensed under GNU Lesser General Public License. See LICENSE file for details.
 *
 * This file is the main entry of the whole program (only for this testcase).
 *
 * On Windows, some operations must be proceeded with high privileges, like [GetFirmwareEnvironmentVariable](https://msdn.microsoft.com/en-us/library/windows/desktop/ms724325(v=vs.85).aspx),
 * which needs [SE_SYSTEM_ENVIRONMENT](https://msdn.microsoft.com/en-us/library/windows/desktop/bb530716(v=vs.85).aspx#SE_SYSTEM_ENVIRONMENT_NAME) to function. In this file we abstract and
 * declare methods to do with system special privileges.
 */

#include "../ast.h"

int main (void) {
    enum AST_FIRMWARE_TYPE type;
    // {8be4df61-93ca-11d2-aa0d-00e098032b8c} {global} efi_guid_global EFI Global Variable
    static char *EFIGlobalVariableNamespace = "{8be4df61-93ca-11d2-aa0d-00e098032b8c}";
    static char *vars[] = {
        "BootCurrent",
        "BootOrder",
        "BootNext",
        "SecureBoot",
        "Timeout",
        "PlatformLang"
    };
    char *buffer = malloc (4096);

    puts ("========== Your machine's UEFI information is as follows:\n");

    if (ast_get_firmware_type (&type)) {
        printf ("Firmware type: %d\n", (int)type);
    } else {
        printf ("Failed to get firmware type! exit.\n");
        return 1;
    }

    for (int i = 0; i < 6; i++) {
        if (ast_read_efivar (buffer, 4096, EFIGlobalVariableNamespace, vars[i])) {
            printf ("%s: %s", vars[i], buffer);
        } else {
            fprintf (stderr, "Failed to read %s!\n", vars[i]);
        }
    }

    return 0;
}
