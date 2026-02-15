// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
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

#include "BSDScript.h"

const BSDScriptStackData_t BSDStack[BSD_SCRIPT_STACK_SIZE];

void BSDScriptDump(FILE *BSDFile, int EntryPointOffset) {
    Byte OpCode;

    if (!BSDFile) {
        DPrintf("BSDScriptDump: Invalid file\n");
        return;
    }
    fseek(BSDFile, EntryPointOffset, SEEK_SET);
    fread(&OpCode, sizeof(OpCode), 1, BSDFile);

    while (1) {
        switch (OpCode) {
            case OP_NOP:
                break;
            case OP_LOAD:
                break;
            case OP_STORE:
                break;
            case OP_EQ:
                break;
            case OP_NE:
                break;
            case OP_OR:
                break;
            case OP_NOT:
                break;
            case OP_LT:
                break;
            case OP_GT:
                break;
            case OP_LE:
                break;
            case OP_GE:
                break;
            case OP_ADD:
                break;
            case OP_SUB:
                break;
            case OP_MUL:
                break;
            case OP_DIV:
                break;
            case OP_JZ:
                break;
            case OP_JMP:
                break;
            case OP_CALL8:
                break;
            case OP_CALL16:
                break;
            case OP_RET:
                break;
            case OP_TEST_FLAG:
                break;
            case OP_SET_FLAG:
                break;
            case OP_DBG_LINE_NUM:
                break;
            default:
                DPrintf("BSDScriptDump: Unknown OpCode %i\n",OpCode);
                assert(1!=1);
                break;
        }
    }
}
