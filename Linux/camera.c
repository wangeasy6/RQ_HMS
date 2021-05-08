#include "camera.h"

int cam_fd;

rgb_buffers rgb_buf;
extern int send_data(const char *data,const unsigned int length);
extern SHARED *g_data;

static void cap_abilities(__u32 capabilities)
{
	printf("capabilities:");
	if(capabilities & V4L2_CAP_VIDEO_CAPTURE)  fprintf(stdout,"\tV4L2_CAP_VIDEO_CAPTURE");
	if(capabilities & V4L2_CAP_VIDEO_OUTPUT)   fprintf(stdout,"\tV4L2_CAP_VIDEO_OUTPUT");
	if(capabilities & V4L2_CAP_VIDEO_OVERLAY)  fprintf(stdout,"\tV4L2_CAP_VIDEO_OVERLAY");
	if(capabilities & V4L2_CAP_VBI_CAPTURE)   fprintf(stdout,"\tV4L2_CAP_VBI_CAPTURE");
	if(capabilities & V4L2_CAP_VBI_OUTPUT)   fprintf(stdout,"\tV4L2_CAP_VBI_OUTPUT");
	if(capabilities & V4L2_CAP_SLICED_VBI_CAPTURE) fprintf(stdout,"\tV4L2_CAP_SLICED_VBI_CAPTURE");
	if(capabilities & V4L2_CAP_SLICED_VBI_OUTPUT) fprintf(stdout,"\tV4L2_CAP_SLICED_VBI_OUTPUT");
	if(capabilities & V4L2_CAP_RDS_CAPTURE)   fprintf(stdout,"\tV4L2_CAP_RDS_CAPTURE");
	if(capabilities & V4L2_CAP_VIDEO_OUTPUT_OVERLAY) fprintf(stdout,"\tV4L2_CAP_VIDEO_OUTPUT_OVERLAY");
	if(capabilities & V4L2_CAP_HW_FREQ_SEEK)   fprintf(stdout,"\tV4L2_CAP_HW_FREQ_SEEK");
	if(capabilities & V4L2_CAP_RDS_OUTPUT)   fprintf(stdout,"\tV4L2_CAP_RDS_OUTPUT");
	if(capabilities & V4L2_CAP_VIDEO_CAPTURE_MPLANE) fprintf(stdout,"\tV4L2_CAP_VIDEO_CAPTURE_MPLANE");
	if(capabilities & V4L2_CAP_VIDEO_OUTPUT_MPLANE) fprintf(stdout,"\tV4L2_CAP_VIDEO_OUTPUT_MPLANE");
	if(capabilities & V4L2_CAP_TUNER )    fprintf(stdout,"\tV4L2_CAP_TUNER ");
	if(capabilities & V4L2_CAP_AUDIO)    fprintf(stdout,"\tV4L2_CAP_AUDIO");
	if(capabilities & V4L2_CAP_RADIO)    fprintf(stdout,"\tV4L2_CAP_RADIO");
	if(capabilities & V4L2_CAP_MODULATOR)   fprintf(stdout,"\tV4L2_CAP_MODULATOR");
	if(capabilities & V4L2_CAP_READWRITE)   fprintf(stdout,"\tV4L2_CAP_READWRITE");
	if(capabilities & V4L2_CAP_ASYNCIO)    fprintf(stdout,"\tV4L2_CAP_ASYNCIO");
	if(capabilities & V4L2_CAP_STREAMING)   fprintf(stdout,"\tV4L2_CAP_STREAMING");
}

