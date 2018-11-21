/*************************************************************************
    > File Name: thresh_get.cpp
    > Author: jiang
    > Mail: 760021776@qq.com 
    > Created Time: 2018年11月21日 星期三 10时30分29秒
 ************************************************************************/

#include<iostream>
#include<string>
#include<algorithm>
#include <opencv2/opencv.hpp>
using namespace std;

#define windowname  "show"
string one_channel;

void BGRtoHSV(int *BGR,int *HSV)
{
 //   char HSV[3];
    int max,min;
    max = *max_element(BGR,BGR+3);
    min = *min_element(BGR,BGR+3);
    if(max == min){
        HSV[0] = 0;
    }
    else if(max == BGR[2] && BGR[1] >= BGR[0]){
        HSV[0] = (60.0*(BGR[1] - BGR[0])/(max - min))/2;
    }
    else if(max == BGR[2] && BGR[1] < BGR[0]){
        HSV[0] = (60.0*(BGR[1] - BGR[0])/(max - min) + 360)/2;
    }
    else if(max == BGR[0]){
        HSV[0] = (60.0*(BGR[2] - BGR[1])/(max - min) + 240)/2;
    }
    else if(max == BGR[1]){
        HSV[0] = (60.0*(BGR[0] - BGR[2])/(max - min) + 120)/2;
    }
    if(max == 0)
        HSV[1] = 0;
    else
        HSV[1]= 255*(1 - 1.0*min/max);
    HSV[2] = max;
//    cout << "hsv : ";
//    for(int i = 0; i <3 ;i++){
//            cout << HSV[i] <<endl;
//    }
}

bool display_pointValue(cv::Mat image,int x,int y,string &value)   //返回图像类型0 灰度；1 彩色    char a[]   ==  char *a  == &string（传递更容易）
{
//    cv::cvtColor(image,image,CV_BGR2GRAY);
    if(image.type() == CV_8UC1){
        char gray_value[] = "000";
        gray_value[0] += image.at<uchar>(y,x)/100;
        gray_value[1] += image.at<uchar>(y,x)%100/10;
        gray_value[2] += image.at<uchar>(y,x)%10;

        //value = strcat(string1,gray_value);
        value = gray_value;
        return 0;
    }
    else if(image.type() == CV_8UC3){
        char value_3[] = "BGR:000,000,000;HSV:000,000,000";
        int *bgr = new int[3];
        int *hsv = new int[3];
        // char *bgr,*hsv;
        for(int i = 0;i < 3;i ++){
            bgr[i] = image.at<cv::Vec3b>(y,x)[i];
        }
        BGRtoHSV(bgr,hsv);
        for(int i = 0;i<3;i++){
             value_3[4+4*i] += bgr[i]/100;
             value_3[5+4*i] += bgr[i]%100/10;
             value_3[6+4*i] += bgr[i]%10;
            }
        for(int i = 0;i <3;i++){
            value_3[20+4*i] += hsv[i]/100;
            value_3[21+4*i] += hsv[i]%100/10;
            value_3[22+4*i] += hsv[i]%10;
        }
        value = value_3;
        delete []bgr;
        delete []hsv;
        return 1;
    }

}

void on_mouse(int EVENT, int x, int y, int flags, void* userdata)
{
    cv::Mat image;
    bool img_type;

    char location[] = " (000,000)";
    char numx[3],numy[3];;
    image = *(cv::Mat*)userdata;
    static cv::Mat mid_imag = image.clone();
    cv::Point p(x, y);
    numx[0] = x/100;
    numx[1] = (x - x/100*100)/10;
    numx[2] =  x%10;
    numy[0] = y/100;
    numy[1] = (y - y/100*100)/10;
    numy[2] =  y%10;
    string value;
    string strBGR,strHSV;
 //   static int point_num = 0;
    switch(EVENT){                           // switch 里面不能定义变量
    case CV_EVENT_LBUTTONDOWN:
        img_type = display_pointValue(mid_imag,x,y,value);

	for(int i = 0;i <3;i ++){
		 location[2+i] =  location[2+i] +numx[i];
		 location[6+i] =  location[6+i] +numy[i];
	}	
	if(img_type)     						//彩色图
	{
		strBGR = value.substr(0,15);                           //取value 0-15位赋给bgr
		strHSV = value.substr(16,30);

		cout<<location<<"  " << strBGR <<"      "<< strHSV<<endl ;
		cv::circle(mid_imag,p,2,cv::Scalar(0,0,0));
		cv::putText(mid_imag,location,cvPoint(x,y),cv::FONT_HERSHEY_SIMPLEX,0.4,cv::Scalar(0,0,255));
		cv::putText(mid_imag,strBGR,cvPoint(x,y+20),cv::FONT_HERSHEY_SIMPLEX,0.4,cv::Scalar(0,0,0));
		cv::putText(mid_imag,strHSV,cvPoint(x,y+40),cv::FONT_HERSHEY_SIMPLEX,0.4,cv::Scalar(200,30,200));
	}
	else{
		cout<<location<<"  " << one_channel+value  <<endl ;
		cv::circle(mid_imag,p,2,cv::Scalar(0,0,0));
		cv::putText(mid_imag,location,cvPoint(x,y),cv::FONT_HERSHEY_SIMPLEX,0.4,cv::Scalar(0,0,255));	
		cv::putText(mid_imag,one_channel+value,cvPoint(x,y+20),cv::FONT_HERSHEY_SIMPLEX,0.4,cv::Scalar(200,30,200));
	}
        cv::imshow(windowname,mid_imag);
        break;

    case CV_EVENT_RBUTTONDOWN:           //已经显示了需要的点，右键清除这些点
        cout << "clear" << endl;
        mid_imag = image.clone();
        cv::imshow(windowname,image);
        break;
    }
}

int main()
{
	
	cv::Mat image;
	string file;
        char flag = 0;
	for( ; ; ){
		cout << "输入图片路径及名称:" << endl;
		getline(cin,file);
		image = cv::imread(file);
		if(!image.data){
			cout << "error : 文件不存在，请重新输入！" << endl;
			continue;
		}
		else{
                        cout << "打开成功！"<<endl;
			cout <<"输入1-灰度图,r g b h s v对应单通道；输入其他为彩色图" << endl;
		}

                vector<cv::Mat> BGR;
                vector<cv::Mat> HSV;

                cin >> flag;

                if(flag){
                    switch (flag){
                    case '1':
			one_channel = "gray:";
                        cv::cvtColor(image,image,CV_BGR2GRAY);
                        break;

                    case 'b':
			one_channel = "blue:";
                        cv::split( image, BGR );    //分离彩色通道
                        image = BGR.at(0);
                        break;
                    case 'g':
			one_channel = "green:";
                        cv::split( image, BGR );
                        image = BGR.at(1);
                        break;
                    case 'r':
			one_channel = "red:";
                        cv::split( image, BGR );
                        image = BGR.at(2);
                        break;

                    case 'h':
			one_channel = "hue:";
                        cv::split( image, HSV );
                        image = HSV.at(0);
                        break;
                    case 's':
			one_channel = "saturation:";
                        cv::split( image, HSV );
                        image = HSV.at(1);
                        break;
                    case 'v':
			one_channel = "value:";
                        cv::split( image, HSV );
                        image = HSV.at(2);
                        break;
                    }
                }


		cout << "左键显示；右键清除"<<endl; 
	
		cv::imshow(windowname,image);
		cv::setMouseCallback(windowname, on_mouse,&image);

		cout << "按 ESC 退出"<<endl;
    		if(cv::waitKey(0) == 27){
			break;
		}
	}
    	return 0;
}
