#include "Direction.h"


CvSize2D32f CalculateDirection(IplImage* lst, IplImage *cur,IplImage *mask)
{
	int number_of_features = 200;
	CvPoint2D32f *frame1_features = new CvPoint2D32f[number_of_features];
    CvPoint2D32f *frame2_features = new CvPoint2D32f[number_of_features];
	char *status = new char[number_of_features];

	CvTermCriteria criteria;  
    criteria = cvTermCriteria (CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 64, 0.01);

	CvSize img_sz = cvGetSize(lst); 
	IplImage* src_img1 = cvCreateImage(img_sz, IPL_DEPTH_8U, 1);
	cvCvtColor(lst, src_img1, CV_RGB2GRAY);

	IplImage* src_img2 = cvCreateImage(img_sz, IPL_DEPTH_8U, 1);
	cvCvtColor(cur, src_img2, CV_RGB2GRAY);
  
	IplImage* eig_img = cvCreateImage (img_sz, IPL_DEPTH_32F, 1); 
	IplImage* temp_img = cvCreateImage (img_sz, IPL_DEPTH_32F, 1);
 
	IplImage* prev_pyramid = cvCreateImage (cvSize (src_img1->width + 8, src_img1->height / 3), IPL_DEPTH_8U, 1);  
	IplImage* curr_pyramid = cvCreateImage (cvSize (src_img1->width + 8, src_img1->height / 3), IPL_DEPTH_8U, 1);

    cvGoodFeaturesToTrack (src_img1, eig_img, temp_img, frame1_features, &number_of_features, 0.001, 5, mask);
    cvCalcOpticalFlowPyrLK (src_img1, src_img2, prev_pyramid, curr_pyramid,
		frame1_features, frame2_features, number_of_features, cvSize (10, 10), 4, status, NULL, criteria, 0);

	float x=0,y=0;
	float xt=0,yt=0;

	for (int i=0;i<number_of_features;i++)
	{
		if (status[i]!=0)
		{
			xt=frame2_features[i].x-frame1_features[i].x;
			yt=frame2_features[i].y-frame1_features[i].y;
			x=xt+x;
			y=yt+y;
		}
	}
	x=x/100;
	y=y/100;
	CvSize2D32f direct=cvSize2D32f(x,y);

	cvReleaseImage(&src_img1);
	cvReleaseImage(&src_img2);
	cvReleaseImage(&eig_img);
	cvReleaseImage(&temp_img);
	cvReleaseImage(&prev_pyramid);
	cvReleaseImage(&curr_pyramid);

	delete []status;
	delete []frame1_features;
	delete []frame2_features;
	return direct;
}