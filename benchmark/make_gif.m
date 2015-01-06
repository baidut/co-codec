
% 生成动态图片，从质量差到质量好
% usage
% make_gif('cat.gif','data/newcat%03d',[1:100],0.3,'quality:%03d%%')

function make_gif(out_file,img_name_format,indexs,time,title_format)
%TOOD 动态图旁边加计数 0-100
% 带边框版本，可以添加标题等元素
% clc;clear all;close all;
% out_file 'name.gif'
% indexs 1:100

for i=indexs
	dec_file = num2str(i,'data/newcat%03d.ppm'); 
    Img=imread(dec_file);
    figure(i)
    imshow(Img);
    title(num2str(i,title_format));
    frame=getframe(i);
    im=frame2im(frame);%制作gif文件，图像必须是index索引图像
    [I,map]=rgb2ind(im,256);
    k=i-0;
    if k==1;
        imwrite(I,map,out_file,'gif','Loopcount',inf,...
            'DelayTime',time);%loopcount只是在i==1的时候才有用
    else
        imwrite(I,map,out_file,'gif','WriteMode','append',...
            'DelayTime',time); %layTime用于设置gif文件的播放快慢
    end
end
close all;

%{
	去边框版本
clc;clear all;close all;
filename= 'cat.gif';
width=720;
for i=20:50
    dec_file = num2str(i,'cat%03d.ppm'); 
    Img=imread(dec_file);
    %% Img=imresize(Img,[128,128]);
    figure(i)
    imshow(Img);
    set(gcf,'color','w');
    set(gca,'units','pixels','Visible','off');
    q=get(gca,'position');
    q(1)=0;%设置左边距离值为零
    q(2)=0;%设置右边距离值为零
    set(gca,'position',q);
    frame=getframe(gcf,[1,1,width,width]);%% frame=getframe(gcf,[1,1,128,128]);%
    im=frame2im(frame);%制作gif文件，图像必须是index索引图像
    imshow(im);
    [I,map]=rgb2ind(im,256);
    k=i-0;
    if k==1;
        imwrite(I,map,filename,'gif','Loopcount',inf,...
            'DelayTime',0.1);%loopcount只是在i==1的时候才有用
    else
        imwrite(I,map,filename,'gif','WriteMode','append',...
            'DelayTime',0.1);%DelayTime用于设置gif文件的播放快慢
    end
end
close all;

%}