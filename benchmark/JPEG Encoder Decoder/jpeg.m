function jpeg
% This is a JPEG encoding & decoding program of still image.
% it does not use level shifting.
% Discrete Cosine transform (DCT) is performed both by classical & Chen's
% Flowgraph methods. Predefined JPEG quantization array & Zigzag order are
% used here. 'RUN', 'LEVEL' coding is used instead of  Huffman coding.
% Compression ratio is compared for each DCT method. Effect of coarse and fine quantization is
% also examined. The execution time of 2 DCT methods is also checked.
% In addition, most energatic DCT coefficients are also applied to examine
% the effect in MatLab 7.4.0 R2007a. Input is 9 gray scale pictures &
% output is 9*9=81 pictures to compare. Blocking effect is obvious.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    %-------------------------- Initialization -----------------------------
    % JPEG default quantization array
    Q_8x8 =uint8([
            16 11 10 16  24  40  51  61
            12 12 14 19  26  58  60  55
            14 13 16 24  40  57  69  56
            14 17 22 29  51  87  80  62
            18 22 37 56  68 109 103  77
            24 35 55 64  81 104 113  92
            49 64 78 87 103 121 120 101
            72 92 95 98 112 100 103 99]);

    % I am interested in the energy of the dct coefficients, I will use 
    % this matrix (found from long observation) to select dct coefficients.
    % lowest number -> highest priority
    dct_coefficient_priority_8x8 =[
           1   2   6   7  15  16  28  29;
           3   5   8  14  17  27  30  43;
           4   9  13  18  26  31  42  44;
          10  12  19  25  32  41  45  54;
          11  20  24  33  40  46  53  55;
          21  23  34  39  47  52  56  61;
          22  35  38  48  51  57  60  62;
          36  37  49  50  58  59  63  64];
    % if we decide to take 10 coefficients with the most energy, we will assign 
    % 99 to ignore the other coefficients and remain with a matrix of 8x8

    %This suitable Zigzag order is formed from the  JPEG standard
    ZigZag_Order = uint8([
            1  9  2  3  10 17 25 18
            11 4  5  12 19 26 33 41
            34 27 20 13 6  7  14 21 
            28 35 42 49 57 50 43 36 
            29 22 15 8  16 23 30 37
            44 51 58 59 52 45 38 31 
            24 32 39 46 53 60 61 54 
            47 40 48 55 62 63 56 64]);

    % Finding the reverse zigzag order (8x8 matrix)
    reverse_zigzag_order_8x8 = zeros(8,8);
    for k = 1:(size(ZigZag_Order,1) *size(ZigZag_Order,2)) 
        reverse_zigzag_order_8x8(k) = find(ZigZag_Order== k); 
    end;
                    
    Compressed_image_size=0;
    %---------------------------------------------------------------------

    
close all;

