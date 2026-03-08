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
 * @file dynamic_trace_writer.cpp
 * @details Implementation of DynamicTraceFile class.
 */

#include "dynamic_trace_writer.hpp"
#include "file_handler.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" {
#include <alloca.h>
}

int DynamicTraceWriter::OpenFile(const char* sourceDir, const char* imageName,
                                 int tid) {
    unsigned long bufferSize;
    char* path;

    bufferSize = GetPathTidInSize(sourceDir, imageName, DYNAMIC_TRACE_SUFFIX);
    path = (char*)alloca(bufferSize);
    FormatPathTidIn(path, sourceDir, imageName, DYNAMIC_TRACE_SUFFIX, tid, bufferSize);
    this->file = gzopen(path, "w");
    if (this->file == NULL) {
        printf("[OpenFile] Failed to alloc gz file in dynamic trace!");
        return 1;
    }

    return 0;
}

int DynamicTraceWriter::AddThreadEvent(ThreadEventType evType, const char* rtn) {
    char* rtnHeader = (char*)alloca(sizeof('#') + strlen(rtn) + sizeof('\n') + sizeof('\0'));
    char* event = (char*)alloca(sizeof('$') + MAX_U32_DIGITS + sizeof('\n') + sizeof('\0'));

    sprintf(rtnHeader, "#%s\n", rtn);
    sprintf(event, "$%u\n", evType);

    unsigned long len = strlen(rtnHeader);
    if (gzwrite(this->file, rtnHeader, (unsigned int)len) != (int)len) {
        return 1;
    }
    len = strlen(event);
    if (gzwrite(this->file, event, (unsigned int)len) != (int)len) {
        return 1;
    }

    return 0;
}

int DynamicTraceWriter::AddBasicBlockTag(unsigned int tag) {
    static char unsignedIntToString[MAX_U32_DIGITS + 1 + sizeof('\0')];
    sprintf(unsignedIntToString, "%u\n", tag);
    unsigned long len = strlen(unsignedIntToString);
    return (gzwrite(this->file, unsignedIntToString, (unsigned int)len) != (int)len);
}
