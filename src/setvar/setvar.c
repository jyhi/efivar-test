/**
 * @file setvar.c
 * @version 0.3
 * @author Junde Yhi <lmy441900@aosc.xyz>
 * @copyright (C) 2012-2017 Anthon Open Source Community Development Hub
 * @copyright This program is licensed under GNU Lesser General Public License. See LICENSE file for details.
 *
 * This file is a testcase for AST Startup Toolkit's efivar preparation procedure.
 *
 * To let UEFI boot our specified image, we need to set:
 *
 * - `BootNext`
 * - `Boot####` (where `####` varies)
 *
 * To avoid annoying Secure Boot, we need to detect `SecureBoot` too.
 *
 * `Boot####` contains a set of data structure. For details, see:
 *   www.uefi.org/sites/default/files/resources/UEFI Spec 2_6.pdf
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <windows.h>
#include "../ast.h"

#define EFI_GLOBAL_GUID "{8be4df61-93ca-11d2-aa0d-00e098032b8c}"

/**
 * 16-Bit fixed-length character type.
 *
 * C has no such type, but as UEFI specification declares, we as well define a
 * corresponding CHAR16 (to contain UCS-2 (UTF-16) characters).
 *
 * Windows has such definition (Well, `wchar_t` is 16-bit long in Windows), but
 * we don't use it here.
 */
typedef uint16_t char16_t;

/**
 * EFI device path protocol descriptor.
 *
 * The device path describes the location of the device the handle is for. See
 * UEFI specification 2.6: 9.2 EFI Device Path Protocol.
 */
typedef struct _EFI_DEVICE_PATH_PROTOCOL {
    uint8_t Type;
    uint8_t SubType;
    uint8_t Length[2];
} EFI_DEVICE_PATH_PROTOCOL;

/**
 * Load options.
 *
 * Each load option variable contains a EFI_LOAD_OPTION descriptor that is a
 * byte packed buffer of variable length fields. See UEFI specification 2.6:
 * 3.1.3 Load Options.
 */
typedef struct _EFI_LOAD_OPTION {
    uint32_t Attributes;
    uint16_t FilePathListLength;
    char16_t *Description;
    EFI_DEVICE_PATH_PROTOCOL *FilePathList;
    uint8_t  *OptionalData;
} EFI_LOAD_OPTION;

/**
 * Function performing a series of operation to prepare a boot option in EFI and set it to boot next time.
 */
int efivar_set (void);

/**
 * Function filling in an EFI_LOAD_OPTION.
 */
int efi_load_option_fill (EFI_LOAD_OPTION *option);

