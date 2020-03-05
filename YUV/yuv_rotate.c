
// Supported rotation
enum VideoRotationMode {
	kRotateNONE = 0,
	kRotate90 = 90,
	kRotate180 = 180,
	kRotate270 = 270,
};
yuv::libyuv::RotationMode ConvertRotationMode(VideoRotationMode rotation) {
	switch (rotation) {
	case kRotateNone:
		return yuv::libyuv::kRotate0;
	case kRotate90:
		return yuv::libyuv::kRotate90;
	case kRotate180:
		return yuv::libyuv::kRotate180;
	case kRotate270:
		return yuv::libyuv::kRotate270;
	default:
		return yuv::libyuv::kRotate0;
	}
}
void EduI420Rotate(unsigned char * Src_data, unsigned char * Dst_data,
	int src_width, int src_height, VideoRotationMode rotatemodel)
{
	// YUV420 video size
	int I420_Size = src_width * src_height * 3 / 2;
	int I420_Y_Size = src_width * src_height;
	int I420_U_Size = (src_width >> 1)*(src_height >> 1);
	int I420_V_Size = I420_U_Size;

	// src: buffer address of  Y channel、U channel and V channel
	unsigned char * Y_data_Src = Src_data;
	unsigned char * U_data_Src = Src_data + I420_Y_Size;
	unsigned char * V_data_Src = Src_data + I420_Y_Size + I420_U_Size;
	int src_stride_y = src_width;
	int src_stride_u = src_width >> 1;
	int src_stride_v = src_stride_u;

	//dst: buffer address of Y channel、U channel and V channel
	unsigned char * Y_data_Dst = Dst_data;
	unsigned char * U_data_Dst = Dst_data + I420_Y_Size;
	unsigned char * V_data_Dst = Dst_data + I420_Y_Size + I420_U_Size;
	int Dst_Stride_Y;
	int Dst_Stride_U;
	int Dst_Stride_V;
	if (rotatemodel == yuv::libyuv::kRotate90 || rotatemodel == yuv::libyuv::kRotate270)
	{
		Dst_Stride_Y = src_height;
		Dst_Stride_U = src_height >> 1;
		Dst_Stride_V = Dst_Stride_U;
	}
	else
	{
		Dst_Stride_Y = src_width;
		Dst_Stride_U = src_width >> 1;
		Dst_Stride_V = Dst_Stride_U;
	}

	yuv::libyuv::I420Rotate(Y_data_Src, src_stride_y,
		U_data_Src, src_stride_u,
		V_data_Src, src_stride_v,
		Y_data_Dst, Dst_Stride_Y,
		U_data_Dst, Dst_Stride_U,
		V_data_Dst, Dst_Stride_V,
		src_width, src_height,
		ConvertRotationMode(rotatemodel));
}