int camera_init(const char *devpath, unsigned int *width, unsigned int *height, int *size )
{
	F_STR;
	cam_fd = open(devpath, O_RDWR);
	if(cam_fd == -1){
		perror("dev open :\n");
		return FAILED;
	}
	
	//Show device info
	struct v4l2_capability cap;
	if(ioctl(cam_fd, VIDIOC_QUERYCAP, &cap) < 0){
		perror("ioctl");
		return FAILED;
	}
	printf("\nDriver Info:\nDriverName: %s\nCard Name: %s\nBus info: %s\nDriverVersion: %u.%u.%u\n",
		cap.driver, cap.card, cap.bus_info, (cap.version>>16)&0xFF, (cap.version>>8)&0xFF, cap.version&0xFF);
	cap_abilities(cap.capabilities);
	printf("\n\n");

	//Set video capture format
	struct v4l2_format fmt;
	memset(&fmt, 0, sizeof(fmt));
	fmt.type     			= V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width       = *width;
	fmt.fmt.pix.height      = *height;
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
	fmt.fmt.pix.field       = V4L2_FIELD_INTERLACED;
	
	if(ioctl(cam_fd, VIDIOC_S_FMT, &fmt) == -1){
		perror("VIDIOC_S_FMT\n");
		return -1;
	}
	
	if(ioctl(cam_fd, VIDIOC_G_FMT, &fmt) == -1){
		perror("VIDIOC_G_FMT\n");
		return -1;
	}
	printf("Current data format information:\n\twidth:%d\theight:%d\n\n",fmt.fmt.pix.width,fmt.fmt.pix.height);

	//Get buffer * 4
	struct v4l2_requestbuffers req;
	memset(&req, 0, sizeof(req));
	req.count	= 4;
	req.type	= V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory	= V4L2_MEMORY_MMAP;
	
	if(ioctl(cam_fd, VIDIOC_REQBUFS, &req) == -1){
		return FAILED;
	}

	//buffers = calloc(req.count, sizeof(*buffers));
	
	//Get buffer address and length
	struct v4l2_buffer buf;
	for(int numBufs = 0; numBufs < req.count; numBufs++){
		memset(&buf, 0, sizeof(buf));
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = numBufs;
		
		//Mapping
		if(ioctl(cam_fd, VIDIOC_QUERYBUF, &buf) == -1){	
			return FAILED;
		}
		
		buffers[numBufs].length = buf.length;
		buffers[numBufs].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE,
								MAP_SHARED, cam_fd, buf.m.offset);
		
		if(buffers[numBufs].start == MAP_FAILED){
			return FAILED;
		}
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		
		//Put in buffer queue
		if(ioctl(cam_fd, VIDIOC_QBUF, &buf) == -1){
			return FAILED;
		}
	}
	
	*width = fmt.fmt.pix.width;
	*height = fmt.fmt.pix.height;
	*size = buffers[0].length;
	
	F_END;
	return SECCESS;
	
}

int camera_start(int fd)
{
	int ret;
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	ret = ioctl(fd, VIDIOC_STREAMON, &type);
	if (ret == -1) {
		return FAILED;
	}
	
	return SECCESS;
}

///Write JPEG buffer to file
int camera_jpeg_gather(char *buffers,int len)
{
	FILE *fp;
    if((fp=fopen("./capture.jpg","w+"))==NULL)
	{
		perror("Capture JPEG:");
		return -1;
	}
	fwrite(buffers,len,1,fp);
	fclose(fp);
	return 0;
}

//YUYV to RGB
void convert_yuv_to_rgb(char *yuv, char *rgb, int width, int height,unsigned int bps)
{
	unsigned int i;
	int y1, y2, u, v;
	unsigned char *src = yuv;
	unsigned char *dst = rgb;
	unsigned int count = width * height / 2;
	
	switch (bps) {
	case 24:
		for (i = 0; i < count; i++) {
			
			y1 = *src++;
			u  = *src++;
			y2 = *src++;
			v  = *src++;
			
			*dst++ = ROUND_0_255(y1 + radj[v]);
			*dst++ = ROUND_0_255(y1 - gadj1[u] - gadj2[v]);
			*dst++ = ROUND_0_255(y1 + badj[u]);

			*dst++ = ROUND_0_255(y2 + radj[v]);
			*dst++ = ROUND_0_255(y2 - gadj1[u] - gadj2[v]);
			*dst++ = ROUND_0_255(y2 + badj[u]);
		}
		break;
	}
}





//RGB to JPEG
int convert_rgb_to_jpg_work(char *rgb, char *jpeg, unsigned int width, unsigned int height, unsigned int bpp, int quality)
{	
	// init
	memset(&jinfo, 0, sizeof(struct jpeg_mgr_info));
	jinfo.cinfo.err = jpeg_std_error(&jinfo.jerr);
	jpeg_create_compress(&jinfo.cinfo);
	
	jinfo.written = width * height * bpp / 3;
	jpeg_mem_dest(&jinfo.cinfo, (unsigned char **)&jpeg, &jinfo.written);

	jinfo.cinfo.image_width = width;
	jinfo.cinfo.image_height = height;
	jinfo.cinfo.input_components = bpp / 8;
	jinfo.cinfo.in_color_space = JCS_RGB;
	jpeg_set_defaults(&jinfo.cinfo);
	jpeg_set_quality(&jinfo.cinfo, quality, TRUE);

	jpeg_start_compress(&jinfo.cinfo, TRUE);

	while(jinfo.cinfo.next_scanline < height) {
		jinfo.row_pointer[0] = rgb + jinfo.cinfo.next_scanline * width * bpp / 8;
		jpeg_write_scanlines(&jinfo.cinfo, jinfo.row_pointer, 1);
	//	jinfo.cinfo.next_scanline++;
	}

	jpeg_finish_compress(&jinfo.cinfo);
	return (jinfo.written);
}

