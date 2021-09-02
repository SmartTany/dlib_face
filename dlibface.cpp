#include "dlibface.h"

int FACE_RECOGNITION::load_db_faces(void)
{
	//long  hFile = 0;
	int i=0;
	dir=opendir("./facedatalib");

	//dlib::deserialize("../../dlib/model/mmod_human_face_detector.dat") >> face;             //模糊面部模型                                               //模型导入
	dlib::deserialize("./model/shape_predictor_68_face_landmarks.dat") >> sp;      //人脸面部的68个特征点模型
	dlib::deserialize("./model/dlib_face_recognition_resnet_model_v1.dat") >> net; //面部神经网络模型

	dlib::matrix<dlib::rgb_pixel>img,img1;

	//if ( fileinfo=readdir(dir) != NULL)
	//{
		char path[300];
		while((fileinfo=readdir(dir)) != NULL)                 //依次加载图片库里的文件
		{
			if(strcmp(fileinfo->d_name,".")!=0 && strcmp(fileinfo->d_name,"..")!=0)   //Linux目录下的默认文件
			{
				if(!strcmp(strstr(fileinfo->d_name,".")+1,"jpg"))
				{
			      sprintf(path,"./facedatalib/%s", fileinfo->d_name);       //将所有的图片信息输出到指定路>径
			      sprintf(path_buffer[i],"%s",fileinfo->d_name);
			      i++;
			      dlib::load_image(img, path);

			      std::vector<dlib::rectangle> dets = detector(img);  //用dlib自带的人脸检测器检测人脸，然后将人脸位置大小信息存放到dets中
			      img1 = img;
			      cv::Mat h_I = dlib::toMat(img1);
			      std::vector<dlib::full_object_detection> shapes;
			      if(dets.size()<1)
				      cout << "There is no face" << endl;
			      else if(dets.size() > 1)
				      cout << "There is too mang face" << endl;
			      else
			      {
				     shapes.push_back(sp(img, dets[0]));             //画人脸轮廓，68点

				      if (!shapes.empty())
				      {
					      for (int j = 0; j < 68; j++)
					      {
						      cv::circle(h_I, cv::Point(shapes[0].part(j).x(), shapes[0].part(j).y()), 3, cv::Scalar(255, 0, 0), -1);
					      }
				      }
				      dlib::cv_image<dlib::rgb_pixel> dlib_img(h_I);        //复制已初始化好的150*150像素并适当旋转
				      std::vector<dlib::matrix<dlib::rgb_pixel>> faces;         //定义存放截取人脸数据组
				      auto shape = sp(img, dets[0]);
				      dlib::matrix<dlib::rgb_pixel> face_chip;
				      dlib::extract_image_chip(img, dlib::get_face_chip_details(shape, 150, 0.25), face_chip);//截取人脸部分，并将大小调为150*150
				      faces.push_back(move(face_chip));
				      //Mat pic= imread(path);
				      //imshow("test",pic);
				      // image_window win1(img); //显示原图
				     // win1.add_overlay(dets[0]);    //在原图上框出人脸显示68点图
				     // image_window win2(dlib_img);  

				     // image_window win3(faces[0]);//显示截取的人脸图像

				      std::vector<dlib::matrix<float, 0, 1>> face_descriptors = net(faces);//将150*150人脸图像载入Resnet残差网络，返回128D人脸特征存于face_descriptors
				      vec.push_back(face_descriptors[0]);                            //保存这一个人脸的特征向量到vec向量的对应位置					          
				      
				      //cout << "The vector of picture " << fileinfo->d_name << endl;
				      //cv::waitKey(1000);
				                                    			      
			      }
		        }	
		   }	
		}
		closedir(dir);
		return 1;
	//}
}

void FACE_RECOGNITION::face_compares(void)
{
	
	
	string path = "./picturelib/test.jpg";
	cv::Mat h_II = cv::imread(path);
	float vec_error[30];                         //定义一个浮点型的数组，用于存放一个人脸编码与人脸库的每一个人脸编码的差值；
	dlib::matrix<dlib::rgb_pixel>img;                        //定义外部输入图片的信息
	dlib::load_image(img, path);
	std::vector<dlib::matrix<dlib::rgb_pixel>> faces_compare;
    
	for (auto face_compare : detector(img))
	{
		auto shape_comp = sp(img, face_compare);
		dlib::matrix<dlib::rgb_pixel> face_chip_compare;
		dlib::extract_image_chip(img, dlib::get_face_chip_details(shape_comp, 150, 0.25), face_chip_compare);
		faces_compare.push_back(move(face_chip_compare));
	}
	std::vector<dlib::rectangle> dets_test = detector(img);

	std::vector<dlib::matrix<float, 0, 1>> face_compare_descriptors = net(faces_compare);

	std::vector<dlib::sample_pair> edges;


	for (size_t i = 0; i < face_compare_descriptors.size(); ++i)                 //比对，识别
	{
		size_t m = 100;
		float error_min = 100.0;
		for (size_t j = 0; j < vec.size(); ++j)
		{
			vec_error[j] = (double)length(face_compare_descriptors[i] - vec[j]);
			//cout << "The error of two picture is:" << vec_error[j] << endl;

			//if (length(face_descriptors[i] - face_descriptors[j]) < 0.6)
			if (vec_error[j] < error_min)
			{
				error_min = vec_error[j];
				m = j;
			}

		}
		//cout << "min error of two face:" << error_min << endl;
		//II = dlib::toMat(img);

		std::string text = "unknown";
		if ((error_min < 0.6) && (m <= 27))
			text = path_buffer[m];                      //通过m定位文件，得到文件名
		        text = text.substr(0 , text.length()-4);    //去掉图片后缀

		int font_face = cv::FONT_HERSHEY_COMPLEX;
		double font_scale = 0.5;
		int thickness = 1;
		int baseline;

		cv::Size text_size = cv::getTextSize(text, font_face, font_scale, thickness, &baseline);
		cv::Point origin;
		cv::Point PointT(dets_test[i].left(),dets_test[i].bottom()+20);
	        origin.x = dets_test[i].left();
                origin.y = dets_test[i].top();
		cv::rectangle(h_II, cv::Rect(dets_test[i].left(), dets_test[i].top(), dets_test[i].width(), dets_test[i].width()), cv::Scalar(0, 0, 255), 1, 1, 0);//画矩形框
		cv::putText(h_II, text, PointT, font_face, font_scale, cv::Scalar(255, 0, 0), thickness, 0.3, 0);//给图片加文字
	}

	dlib::cv_image<dlib::rgb_pixel> img4(h_II);
	//image_window win(img4);
	//cv::Mat pic = dlib::toMat(img4);
	cv::imshow("test",h_II);
	cv::waitKey(0);
	system("pause");
	//if (cv::waitKey(100) == 27)break;

}





