/**
 * @file firmware.c
 * @version 0.3
 * @author Junde Yhi <lmy441900@gmail.com>
 * @copyright (C) 2012-2017 Anthon Open Source Community Development Hub
 * @copyright This program is licensed under GNU Lesser General Public License. See LICENSE file for details.
 *
 * This file implements functions to read some EFI variables according to the standard UEFI specification. See:
 * http://www.uefi.org/sites/default/files/resources/UEFI%20Spec%202_6.pdf
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <windows.h>
#include "../privilege/privilege.h"

int ast_read_efivar_standard (void);

int ast_read_efivar_standard (void)
{
    uint16_t efiBootCurrent   = 0;
    uint16_t efiBootNext      = 0;
    uint16_t efiTimeout       = 0;
    uint16_t *efiBootOrder    = NULL;
    uint8_t  efiSecureBoot    = 0;
    char     *efiPlatformLang = NULL;
    DWORD    nBytesStored     = 0;

    if (ast_privilege_obtain (SE_SYSTEM_ENVIRONMENT_NAME) == EXIT_SUCCESS) {
        nBytesStored = GetFirmwareEnvironmentVariable ("BootCurrent", "{8be4df61-93ca-11d2-aa0d-00e098032b8c}", &efiBootCurrent, sizeof (efiBootCurrent));
        if (nBytesStored != 0) {
            printf ("BootCurrent: %x\n", efiBootCurrent);
        } else {
            fprintf (stderr, "Failed to read BootCurrent with err %lu.\n", GetLastError ());
        }

        nBytesStored = GetFirmwareEnvironmentVariable ("BootNext", "{8be4df61-93ca-11d2-aa0d-00e098032b8c}", &efiBootNext, sizeof (efiBootNext));
        if (nBytesStored != 0) {
            printf ("BootNext: %x\n", efiBootNext);
        } else {
            fprintf (stderr, "Failed to read BootNext with err %lu.\n", GetLastError ());
        }

        nBytesStored = GetFirmwareEnvironmentVariable ("Timeout", "{8be4df61-93ca-11d2-aa0d-00e098032b8c}", &efiTimeout, sizeof (efiTimeout));
        if (nBytesStored != 0) {
            printf ("Timeout: %x\n", efiTimeout);
        } else {
            fprintf (stderr, "Failed to read Timeout with err %lu.\n", GetLastError ());
        }

        // Initialize efiBootOrder first.
        // XXX: Fixed length
        efiBootOrder = calloc (256, sizeof (uint16_t));
        if (efiBootOrder == NULL) {
            abort ();
        }
        nBytesStored = GetFirmwareEnvironmentVariable ("BootOrder", "{8be4df61-93ca-11d2-aa0d-00e098032b8c}", efiBootOrder, 256 * sizeof (uint16_t));
        if (nBytesStored != 0) {
            printf ("BootOrder: ");
            for (int i = 0; efiBootOrder[i] != '\0'; i++) {
                printf ("%x, ", efiBootOrder[i]);
            }
            printf ("(end)\n");
        } else {
            fprintf (stderr, "Failed to read BootCurrent with err %lu.\n", GetLastError ());
        }
        free (efiBootOrder);

        nBytesStored = GetFirmwareEnvironmentVariable ("SecureBoot", "{8be4df61-93ca-11d2-aa0d-00e098032b8c}", &efiSecureBoot, sizeof (efiSecureBoot));
        if (nBytesStored != 0) {
            printf ("SecureBoot: %d\n", efiSecureBoot);
        } else {
            fprintf (stderr, "Failed to read BootCurrent with err %lu.\n", GetLastError ());
        }

        // Initialize efiPlatformLang first.
        efiPlatformLang = calloc (4096, sizeof (char));
        if (efiPlatformLang == NULL) {
            abort ();
        }
        nBytesStored = GetFirmwareEnvironmentVariable ("PlatformLang", "{8be4df61-93ca-11d2-aa0d-00e098032b8c}", efiPlatformLang, 4096);
        if (nBytesStored != 0) {
            printf ("PlatformLang: %s\n", efiPlatformLang);
        } else {
            fprintf (stderr, "Failed to read BootCurrent with err %lu.\n", GetLastError ());
        }
        free (efiPlatformLang);
    } else {
        fprintf (stderr, "Cannot obtain sufficient privilege. Abort.");
        abort ();
    }

    return 1; // True
}
