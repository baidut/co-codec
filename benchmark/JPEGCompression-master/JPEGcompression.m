% Do a basic implementation of JPEG.
% Divide the image into 8x8 blocks.
% Compute the DCT (discrete cosine transform) of each block. This is implemented in popular packages such as Matlab.
% Quantize each block. You can do this using the tables in the video or simply divide each coefficient by N, round the result to the nearest integer, and multiply back by N. Try for different values of N.
% You can also try preserving the 8 largest coefficients (out of the total of 8x8=64), and simply rounding them to the closest integer.
% Visualize the results.

result = jpeg_compression(image_path, qf)

    % reads image path and ensures anything outside of 0-255 is bound
    I = uint8(imread(image_path));
    
    [height,width] = size(I);
    
    
    % crops image to ensure it can be broken into 8x8 blocks
    Im_red = imcrop(I,[1,1, width, height ]);
    
    % transforms it into other colour space 
%    I_YCR = im2double(rgb2ycbrc(Im_red));
    
    % or deduct 128 from all - Normalise
    Im_ded = im2double(Im_red - 128);

    % or if we do this by scale quantization


    % Below I assume we want non-uniform quantization
    % Quantization table for luminance Y
        q_max = 255;
        q_y = ...
           [16 11 10 16 124 140 151 161;
            12 12 14 19 126 158 160 155;
            14 13 16 24 140 157 169 156;
            14 17 22 29 151 187 180 162;
            18 22 37 56 168 109 103 177;
            24 35 55 64 181 104 113 192;
            49 64 78 87 103 121 120 101;
            72 92 95 98 112 100 103 199];
        
        % increase accuracy by making it a double
        q_y = double(q_y); 

        % Quantization for Chrominance
        q_C = ...
           [17 18 24 47 99 99 99 99;
            18 21 26 66 99 99 99 99;
            24 26 66 99 99 99 99 99;
            99 99 99 99 99 99 99 99;
            99 99 99 99 99 99 99 99;
            99 99 99 99 99 99 99 99;
            99 99 99 99 99 99 99 99;
            99 99 99 99 99 99 99 99];
        
    q_max = 255;
    
    if qf < 50
        q_scale = floor(5000 / qf);
    else
        q_scale = 200 - 2 * qf;
    end
    
    q_y = round(q_y * q_scale / 100)

    % this creates the cosine transformation matrix which is the dct for matrix multiplication
    dct_matrix = dctmtx(8);
    % here we define what the dct matrix will be 
    dct = @(block_struct) dct_matrix * @(block_struct) dct_matrix';
    % similarly we obtain the inverse
    idct = @(block_struct) dct_matrix' * @(block_struct) dct_matrix;

    % breaks image into 8x8 blocks
    B = blockproc(I_ded, [8 8], dct).*q_max;
    % Quantization through dividing and then rounding
    Xq = blockproc(B, [8 8], @(block_struct) round(round(block_struct.data)./ q_y));
    % Now multiple through
    Xd = blockproc(Xq,[8 8], @(block_struct)block_struct.data) .*q_y));
    
    % denormalization
    result = blockproc(Xd ./ q_max, [8 8], idct) + 128;

    % shows the border around the image
    imshow(result, 'Border', 'tight');
