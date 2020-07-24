#include"kinect2_grabber.h"
#include <kinect.h>
#include<pcl\io\pcd_io.h>
#include <pcl/filters/conditional_removal.h>
#include <pcl/visualization/cloud_viewer.h> 
#include <pcl/visualization/pcl_visualizer.h>
typedef pcl::PointXYZRGBA PointType;
int main(void)
{
	int c = 0;
	int total = 0;//�ļ���
	//pcl::PointCloud<PointType> cloud_filtered;// ����Χ���˺�ĵ���
	boost::shared_ptr<pcl::visualization::PCLVisualizer> m_viewer;
	//��ʾ���ڳ�ʼ��
	m_viewer.reset(new pcl::visualization::PCLVisualizer("viewer", false));
	m_viewer->setBackgroundColor(0, 0, 0);//���ñ�����ɫ
	m_viewer->initCameraParameters();
	//m_viewer->addCoordinateSystem();//��Ӻ�����������
	m_viewer->createInteractor();
	// ��ȡKinect�豸	
	// Point Cloud
	pcl::PointCloud<PointType>::ConstPtr pointCloud_XYZRGBA;  //ָ�����
	// Retrieved Point Cloud Callback Function ��⵽�ĵ��ƻص�����
	//boost::mutex mutex; //������̻�����
	//boost::function<void(const pcl::PointCloud<PointType>::ConstPtr&)> function = [&pointCloud_XYZRGBA, &mutex](const pcl::PointCloud<PointType>::ConstPtr& ptr)
	//{ //ֻ����ConstPtr��������Ptr
	//	boost::mutex::scoped_lock lock(mutex);
	//	/* Point Cloud Processing */
	//	pointCloud_XYZRGBA = ptr->makeShared();
		//ptr->makeShared() = NULL;
	//};
	// Retrieved Point Cloud Callback Function ��⵽�ĵ��ƻص�����
	//It should be written in callback function that retrieve new point cloud.
	boost::mutex mutex; //������̻�����
	boost::function<void(const pcl::PointCloud<PointType>::ConstPtr&)> pointcloud_function =  [&pointCloud_XYZRGBA, &mutex](const pcl::PointCloud<PointType>::ConstPtr& ptr)
	{
	    //Save Point Cloud 
	    boost::mutex::scoped_lock lock(mutex);
		pointCloud_XYZRGBA = ptr->makeShared();
	};
	boost::shared_ptr<pcl::Grabber> grabberForKinect = boost::shared_ptr<pcl::Grabber>(new pcl::Kinect2Grabber);// Kinect2Grabber
	boost::signals2::connection connection = grabberForKinect->registerCallback(pointcloud_function); // Register Callback Function
	grabberForKinect->start();  //Start Grabber
	while (!m_viewer->wasStopped())
	{
		// Update Viewer
		c++;
		m_viewer->spinOnce(1); //���ý������򲢸�����Ļһ�Ρ�
		boost::mutex::scoped_try_lock lock(mutex);
		//cloud_filtered = filterByScope(pointCloud_XYZRGBA, Z_nearby, Z_faraway, X_left, X_right, Y_bottom, Y_top);//��ȡ��Ӧ��Χͼ��
		if (lock.owns_lock() && pointCloud_XYZRGBA)
		{
			//cloud_filtered = filterByScope(pointCloud_XYZRGBA, Z_nearby, Z_faraway, X_left, X_right, Y_bottom, Y_top);
			//5 ��ʾ Point Cloud
			if (!m_viewer->updatePointCloud(pointCloud_XYZRGBA->makeShared(), "cloud"))
			{//pointCloud_XYZRGBA��ָ�����ͣ�������( . )�����
				m_viewer->addPointCloud(pointCloud_XYZRGBA->makeShared(), "cloud");
			}
			if (c % 5 == 0)
			{//ÿ��һ��ʱ�䱣��һ��ͼƬ
				//�洢ͼƬ��
				char name[100];
				sprintf(name, "E:\\Kinect V2.0\\Kinect2.0-pcd\\final\\contradiction\\kinect_2\\%d.pcd", total++);
				pcl::io::savePCDFileASCII(name, *pointCloud_XYZRGBA);//��.pcd�ĸ�ʽ����������ݵ�����
			}
		}
	}//while
}