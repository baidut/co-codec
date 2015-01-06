function JPEGEncodeDecode
%UNTITLED7 Summary of this function goes here
%   Detailed explanation goes here
 
img=imread('bridge.bmp');
subplot(121);imshow(img);title('原图');          %显示原图
 
 
img_ycbcr = rgb2ycbcr(img);             % rgb->yuv
[row,col,~]=size(img_ycbcr);       % 取出行列数，~表示3个通道算1列
 
 
%对图像进行扩展
row_expand=ceil(row/16)*16;        %行数上取整再乘16，及扩展成16的倍数
if mod(row,16)~=0            %行数不是16的倍数，用最后一行进行扩展
    for i=row:row_expand
        img_ycbcr(i,:,:)=img_ycbcr(row,:,:);
    end
end
col_expand=ceil(col/16)*16;  %列数上取整
if mod(col,16)~=0         %列数不是16的倍数，用最后一列进行扩展
    for j=col:col_expand
        img_ycbcr(:,j,:)=img_ycbcr(:,col,:);
    end
end
 
 
%对Y,Cb,Cr分量进行4:2:0采样
Y=img_ycbcr(:,:,1);                    %Y分量
Cb=zeros(row_expand/2,col_expand/2);        %Cb分量
Cr=zeros(row_expand/2,col_expand/2);        %Cr分量
for i=1:row_expand/2
    for j=1:2:col_expand/2-1          %奇数列
        Cb(i,j)=double(img_ycbcr(i*2-1,j*2-1,2));     
        Cr(i,j)=double(img_ycbcr(i*2-1,j*2+1,3));     
    end
end
for i=1:row_expand/2
    for j=2:2:col_expand/2            %偶数列
        Cb(i,j)=double(img_ycbcr(i*2-1,j*2-2,2));     
        Cr(i,j)=double(img_ycbcr(i*2-1,j*2,3));     
    end
end
 
 
 
%分别对三种颜色分量进行编码
Y_Table=[16  11  10  16  24  40  51  61
    12  12  14  19  26  58  60  55
    14  13  16  24  40  57  69  56
    14  17  22  29  51  87  80  62
    18  22  37  56  68 109 103  77
    24  35  55  64  81 104 113  92
    49  64  78  87 103 121 120 101
    72  92  95  98 112 100 103  99];%亮度量化表
CbCr_Table=[17, 18, 24, 47, 99, 99, 99, 99;
    18, 21, 26, 66, 99, 99, 99, 99;
    24, 26, 56, 99, 99, 99, 99, 99;
    47, 66, 99 ,99, 99, 99, 99, 99;
    99, 99, 99, 99, 99, 99, 99, 99;
    99, 99, 99, 99, 99, 99, 99, 99;
    99, 99, 99, 99, 99, 99, 99, 99;
    99, 99, 99, 99, 99, 99, 99, 99];%色差量化表
 
Qua_Factor=0.5;%量化因子,最小为0.01,最大为255,建议在0.5和3之间,越小质量越好文件越大
 
%对三个通道分别DCT和量化
Y_dct_q=Dct_Quantize(Y,Qua_Factor,Y_Table);
Cb_dct_q=Dct_Quantize(Cb,Qua_Factor,CbCr_Table);
Cr_dct_q=Dct_Quantize(Cr,Qua_Factor,CbCr_Table);
 
 
%对三个通道分别反量化和反DCT
Y_in_q_dct=Inverse_Quantize_Dct(Y_dct_q,Qua_Factor,Y_Table);
Cb_in_q_dct=Inverse_Quantize_Dct(Cb_dct_q,Qua_Factor,CbCr_Table);
Cr_in_q_dct=Inverse_Quantize_Dct(Cr_dct_q,Qua_Factor,CbCr_Table);
 
%恢复出YCBCR图像
YCbCr_in(:,:,1)=Y_in_q_dct;
for i=1:row_expand/2
    for j=1:col_expand/2
        YCbCr_in(2*i-1,2*j-1,2)=Cb_in_q_dct(i,j);
        YCbCr_in(2*i-1,2*j,2)=Cb_in_q_dct(i,j);
        YCbCr_in(2*i,2*j-1,2)=Cb_in_q_dct(i,j);
        YCbCr_in(2*i,2*j,2)=Cb_in_q_dct(i,j);
       
        YCbCr_in(2*i-1,2*j-1,3)=Cr_in_q_dct(i,j);
        YCbCr_in(2*i-1,2*j,3)=Cr_in_q_dct(i,j);
        YCbCr_in(2*i,2*j-1,3)=Cr_in_q_dct(i,j);
        YCbCr_in(2*i,2*j,3)=Cr_in_q_dct(i,j);
    end
end
 
I_in=ycbcr2rgb(YCbCr_in);
I_in(row+1:row_expand,:,:)=[];%去掉扩展的行
I_in(:,col+1:col_expand,:)=[];%去掉扩展的列
 
subplot(122);imshow(I_in);title('重构后的图片');
end
 
 
%% DCT和量化函数：
function [Matrix]=Dct_Quantize(I,Qua_Factor,Qua_Table)
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
I=double(I)-128;   %层次移动128个灰度级，详见书本P401
 
%% ?t2变换：把ImageSub分成8*8像素块，分别进行dct2变换，得变换系数矩阵Coef
I=blkproc(I,[8 8],'dct2(x)');
 
Qua_Matrix=Qua_Factor.*Qua_Table;              %量化矩阵
I=blkproc(I,[8 8],'round(x./P1)',Qua_Matrix);  %量化，四舍五入
 
Matrix=I;          %得到量化后的矩阵
end
 
%% 反量化和反DCT函数：
function [ Matrix ] = Inverse_Quantize_Dct( I,Qua_Factor,Qua_Table )
%UNTITLED3 Summary of this function goes here
%   Detailed explanation goes here
 
Qua_Matrix=Qua_Factor.*Qua_Table;     %反量化矩阵
I=blkproc(I,[8 8],'x.*P1',Qua_Matrix);%反量化，四舍五入
 
[row,column]=size(I);
 
I=blkproc(I,[8 8],'idct2(x)');   %T反变换
 
I=uint8(I+128);
for i=1:row
    for j=1:column
        if I(i,j)>255
            I(i,j)=255;
        elseif I(i,j)<0
            I(i,j)=0;
        end
    end
end
 
Matrix=I;       %反量化和反Dct后的矩阵
end