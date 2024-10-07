// Copyright (C) 2024 Yu Hongbo <yuhongbo@member.fsf.org>, 
//                    CNOCTAVE <cnoctave@qq.com>
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation; either version 3 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
// details.
//
// You should have received a copy of the GNU General Public License along with
// this program; if not, see <http://www.gnu.org/licenses/>.

#include <iostream>
#include <fstream>
#include <vector>
extern "C" {
#include "zstd.h"
}

#include <octave/oct.h>


DEFUN_DLD (zstd_decompress, args, , "\
-*- texinfo -*-\n\
@deftypefn {Loadable Function} @var{0} = zstd_decompress(@var{compressed_filename}, @var{decompressed_filename})\n\
Input compressed filename @var{compressed_filename} and decompressed filename @var{decompressed_filename}.\n\
Always return @var{0}.\n\
@end deftypefn\n\
")
{
  octave_value retval;
  if (args.length () != 2)
    print_usage ();
  if ((! args(0).is_string ()) || (! args(1).is_string ()))
    error ("zstd_decompress: compressed_filename should be a string");
  if ((args(0).isempty ()) || (args(1).isempty ()))
    error ("zstd_decompress: decompressed_filename should not be empty");
  std::string compressed_filename = args(0).string_value ();
  std::string decompressed_filename = args(1).string_value ();


  std::ifstream compressed_file(compressed_filename, std::ios::binary);
  std::ofstream decompressed_file(decompressed_filename, std::ios::binary);

  if (!compressed_file || !decompressed_file) {
    error ("zstd_decompress: cannot open files");
  }

  // Create Zstd decompression stream
  ZSTD_DStream* dstream = ZSTD_createDStream();
  if (dstream == nullptr) {
    error ("zstd_decompress: failed to create decompression stream");
  }

  // Input and output buffers
  const size_t inBufferSize = 1 << 16;  // 64KB
  const size_t outBufferSize = 1 << 20; // 1MB
  std::vector<char> inBuffer(inBufferSize);
  std::vector<char> outBuffer(outBufferSize);

  // Initialize decompression stream
  ZSTD_inBuffer input = { inBuffer.data(), inBuffer.size(), 0 };
  ZSTD_outBuffer output = { outBuffer.data(), outBuffer.size(), 0 };

  // Read and decompress data
  while (true) {
    // Read compressed data into input buffer
    input.pos = 0; // Reset input buffer position
    const void* voidPtr = static_cast<const void*>(input.src); // 转换为 const void*
    // 不安全的转换: 从 const void* 到 char*
    char* charPtr = const_cast<char*>(static_cast<const char*>(voidPtr));
    compressed_file.read(charPtr, input.size);
    if (compressed_file.eof()) {
      input.size = compressed_file.gcount(); // Handle potential partial read at EOF
      if (input.size == 0) break; // No more data to decompress
    } else if (!compressed_file) {
      ZSTD_freeDStream(dstream);
      error ("zstd_decompress: failed to read compressed data" );
    }

    // Decompress data
    size_t ret = ZSTD_decompressStream(dstream, &output, &input);
    if (ZSTD_isError(ret)) {
      ZSTD_freeDStream(dstream);
      std::string concatenated = std::string("zstd_decompress: decompression failed: ") + std::string(ZSTD_getErrorName(ret));
      error ("%s", concatenated.c_str());
    }

    const void* voidPtrOutput = static_cast<const void*>(output.dst); // 转换为 const void*
    // 不安全的转换: 从 const void* 到 char*
    char* charPtrOutput = const_cast<char*>(static_cast<const char*>(voidPtrOutput));
    // Write decompressed data to output file
    decompressed_file.write(charPtrOutput, output.pos);
    if (!decompressed_file) {
      ZSTD_freeDStream(dstream);
      error ("zstd_decompress: failed to write decompressed data" );
    }

    // If the output buffer is not full, it means we've reached the end of the compressed data
    if (output.pos < output.size) {
      break;
    }
  }

  // Clean up and return
  ZSTD_freeDStream(dstream);
  retval = octave_value (0);

  return retval;
}
