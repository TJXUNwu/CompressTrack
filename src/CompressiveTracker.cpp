#include "CompressiveTracker.h"
#include <math.h>
#include <iostream>
using namespace cv;
using namespace std;

//------------------------------------------------
CompressiveTracker::CompressiveTracker(void)
{
	featureMinNumRect = 2;
	featureMaxNumRect = 4;	// number of rectangle from 2 to 4
	featureNum = 50;	// number of all weaker classifiers, i.e,feature pool
	rOuterPositive = 4;	// radical scope of positive samples
	rSearchWindow = 25; // size of search window
	muPositive = vector<float>(featureNum, 0.0f);
	muNegative = vector<float>(featureNum, 0.0f);
	sigmaPositive = vector<float>(featureNum, 1.0f);
	sigmaNegative = vector<float>(featureNum, 1.0f);
	learnRate = 0.85f;	// Learning rate parameter
}

CompressiveTracker::~CompressiveTracker(void)
{
}

/*Description: compute Haar features
Arguments:
-_objectBox: [x y width height] object rectangle
-_numFeature: total number of features.The default is 50.
*/
/*ͨ������ͼ������ɼ�����ÿһ��������harr�������������ͨ����featuresWeight�����
���൱��ͶӰ����������������ˣ�Ҳ���ǽ���ϡ�����ˡ����ﲻ���׵Ļ������Կ���
�����е�ͼ�����ͱȽ�ֱ���ˡ�
����һ�㣺ʵ����������õĲ�����������harr�������Ҳ����з�����������������
��box�в����õ��Ĳ�ͬ���ο�ĻҶȼ�Ȩ��ͣ���Ȩ���Ǹ�����ʱ����ǻҶȲ
��Ϊ�˱������㣬�����涼��harr������������
ÿһ��������50��harr������ÿһ��harr��������2��3�����ѡ��ľ��ο������ɵģ�
����Щ���ο�ĻҶȼ�Ȩ�����Ϊ��һ��harr����������ֵ�� */
void CompressiveTracker::HaarFeature(Rect& _objectBox, int _numFeature)
{ 
	//_numFeature��һ������box��harr������������50����������˵����  
	//ÿһ��harr��������2��3�����ѡ��ľ��ο�vector<Rect>()���ͣ������ɵġ�
	features = vector<vector<Rect>>(_numFeature, vector<Rect>());

	//ÿһ����Ӧ�����ľ��ο��Ӧ��һ��Ȩ�أ�ʵ���Ͼ������������������Ӧ��Ԫ�أ����������Ӧ������  
	//��ˣ���ʾ��Ȩ�صĳ̶�����֪������������仰˵��featuresWeight��������������� 
	//��������Ԫ�صĸ�ֵ�������еĵڶ����֡�
	featuresWeight = vector<vector<float>>(_numFeature, vector<float>());

	//numRect��ÿ�������ľ��ο��������������˵��������������е�s�����м汸���ֹ��ܣ�  
	//������˵sȡ2����3ʱ�����������Johnson-Lindenstrauss���ۡ�  
	int numRect;
	Rect rectTemp;
	float weightTemp;
      
	for (int i=0; i<_numFeature; i++)
	{
		//������ɷ���ĳ�����ʷֲ��������������˵ sample�������⡣  
		//���磬����Ҫ��һ��������̬�ֲ�����������õ� 100 ����������ô�϶��鵽�ӽ����ֵ��������  
		//����Ҫ����࣬�Ӷ����³鵽�������ܶ��Ǽ������Ǹ����ġ�
		//rng.uniform()����һ����[ 1��2����Χ���Ȳ����������������[ 1��2���ڷ��Ӿ��ȷֲ���ȡ��ֵͬ������ͬ��  
		//��ô����Ĺ��ܾ��ǵõ�[2��4����Χ���������Ȼ����cvFloor���ز����ڲ������������ֵ����Ҫô��2��Ҫô��3��
		numRect = cvFloor(rng.uniform((double)featureMinNumRect, (double)featureMaxNumRect));
	
		for (int j=0; j<numRect; j++)
		{ //����һ��box���������һ�����ο��Ǻ������box��x��y������޹��ˣ����ұ��뱣֤��ѡ��  
		 //��������ο򲻻ᳬ�������box�ķ�Χ�����ǰ�  
		 //�������3�������2��ɶ��˼�أ��ҾͲ����ˣ����������Ϊ�˱���������ο�̫����box�ı�Ե��  
		 //Ҫ���Ե��С2�����أ���֪���������Բ��ԣ�����ָ�� 
			
			rectTemp.x = cvFloor(rng.uniform(0.0, (double)(_objectBox.width - 3)));
			rectTemp.y = cvFloor(rng.uniform(0.0, (double)(_objectBox.height - 3)));
			//cvCeil ���ز�С�ڲ�������С����ֵ  
			rectTemp.width = cvCeil(rng.uniform(0.0, (double)(_objectBox.width - rectTemp.x - 2)));
			rectTemp.height = cvCeil(rng.uniform(0.0, (double)(_objectBox.height - rectTemp.y - 2)));
			//����õ�������ģ�塣ע��Ŷ������ľ��ο��������box�����λ��Ŷ�������������ͼ���Ŷ  
			features[i].push_back(rectTemp);


			//weightTemp = (float)pow(-1.0, c);  
			//pow(-1.0, c)Ҳ����-1��c�η�����c�����ȡ0����1��Ҳ����˵weightTemp������������߸���  
			//������������У�����Ԫ�������֣�sqrt(s)��-sqrt(s)���㡣Ϊ����Ϊ���ĸ�������ȵģ�  
			//�����Ϊʲô��[2��4�����Ȳ�����ԭ�򣬾���ȡ0����1����һ����  
			//��������Ϊʲô��sqrt(s)��֮һ�أ�����ʲôʱ����0�أ���������0�ĸ��ʲ���ͦ�����  
			//û��0Ԫ�أ�������ϡ�����ѹ���أ�����������ָ��������Ȼϡ�������һ���ô�  
			//����ֻ�豣�����Ԫ�ء������������й�ϵ�𣿣�  
			weightTemp = (float)pow(-1.0, cvFloor(rng.uniform(0.0, 2.0))) / sqrt(float(numRect));
			//����ÿһ������ģ���Ӧ��Ȩ��  
			featuresWeight[i].push_back(weightTemp);
           
		}
	}
}

