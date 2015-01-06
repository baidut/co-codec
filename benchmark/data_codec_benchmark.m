

%% ANS vs Arithmetic Coding

Environment
program
related file

!jpeg.exe cat.jpg cat.ppm
I = imread ('cat.ppm');
imshow(I)

% 计算源文件的大小
for i=1:99
	in_file=num2str(i,'img%02d.bmp');
	temp = dir(in_file);
	in_size(i) = temp.bytes;
end

for i=1:99
	in_file=num2str(i,'img%02d.bmp');
	temp = dir(in_file);
	in_size(i) = temp.bytes;
	[aric_ratio(i), aric_enc_speed(i), aric_dec_speed(i) ] = entropy_codec_benchmark('aric.exe -c %s %s','aric.exe -d %s %s',in_file,'tmp_encode','tmp_decode');
	[fse_ratio(i), fse_enc_speed(i), fse_dec_speed(i) ] = entropy_codec_benchmark('fse.exe %s -o %s','fse.exe -d %s -o %s',in_file,'tmp_encode','tmp_decode');
end

plot(in_size,aric_enc_speed,':pb',in_size,fse_enc_speed,':*r');
xlabel('bytes'),ylabel('bytes/s'),title('Compression Speed');
legend('aric','FSE',4);

plot(in_size,aric_dec_speed,':pb',in_size,fse_dec_speed,':*r');
xlabel('bytes'),ylabel('bytes/s'),title('Decompression Speed');
legend('aric','FSE',4);

plot(in_size,aric_ratio,':pb',in_size,fse_ratio,':*r');
xlabel('bytes'),ylabel('ratio'),title('Ratio');
legend('aric','FSE',4);



% 下一步测试超大文件的压缩性能

% 用probaGenerator生成测试序列
A = [20,50,70,90,95];
len = length(A);
for i=1:len
	cmdstr=sprintf('probaGenerator.exe %2d%%',A(i));
	dos(cmdstr);
	rename =sprintf(',proba%2d.bin',A(i))
	eval(['!rename', ',proba.bin' rename]);
end

% 生成1到100的数据
 
% dos('ren 2.txt 22.txt') 或 eval(['!rename', ',Culture1_zlib.txt' ',3.txt'])

for i=1:99
	cmdstr=sprintf('probaGenerator.exe %02d%%',i);
	
	dos(cmdstr);
	if(exist('proba.bin','file')==0) %系统找不到指定的文件。
		error('proba.bin not found!');
		break;
	end

	out_file =sprintf('proba%02d.bin',i);
	if(exist(out_file,'file')==2)% 存在一个重名文件，或是找不到文件。
		disp(['rewrite file! -- ',out_file]);
		delete(out_file);
	end
	cmdstr=sprintf('ren proba.bin %s',out_file);
	dos(cmdstr);
end

%进行评测对比
for i=1:99
	in_file=num2str(i,'proba%02d.bin');
	temp = dir(in_file);
	in_size(i) = temp.bytes;
	[aric_ratio(i), aric_enc_speed(i), aric_dec_speed(i) ] = entropy_codec_benchmark('aric.exe -c %s %s','aric.exe -d %s %s',in_file,'tmp_encode','tmp_decode');
	[fse_ratio(i), fse_enc_speed(i), fse_dec_speed(i) ] = entropy_codec_benchmark('fse.exe %s -o %s','fse.exe -d %s -o %s',in_file,'tmp_encode','tmp_decode');
end

A=1:1:99;
plot(A,aric_enc_speed,':pb',A,fse_enc_speed,':*r');
xlabel('proba'),ylabel('bytes/s'),title('Compression Speed');
legend('aric','FSE',4);

plot(A,aric_dec_speed,':pb',A,fse_dec_speed,':*r');
xlabel('proba'),ylabel('bytes/s'),title('Decompression Speed');
legend('aric','FSE',4);

plot(A,aric_ratio,':pb',A,fse_ratio,':*r');
xlabel('proba'),ylabel('ratio'),title('Ratio');
legend('aric','FSE',4);



%计算源文件的熵


A=1:1:10;
plot(A,aric_ratio,':pb',A,fse_ratio,':*r');
xlabel('proba'),ylabel('ratio'),title('Ratio');
legend('aric','FSE',4);


i_start = 30;
i_end = 32;
A=i_start:1:i_end;
plot(A,aric_ratio(i_start:i_end),':pb',A,fse_ratio(i_start:i_end),':*r');
xlabel('proba'),ylabel('ratio'),title('Ratio');
legend('aric','FSE',4);


A=1:1:99;
dif = aric_ratio - fse_ratio;
plot(A,dif,':pb',A,zero(99),'-');
xlabel('proba'),ylabel('ratio'),title('Diff');


实验记录

图像格式的评测方法
格式图片进行比对，整体画质，局部放大是否丢失细节，文件大小 http://www.cnbeta.com/articles/124005.htm

