
% 数据压缩程序对比综合评测主要分为：

% 对不同大小的文件的压缩性能
% 对不同概率生成的二进制文件的压缩性能

% 单个程序对单个文件的压缩性能评测： 压缩率，压缩速度，解压速度


function [ratio,enc_speed,dec_speed] = entropy_codec_benchmark(enc_cmd_format,dec_cmd_format,in_file,enc_file,dec_file)
% measure the performance of specific entropyCodec program

%TOOD: support input_file_list
%TOOD: support cmd_format_list
%TOOD: support varargin

% check file exist or not
%usage1:  [ratio, enc_speed, dec_speed ] = entropy_codec_benchmark('fse.exe %s -o %s','fse.exe -d %s -o %s','img.bmp','img_fse','img_fse.bmp')
% usage2: 
% aric -c FILE COMPRESSED\n       aric -d COMPRESSED FILE
% [ratio, enc_speed, dec_speed ] = entropy_codec_benchmark('aric.exe -c %s %s','aric.exe -d %s %s','img.bmp','img_aric','img_aric.bmp')

% default arg

if nargin<3;error('enc_cmd_format & dec_cmd_format & input_file is recommended!');end
if nargin<5
	dec_file = sprintf('%s_decode',in_file);
	if nargin<4
		enc_file = sprintf('%s_encode',in_file);
	end
end

if  exist(in_file,'file')==0
	disp(['input file not exist! -- ',in_file]);
	return 
end

if  exist(enc_file,'file')==2
	disp(['rewrite encode file! -- ',enc_file]);
	delete(enc_file);
end

if  exist(dec_file,'file')==2
	disp(['rewrite decode file! -- ',dec_file]);
	delete(dec_file);
end

% cmdstr

enc_cmdstr = sprintf(enc_cmd_format,in_file,enc_file);
dec_cmdstr = sprintf(dec_cmd_format,enc_file,dec_file);

%run benchmark

tic;
	dos(enc_cmdstr);
t_enc_fse =toc;
tic;
	dos(dec_cmdstr);
t_dec_fse =toc;

temp = dir(in_file);
in_size = temp.bytes;
temp = dir(enc_file);
enc_size = temp.bytes;

ratio = enc_size / in_size ;
enc_speed = in_size / t_enc_fse ;
dec_speed = enc_size / t_dec_fse;