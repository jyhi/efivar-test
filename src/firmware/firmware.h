/**
 * @file firmware.h
 * @version 0.3
 * @author Junde Yhi <lmy441900@gmail.com>
 * @copyright (C) 2012-2016 Anthon Open Source Community Development Hub
 * @copyright This program is licensed under GNU Lesser General Public License. See LICENSE file for details.
 *
 * This header file declares interfaces to operate PC firmwares like EFI.
 */

#ifndef _AST_FIRMWARE_H
#define _AST_FIRMWARE_H

#include <stdio.h>
#include <windows.h>

/**
 * Enumeration to mark firmware types.
 *
 * Values of these constants follow the Microsoft API enumeration
 * [FIRMWARE_TYPE](https://msdn.microsoft.com/en-us/library/windows/desktop/hh848320(v=vs.85).aspx):
 * @see ast_get_firmware_type
 */
enum AST_FIRMWARE_TYPE {
    AST_FIRMWARE_TYPE_UNKNOWN = 0, /**< The firmware type is unknown. */
    AST_FIRMWARE_TYPE_BIOS    = 1, /**< The computer booted in legacy BIOS mode. */
    AST_FIRMWARE_TYPE_UEFI    = 2, /**< The computer booted in UEFI mode. */
    AST_FIRMWARE_TYPE_NOTIMPL = 3  /**< Not implemented. */
};

/**
 * Function to get firmware type.
 *
 * @param *type [out] Pointer to an AST_FIRMWARE_TYPE enumeration, which will contain the obtained firmware type.
 * @return EXIT_SUCCESS if operation succeeded, or particular return code may return.
 */
int ast_get_firmware_type (enum AST_FIRMWARE_TYPE *type);

/**
 * Function to read EFI variable.
 *
 * This function automatically gains its necessary privileges. If it fails to gain privileges,
 * it will return AST_RETURN_ACCESS_DENIED.
 *
 * @param var    [out] Buffer to put variable value.
 * @param bufSiz [in]  Size of the buffer.
 * @param guid   [in]  GUID namespace.
 * @param name   [in]  Variable name.
 * @return EXIT_SUCCESS if operation succeeded, or particular return code may return.
 *
 * @see ast_write_efivar
 */
int ast_read_efivar (char *var, size_t bufSiz, char *guid, char *name);

/**
 * Function to write EFI variable.
 *
 * This function automatically gains its necessary privileges. If it fails to gain privileges,
 * it will return AST_RETURN_ACCESS_DENIED.
 *
 * __NOTE:__ This function does not guarantee its effect. If the firmware fails, this function simply
 * returns AST_RETURN_OPERATION_FAILED and you should take measures to save the firmware.
 *
 * @param value [in] Value to be put into the specified EFI variable.
 * @param guid  [in] GUID namespace.
 * @param name  [in] Variable name.
 * @return EXIT_SUCCESS if operation succeeded, or particular return code may return.
 *
 * @see ast_read_efivar
 */
int ast_write_efivar (char *value, char *guid, char *name);

#endif /* end of include guard: _AST_FIRMWARE_H */
