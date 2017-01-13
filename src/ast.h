/**
 * @file ast.h
 * @version 0.3
 * @author Junde Yhi <lmy441900@gmail.com>
 * @copyright (C) 2012-2016 Anthon Open Source Community Development Hub
 * @copyright This program is licensed under GNU Lesser General Public License. See LICENSE file for details.
 *
 * This file is the main header file for main program's use (only for this testcase).
 *
 * On Windows, some operations must be proceeded with high privileges, like [GetFirmwareEnvironmentVariable](https://msdn.microsoft.com/en-us/library/windows/desktop/ms724325(v=vs.85).aspx),
 * which needs [SE_SYSTEM_ENVIRONMENT](https://msdn.microsoft.com/en-us/library/windows/desktop/bb530716(v=vs.85).aspx#SE_SYSTEM_ENVIRONMENT_NAME) to function. In this file we abstract and
 * declare methods to do with system special privileges.
 */

#ifndef _AST_H
#define _AST_H

#include "firmware/firmware.h"
#include "privilege/privilege.h"
#include "firmware/readefivar.c"

#endif /* end of include guard: _AST_H */
