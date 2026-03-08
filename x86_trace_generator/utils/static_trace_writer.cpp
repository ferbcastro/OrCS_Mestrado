//
// Copyright (C) 2024  HiPES - Universidade Federal do Paraná
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

/**
 * @file static_trace_writer.cpp
 * @details Implementation of StaticTraceFile class.
 */

#include "static_trace_writer.hpp"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "file_handler.hpp"

extern "C" {
#include <alloca.h>
#include <zlib.h>
}

int StaticTraceWriter::OpenFile(const char* sourceDir, const char* imageName) {
    unsigned long bufferSize;
    char* path;

    bufferSize = GetPathTidOutSize(sourceDir, imageName, STATIC_TRACE_SUFFIX);
    path = (char*)alloca(bufferSize);
    FormatPathTidOut(path, sourceDir, imageName, STATIC_TRACE_SUFFIX, bufferSize);

    printf("Tentando abrir com gzopen\n");
    this->file = gzopen(path, "w");
    if (this->file == NULL) {
        printf("[OpenFile] Failed to alloc gz file [%s]\n", path);
        return 1;
    }
    printf("Sucesso ao abrir arquivo\n");

    unsigned long traceStringSize = this->GetTraceStringMaxSize();
    this->traceString = new char[traceStringSize + sizeof('\0')];

    return 0;
}

int StaticTraceWriter::AddRoutineName(const char* rtn) {
    char* rtnNameFormatted = (char*)alloca(sizeof('#') + strlen(rtn) + sizeof('\n') + sizeof('\0'));
    sprintf(rtnNameFormatted, "#%s\n", rtn);
    unsigned long len = strlen(rtnNameFormatted);
    return (gzwrite(this->file, rtnNameFormatted, (unsigned int)len) != (int)len);
}

int StaticTraceWriter::AddBasicBlockHeader(unsigned int basicBlockTag) {
    static char unsignedIntInStringFormat[sizeof('@') + MAX_U32_DIGITS + sizeof('\n') + sizeof('\0')];
    sprintf(unsignedIntInStringFormat, "@%u\n", basicBlockTag);
    unsigned long len = strlen(unsignedIntInStringFormat);
    return (gzwrite(this->file, unsignedIntInStringFormat, (unsigned int)len) != (int)len);
}

int StaticTraceWriter::AddInstruction(const opcode_package_t* inst) {
    if (this->traceString == NULL) {
        printf("[AddInstruction] trace string is null\n");
        return 1;
    }

    this->OpcodeToTraceString(inst, this->traceString);
    unsigned long len = strlen(this->traceString);
    return (gzwrite(this->file, this->traceString, (unsigned int)len) != (int)len);
}

unsigned long StaticTraceWriter::GetTraceStringMaxSize() {
    unsigned long totalSize = 0;

    /* size for instruction mnemonic */
    totalSize += sizeof(opcode_package_t::opcode_assembly);
    /* size for instruction address */
    totalSize += MAX_U64_DIGITS;
    /* size for instruction size */
    totalSize += MAX_U32_DIGITS;
    /* size for number of registers loaded */
    totalSize += MAX_U32_DIGITS;
    /* size for number of registers stored */
    totalSize += MAX_U32_DIGITS;
    /* size for the loaded registers */
    totalSize += sizeof(opcode_package_t::read_regs) * MAX_U32_DIGITS;
    /* size for the stored registers */
    totalSize += sizeof(opcode_package_t::write_regs) * MAX_U32_DIGITS;
    /* size for index register */
    totalSize += MAX_U32_DIGITS;
    /* size for base register */
    totalSize += MAX_U32_DIGITS;
    /* size for number of memory load operations */
    totalSize += MAX_U32_DIGITS;
    /* size for number of memory store operations */
    totalSize += MAX_U32_DIGITS;
    /* size for branch type */
    totalSize += MAX_U32_DIGITS;
    /* size for boolean 'is_indirect' */
    totalSize += BOOL_VAL_SIZE;
    /* size for boolean 'is_predicated' */
    totalSize += BOOL_VAL_SIZE;
    /* size for boolean 'is_hive' */
    totalSize += BOOL_VAL_SIZE;
    /* size for 'hive_read1' */
    totalSize += MAX_U32_DIGITS;
    /* size for 'hive_read2' */
    totalSize += MAX_U32_DIGITS;
    /* size for 'hive_write' */
    totalSize += MAX_U32_DIGITS;
    /* size for boolean 'is_vima' */
    totalSize += BOOL_VAL_SIZE;
    /* each value is separated by a space character */
    totalSize = totalSize * 2 - 1;

    return totalSize;
}