for Image_Index = 8:8    % the whole program will be tested for 9 images (0->8)
   figure;          %keep the input-output for each image seperately

   
    %--------------------------load a picture ----------------------------
    switch Image_Index
    case {0,1}, input_image_128x128 = im2double( imread( sprintf( '%d.tif',Image_Index ),'tiff' ) );
    otherwise, input_image_128x128 = im2double( imread( sprintf( '%d.tif',Image_Index),'jpeg' ) );
    end   
    %-------------------------- ------------------------------------------

    
    %---------------- show the input image -------------------------------
    subplot(3,3,1);
    imshow(input_image_128x128);
    title( sprintf('original image #%d',Image_Index) );
    %---------------------------------------------------------------------

    for Quantization_Quality = 0:1 % 0 -> coarse quantization, 1 -> fine quantization

    for DCT_type = 0:1    % 0 -> classic DCT, 1 -> Flowgraph fast DCT

    for chosen_number_of_dct_coefficient = 1:63:64    % select 1 or 64 dct coefficients

        
    %---------------- choose energetic DCT coefficients ------------------
    % I will use this matrix to choose only the wanted number of dct coefficients
    % the matrix is initialized to zeros -> zero coefficient is chosen at the beginning
    coef_selection_matrix_8x8 = zeros(8,8);

    % this loop will choose 1 dct coefficients each time
    for l=1:chosen_number_of_dct_coefficient 
        % find the most energetic coefficient from the mean_matrix
        [y,x] = find(dct_coefficient_priority_8x8==min(min(dct_coefficient_priority_8x8)));
    
        % select specific coefficients by location index y,x for the image to be compressed
        coef_selection_matrix_8x8(y,x) = 1;
    
        % set it as 99 for the chosen dct coefficient, so that in the next loop, we will choose the "next-most-energetic" coefficient
        dct_coefficient_priority_8x8(y,x) = 99;
    end
    
    % replicate the selection matrix for all the parts of the dct transform
    selection_matrix_128x128 = repmat( coef_selection_matrix_8x8,16,16 );
    %---------------------------------------------------------------------
    
    
    tic ;    % start mark for elapsed time for encoding & decoding
    
    
    %------------------------- Forward DCT -------------------------------        
    % for each picture perform a 2 dimensional dct on 8x8 blocks.    
    if DCT_type==0
        dct_transformed_image = Classic_DCT(input_image_128x128) .* selection_matrix_128x128;
    else
        dct_transformed_image = image_8x8_block_flowgraph_forward_dct(input_image_128x128) .* selection_matrix_128x128;
    end
    %---------------------------------------------------------------------

    
    %---------------- show the DCT of image ------------------------------
% one can use this portion to show DCT coefficients of the image
%    subplot(2,2,2);
%    imshow(dct_transformed_image);
%    title( sprintf('8x8 DCT of image #%d',Image_Index) );
    %---------------------------------------------------------------------
    
    
    %normalize dct_transformed_image by the maximum coefficient value in dct_transformed_image 
    Maximum_Value_of_dct_coeffieient = max(max(dct_transformed_image));
    dct_transformed_image = dct_transformed_image./Maximum_Value_of_dct_coeffieient;
    
    %integer conversion of dct_transformed_image 
    dct_transformed_image_int = im2uint8( dct_transformed_image ); 
    
    
    %-------------------- Quantization -----------------------------------
    % replicate the 'Q_8x8' for at a time whole (128x128) image quantization
    if Quantization_Quality==0
        quantization_matrix_128x128 = repmat(Q_8x8,16,16 ); %for coarse quantization
    else
        quantization_matrix_128x128 = repmat(uint8(ceil(double(Q_8x8)./40)),16,16 );  %for fine quantization
    end
    
    %at a time whole image (128x128) quantization
    quantized_image_128x128 =  round(dct_transformed_image_int ./quantization_matrix_128x128) ; %round operation should be done here for lossy quantization
    %---------------------------------------------------------------------
    

    % Break 8x8 block into columns
    Single_column_quantized_image=im2col(quantized_image_128x128, [8 8],'distinct');

    
    %--------------------------- zigzag ----------------------------------
    % using the MatLab Matrix indexing power (specially the ':' operator) rather than any function
    ZigZaged_Single_Column_Image=Single_column_quantized_image(ZigZag_Order,:);    
    %---------------------------------------------------------------------


    %---------------------- Run Level Coding -----------------------------
    % construct Run Level Pair from ZigZaged_Single_Column_Image
    run_level_pairs=uint8([]);
    for block_index=1:256    %block by block - total 256 blocks (8x8) in the 128x128 image
        single_block_image_vector_64(1:64)=0;
        for Temp_Vector_Index=1:64
            single_block_image_vector_64(Temp_Vector_Index) = ZigZaged_Single_Column_Image(Temp_Vector_Index, block_index);  %select 1 block sequentially from the ZigZaged_Single_Column_Image
        end
        non_zero_value_index_array = find(single_block_image_vector_64~=0); % index array of next non-zero entry in a block
        number_of_non_zero_entries = length(non_zero_value_index_array);  % # of non-zero entries in a block

    % Case 1: if first ac coefficient has no leading zeros then encode first coefficient
        if non_zero_value_index_array(1)==1,  
           run=0;   % no leading zero
            run_level_pairs=cat(1,run_level_pairs, run, single_block_image_vector_64(non_zero_value_index_array(1)));
        end

    % Case 2: loop through each non-zero entry    
        for n=2:number_of_non_zero_entries, 
            % check # of leading zeros (run)
            run=non_zero_value_index_array(n)-non_zero_value_index_array(n-1)-1;
            run_level_pairs=cat(1, run_level_pairs, run, single_block_image_vector_64(non_zero_value_index_array(n)));
        end
        
    % Case 3: "End of Block" mark insertion
        run_level_pairs=cat(1, run_level_pairs, 255, 255);
    end
    %---------------------------------------------------------------------
    
    
    Compressed_image_size=size(run_level_pairs);        % file size after compression
    Compression_Ratio = 20480/Compressed_image_size(1,1);