/*�ɼ����� Description: compute the coordinate of positive and negative sample image templates
Arguments:
-_image:        processing frame
-_objectBox:    recent object position
-_rInner:       inner sampling radius
-_rOuter:       Outer sampling radius
-_maxSampleNum: maximal number of sampled images
-_sampleBox:    Storing the rectangle coordinates of the sampled images.
////����һ֡���ٵ�Ŀ��box����Χ�ɼ������������͸�����������ʼ�����߸��·������� 
*/
void CompressiveTracker::sampleRect(Mat& _image, Rect& _objectBox, float _rInner, float _rOuter, int _maxSampleNum, vector<Rect>& _sampleBox)
{
	int rowsz = _image.rows - _objectBox.height - 1;
	int colsz = _image.cols - _objectBox.width - 1;
	//����������һ֡���ٵ�Ŀ��box����Χ�ɼ��������͸������ģ��������Χ��ͨ����  
	//���Ŀ��Ϊ���ĵ�����Բ����ʾ��������Բ�İ뾶��_rInner��_rOuter��  
	//����������һ֡���ٵ�Ŀ��box��С��_rInner����ķ�Χ�ڲɼ���������  
	//�ڴ���_rOuter����ķ�Χ�ڲɼ��������������л���һ���Ͻ磬������  
	//����û�У���ʵ����Ҳûʲô��Ҫ�ޣ�  

	float inradsq = _rInner*_rInner;
	float outradsq = _rOuter*_rOuter;

  	
	int dist;
	//���ĸ���Ϊ�˷�ֹ�ɼ��Ŀ򳬳�ͼ��Χ�ģ��Բɼ���box��x��y����������  
	int minrow = max(0,(int)_objectBox.y-(int)_rInner);
	int maxrow = min((int)rowsz-1,(int)_objectBox.y+(int)_rInner);
	int mincol = max(0,(int)_objectBox.x-(int)_rInner);
	int maxcol = min((int)colsz-1,(int)_objectBox.x+(int)_rInner);
    
	
	
	int i = 0;
	//��ĸ�൱��x�ܲɼ��ķ�Χ����y�ܲɼ��ķ�Χ��Ҳ���ǿ��Բɼ������box������  
	//��ô_maxSampleNum��������Ҫ�ɼ���box�����������϶���С�ڻ��ߵ�������  
	//�����prob�Ǹ�����أ��������õ����ĵط�˵  

	float prob = ((float)(_maxSampleNum))/(maxrow-minrow+1)/(maxcol-mincol+1);

	int r;
	int c;
    
    _sampleBox.clear();//important
    Rect rec(0,0,0,0);

	for( r=minrow; r<=(int)maxrow; r++ )
		for( c=mincol; c<=(int)maxcol; c++ ){
			//�������ɵ�box��Ŀ��box�ľ���  
			dist = (_objectBox.y-r)*(_objectBox.y-r) + (_objectBox.x-c)*(_objectBox.x-c);
			//�����������Ǳ�֤������Ҫ��_rInner��_rOuter�ķ�Χ��  
			 //��ôrng.uniform(0.,1.) < prob ����Ǹ�����أ�  
			//�������濴�����_maxSampleNum�����Ǹ���������prob�ʹ���1��������  
			 //rng.uniform(0.,1.) < prob����������������㣬��ʾ�������Χ������  
			//����box�Ҷ�Ҫ�ˣ���Ϊ�ұ�����Ҫ����ģ����������������ô�࣬��ô���ܸ��ģ��ҿ϶�ȫҪ�ˣ���  
			//����������̫���ˣ��Ҳ�Ҫ��ô�࣬Ҳ����prob<1�����Ҿ�������������ߺ���  

			if( rng.uniform(0.,1.)<prob && dist < inradsq && dist >= outradsq ){

                rec.x = c;
				rec.y = r;
				rec.width = _objectBox.width;//û�����߶Ȳ��䣿��������box�Ĵ�С��û�仯 
				rec.height= _objectBox.height;
				
                _sampleBox.push_back(rec);				
				
				i++;
			}
		}
	
		_sampleBox.resize(i);
		
}

