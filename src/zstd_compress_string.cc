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


DEFUN_DLD (zstd_compress_string, args, , "\
-*- texinfo -*-\n\
@deftypefn {Loadable Function} @var{compressed_string} = zstd_compress_string(@var{string})\n\
Input string @var{string}, return compressed string @var{compressed_string}.\n\
@end deftypefn\n\
")
{
  octave_value retval;
  if (args.length () != 1)
    print_usage ();
  if (! args(0).is_string ())
    error ("zstd_compress_string: compressed_string should be a string");
  if (args(0).isempty ())
    error ("zstd_compress_string: decompressed_string should not be empty");
  std::string input_string = args(0).string_value ();

  // 获取压缩后的最大大小
  size_t compressed_size = ZSTD_compressBound(input_string.size());

  // 分配内存给压缩后的数据
  std::vector<char> compressed_data(compressed_size);

  // 压缩数据
  size_t result = ZSTD_compress(compressed_data.data(), compressed_data.size(), input_string.data(), input_string.size(), 1);
  if (ZSTD_isError(result)) {
      std::string concatenated = std::string("zstd_compress_string: compression failed: ") + std::string(ZSTD_getErrorName(result));
      error ("%s", concatenated.c_str());
  }

  // 返回压缩后的字符串（只包含实际压缩的数据）
  retval = octave_value (std::string(compressed_data.data(), result));

  return retval;
}
