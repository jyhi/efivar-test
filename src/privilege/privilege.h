/**
 * @file privilege.h
 * @version 0.3
 * @author Junde Yhi <lmy441900@gmail.com>
 * @copyright (C) 2012-2016 Anthon Open Source Community Development Hub
 * @copyright This program is licensed under GNU Lesser General Public License. See LICENSE file for details.
 *
 * This header file declares interfaces to do with system privileges.
 *
 * On Windows, some operations must be proceeded with high privileges, like [GetFirmwareEnvironmentVariable](https://msdn.microsoft.com/en-us/library/windows/desktop/ms724325(v=vs.85).aspx),
 * which needs [SE_SYSTEM_ENVIRONMENT](https://msdn.microsoft.com/en-us/library/windows/desktop/bb530716(v=vs.85).aspx#SE_SYSTEM_ENVIRONMENT_NAME) to function. In this file we abstract and
 * declare methods to do with system special privileges.
 */

#ifndef _AST_PRIVILEGE_H
#define _AST_PRIVILEGE_H

#include <windows.h>

/**
 * Enumeration that indicates the privilege status.
 *
 * See [AdjustTokenPrivileges function](https://msdn.microsoft.com/en-us/library/windows/desktop/aa375202(v=vs.85).aspx)
 * for more information.
 *
 * @see ast_privilege_check_status
 */
enum AST_PRIVILEGE_STATUS {
    AST_PRIVILEGE_ENABLED  = SE_PRIVILEGE_ENABLED, /**< The privilege is enabled */
    AST_PRIVILEGE_REMOVED  = SE_PRIVILEGE_REMOVED, /**< The privilege is removed from the access token */
    AST_PRIVILEGE_DISABLED = 0                     /**< The privilege is disabled */
};

/**
 * Obtain the specified privilege for the current process.
 *
 * This function will invoke [GetCurrentProcess](https://msdn.microsoft.com/en-us/library/windows/desktop/ms683179(v=vs.85).aspx)
 * and open a primary token for the current process, and then use [AdjustTokenPrivileges](https://msdn.microsoft.com/en-us/library/windows/desktop/aa375202(v=vs.85).aspx)
 * to enable the specified privilege.
 *
 * @param privName [in] Name of the privilege. On Windows this is specified as constants in Winbase.h (which is included in Windows.h).
 *                      Just pass the constant to this function.
 * @return EXIT_SUCCESS if operation succeeded, or particular return code may return.
 * @see ast_privilege_remove
 */
int ast_privilege_obtain (char *privName);

/**
 * Remove the specified privilege for the current process.
 *
 * This function works the same as ast_privilege_obtain, but removes the specified privilege from the current pricess' access token.
 *
 * @param privName [in] Name of the privilege. On Windows this is specified as constants in Winbase.h (which is included in Windows.h).
 *                      Just pass the constant to this function.
 * @return EXIT_SUCCESS if operation succeeded, or particular return code may return.
 * @see ast_privilege_obtain
 */
int ast_privilege_remove (char *privName);

/**
 * Check the status of the current process.
 *
 * @param privName [in] Name of the privilege. On Windows this is specified as constants in Winbase.h (which is included in Windows.h).
 *                      Pass the constant to this function to obtain it.
 * @param isStatus [in] Status of the privilege to check.
 * @return EXIT_SUCCESS if the specified privilege is at such status.
 *         EXIT_FAILURE if the specified privilege is not at such status.
 * @see ast_privilege_is_enabled, ast_privilege_is_disabled, ast_privilege_is_removed
 */
int ast_privilege_check_status (char *privName, enum AST_PRIVILEGE_STATUS isStatus);

/**
 * Wrapper of ast_privilege_check_status, making it easier to check if the specified privilege is enabled.
 *
 * @param name [in] Name of the privilege.
 * @return The same as ast_privilege_check_status.
 */
#define ast_privilege_is_enabled(name) ast_privilege_check_status (name, AST_PRIVILEGE_ENABLED)

/**
 * Wrapper of ast_privilege_check_status, making it easier to check if the specified privilege is disabled.
 *
 * @param name [in] Name of the privilege.
 * @return The same as ast_privilege_check_status.
 */
#define ast_privilege_is_disabled(name) ast_privilege_check_status (name, AST_PRIVILEGE_DISABLED)

/**
 * Wrapper of ast_privilege_check_status, making it easier to check if the specified privilege is removed.
 *
 * @param name [in] Name of the privilege.
 * @return The same as ast_privilege_check_status.
 */
#define ast_privilege_is_removed(name) ast_privilege_check_status (name, AST_PRIVILEGE_REMOVED)

#endif /* end of include guard: _AST_PRIVILEGE_H */