//���أ��ɼ�����
//���sampleRect�����غ�������������һ֡���ٵ�Ŀ��box����Χ������С��_srw���ɼ�����box������⡣  
//��������Ǹ���һ���������Ǹ�������һ֡�Ѿ�����Ŀ��Ļ����ϣ��ɼ��������������·������ġ�  
//�����Ǹ������������ᵽ���㷨�ĵ��ĸ����裬����ǵ�һ�����衣Ȼ����̲�࣬ûʲô��˵����
void CompressiveTracker::sampleRect(Mat& _image, Rect& _objectBox, float _srw, vector<Rect>& _sampleBox)
/* Description: Compute the coordinate of samples when detecting the object.*/
{
	int rowsz = _image.rows - _objectBox.height - 1;
	int colsz = _image.cols - _objectBox.width - 1;
	float inradsq = _srw*_srw;	
	

	int dist;

	int minrow = max(0,(int)_objectBox.y-(int)_srw);
	int maxrow = min((int)rowsz-1,(int)_objectBox.y+(int)_srw);
	int mincol = max(0,(int)_objectBox.x-(int)_srw);
	int maxcol = min((int)colsz-1,(int)_objectBox.x+(int)_srw);

	int i = 0;

	int r;
	int c;

	Rect rec(0,0,0,0);
    _sampleBox.clear();//important

	for( r=minrow; r<=(int)maxrow; r++ )
		for( c=mincol; c<=(int)maxcol; c++ ){
			dist = (_objectBox.y-r)*(_objectBox.y-r) + (_objectBox.x-c)*(_objectBox.x-c);

			if( dist < inradsq ){

				rec.x = c;
				rec.y = r;
				rec.width = _objectBox.width;
				rec.height= _objectBox.height;

				_sampleBox.push_back(rec);				

				i++;
			}
		}
	
		_sampleBox.resize(i);
}