熵编码的改进作用不明显，由于所占比重不大，加速比不大。只能从文件大小的差异和编解码速度的差异上比较。
画质和大小的偏向可调整的测试，找出综合最优方案——一张图片从左到右逐渐提升画质，增大大小。

https://github.com/thorfdbg/libjpeg

make： autoheader未找到的解决 安装autoconf sudo apt-get install autoconf  参见Ubuntu下使用automake自动生成makefilehttp://blog.sina.com.cn/s/blog_6284843d0100givb.html


1. PPM介绍
PPM（Portable PixMap）是portable像素图片，是有netpbm项目定义的一系列的portable图片格式中的一个。这些图片格式都相对比较容易处理，跟平台无关，所以称之为portable，简单理解，就是比较直接的图片格式，比如PPM，其实就是把每一个点的RGB分别保存起来。所以，PPM格式的文件是没有压缩的，相对比较大，但是由于图片格式简单，一般作为图片处理的中间文件（不会丢失文件信息），或者作为简单的图片格式保存。

PPM图像格式(Portable Pixelmap)

　　一种linux图片格式，可用ACDSEE批量转变。它是一种简单的图像格式，仅包含格式、图像宽高、bit数等信息和图像数据。
　　图像数据的保存格式可以用ASCII码，也可用二进制，下面列举ppm格式中比较简单的一种：24位彩色、二进制保存的图像。
　　文件头+rgb数据:
　　P6\n
　　width height\n
　　255\n
　　rgbrgb...
　　其中P6表示ppm的这种格式；\n表示换行符；width和height表示图像的宽高，用空格隔开；255表示每个颜色分量的最大值；rgb数据从上到下，从左到右排放。



程序可以实现jpeg和ppm文件的相互转化。要进行评测，还需要在windows下编译成功，否则需要在linux下运行matlab。


linux libjpeg 安装 http://www.cnblogs.com/michile/archive/2013/02/19/2917027.html

codeblock下添加所有文件，编译选项添加查找文件目录即可 0error 2warning


评测过程写入wiki




%tmp
%转为pdf

%由于最后的getchar造成等待，删除之
dos命令是阻塞的

如果可以应用，则有意义作深入研究

附件为确认文件,请查收 
Please refer to the attachment for verification purposes.
Please refer to the attachment for confirmation purposes.

这里有两种说法 稍微有一点不同，verification 一般是让你过目 如果内容没有问题的话就不用回答了这样的
confirmation大部分是要求有回复的，要求对方confirm，也就是确认。

看自己的情况选一个吧~

修改：当然楼上的兄弟说的我忘了加了 那就是approved，已经确认 这个和verification又有点不同了，approved就是说这个email的作用是告诉你已经通过了。Verification还是需要确认的。




新图像格式BPG被提出 用以取代JPEG格式 http://news.mydrivers.com/1/351/351777.htm

image coding

libjpeg is a widely used free library with functions for handling the image data format JPEG. It implements a JPEG codec (encoding and decoding) alongside various utilities for handling JPEG data.



JPEG家族
jpeg维基下的标准


github.com_thorfdbg_libjpeg_master_27_commits


QM-Coder
http://www.freepatentsonline.com/6091854.html
QM arithmetic encoder is a high efficiency encoder designed for the simplicity and speed.
QM算术编码器是一种简单和快速的高效算术编码器,它使用定点算法,限制输入符号是单一的,用近似来代替乘法。


CABAC -Context Adaptive QM Coding
https://github.com/jigar23/CABAC
前文参考之适应性二元算术编码(Context-based Adaptive Binary Arithmetic Coding,简称CABAC)是视讯压缩标准H.264采用的算法。



3月1日
1月30日



买车票
北京

天水到北京火车 Z76 Z152 2-28
始发站


工作1，先测试编译通过整个原始工程

从main入手




核心代码

int main(){
  // ...
  if (quality < 0 && lossless == false && lsmode < 0) {
    Reconstruct(argv[1],argv[2],forceint,forcefix,colortrafo,pfm);
  } else {
    Encode(argv[1],argv[2],quality,hdrquality,maxerror,colortrafo,lossless,progressive,
	   reversible,residuals,optimize,accoding,dconly,levels,pyramidal,writednl,restart,
	   gamma,lsmode,hadamard,noiseshaping,hiddenbits,sub);
  }	

  return 0;
}

主线（忽略错误分支）


存在虚函数，无法阅读

看目录结构



j2k_codec 目前所能找到的速度最快的Jpeg2000压缩和解压缩软件原码，解压缩速度是著名的jasper的58倍-can now find the fastest Jpeg2000 compression software source, extract the speed of the famous 58 times jasper



项目的简介长一些比较容易被检索和关注







查看编码算法的调用情况，进行分析，


代码结构可视化分析

UML类图 定位熵编码相关的类

接口框架的分析





通过dynamic_cast看接口调用 