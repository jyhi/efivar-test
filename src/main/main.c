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
    // static char *EFIGlobalVariableNamespace = "{8be4df61-93ca-11d2-aa0d-00e098032b8c}";
    // static char *vars[] = {
    //     "BootCurrent",
    //     "BootOrder",
    //     "BootNext",
    //     "SecureBoot",
    //     "Timeout",
    //     "PlatformLang"
    // };
    // char *buffer = malloc (4096);

    puts ("========== Your machine's UEFI information is as follows:\n");

    if (ast_get_firmware_type (&type) == EXIT_SUCCESS) {
        char *firmwareName = malloc (17);
        switch ((int)type) {
            case AST_FIRMWARE_TYPE_UNKNOWN:
                strncpy (firmwareName, "Unknown", 17);
                break;
            case AST_FIRMWARE_TYPE_BIOS:
                strncpy (firmwareName, "BIOS", 17);
                break;
            case AST_FIRMWARE_TYPE_UEFI:
                strncpy (firmwareName, "UEFI", 17);
                break;
            case AST_FIRMWARE_TYPE_NOTIMPL: // fall through
            default:
                strncpy (firmwareName, "Not Implemented", 17);
                break;
        }
        printf ("Firmware type: %s\n", firmwareName);
    } else {
        printf ("Failed to get firmware type! exit.\n");
        return 1;
    }

    // for (int i = 0; i < 6; i++) {
    //     memset (buffer, '\0', 4096);
    //     if (ast_read_efivar (buffer, 4096, EFIGlobalVariableNamespace, vars[i]) == EXIT_SUCCESS) {
    //         if (i < 2) {
    //             char order = '\0';
    //             printf ("%s: ", vars[i]);
    //             for (int j = 0; (order = buffer[j]) != '\0'; j++) {
    //                 printf ("%#o(%#x), ", (int)order, (int)order);
    //             }
    //             printf ("(end)\n");
    //         } else {
    //             printf ("%s: %s\n", vars[i], buffer);
    //         }
    //     } else {
    //         fprintf (stderr, "Failed to read %s!\n", vars[i]);
    //     }
    // }

    ast_read_efivar_standard ();

    return 0;
}