% % %  -------------------------------------------------------------------
% % %  -------------------------------------------------------------------
% % %                DECODING
% % %  -------------------------------------------------------------------
% % %  -------------------------------------------------------------------

    

    %---------------------- Run Level Decoding ---------------------------
    % construct  ZigZaged_Single_Column_Image from Run Level Pair 
    c=[];
    for n=1:2:size(run_level_pairs), % loop through run_level_pairs
        % Case 1 & Cae 2 
        % concatenate zeros according to 'run' value
        if run_level_pairs(n)<255 % only end of block should have 255 value
            zero_count=0;
            zero_count=run_level_pairs(n);
            for l=1:zero_count    % concatenation of zeros accouring to zero_count
                c=cat(1,c,0);   % single zero concatenation
            end
            c=cat(1,c,run_level_pairs(n+1)); % concatenate single'level' i.e., a non zero value
       
        % Case 3: End of Block decoding
        else
            number_of_trailing_zeros= 64-mod(size(c),64);
            for l= 1:number_of_trailing_zeros    % concatenate as much zeros as needed to fill a block
                c=cat(1,c,0); 
            end
        end
    end
    %---------------------------------------------------------------------
    

    %---------------------------------------------------------------------
    %    prepare the ZigZaged_Single_Column_Image vector (each column represents 1 block) from the
    %    intermediate concatenated vector "c"
    for i=1:256
        for j=1:64
            ZigZaged_Single_Column_Image(j,i)=c(64*(i-1)+j);
        end
    end
    %---------------------------------------------------------------------
     
    
    %--------------------------- reverse zigzag --------------------------
    %reverse zigzag procedure using the matrix indexing capability of MatLab (specially the ':' operator)
    Single_column_quantized_image = ZigZaged_Single_Column_Image(reverse_zigzag_order_8x8,:);
    %---------------------------------------------------------------------
    

   %image matrix construction from image column
    quantized_image_128x128 = col2im(Single_column_quantized_image,   [8 8],   [128 128],   'distinct');

    
    %-------------------- deQuantization ---------------------------------
    dct_transformed_image =  quantized_image_128x128.*quantization_matrix_128x128 ;
    %---------------------------------------------------------------------

    
    %-------------------------- Inverse DCT ------------------------------
    % restore the compressed image from the given set of coeficients
    if DCT_type==0
        restored_image = image_8x8_block_inv_dct( im2double(dct_transformed_image).*Maximum_Value_of_dct_coeffieient  ); %Maximum_Value_of_dct_coeffieient is used for reverse nornalization
    else
        restored_image = image_8x8_block_flowgraph_inverse_dct( im2double(dct_transformed_image).*Maximum_Value_of_dct_coeffieient  ); %Maximum_Value_of_dct_coeffieient is used for reverse nornalization
    end
    %---------------------------------------------------------------------

    
    elapsed_time = toc;    % time required for both enconing & decoding

    
    %-------------------------- Show restored image ----------------------
    subplot(3,3, Quantization_Quality*2^2+ DCT_type*2+ floor(chosen_number_of_dct_coefficient/64)+2);
    imshow( restored_image );
    
    if DCT_type == 0 
        if Quantization_Quality == 0
            title( sprintf('coarse quantize\nClassic DCT\nRestored image with %d coeffs\nCompression ratio %.2f\nTime %f',chosen_number_of_dct_coefficient,Compression_Ratio,elapsed_time) );
        else
            title( sprintf('fine quantize\nclassic DCT\nRestored image with %d coeffs\nCompression ratio %.2f\nTime %f',chosen_number_of_dct_coefficient,Compression_Ratio,elapsed_time) );
        end
    else
        if Quantization_Quality == 0
            title( sprintf('coarse quantize\nFast DCT\nRestored image with %d coeffs\nCompression ratio %.2f\nTime %f',chosen_number_of_dct_coefficient,Compression_Ratio,elapsed_time) );
        else
            title( sprintf('fine quantize\nFast DCT\nRestored image with %d coeffs\nCompression ratio %.2f\nTime %f',chosen_number_of_dct_coefficient,Compression_Ratio,elapsed_time) );
        end        
    end
    %---------------------------------------------------------------------
    
    
    end    % end of coefficient number loop
    end    % end of DCT type loop
    end    % end of quantization qualoty loop
