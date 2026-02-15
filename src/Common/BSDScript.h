/*
===========================================================================
    Copyright (C) 2018-2026 Adriano Di Dio.

    Medal-Of-Honor-PSX-File-Viewer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Medal-Of-Honor-PSX-File-Viewer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Medal-Of-Honor-PSX-File-Viewer.  If not, see <http://www.gnu.org/licenses/>.
===========================================================================
*/

#ifndef __BSD_SCRIPT_H_
#define __BSD_SCRIPT_H_

#include "Common.h"

// VM stack size is made up of 64 structures each one containing 8 bytes (Type and Value)
// for a total of 512 bytes
#define BSD_SCRIPT_STACK_SIZE 64

typedef enum {
    OP_NOP        = 0x00,
    OP_LOAD       = 0x02,
    OP_STORE      = 0x03,

    OP_EQ         = 0x04,
    OP_NE         = 0x05,
    OP_OR         = 0x06,
    OP_NOT        = 0x07,
    OP_LT         = 0x08,
    OP_GT         = 0x09,
    OP_LE         = 0x0A,
    OP_GE         = 0x0B,

    OP_ADD        = 0x0C,
    OP_SUB        = 0x0D,
    OP_MUL        = 0x0E,
    OP_DIV        = 0x0F,

    OP_JZ         = 0x10,
    OP_JMP        = 0x11,

    OP_CALL8      = 0x12,
    OP_CALL16     = 0x13,

    OP_RET        = 0x14,

    OP_TEST_FLAG      = 0x15,
    OP_SET_FLAG       = 0x16,

    OP_DBG_LINE_NUM    = 0x17
} BSDOpCode_t;

typedef enum {
    BSD_STACK_DATA_TYPE_BOOL  = 0xB,
    BSD_STACK_DATA_TYPE_INT   = 0xA,
    BSD_STACK_DATA_TYPE_FLOAT = 0xE,
    BSD_STACK_DATA_TYPE_PTR   = 0x10
} BSDStackDataType_t;

typedef struct BSDScriptStackData_s {
    BSDStackDataType_t Type;
    void *Data;
} BSDScriptStackData_t;

void BSDScriptDump(FILE *BSDFile, int EntryPointOffset);

#endif //__BSD_SCRIPT_H_