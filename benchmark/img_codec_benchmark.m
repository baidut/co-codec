
%% 测试jpeg的有损压缩性能 显示效果和压缩性能评估
% 评估参数: 采用不同质量的压缩速度、压缩后大小，图像视觉感官（合成图）
放在一个轴上显示 

%jpeg很好的讲解 http://blog.csdn.net/lhfslhfs/article/details/7273216
% jpeg评测
% matlab自带的jpeg压缩法 无 


% cmake 等等
% q- 质量为整数？ 图片尺寸为整数，每个像素列一个质量 质量为int类型 取整即可 0-100之间


%% 有损压缩测试
% 调用程序传入不同的质量参数，转回ppm，（matlab不干预）得到生成的图像的矩阵
% 取各个矩阵的一部分组合，从左到右
% 可以设置细致程度，矩阵可能过多，设置临时矩阵存放

%进行评测对比
% 计算图片信息
in_file = 'newcat.ppm';
I=imread(in_file); %读入原始图片
length=size(I,1); %获取图像长
width=size(I,2); %获取图像宽
temp = dir(in_file);
in_size = temp.bytes; %获取图像大小
% 生成各种质量的图片，采用huffman速度快，记录速度和文件大小
for i=1:100 % 质量渐变
	prefix = num2str(i,'libjpeg.exe -q %d ');
	enc_cmd_format_huff = strcat(prefix,' %s %s');
	enc_cmd_format_aric = strcat(prefix,' -a %s %s');
	dec_cmd_format = 'libjpeg.exe %s %s';
	[huff_ratio(i), huff_enc_speed(i), huff_dec_speed(i) ] = entropy_codec_benchmark(enc_cmd_format_huff,dec_cmd_format,in_file,'tmp_encode','tmp_decode');
	[aric_ratio(i), aric_enc_speed(i), aric_dec_speed(i) ] = entropy_codec_benchmark(enc_cmd_format_aric,dec_cmd_format,in_file,'tmp_encode','tmp_decode');
end

% 评测会删除临时生成的图片，需要生成图片
% 生成各种质量的图片
for i=1:100 % 质量渐变
	enc_file = 'tmp_encode';% num2str(i,'cat%03d.jpg');
	dec_file = num2str(i,'cat%03d.ppm');
	enc_cmd_str = sprintf('libjpeg.exe -q %d %s %s',i,in_file,enc_file);
	dec_cmd_str = sprintf('libjpeg.exe %s %s',enc_file,dec_file);
	dos(enc_cmd_str);
	dos(dec_cmd_str);
end

%% 根据数据绘图 todo自动统计测评函数 添加绘图
X = 1:100;
plot(X ,aric_enc_speed,':pb',X,huff_enc_speed,':*r');
xlabel('quality'),ylabel('bytes/s'),title('Compression Speed');
legend('aric','huff',4);

plot(X ,aric_dec_speed,':pb',X ,huff_dec_speed,':*r');
xlabel('quality'),ylabel('bytes/s'),title('Decompression Speed');
legend('aric','huff',4);

plot(X,aric_ratio,':pb',X,huff_ratio,':*r');
xlabel('bytes'),ylabel('ratio'),title('Ratio');
legend('aric','huff',4);

%拼成一个图片
in_file = 'cat.ppm';
I=imread(in_file); %读入原始图片
width=size(I,2); %获取图像宽

for i=1:100 % 质量渐变
	dec_file = num2str(i,'newcat/newcat%03d.ppm');
	I = imread(dec_file);
	left = 1+int32(floor( width*(i-1)/100 )); % matlab 下标从1开始
	right = int32(floor( width*i/100 )); %Index exceeds matrix dimensions. i = 100
	temp(:,:,:)=I(:,left:right,:);% I=I(:,[left:right]); 结果长度会多100 注意图像不是矩阵
	if i == 1
		A = temp;
	else
		A = [A temp];
	end 
	clear temp;%避免Subscripted assignment dimension mismatch.
end

%修改尺寸测试（为了使得区分更加明显）

in_file = 'cat2.jpg';
I=imread(in_file); %读入原始图片
length=size(I,1); %获取图像长
width=size(I,2); %获取图像宽
I=imresize(I,[128,128]);

%改成前30 之后基本看不出变化
from_q = 1;
to_q = 10;

diff_q = to_q - from_q + 1;
for i=from_q:to_q % 质量渐变
	dec_file = num2str(i,'newcat/newcat%03d.ppm');
	I = imread(dec_file);
	left = 1+int32(floor( width*(i-1)/diff_q )); % matlab 下标从1开始
	right = int32(floor( width*i/diff_q )); %Index exceeds matrix dimensions. i = 100
	temp(:,:,:)=I(:,left:right,:);% I=I(:,[left:right]); 结果长度会多100 注意图像不是矩阵
	if i == 1
		A = temp;
	else
		A = [A temp];
	end 
	clear temp;%避免Subscripted assignment dimension mismatch.
end

优缺点：图像格式支持少

%% 生成100个测试图片
% BMP BMP图像文件格式 BMP是一种与硬件设备无关的图像文件格式，使用非常广。它采用位映射存储格式，除了图像深度可选以外，不采用其他任何压缩，因此，BMP文件所占用的空间很大。
I=imread('img.bmp'); %读入原始图片
length=size(I,1); %获取图像长
width=size(I,2); %获取图像宽
for num=1:99
	J=imresize(I,[length*num/100,width*num/100]);
	filename=num2str(num,'img%02d.bmp');
	imwrite(J,filename);
end