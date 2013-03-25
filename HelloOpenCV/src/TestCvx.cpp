#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cvaux.h>

#include "CvxText.h"

int main13 (int argc, char *argv[])
{
   // 打开一个影象
   IplImage *img = cvLoadImage("building.jpg", 1);

   // 输出汉字
   {
	  // "wqy-microhei.ttc"为文泉驿黑体
	  CvxText text("/usr/share/fonts/truetype/wqy/wqy-microhei.ttc");

//	  const char *msg = "输出 汉 字在OpenCV中！";
	  const wchar_t *msg = L"在OpenCV中输出汉字！";

	  float p = 0.8;
	  text.setFont(NULL, NULL, NULL, &p);   // 透明处理
	  text.putText(img, msg, cvPoint(50, 100), CV_RGB(255,0,0));
   }

   // 定义窗口，并显示影象
   cvNamedWindow( "img",   CV_WINDOW_AUTOSIZE );    //创建用于显示源图像的窗口
   cvShowImage( "img", img );        //显示源图像
   cvWaitKey(-1);                    //等待用户响应

   cvDestroyWindow( "img" );        //销毁窗口“img”
   cvReleaseImage(&img);            //释放源图像占用的内存

   return 0;
}
