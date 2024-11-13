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
  std::vector<char> compressed_data((std::istreambuf_iterator<char>(compressed_file)), std::istreambuf_iterator<char>());
  size_t compressed_size = compressed_data.size();

  // 获取未压缩数据的大小（这通常需要外部信息，例如从文件头或其他元数据）
  // 在这个示例中，我们假设我们有这个信息，或者使用一个足够大的缓冲区来容纳任何可能的解压缩数据。
  // 为了简化，我们使用一个足够大的静态值。
  // 将 std::vector<char> 转换为 const void*
  const void* dataPtr = static_cast<const void*>(compressed_data.data());
  size_t decompressed_size = ZSTD_getFrameContentSize(dataPtr, compressed_size);
  std::vector<char> decompressed_buffer(decompressed_size);

  size_t decompressed_ret = ZSTD_decompress(decompressed_buffer.data(), decompressed_buffer.size(),
                                              compressed_data.data(), compressed_size);
  if (ZSTD_isError(decompressed_ret)) {
      std::string concatenated = std::string("zstd_decompress: decompression failed: ") + std::string(ZSTD_getErrorName(decompressed_ret));
      error ("%s", concatenated.c_str());
  }

  std::ofstream decompressed_file(decompressed_filename, std::ios::binary);
  decompressed_file.write(decompressed_buffer.data(), decompressed_size);
  retval = octave_value (0);

  return retval;
}
