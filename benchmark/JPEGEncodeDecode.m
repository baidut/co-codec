function JPEGEncodeDecode
%UNTITLED7 Summary of this function goes here
%   Detailed explanation goes here

%% ��ʾԭͼ
img=imread('img07.bmp');
subplot(121);imshow(img);title('ԭͼ');          
%% ɫ�ʿռ�ת��������
img_ycbcr = rgb2ycbcr(img);
subplot(122);imshow(img_ycbcr);title('ɫ�ʿռ�ת�����ͼƬ');
%% ��ͼ�������չ
[row,col,~]=size(img_ycbcr); % ȡ����������~��ʾ3��ͨ����1��
%��������16�ı����������һ�н�����չ
row_expand=ceil(row/16)*16;  %������ȡ���ٳ�16������չ��16�ı���
if mod(row,16)~=0            
    for i=row:row_expand
        img_ycbcr(i,:,:)=img_ycbcr(row,:,:);
    end
end
%��������16�ı����������һ�н�����չ
col_expand=ceil(col/16)*16;  %������ȡ��
if mod(col,16)~=0            
    for j=col:col_expand
        img_ycbcr(:,j,:)=img_ycbcr(:,col,:);
    end
end
% ��չ��ʽ�����޸�
subplot(122);imshow(img_ycbcr);%title('��չ���ͼƬ');
%% ��Y,Cb,Cr��������4:2:0����
Y=img_ycbcr(:,:,1);                    %Y����
Cb=zeros(row_expand/2,col_expand/2);   %Cb����
Cr=zeros(row_expand/2,col_expand/2);   %Cr����
for i=1:row_expand/2
    for j=1:2:col_expand/2-1          %������
        Cb(i,j)=double(img_ycbcr(i*2-1,j*2-1,2));     
        Cr(i,j)=double(img_ycbcr(i*2-1,j*2+1,3));     
    end
end
for i=1:row_expand/2
    for j=2:2:col_expand/2            %ż����
        Cb(i,j)=double(img_ycbcr(i*2-1,j*2-2,2));     
        Cr(i,j)=double(img_ycbcr(i*2-1,j*2,3));     
    end
end
%�ֱ��������ɫ�������б���
Y_Table=[16  11  10  16  24  40  51  61
    12  12  14  19  26  58  60  55
    14  13  16  24  40  57  69  56
    14  17  22  29  51  87  80  62
    18  22  37  56  68 109 103  77
    24  35  55  64  81 104 113  92
    49  64  78  87 103 121 120 101
    72  92  95  98 112 100 103  99];%����������
CbCr_Table=[17, 18, 24, 47, 99, 99, 99, 99;
    18, 21, 26, 66, 99, 99, 99, 99;
    24, 26, 56, 99, 99, 99, 99, 99;
    47, 66, 99 ,99, 99, 99, 99, 99;
    99, 99, 99, 99, 99, 99, 99, 99;
    99, 99, 99, 99, 99, 99, 99, 99;
    99, 99, 99, 99, 99, 99, 99, 99;
    99, 99, 99, 99, 99, 99, 99, 99];%ɫ��������

Qua_Factor=0.5;%��������,��СΪ0.01,���Ϊ255,������0.5��3֮��,ԽС����Խ���ļ�Խ��
 
%������ͨ���ֱ�DCT������
Y_dct_q=Dct_Quantize(Y,Qua_Factor,Y_Table);
Cb_dct_q=Dct_Quantize(Cb,Qua_Factor,CbCr_Table);
Cr_dct_q=Dct_Quantize(Cr,Qua_Factor,CbCr_Table);
%% ����
%������ͨ���ֱ������ͷ�DCT
Y_in_q_dct=Inverse_Quantize_Dct(Y_dct_q,Qua_Factor,Y_Table);
Cb_in_q_dct=Inverse_Quantize_Dct(Cb_dct_q,Qua_Factor,CbCr_Table);
Cr_in_q_dct=Inverse_Quantize_Dct(Cr_dct_q,Qua_Factor,CbCr_Table);
 
%�ָ���YCBCRͼ��
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
I_in(row+1:row_expand,:,:)=[];%ȥ����չ����
I_in(:,col+1:col_expand,:)=[];%ȥ����չ����
 
subplot(122);imshow(I_in);title('�ع����ͼƬ');
end
 
 
%% DCT������������
function [Matrix]=Dct_Quantize(I,Qua_Factor,Qua_Table)
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
I=double(I)-128;   %����ƶ�128���Ҷȼ�������鱾P401
 
%t2�任����ImageSub�ֳ�8*8���ؿ飬�ֱ����dct2�任���ñ任ϵ������Coef
I=blkproc(I,[8 8],'dct2(x)');
 
Qua_Matrix=Qua_Factor.*Qua_Table;              %��������
I=blkproc(I,[8 8],'round(x./P1)',Qua_Matrix);  %��������������
 
Matrix=I;          %�õ�������ľ���
end
 
%% �������ͷ�DCT������
function [ Matrix ] = Inverse_Quantize_Dct( I,Qua_Factor,Qua_Table )
%UNTITLED3 Summary of this function goes here
%   Detailed explanation goes here
 
Qua_Matrix=Qua_Factor.*Qua_Table;     %����������
I=blkproc(I,[8 8],'x.*P1',Qua_Matrix);%����������������
 
[row,column]=size(I);
 
I=blkproc(I,[8 8],'idct2(x)');   %T���任
 
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
 
Matrix=I;       %�������ͷ�Dct��ľ���
end