//RGB to BMP and save
void savebmp(char * pdata, char * bmp_file, int width, int height )  
{   
	int size = width*height*3*sizeof(char); //3 bytes per pixel
	//Bitmap part 1, file information
	BMPFILEHEADER_T bfh;  
	bfh.bfType = (WORD)0x4d42;  //bm  
	bfh.bfSize = size  // data size  
			+ sizeof( BMPFILEHEADER_T ) // first section size  
			+ sizeof( BMPINFOHEADER_T ) // second section size  
			;  
	bfh.bfReserved1 = 0; // reserved  
	bfh.bfReserved2 = 0; // reserved  
	bfh.bfOffBits = sizeof( BMPFILEHEADER_T )+ sizeof( BMPINFOHEADER_T );	//Real data start index

	//Bitmap part 2, data
	BMPINFOHEADER_T bih;  
	bih.biSize = sizeof(BMPINFOHEADER_T);  
	bih.biWidth = width;  
	bih.biHeight = -height;//BMP image is scanned from the last point.When displayed,the image is upside down,so use -height
	bih.biPlanes = 1;
	bih.biBitCount = 24;  
	bih.biCompression = 0;	//No compression
	bih.biSizeImage = size;  
	bih.biXPelsPerMeter = 2835 ;
	bih.biYPelsPerMeter = 2835 ;  
	bih.biClrUsed = 0;
	bih.biClrImportant = 0;
	FILE * fp = fopen( bmp_file,"wb" );  
	if( !fp ){
		perror("bmp_file");
		return ;
	}

	fwrite( &bfh, 8, 1,  fp );
	fwrite(&bfh.bfReserved2, sizeof(bfh.bfReserved2), 1, fp);  
	fwrite(&bfh.bfOffBits, sizeof(bfh.bfOffBits), 1, fp);  
	fwrite( &bih, sizeof(BMPINFOHEADER_T),1,fp );  
	fwrite(pdata,size,1,fp);  
	fclose( fp );
} 

/*
//Video output settings
struct v412_input input;
ioctl(cam_fd, VIDIOC_S_INPUT, &input);
*/

void *camera_run(void *arg)
{
    struct v4l2_buffer buf;
    int ret;
    unsigned int width = WIDTH;
    unsigned int height = HEIGHT;
    int size;
    jpeg_buffers jpeg_buf;

	ret = camera_init(DEV_PATH, &width, &height, &size); 
	if( FAILED == ret)
	{
		perror("camera_init failed\n");
		pthread_exit(NULL);
	}

	ret = camera_start(cam_fd);
	if( FAILED == ret)
	{
		perror("camera_start");
		close(cam_fd);
		pthread_exit(NULL);
	}

	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;  
	buf.memory = V4L2_MEMORY_MMAP;
#if 0
	int i = 0;
	for(;i<10;i++)
#else
	while(1)
#endif
    {
        ret = ioctl (cam_fd, VIDIOC_DQBUF, &buf);
        if( -1 == ret)
        {
            perror("VIDIOC_DQBUF\n");
            close(cam_fd);
            pthread_exit(NULL);
        }

        convert_yuv_to_rgb(buffers->start, rgb_buf.buf, WIDTH, HEIGHT, 24);
        rgb_buf.length=strlen(rgb_buf.buf);

        jpeg_buf.length = convert_rgb_to_jpg_work(rgb_buf.buf,jpeg_buf.buf,WIDTH,HEIGHT,24,100);


        if( jpeg_buf.length < 2000 )
        {
            print_e( "jpeg_buf is wrong\r\n" );
            ioctl (cam_fd, VIDIOC_QBUF,&buf);
            continue;
        }
        else
        {
            if ( send_data( jpeg_buf.buf, jpeg_buf.length) == jpeg_buf.length )
            {
                camera_jpeg_gather(jpeg_buf.buf,jpeg_buf.length);
            }
        }

#ifdef STDOUT
	printf("%d\n", temp);
	printf("length is: %d\n",buffers[buf.index].length);
#endif
		//process_image(buffers[buf.index].start);
		// close(buffer_store);
		ioctl (cam_fd, VIDIOC_QBUF,&buf);
	}

	jpeg_destroy_compress(&jinfo.cinfo);
	pthread_exit(NULL);
}
