#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

// {8be4df61-93ca-11d2-aa0d-00e098032b8c} {global} efi_guid_global EFI Global Variable
const char *EFIGlobalVariableNamespace = "{8be4df61-93ca-11d2-aa0d-00e098032b8c}";

char *vars[] = {
    "BootCurrent",
    "BootOrder",
    "BootNext",
    "SecureBoot",
    "Timeout",
    "PlatformLang"
};

static void ast_gain_priv ();
static void ast_get_priv ();

int main (void)
{
    ast_get_priv ();
    ast_gain_priv (SE_BACKUP_NAME);
    ast_gain_priv (SE_RESTORE_NAME);
    ast_gain_priv (SE_SYSTEM_ENVIRONMENT_NAME);
    ast_get_priv ();

    return 0;
}





static void ast_gain_priv (char *privName)
{
    const unsigned int BufferSize = 4096;
    HANDLE hToken;
    LUID luid;
    TOKEN_PRIVILEGES  tpNew;
    PTOKEN_PRIVILEGES p_tpPrev = malloc (BufferSize);
    // LUID_AND_ATTRIBUTES luidNAttr;
    DWORD retLen;
    BOOL bRetVal;

    printf (" ** Now gaining privilege %s\n", privName);

    bRetVal = OpenProcessToken (GetCurrentProcess (), TOKEN_ALL_ACCESS, &hToken);
    if (bRetVal != FALSE) {
        bRetVal = LookupPrivilegeValue (NULL, privName, &luid);
        if (bRetVal != FALSE) {
            // Fill tpNew
            tpNew.PrivilegeCount = 1;
            tpNew.Privileges[0].Luid = luid;
            tpNew.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

            bRetVal = AdjustTokenPrivileges (hToken, FALSE, &tpNew, BufferSize, p_tpPrev, &retLen);
            if ((bRetVal != FALSE) && (retLen <= BufferSize)) {
                printf (" ** Seems to gain %s; Error code = %lu.\n\n", privName, GetLastError ());
                free (p_tpPrev);
                return;
            } else {
                fprintf (stderr, "AdjustTokenPrivileges: failed! (%lu)\n", GetLastError ());
            }
        } else {
            fprintf (stderr, "LookupPrivilegeValue: failed! (%lu)\n", GetLastError ());
        }
    } else {
        fprintf (stderr, "OpenProcessToken: failed! (%lu)\n", GetLastError ());
    }

    return;
}





static void ast_get_priv ()
{
    const unsigned int BufferSize = 4096;
    PTOKEN_PRIVILEGES tmpToken = malloc (BufferSize);
    char *strPrivName = malloc (BufferSize);
    DWORD lenPrivName = BufferSize;
    DWORD retLen = 0;
    BOOL bRetVal;
    HANDLE hToken;

    puts ("=== Now listing privilege information:\n");

    bRetVal = OpenProcessToken (GetCurrentProcess (), TOKEN_ALL_ACCESS, &hToken);
    if (bRetVal != FALSE) {
        bRetVal = GetTokenInformation (hToken, TokenPrivileges, (PTOKEN_PRIVILEGES)tmpToken, BufferSize, &retLen);
        if ((bRetVal != FALSE) && (retLen <= BufferSize)) {
            for (unsigned int i = 0; i < tmpToken->PrivilegeCount; i++) {
                // Reset buffer and lenPrivName
                memset (strPrivName, '\0', BufferSize);
                lenPrivName = BufferSize;

                bRetVal = LookupPrivilegeName (NULL, &((tmpToken->Privileges[i]).Luid), strPrivName, &lenPrivName);
                if ((bRetVal != FALSE) && (lenPrivName <= BufferSize)) {
                    printf ("%s: %lu\n", strPrivName, (tmpToken->Privileges[i]).Attributes);
                } else {
                    // strPrivName is too small to contain a string
                    fprintf(stderr, "cycle %d: Error %lu\n", i, GetLastError ());
                }
            }
        } else {
            // tmpToken is too small to contain a TokenPrivileges
            fprintf(stderr, "GetTokenInformation: Error %lu\n", GetLastError ());
        }
    } else {
        fprintf(stderr, "Cannot open a process token (Error %lu).\n", GetLastError ());
    }

    puts ("=== End listing privilege information\n");
    free (tmpToken);
    free (strPrivName);
    return;
}
