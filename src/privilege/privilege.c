/**
 * @file privilege.c
 * @version 0.3
 * @author Junde Yhi <lmy441900@gmail.com>
 * @copyright (C) 2012-2016 Anthon Open Source Community Development Hub
 * @copyright This program is licensed under GNU Lesser General Public License. See LICENSE file for details.
 *
 * This file implements privilege.h.
 *
 * On Windows, some operations must be proceeded with high privileges, like [GetFirmwareEnvironmentVariable](https://msdn.microsoft.com/en-us/library/windows/desktop/ms724325(v=vs.85).aspx),
 * which needs [SE_SYSTEM_ENVIRONMENT](https://msdn.microsoft.com/en-us/library/windows/desktop/bb530716(v=vs.85).aspx#SE_SYSTEM_ENVIRONMENT_NAME) to function. In this file we abstract and
 * declare methods to do with system special privileges.
 */

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "privilege.h"

static int _ast_privilege_do (char *privName, DWORD attr);





int ast_privilege_obtain (char *privName)
{
    if (_ast_privilege_do (privName, SE_PRIVILEGE_ENABLED)) {
        return EXIT_SUCCESS;
    } else {
        return EXIT_FAILURE;
    }
}





int ast_privilege_remove (char *privName)
{
    if (_ast_privilege_do (privName, AST_PRIVILEGE_DISABLED)) { // AST_PRIVILEGE_DISABLED = 0
        return EXIT_SUCCESS;
    } else {
        return EXIT_FAILURE;
    }
}





static int _ast_privilege_do (char *privName, DWORD attr)
{
    HANDLE hToken;
    TOKEN_PRIVILEGES tpNew;
    // XXX: We don't need a previous state here. Invoke ast_privilege_remove to remove a privilege.
    // TOKEN_PRIVILEGES *p_tpPrev = ;

    if (OpenProcessToken (GetCurrentProcess (), TOKEN_ADJUST_PRIVILEGES, &hToken)) {
        if (LookupPrivilegeValue (NULL, privName, &(tpNew.Privileges[0].Luid))) {
            tpNew.PrivilegeCount = 1;
            // tpNew.Privileges[0].Luid is filled just now
            tpNew.Privileges[0].Attributes = attr;

            if (AdjustTokenPrivileges (hToken, FALSE, &tpNew, 0, NULL, NULL)) {
                if (GetLastError () == ERROR_SUCCESS) {
                    // AdjustTokenPrivileges succeeded
                    return EXIT_SUCCESS;
                } else {
                    // AdjustTokenPrivileges succeeded with error code ERROR_NOT_ALL_ASSIGNED (XXX: maybe)
                    fprintf (stderr, "AdjustTokenPrivileges cannot assign privilege %s (error %lu)\n", privName, GetLastError ());
                    return EXIT_FAILURE; // FIXME: Particular exit code
                }
            } else {
                // AdjustTokenPrivileges failed
                fprintf (stderr, "AdjustTokenPrivileges failed with error %lu\n", GetLastError ());
                return EXIT_FAILURE;
            }
        } else {
            // LookupPrivilegeValue failed
            fprintf (stderr, "LookupPrivilegeValue failed with error %lu.\n", GetLastError ());
            return EXIT_FAILURE;
        }
    } else {
        // OpenProcessToken failed
        fprintf (stderr, "OpenProcessToken failed with error %lu.\n", GetLastError ());
        return EXIT_FAILURE;
    }

    return EXIT_FAILURE;
}





int ast_privilege_check_status (char *privName, enum AST_PRIVILEGE_STATUS isStatus)
{
    const size_t BufferSize = 4096; // Should be enough
    char *strPrivName = malloc (BufferSize);
    DWORD lenPrivName = BufferSize;
    TOKEN_PRIVILEGES *tmpToken = malloc (sizeof (TOKEN_PRIVILEGES));
    HANDLE hToken;
    DWORD retLen = 0;
    BOOL bRet = FALSE;

    if (OpenProcessToken (GetCurrentProcess (), TOKEN_QUERY, &hToken)) {
        bRet = GetTokenInformation (hToken, TokenPrivileges, (TOKEN_PRIVILEGES *)tmpToken, sizeof (TOKEN_PRIVILEGES), &retLen);
        if ((bRet) && (retLen <= sizeof (TOKEN_PRIVILEGES))) {
            for (DWORD i = 0; i < tmpToken->PrivilegeCount; i++) {
                // Reset buffer length lenPrivName because LookupPrivilegeName will change its value.
                lenPrivName = BufferSize;

                bRet = LookupPrivilegeName (NULL, &((tmpToken->Privileges[i]).Luid), strPrivName, &lenPrivName);
                if ((bRet) && (lenPrivName <= BufferSize)) {
                    if (strcmp (strPrivName, privName) == 0) {
                        // Privilege name matched; check status via 'Attributes'
                        if (isStatus == (tmpToken->Privileges[i]).Attributes) {
                            CloseHandle (hToken);
                            free (strPrivName);
                            free (tmpToken);
                            return EXIT_SUCCESS;
                        } else {
                            CloseHandle (hToken);
                            free (strPrivName);
                            free (tmpToken);
                            return EXIT_FAILURE;
                        }
                    } else {} // Continue the loop
                } else {
                    // LookupPrivilegeName error
                    fprintf (stderr, "LookupPrivilegeName failed with error %lu.\n", GetLastError ());
                    CloseHandle (hToken);
                    free (strPrivName);
                    free (tmpToken);
                    return EXIT_FAILURE;
                }
            }
        } else {
            // GetTokenInformation error
            fprintf (stderr, "GetTokenInformation failed with error %lu.\n", GetLastError ());
            CloseHandle (hToken);
            free (strPrivName);
            free (tmpToken);
            return EXIT_FAILURE;
        }
    } else {
        // OpenProcessToken error
        fprintf(stderr, "OpenProcessToken failed with error %lu\n", GetLastError ());
        CloseHandle (hToken);
        free (strPrivName);
        free (tmpToken);
        return EXIT_FAILURE;
    }

    return EXIT_FAILURE;
}
