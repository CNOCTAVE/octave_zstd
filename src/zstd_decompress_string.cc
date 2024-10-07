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


DEFUN_DLD (zstd_decompress_string, args, , "\
-*- texinfo -*-\n\
@deftypefn {Loadable Function} @var{decompressed_string} = zstd_decompress_string(@var{compressed_string})\n\
Input compressed string @var{compressed_string}, return decompressed string @var{decompressed_string}.\n\
@end deftypefn\n\
")
{
  octave_value retval;
  if (args.length () != 1)
    print_usage ();
  if (! args(0).is_string ())
    error ("zstd_decompress_string: string should be a string");
  if (args(0).isempty ())
    error ("zstd_decompress_string: string should not be empty");
  std::string input_string = args(0).string_value ();

  // 获取未压缩数据的最大大小（为了简化，这里假设它与压缩数据大小相同，但实际情况可能不同）
  size_t uncompressed_size = input_string.size() * 4; // 这只是一个示例值，实际情况可能有所不同
  std::vector<char> uncompressed_data(uncompressed_size);

  // 解压缩数据
  size_t result = ZSTD_decompress(uncompressed_data.data(), uncompressed_data.size(), input_string.data(), input_string.size());
  if (ZSTD_isError(result)) {
      std::string concatenated = std::string("zstd_decompress_string: decompression failed: ") + std::string(ZSTD_getErrorName(result));
      error ("%s", concatenated.c_str());
  }

  // 返回解压缩后的字符串（只包含实际解压缩的数据）
  retval = octave_value (std::string(uncompressed_data.data(), result));

  return retval;
}