//���أ��ɼ�������FCT�еĲ���
void CompressiveTracker::sampleRect(Mat& _image, Rect& _objectBox, float _srw, vector<Rect>& _sampleBox, int step)
{
	int rowsz = _image.rows - _objectBox.height - 1;
	int colsz = _image.cols - _objectBox.width - 1;
	float inradsq = _srw*_srw;


	int dist;

	int minrow = max(0, (int)_objectBox.y - (int)_srw);
	int maxrow = min((int)rowsz - 1, (int)_objectBox.y + (int)_srw);
	int mincol = max(0, (int)_objectBox.x - (int)_srw);
	int maxcol = min((int)colsz - 1, (int)_objectBox.x + (int)_srw);

	int i = 0;

	int r;
	int c;

	Rect rec(0, 0, 0, 0);
	_sampleBox.clear();//important
	//step��ʾ����
	for (r = minrow; r <= (int)maxrow; r = r + step)
		for (c = mincol; c <= (int)maxcol; c = c + step){
			dist = (_objectBox.y - r)*(_objectBox.y - r) + (_objectBox.x - c)*(_objectBox.x - c);

			if (dist < inradsq){

				rec.x = c;
				rec.y = r;
				rec.width = _objectBox.width;
				rec.height = _objectBox.height;

				_sampleBox.push_back(rec);

				i++;
			}
		}

	_sampleBox.resize(i);

}


// Compute the features of samples
//ͨ������ͼ������ɼ�����ÿһ��������harr�������������ͨ����featuresWeight�����  
//���൱��ͶӰ����������������ˣ�Ҳ���ǽ���ϡ�����ˡ����ﲻ���׵Ļ������Կ���  
//�����е�ͼ�����ͱȽ�ֱ���ˡ���������õ����ǣ�ÿ��������ϡ������harr������  
//����һ�㣺ʵ����������õĲ�����������harr�������Ҳ����з�����������������  
//��box�в����õ��Ĳ�ͬ���ο�ĻҶȼ�Ȩ���
void CompressiveTracker::getFeatureValue(Mat& _imageIntegral, vector<Rect>& _sampleBox, Mat& _sampleFeatureValue)
{
	int sampleBoxSize = _sampleBox.size();
	_sampleFeatureValue.create(featureNum, sampleBoxSize, CV_32F);
	float tempValue;
	int xMin;
	int xMax;
	int yMin;
	int yMax;

	for (int i=0; i<featureNum; i++)
	{
		for (int j=0; j<sampleBoxSize; j++)
		{
			tempValue = 0.0f;
			for (size_t k=0; k<features[i].size(); k++)
			{ //features�б��������ģ�壨���ο��������box�����λ�õģ�  
			  //������Ҫ����box�����������������ͼ���е�����  

				xMin = _sampleBox[j].x + features[i][k].x;
				xMax = _sampleBox[j].x + features[i][k].x + features[i][k].width;
				yMin = _sampleBox[j].y + features[i][k].y;
				yMax = _sampleBox[j].y + features[i][k].y + features[i][k].height;

				//ͨ������ͼ�����ټ���һ�����ο�����غͣ�����ͼ���˽�Ļ������Կ����ҵ�������ģ�  
				//http://blog.csdn.net/zouxy09/article/details/7929570  
				//��ô����tempValue���Ǿ���ϡ�;����Ȩ��ĻҶȺ��ˡ�  
				//ÿһ��harr��������2��3�����ο������ɵģ�����Щ���ο�ĻҶȼ�Ȩ���  
				//��Ϊ��һ��harr����������ֵ��Ȼ��һ��������50��harr����  

				tempValue += featuresWeight[i][k] * 
					(_imageIntegral.at<float>(yMin, xMin) +
					_imageIntegral.at<float>(yMax, xMax) -
					_imageIntegral.at<float>(yMin, xMax) -
					_imageIntegral.at<float>(yMax, xMin));
			}
			_sampleFeatureValue.at<float>(i,j) = tempValue;
		}
	}
}