int efivar_set (void)
{
    uint8_t  efiSecureBoot = 0;
    uint16_t efiBootNext   = 0;
    EFI_LOAD_OPTION efiBootOption = {0};
    DWORD    nBytesStored  = 0;

    if (ast_privilege_obtain (SE_SYSTEM_ENVIRONMENT_NAME) != EXIT_SUCCESS)
    {
        fprintf (stderr, "Failed to get sufficient privilege. Abort.\n");
        exit (1);
    }

    // Check SecureBoot first.
    // We don't want to support this shit.
    nBytesStored = GetFirmwareEnvironmentVariable ("SecureBoot", EFI_GLOBAL_GUID, &efiSecureBoot, sizeof (efiSecureBoot));
    if (nBytesStored != 0)
    {
        printf ("Secure Boot is currently %s.\n", efiSecureBoot == 0 ? "off" : "on");
        if (efiSecureBoot != 0)
        {
            puts ("We do not support Secure Boot now.");
        }
    }
    else
    {
        fprintf (stderr, "Failed to read SecureBoot with error %lu.\n", GetLastError ());
    }

    // Prepare Boot####.
    // To avoid conflict, we need to determine the "####" first (by checking whether our selected number is occupied),
    //   and then fill in the complicated buffer.
    char *efiBootOptionName = malloc (9); // strlen ("Boot####") + 1(NUL);
    char *efiTempBuffer     = malloc (4096);
    for (uint16_t i = 0x0004; i < 0x00FF; i++)
    {
        if (snprintf (efiBootOptionName, 9, "Boot%04X", i) == 4)
        {
            nBytesStored = GetFirmwareEnvironmentVariable (efiBootOptionName, EFI_GLOBAL_GUID, &efiTempBuffer, 4096);
            if (nBytesStored != 0)
            {
                // Boot option exists, switch to the next number...
                continue;
            }
            else
            {
                // Boot option does not exist, so it is ours...
                if (efi_load_option_fill (&efiBootOption))
                {
                    break;
                }
                else
                {
                    fprintf(stderr, "Failed to fill in %s... Abort.\n", efiBootOptionName);
                    exit (1);
                }
            }
        }
        else
        {
            fprintf (stderr, "We met an error when doing snprintf... Abort.");
            abort ();
        }
    }
    // efiBootOptionName will be used later.
    free (efiTempBuffer);
    efiTempBuffer = NULL;

    // Save Boot#### to NVRAM.
    if (!SetFirmwareEnvironmentVariable (
        efiBootOptionName, EFI_GLOBAL_GUID, &efiBootOption,
        sizeof (uint32_t) + sizeof (uint16_t)
        + wcslen ((wchar_t *)efiBootOption.Description) + efiBootOption.FilePathListLength // XXX: uint16_t -> wchar_t (although it works)
        + strlen ((char *)(efiBootOption.OptionalData == NULL ? "" : efiBootOption.OptionalData))))
    {
        fprintf (stderr, "We met an error while setting %s... (error %lu) Abort.\n", efiBootOptionName, GetLastError ());
    }

    // Check if BootNext is available to set (not exists).
    // If BootNext exists, there must be something interesting.
    nBytesStored = GetFirmwareEnvironmentVariable ("BootNext", EFI_GLOBAL_GUID, &efiBootNext, sizeof (efiBootNext));
    if (nBytesStored == 0)
    {
        if (GetLastError () == ERROR_ENVVAR_NOT_FOUND)
        {
            // BootNext does not exist, so it is our turn.
            // Set BootNext to current Boot####.
            if (!SetFirmwareEnvironmentVariable ("BootNext", EFI_GLOBAL_GUID, efiBootOptionName, strlen (efiBootOptionName)))
            {
                fprintf (stderr, "We met an error while setting BootNext... (error %lu) Abort.", GetLastError ());
                abort ();
            }
        }
        else
        {
            // Other kinds of error occurred...
            fprintf (stderr, "Failed to read BootNext with error %lu.\n", GetLastError ());
        }
    }
    else
    {
        fprintf (stderr, "BootNext exists! Abort.\n");
        exit (1);
    }

    free (efiBootOptionName);
    efiBootOptionName = NULL;
    return true;
}

int efi_load_option_fill (EFI_LOAD_OPTION *option)
{
    /* Attributes         = 0x00000001 (LOAD_OPTION_ACTIVE) {4}
     * FilePathListLength = 0x5e (94) {2}
     * Description        = "Boot Linux OS from AST" {46}
     * FilePathList[0] => {42}
     *   Type       = 0x04 (Media Device Path) {1}
     *   SubType    = 0x01 (Hard Drive) {1}
     *   Length[0]  = 0x2a (42) Length[1] = 0x00 {2}
     *   PartitionNumber    = TODO {4}  << WinAPI (<PARTITION_INFORMATION_EX>.PartitionNumber)
     *   PartitionStart     = TODO {8}  << GPT Partition Header (StartingLBA) || (WinAPI returns bytes)
     *   PartitionSize      = TODO {8}  << GPT Partition Header (EndingLBA - StartingLBA + 1) || (WinAPI returns bytes)
     *   PartitionSignature = TODO {16} << GPT Partition Header (UniquePartitionGUID) || WinAPI (<PARTITION_INFORMATION_EX>.Gpt.PartitionId)
     *   PartitionFormat    = 0x02 (GUID Partition Table) (XXX) {1}
     *   SignatureType      = 0x02 (GUID signature) (XXX) {1}
     * FilePathList[1] => {48}
     *   Type       = 0x04 (Media Device Path) {1}
     *   SubType    = 0x04 (File Path) {1}
     *   Length[0]  = 0x30 (48) Length[1] = 0x00 {2}
     *   PathName   = "\EFI\AST\astg2x64.efi" {44}
     * FilePathList[2] => {4}
     *   Type       = 0x7f (End of Hardware Device Path) {1}
     *   SubType    = 0xff (End Entire Device Path) {1}
     *   Length[0]  = 0x04 Length[1] = 0x00 {2}
     * OptionalData = NULL
     *
     * Total length = 146 (0x92)
     */
    return false;
}
