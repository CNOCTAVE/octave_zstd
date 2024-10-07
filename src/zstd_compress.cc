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


DEFUN_DLD (zstd_compress, args, , "\
-*- texinfo -*-\n\
@deftypefn {Loadable Function} @var{0} = zstd_compress(@var{filename}, @var{compressed_filename})\n\
Input filename @var{filename} and compressed filename @var{compressed_filename}. Always return @var{0}.\n\
@end deftypefn\n\
")
{
  octave_value retval;
  if (args.length () != 2)
    print_usage ();
  if ((! args(0).is_string ()) || (! args(1).is_string ()))
    error ("zstd_compress: compressed_filename should be a string");
  if ((args(0).isempty ()) || (args(1).isempty ()))
    error ("zstd_compress: decompressed_filename should not be empty");
  std::string input_filename = args(0).string_value ();
  std::string output_filename = args(1).string_value ();
  // 打开输入文件
  std::ifstream input_file(input_filename, std::ios::binary | std::ios::ate);
  if (!input_file.is_open()) {
    error ("zstd_compress: cannot open input file");
  }

  // 获取输入文件大小
  std::streamsize input_size = input_file.tellg();
  input_file.seekg(0, std::ios::beg);

  // 分配内存以读取文件内容
  std::vector<char> input_buffer(input_size);
  if (!input_file.read(input_buffer.data(), input_size)) {
    input_file.close();
    error ("zstd_compress: cannot read input file");
  }
  input_file.close();

  // 分配足够的内存以存储压缩后的数据
  size_t compressed_size = ZSTD_compressBound(input_size);
  std::vector<char> compressed_buffer(compressed_size);

  // 创建压缩上下文
  ZSTD_CCtx* cctx = ZSTD_createCCtx();
  if (!cctx) {
    error ("zstd_compress: cannot create ZSTD compress context");
  }

  // 执行压缩
  size_t result = ZSTD_compressCCtx(cctx, compressed_buffer.data(), compressed_buffer.size(),
                                    input_buffer.data(), input_size, 1); // 1是压缩级别
  if (ZSTD_isError(result)) {
    ZSTD_freeCCtx(cctx);
    std::string concatenated = std::string("zstd_compress: compress failed: ") + std::string(ZSTD_getErrorName(result));
    error ("%s", concatenated.c_str());
  }

  // 调整压缩数据的大小
  compressed_buffer.resize(result);

  // 将压缩后的数据写入输出文件
  std::ofstream output_file(output_filename, std::ios::binary);
  if (!output_file.is_open()) {
    ZSTD_freeCCtx(cctx);
    error ("zstd_compress: cannot create output file");
  }

  output_file.write(compressed_buffer.data(), compressed_buffer.size());
  output_file.close();

  ZSTD_freeCCtx(cctx);
  retval = octave_value (0);

  return retval;
}