// Update the mean and variance of the gaussian classifier
//��������ͨ���ø�˹�ֲ�ȥ����������ÿһ��harr�����ĸ��ʷֲ��ġ���˹�ֲ��Ϳ���ͨ�������ͷ���  
//����������������Ȼ��ͨ������������ÿһ��harr������˹���ʷֲ��Ķ�����ֵ������������������  
//��box����Ŀ�껹�Ǳ�������������²ɼ��������������������������ͱ�׼������������·����� 
void CompressiveTracker::classifierUpdate(Mat& _sampleFeatureValue, vector<float>& _mu, vector<float>& _sigma, float _learnRate)
{
	Scalar muTemp;
	Scalar sigmaTemp;
    
	for (int i=0; i<featureNum; i++)
	{  //�����������������߸�������ĳ��harr�����������ͱ�׼�� 
		meanStdDev(_sampleFeatureValue.row(i), muTemp, sigmaTemp);
		//���ģ�Ͳ������µĹ�ʽ�����ĵĹ�ʽ6  
		_sigma[i] = (float)sqrt( _learnRate*_sigma[i]*_sigma[i]	+ (1.0f-_learnRate)*sigmaTemp.val[0]*sigmaTemp.val[0] 
		+ _learnRate*(1.0f-_learnRate)*(_mu[i]-muTemp.val[0])*(_mu[i]-muTemp.val[0]));	// equation 6 in paper

		_mu[i] = _mu[i]*_learnRate + (1.0f-_learnRate)*muTemp.val[0];	// equation 6 in paper
	}
}

// Compute the ratio classifier 
void CompressiveTracker::radioClassifier(vector<float>& _muPos, vector<float>& _sigmaPos, vector<float>& _muNeg, vector<float>& _sigmaNeg,
										 Mat& _sampleFeatureValue, float& _radioMax, int& _radioMaxIndex)
{
	float sumRadio;
	//FLT_MAX�����ĸ������ĺ궨�壬��ô-FLT_MAX������С�ĸ�������  
	//������������ ��ô��box�����ķ��������  
	_radioMax = -FLT_MAX;
	//����Ƕ�Ӧ�������Ǹ����Ǵ�ŷ�����������Ǹ�box��  
	_radioMaxIndex = 0;
	float pPos;
	float pNeg;
	int sampleBoxNum = _sampleFeatureValue.cols;

	for (int j=0; j<sampleBoxNum; j++)//ÿ֡�����õ�����Ҫ����box  
	{
		sumRadio = 0.0f;
		for (int i=0; i<featureNum; i++)
		{ //����ÿ�������ĸ��ʣ������ֲ������ڸ�˹�ֲ����ʽ������������ľ�ֵ�ͱ�׼������˹ģ�;Ϳ���  
		  //�õ����ֱ����������͸������Ļ����ϣ����ָ������ĸ����Ƕ��١����������ʱ��ĸ��ʴ���ô  
		 //���Ǿ�˵�����������Ӧ������������������ѧ�ϱȽϴ�С�����Ǽ������߳����ˣ�������ȡ������ֵ  

			pPos = exp( (_sampleFeatureValue.at<float>(i,j)-_muPos[i])*(_sampleFeatureValue.at<float>(i,j)-_muPos[i]) / -(2.0f*_sigmaPos[i]*_sigmaPos[i]+1e-30) ) / (_sigmaPos[i]+1e-30);
			pNeg = exp( (_sampleFeatureValue.at<float>(i,j)-_muNeg[i])*(_sampleFeatureValue.at<float>(i,j)-_muNeg[i]) / -(2.0f*_sigmaNeg[i]*_sigmaNeg[i]+1e-30) ) / (_sigmaNeg[i]+1e-30);
			//paper�ķ���4��������������õ�һ�������������������һ����������box��50�������������ࣩ  
			//�������������õ��Ľ���ܺͣ�ǿ���ࣿ������������Ŀǰ���box������������������Ŀ�꣩��  
			//�����Դ�С���ĸ����������Ȼ�Ҿ���Ϊ����Ŀ���ˡ�����Ȼ���ھ���Ӧ������Ҫ��һЩ����ȥ  
			//��������ٵ���������������ߵķ������ﲻ��һ����ֵ���ǾͲ�����Ŀ��ȣ�  

			sumRadio += log(pPos+1e-30) - log(pNeg+1e-30);	// equation 4
		}
		if (_radioMax < sumRadio)
		{
			_radioMax = sumRadio;
			_radioMaxIndex = j;
		}
	}
}