void StaticTraceWriter::OpcodeToTraceString(const opcode_package_t* op, char* dest) {
    if (dest == NULL) {
        printf("[OpcodeToTraceString] dest is null\n");
        return;
    }

    static char unsignedIntInStringFormat[MAX_U32_DIGITS + sizeof('\0')];
    static char unsignedLongInStringFormat[MAX_U64_DIGITS + sizeof('\0')];

    dest[0] = '\0';
    strcat(dest, op->opcode_assembly);
    strcat(dest, SPACE);
    sprintf(unsignedLongInStringFormat, "%lu", op->opcode_address);
    strcat(dest, unsignedLongInStringFormat);
    strcat(dest, SPACE);
    sprintf(unsignedIntInStringFormat, "%u", op->opcode_size);
    strcat(dest, unsignedIntInStringFormat);
    strcat(dest, SPACE);

    sprintf(unsignedIntInStringFormat, "%u", op->num_load_regs);
    strcat(dest, unsignedIntInStringFormat);
    strcat(dest, SPACE);
    for (unsigned int i = 0; i < op->num_load_regs; i++) {
        sprintf(unsignedIntInStringFormat, "%u", op->read_regs[i]);
        strcat(dest, unsignedIntInStringFormat);
        strcat(dest, SPACE);
    }

    sprintf(unsignedIntInStringFormat, "%u", op->num_store_regs);
    strcat(dest, unsignedIntInStringFormat);
    strcat(dest, SPACE);
    for (unsigned int i = 0; i < op->num_store_regs; i++) {
        sprintf(unsignedIntInStringFormat, "%u", op->read_regs[i]);
        strcat(dest, unsignedIntInStringFormat);
        strcat(dest, SPACE);
    }

    sprintf(unsignedIntInStringFormat, "%u", op->base_reg);
    strcat(dest, unsignedIntInStringFormat);
    strcat(dest, SPACE);
    sprintf(unsignedIntInStringFormat, "%u", op->index_reg);
    strcat(dest, unsignedIntInStringFormat);
    strcat(dest, SPACE);

    sprintf(unsignedIntInStringFormat, "%u", op->num_reads);
    strcat(dest, unsignedIntInStringFormat);
    strcat(dest, SPACE);
    sprintf(unsignedIntInStringFormat, "%u", op->num_writes);
    strcat(dest, unsignedIntInStringFormat);
    strcat(dest, SPACE);

    sprintf(unsignedIntInStringFormat, "%d", op->branch_type);
    strcat(dest, unsignedIntInStringFormat);
    strcat(dest, SPACE);

    sprintf(unsignedIntInStringFormat, "%d", (op->is_indirect) ? 1 : 0);
    strcat(dest, unsignedIntInStringFormat);
    strcat(dest, SPACE);
    sprintf(unsignedIntInStringFormat, "%d", (op->is_predicated) ? 1 : 0);
    strcat(dest, unsignedIntInStringFormat);
    strcat(dest, SPACE);
    sprintf(unsignedIntInStringFormat, "%d", (op->is_hive) ? 1 : 0);
    strcat(dest, unsignedIntInStringFormat);
    strcat(dest, SPACE);

    sprintf(unsignedIntInStringFormat, "%d", op->hive_read1);
    strcat(dest, unsignedIntInStringFormat);
    strcat(dest, SPACE);
    sprintf(unsignedIntInStringFormat, "%d", op->hive_read2);
    strcat(dest, unsignedIntInStringFormat);
    strcat(dest, SPACE);
    sprintf(unsignedIntInStringFormat, "%d", op->hive_write);
    strcat(dest, unsignedIntInStringFormat);
    strcat(dest, SPACE);

    sprintf(unsignedIntInStringFormat, "%d", (op->is_vima) ? 1 : 0);
    strcat(dest, unsignedIntInStringFormat);
    strcat(dest, SPACE);
}
