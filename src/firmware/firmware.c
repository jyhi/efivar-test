/**
 * @file firmware.c
 * @version 0.3
 * @author Junde Yhi <lmy441900@gmail.com>
 * @copyright (C) 2012-2016 Anthon Open Source Community Development Hub
 * @copyright This program is licensed under GNU Lesser General Public License. See LICENSE file for details.
 *
 * This file implements firmware.h.
 */

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <versionhelpers.h>
#include "firmware.h"
#include "../privilege/privilege.h"

static int _ast_get_firmware_type_on_win8_or_greater (enum AST_FIRMWARE_TYPE *T);
static int _ast_get_firmware_type_on_win8_lesser (enum AST_FIRMWARE_TYPE *T);





int ast_get_firmware_type (enum AST_FIRMWARE_TYPE *type)
{
    // NOTE: Since Windows 8, there's a WinAPI called GetFirmwareType
    //   (https://msdn.microsoft.com/en-us/library/windows/desktop/hh848321(v=vs.85).aspx),
    //   which can quickly determine the computer's firmware.
    //
    //   Use GetFirmwareType on newer Windows, or the traditional way -- passing dummy UUID
    //   and variable name to GetFirmwareEnvironmentVariable and check return value.
    if (IsWindows8OrGreater () == TRUE) {
        if (_ast_get_firmware_type_on_win8_or_greater (type)) {
            return EXIT_SUCCESS;
        } else {
            fprintf (stderr, " ** cannot get firmware type.\n");
            return EXIT_FAILURE;
        }
    } else {
        if (_ast_get_firmware_type_on_win8_lesser (type)) {
            return EXIT_SUCCESS;
        } else {
            fprintf(stderr, " ** cannot get firmware type.\n");
        }
    }

    // Whatever.
    return EXIT_FAILURE;
}





int ast_read_efivar (char *var, size_t bufSiz, char *guid, char *name)
{
    DWORD nBytesStored = 0;
    if (ast_privilege_obtain (SE_SYSTEM_ENVIRONMENT_NAME)) {
        // "If the function succeeds, the return value is the number of bytes stored in the pBuffer buffer."
        //   -- https://msdn.microsoft.com/en-us/library/windows/desktop/ms724325(v=vs.85).aspx
        nBytesStored = GetFirmwareEnvironmentVariable (name, guid, var, bufSiz);
        if (nBytesStored != 0) {
            if (nBytesStored <= bufSiz) {
                return EXIT_SUCCESS;
            } else {
                // Memory overflows???
                fprintf (stderr, " ** Warning: nBytesStored > bufSiz.\n");
                return EXIT_SUCCESS; // XXX
            }
        } else {
            // GetFirmwareEnvironmentVariable failed
            fprintf (stderr, " ** GetFirmwareEnvironmentVariable failed with error %lu.\n", GetLastError ());
            return EXIT_FAILURE;
        }
    } else {
        // ast_privilege_obtain failed
        fprintf (stderr, " ** Cannot obtain sufficient privilege. Failed to read efivar.\n");
        return EXIT_FAILURE;
    }
}





int ast_write_efivar (char *value, char *guid, char *name)
{
    return EXIT_FAILURE;
}





static int _ast_get_firmware_type_on_win8_or_greater (enum AST_FIRMWARE_TYPE *T)
{
    FIRMWARE_TYPE type = FirmwareTypeUnknown;
    if (GetFirmwareType (&type)) {
        // AST_FIRMWARE_TYPE declares the same values as FIRMWARE_TYPE
        *T = type;
        return EXIT_SUCCESS;
    } else {
        fprintf (stderr, " ** GetFirmwareType failed with error %lu.\n", GetLastError ());
        return EXIT_FAILURE;
    }

    return EXIT_FAILURE;
}





static int _ast_get_firmware_type_on_win8_lesser (enum AST_FIRMWARE_TYPE *T)
{
    static const char *EFIDummyGUID = "{000000000-0000-0000-0000-000000000000}";
    static const char *EFIDummyName = "";
    void *buffer = malloc (1);
    DWORD ret = 0;

    if (ast_privilege_obtain (SE_SYSTEM_ENVIRONMENT_NAME)) {
        ret = GetFirmwareEnvironmentVariable (EFIDummyName, EFIDummyGUID, buffer, 1);
        if (ret == ERROR_INVALID_FUNCTION) {
            // Not a UEFI machine. XXX: Currently returns BIOS.
            *T = AST_FIRMWARE_TYPE_BIOS;
            return EXIT_SUCCESS;
        } else {
            // XXX: Is a UEFI machine, whatever.
            *T = AST_FIRMWARE_TYPE_UEFI;
            return EXIT_SUCCESS;
        }
    } else {
        fprintf (stderr, " ** failed to obtain appropriate system privilege.\n");
        return EXIT_FAILURE;
    }

    return EXIT_FAILURE;
}
