## Copyright (C) 2024 Yu Hongbo <yuhongbo@member.fsf.org>, 
##                    CNOCTAVE <cnoctave@qq.com>
##
## This program is free software; you can redistribute it and/or modify it under
## the terms of the GNU General Public License as published by the Free Software
## Foundation; either version 3 of the License, or (at your option) any later
## version.
##
## This program is distributed in the hope that it will be useful, but WITHOUT
## ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
## FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
## details.
##
## You should have received a copy of the GNU General Public License along with
## this program; if not, see <http://www.gnu.org/licenses/>.

## -*- texinfo -*-
## @deftypefn {} {} tar_zstd_compress (@var{source1}, @var{source2}, @var{...} , @var{tar_zstd_filename})
## 将一个或多个文件或文件夹压缩为.tar.zst文件
## @example
## param: source1, source2, ... , tar_zstd_filename
##
## return: status
## @end example
##
## @end deftypefn

function status = tar_zstd_compress(varargin)
    if nargin < 2
        print_usage();
    endif
    temp_tar_filename = tempname();
    tar_zstd_filename = varargin{end};
    varargin{end} = temp_tar_filename;
    status = tar_pack(varargin{:});
    unwind_protect
        if (status == 0)
            zstd_compress(temp_tar_filename, tar_zstd_filename);
        else
            error("tar_zstd_compress: Tar pack failed.");
        endif
    unwind_protect_cleanup
        delete(temp_tar_filename);
    end_unwind_protect
end