//�����һ֡��Ҫ���ٵ�Ŀ��box(���ļ�������û�����ѡ)����ʼ������
void CompressiveTracker::init(Mat& _frame, Rect& _objectBox)
{
	// compute feature template
	//����box��harr����ģ�壬�ȴ���
	HaarFeature(_objectBox, featureNum);

	//��Ϊ���ǵ�һ֡��Ŀ��box�������ļ���������û�����ѡ�ģ�����֪�ģ�  
	//��������ͨ�������Ŀ��box��Χ���ɼ��������͸���������ʼ�����ǵķ�����  
	// compute sample templates
	sampleRect(_frame, _objectBox, rOuterPositive, 0, 1000000, samplePositiveBox);
	sampleRect(_frame, _objectBox, rSearchWindow*1.5, rOuterPositive+4.0, 100, sampleNegativeBox);
	
	//�������ͼ�����Կ��ٵļ���harr����
	integral(_frame, imageIntegral, CV_32F);

	//ͨ������Ļ���ͼ���������ǲ�����������������box��harr����  
	getFeatureValue(imageIntegral, samplePositiveBox, samplePositiveFeatureValue);
	getFeatureValue(imageIntegral, sampleNegativeBox, sampleNegativeFeatureValue);

	//ͨ�������������������������ʼ��������  
	classifierUpdate(samplePositiveFeatureValue, muPositive, sigmaPositive, learnRate);
	classifierUpdate(sampleNegativeFeatureValue, muNegative, sigmaNegative, learnRate);
}

//���ٴ���������һ֡���ٵ���box���������µ�һ֡  
void CompressiveTracker::processFrame(Mat& _frame, Rect& _objectBox)
{
	// predict
	//����һ֡���ٵ���boxbox��Χ���ɼ���Ҫ����box��  
	sampleRect(_frame, _objectBox, rSearchWindow, detectBox);//��ӦCT
	//sampleRect(_frame, _objectBox, rSearchWindow,detectBox,4);//��ӦFCT�������б�ע�ͣ���Ӧ�ڶ�����Ҳ��ע��
	//������һ֡�Ļ���ͼ  
	integral(_frame, imageIntegral, CV_32F);
	//�û���ͼ����������ɼ�����ÿ��box��harr���� 
	getFeatureValue(imageIntegral, detectBox, detectFeatureValue);
	int radioMaxIndex;
	float radioMax;
	//�������ÿ��box����ƥ�����  
	radioClassifier(muPositive, sigmaPositive, muNegative, sigmaNegative, detectFeatureValue, radioMax, radioMaxIndex);
	//�õ�������ߵ��Ǹ�Ŀ��box  
	_objectBox = detectBox[radioMaxIndex];


	//FCT�в����ɴֵ����Ĳ�����ʽ��
	//�ڶ��β������뾶Ϊ10������Ϊ1
	//sampleRect(_frame, _objectBox, 10, detectBox, 1);
	//integral(_frame, imageIntegral, CV_32F);
	//getFeatureValue(imageIntegral, detectBox, detectFeatureValue);
	//radioClassifier(muPositive, sigmaPositive, muNegative, sigmaNegative, detectFeatureValue, radioMax, radioMaxIndex);
	//_objectBox = detectBox[radioMaxIndex];

	// update
	//���¸��ٵ������Ŀ��box����Χ���ɼ��������͸��������������ǵķ�����  
	sampleRect(_frame, _objectBox, rOuterPositive, 0.0, 1000000, samplePositiveBox);
	sampleRect(_frame, _objectBox, rSearchWindow*1.5, rOuterPositive+4.0, 100, sampleNegativeBox);

	//ͨ������Ļ���ͼ���������ǲ�����������������box��harr����  
	getFeatureValue(imageIntegral, samplePositiveBox, samplePositiveFeatureValue);
	getFeatureValue(imageIntegral, sampleNegativeBox, sampleNegativeFeatureValue);

	//ͨ������������������������������ǵķ�����  
	classifierUpdate(samplePositiveFeatureValue, muPositive, sigmaPositive, learnRate);
	classifierUpdate(sampleNegativeFeatureValue, muNegative, sigmaNegative, learnRate);
}