end    % end of image index loop

end    % end of 'jpeg' function
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%         I N N E R   F U N C T I O N   I M P L E M E N T A T I O N
%% -----------------------------------------------------------------------
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% ------------------------------------------------------------------------
% Classic_DCT_Block_8x8 - implementation of a 2 Dimensional DCT
% assumption: input matrix is a square matrix !
% ------------------------------------------------------------------------
function out = Classic_DCT_Block_8x8( in )


% get input matrix size
N = size(in,1);

% build the matrix
n = 0:N-1;
for k = 0:N-1
   if (k>0)
      C(k+1,n+1) = cos(pi*(2*n+1)*k/2/N)/sqrt(N)*sqrt(2);
   else
      C(k+1,n+1) = cos(pi*(2*n+1)*k/2/N)/sqrt(N);
   end   
end
out = C*in*(C');
end    % end of Classic_DCT_Block_8x8 function


% ------------------------------------------------------------------------
% pdip_inv_dct2 - implementation of an inverse 2 Dimensional DCT
% assumption: input matrix is a square matrix !
% ------------------------------------------------------------------------
function out = pdip_inv_dct2( in )

% get input matrix size
N = size(in,1);

% build the matrix
n = 0:N-1;
for k = 0:N-1
   if (k>0)
      C(k+1,n+1) = cos(pi*(2*n+1)*k/2/N)/sqrt(N)*sqrt(2);
   else
      C(k+1,n+1) = cos(pi*(2*n+1)*k/2/N)/sqrt(N);
   end   
end
out = (C')*in*C;
end


% ------------------------------------------------------------------------
% Classic_DCT - perform a block DCT for an image
% ------------------------------------------------------------------------
function transform_image = Classic_DCT( input_image )

transform_image = zeros( size( input_image,1 ),size( input_image,2 ) );
for m = 0:15
    for n = 0:15
        transform_image( m*8+[1:8],n*8+[1:8] ) = Classic_DCT_Block_8x8( input_image( m*8+[1:8],n*8+[1:8] ) );
    end
end
end


% ------------------------------------------------------------------------
% image_8x8_block_flowgraph_forward_dct - perform a block Flowgraph forward DCT for an image
% ------------------------------------------------------------------------
function transform_image = image_8x8_block_flowgraph_forward_dct( input_image )

transform_image = zeros( size( input_image,1 ),size( input_image,2 ) );
for m = 0:15
    for n = 0:15
        transform_image( m*8+[1:8],n*8+[1:8] ) = flowgraph_forward_dct( input_image( m*8+[1:8],n*8+[1:8] ) );
    end
end
end


% ------------------------------------------------------------------------
% image_8x8_block_inv_dct - perform a block inverse DCT for an image
% ------------------------------------------------------------------------
function restored_image = image_8x8_block_inv_dct( transform_image )

restored_image = zeros( size( transform_image,1 ),size( transform_image,2 ) );
for m = 0:15
    for n = 0:15
        restored_image( m*8+[1:8],n*8+[1:8] ) = pdip_inv_dct2( transform_image( m*8+[1:8],n*8+[1:8] ) );
    end
end
end


% ------------------------------------------------------------------------
% image_8x8_block_flowgraph_inverse_dct - perform a block Flowgraph inverse DCT for an image
% ------------------------------------------------------------------------
function restored_image = image_8x8_block_flowgraph_inverse_dct( transform_image )

restored_image = zeros( size( transform_image,1 ),size( transform_image,2 ) );
for m = 0:15
    for n = 0:15
        restored_image( m*8+[1:8],n*8+[1:8] ) = flowgraph_inverse_dct( transform_image( m*8+[1:8],n*8+[1:8] ) );
    end
end
end


% ------------------------------------------------------------------------
% FLOWGRAPH forward dct (Chen,Fralick and Smith)
% ------------------------------------------------------------------------
function [DCT_8x8] = flowgraph_forward_dct(in_8x8)

% constant cosine values will be used for both forward & inverse flowgraph DCT
c1=0.980785;
c2=0.923880;
c3=0.831470;
c4=0.707107;
c5=0.555570;
c6=0.382683;
c7=0.195090;


%---------------------------row calculation FDCT--------------------------
for row_number=1:8
    
    %sample image value initialization from input matrix
    f0=in_8x8(row_number,1);
    f1=in_8x8(row_number,2);
    f2=in_8x8(row_number,3);
    f3=in_8x8(row_number,4);
    f4=in_8x8(row_number,5);
    f5=in_8x8(row_number,6);
    f6=in_8x8(row_number,7);
    f7=in_8x8(row_number,8);

   %first stage of FLOWGRAPH (Chen,Fralick and Smith)
    i0=f0+f7;
    i1=f1+f6;
    i2=f2+f5;
    i3=f3+f4;
    i4=f3-f4;
    i5=f2-f5;
    i6=f1-f6;
    i7=f0-f7;
    
    %second stage of FLOWGRAPH (Chen,Fralick and Smith)
    j0=i0+i3;
    j1=i1+i2;
    j2=i1-i2;
    j3=i0-i3;
    j4=i4;
    j5=(i6-i5)*c4;
    j6=(i6+i5)*c4;
    j7=i7;
    
    %third stage of FLOWGRAPH (Chen,Fralick and Smith)
    k0=(j0+j1)*c4;
    k1=(j0-j1)*c4;
    k2=(j2*c6)+(j3*c2);
    k3=(j3*c6)-(j2*c2);
    k4=j4+j5;
    k5=j4-j5;
    k6=j7-j6;
    k7=j7+j6;
    
    %fourth stage of FLOWGRAPH; 1-dimensional DCT coefficients
    F0=k0/2;
    F1=(k4*c7+k7*c1)/2;
    F2=k2/2;
    F3=(k6*c3-k5*c5)/2;
    F4=k1/2;
    F5=(k5*c3+k6*c5)/2;
    F6=k3/2;
    F7=(k7*c7-k4*c1)/2;
    
    %DCT coefficient assignment
   One_D_DCT_Row_8x8(row_number,1)=F0;
   One_D_DCT_Row_8x8(row_number,2)=F1;
   One_D_DCT_Row_8x8(row_number,3)=F2;
   One_D_DCT_Row_8x8(row_number,4)=F3;
   One_D_DCT_Row_8x8(row_number,5)=F4;
   One_D_DCT_Row_8x8(row_number,6)=F5;
   One_D_DCT_Row_8x8(row_number,7)=F6;
   One_D_DCT_Row_8x8(row_number,8)=F7;

end    %end of row calculations
%---------------------------end: row calculation FDCT---------------------


%--------------------------- column calculation FDCT----------------------
for column_number=1:8   %start of column calculation
    
    %sample image value initialization
    f0=One_D_DCT_Row_8x8(1,column_number);
    f1=One_D_DCT_Row_8x8(2,column_number);
    f2=One_D_DCT_Row_8x8(3,column_number);
    f3=One_D_DCT_Row_8x8(4,column_number);
    f4=One_D_DCT_Row_8x8(5,column_number);
    f5=One_D_DCT_Row_8x8(6,column_number);
    f6=One_D_DCT_Row_8x8(7,column_number);
    f7=One_D_DCT_Row_8x8(8,column_number);
 
   %first stage of FLOWGRAPH (Chen,Fralick and Smith)
    i0=f0+f7;
    i1=f1+f6;
    i2=f2+f5;
    i3=f3+f4;
    i4=f3-f4;
    i5=f2-f5;
    i6=f1-f6;
    i7=f0-f7;
    
    %second stage of FLOWGRAPH (Chen,Fralick and Smith)
    j0=i0+i3;
    j1=i1+i2;
    j2=i1-i2;
    j3=i0-i3;
    j4=i4;
    j5=(i6-i5)*c4;
    j6=(i6+i5)*c4;
    j7=i7;
    
    %third stage of FLOWGRAPH (Chen,Fralick and Smith)
    k0=(j0+j1)*c4;
    k1=(j0-j1)*c4;
    k2=(j2*c6)+(j3*c2);
    k3=(j3*c6)-(j2*c2);
    k4=j4+j5;
    k5=j4-j5;
    k6=j7-j6;
    k7=j7+j6;
    
    %fourth stage of FLOWGRAPH; Desired DCT coefficients
    F0=k0/2;
    F1=(k4*c7+k7*c1)/2;
    F2=k2/2;
    F3=(k6*c3-k5*c5)/2;
    F4=k1/2;
    F5=(k5*c3+k6*c5)/2;
    F6=k3/2;
    F7=(k7*c7-k4*c1)/2;
    
    %DCT coefficient assignment
    DCT_8x8(1,column_number)=F0;
    DCT_8x8(2,column_number)=F1;
    DCT_8x8(3,column_number)=F2;
    DCT_8x8(4,column_number)=F3;
    DCT_8x8(5,column_number)=F4;
    DCT_8x8(6,column_number)=F5;
    DCT_8x8(7,column_number)=F6;
    DCT_8x8(8,column_number)=F7;
 
end    %end of column calculations
%---------------------------end: column calculation FDCT------------------


end    % end of function flowgraph_forward_dct


% ------------------------------------------------------------------------
% FLOWGRAPH Inverse dct (Chen,Fralick and Smith)
% ------------------------------------------------------------------------
function [out_8x8] = flowgraph_inverse_dct(DCT_8x8)

% constant cosine values will be used for both forward & inverse flowgraph DCT
c1=0.980785;
c2=0.923880;
c3=0.831470;
c4=0.707107;
c5=0.555570;
c6=0.382683;
c7=0.195090;


%---------------------------row calculation Inverse DCT-------------------
for row_number=1:8
    
    %DCT coefficient initialization
    F0=DCT_8x8(row_number,1);
    F1=DCT_8x8(row_number,2);
    F2=DCT_8x8(row_number,3);
    F3=DCT_8x8(row_number,4);
    F4=DCT_8x8(row_number,5);
    F5=DCT_8x8(row_number,6);
    F6=DCT_8x8(row_number,7);
    F7=DCT_8x8(row_number,8);

    % first stage of FLOWGRAPH (Chen,Fralick and Smith)
    k0=F0/2;
    k1=F4/2;
    k2=F2/2;
    k3=F6/2;
    k4=(F1/2*c7-F7/2*c1);
    k5=(F5/2*c3-F3/2*c5);
    k6=F5/2*c5+F3/2*c3;
    k7=F1/2*c1+F7/2*c7;
    
    % second stage of FLOWGRAPH (Chen,Fralick and Smith)
    j0=(k0+k1)*c4;
    j1=(k0-k1)*c4;
    j2=(k2*c6-k3*c2);
    j3=k2*c2+k3*c6;
    j4=k4+k5;
    j5=(k4-k5);
    j6=(k7-k6);
    j7=k7+k6;

    % third stage of FLOWGRAPH (Chen,Fralick and Smith)
    i0=j0+j3;
    i1=j1+j2;
    i2=(j1-j2);
    i3=(j0-j3);
    i4=j4;
    i5=(j6-j5)*c4;
    i6=(j5+j6)*c4;
    i7=j7;
    
    % fourth stage of FLOWGRAPH (Chen,Fralick and Smith)
    f0=i0+i7;
    f1=i1+i6;
    f2=i2+i5;
    f3=i3+i4;
    f4=(i3-i4);
    f5=(i2-i5);
    f6=(i1-i6);
    f7=(i0-i7);

    %1 dimensional sample image vale assignment only after row calculations
    One_D_IDCT_Row_8x8(row_number,1)=f0;
    One_D_IDCT_Row_8x8(row_number,2)=f1;
    One_D_IDCT_Row_8x8(row_number,3)=f2;
    One_D_IDCT_Row_8x8(row_number,4)=f3;
    One_D_IDCT_Row_8x8(row_number,5)=f4;
    One_D_IDCT_Row_8x8(row_number,6)=f5;
    One_D_IDCT_Row_8x8(row_number,7)=f6;
    One_D_IDCT_Row_8x8(row_number,8)=f7;

end
%---------------------------end: row calculation Inverse DCT--------------


%---------------------------column calculation Inverse DCT----------------
for column_number=1:8
    
    %DCT coefficient initialization
    F0=One_D_IDCT_Row_8x8(1,column_number);
    F1=One_D_IDCT_Row_8x8(2,column_number);
    F2=One_D_IDCT_Row_8x8(3,column_number);
    F3=One_D_IDCT_Row_8x8(4,column_number);
    F4=One_D_IDCT_Row_8x8(5,column_number);
    F5=One_D_IDCT_Row_8x8(6,column_number);
    F6=One_D_IDCT_Row_8x8(7,column_number);
    F7=One_D_IDCT_Row_8x8(8,column_number);

    % first stage of FLOWGRAPH (Chen,Fralick and Smith)
    k0=F0/2;
    k1=F4/2;
    k2=F2/2;
    k3=F6/2;
    k4=(F1/2*c7-F7/2*c1);
    k5=(F5/2*c3-F3/2*c5);
    k6=F5/2*c5+F3/2*c3;
    k7=F1/2*c1+F7/2*c7;
    
    % second stage of FLOWGRAPH (Chen,Fralick and Smith)
    j0=(k0+k1)*c4;
    j1=(k0-k1)*c4;
    j2=(k2*c6-k3*c2);
    j3=k2*c2+k3*c6;
    j4=k4+k5;
    j5=(k4-k5);
    j6=(k7-k6);
    j7=k7+k6;

    % third stage of FLOWGRAPH (Chen,Fralick and Smith)
    i0=j0+j3;
    i1=j1+j2;
    i2=(j1-j2);
    i3=(j0-j3);
    i4=j4;
    i5=(j6-j5)*c4;
    i6=(j5+j6)*c4;
    i7=j7;
    
    % fourth stage of FLOWGRAPH (Chen,Fralick and Smith)
    f0=i0+i7;
    f1=i1+i6;
    f2=i2+i5;
    f3=i3+i4;
    f4=(i3-i4);
    f5=(i2-i5);
    f6=(i1-i6);
    f7=(i0-i7);

    % Desired sample image values assignment only after 2 dimensional inverse transformation
    out_8x8(1,column_number)=f0;
    out_8x8(2,column_number)=f1;
    out_8x8(3,column_number)=f2;
    out_8x8(4,column_number)=f3;
    out_8x8(5,column_number)=f4;
    out_8x8(6,column_number)=f5;
    out_8x8(7,column_number)=f6;
    out_8x8(8,column_number)=f7;

end
%---------------------------end: column calculation Inverse DCT-----------


end    % end of function flowgraph_inverse_dct