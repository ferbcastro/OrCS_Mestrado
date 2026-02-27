#ifndef SINUCA3_GENERATOR_STATIC_FILE_HPP_
#define SINUCA3_GENERATOR_STATIC_FILE_HPP_

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
 * @file static_trace_writer.hpp
 * @details A static trace contains all the basic blocks 'touched' within
 * execution with its instructions. This way the dynamic trace has to store the
 * indexes of the basic blocks executed so that the execution may be simulated.
 * Remember that a basic block is a piece of code with a single entrance point
 * and a single exit. As the name suggests, all information regarding the
 * instruction that is not dynamic (e.g. the number of registers accessed) is
 * stored in the static file. The reader expects the number of instructions of
 * the basic block being read before the instructions per se, hence a method to
 * add a StaticTraceRecord with the size is implemented. The implementation
 * does not force the 'AddBasicBlockSize' and 'AddInstruction' to be called in a
 * certain order for things to work.
 */

#include <cstdlib>
#include "file_handler.hpp"
#include "../../package/opcode_package.hpp"

extern "C" {
#include "zlib.h"
}

#define BOOL_VAL_SIZE 1
#define SPACE " "

/** @brief Check static_trace_writer.hpp documentation for details */
class StaticTraceWriter {
  private:
    gzFile file;
    char* traceString;

    unsigned long GetTraceStringMaxSize();

    void OpcodeToTraceString(const opcode_package_t* inst, char* dest);

  public:
    StaticTraceWriter() : file(NULL), traceString(NULL) {};
    ~StaticTraceWriter() {
        if (this->file)
            gzclose(this->file);
        if (this->traceString)
            delete[] this->traceString;
    }

    int OpenFile(const char* sourceDir, const char* imageName);

    int AddRoutineName(const char* rtn);

    int AddBasicBlockTag(unsigned int basicBlockTag);

    int AddInstruction(const opcode_package_t* inst);
};

#endif
