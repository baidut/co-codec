This is an implementation of a JPEG compression through use of a Discrete Cosine Transform in Matlab.

The steps are as follows:

1) Break image into 8x8 blocks

2) Transformation by using the DCT - Matlab uses dctmtx to create coefficients needed for transformation

3) Quantization - here I have used luminance quantization, but similarly the chrominance can be used. These are both non-uniform forms of quantization. Alternatively one can divide by N round, then multiple by N.

4) Take the Inverse DCT and apply to the current blocks

