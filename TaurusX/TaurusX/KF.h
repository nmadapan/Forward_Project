// (C) Mithun Jacob
#ifndef __KF_H__
#define __KF_H__

#include <opencv2/core/core.hpp>
using namespace cv;
#include <vector>
#include "Basic.h"
using namespace std;

// Kalman filter
class KF
{
	public:	
	Mat F,H,Q,R,Pk_1_k_1,xk_1_k_1;
	bool init, ready;
	KF()
	{
		init = ready = false;
	}

	// Deep copy-constructor
	KF(const KF & kal):init(kal.init),ready(kal.ready)
	{
		kal.F.copyTo(F);
		kal.H.copyTo(H);
		kal.Q.copyTo(Q);
		kal.R.copyTo(R);
		kal.Pk_1_k_1.copyTo(Pk_1_k_1);
		kal.xk_1_k_1.copyTo(xk_1_k_1);		
	}

	bool fully_initialized()
	{
		return ready;
	}

	void computePost(Mat & zk)
	{
		const int n = xk_1_k_1.rows;
		// Predict
		Mat xk_k_1 = F*xk_1_k_1;
		Mat Pk_k_1 = F*Pk_1_k_1*F.t() + Q;

		// Update
		Mat yk = zk - H*xk_k_1;
		Mat S = H*Pk_k_1*H.t() + R;
		Mat Sinv;
		invert(S,Sinv);
		Mat K = Pk_k_1*H.t()*Sinv;
		Mat xkk = xk_k_1 + K*yk;
		Mat Pkk = (Mat::eye(n,n,CV_64F) - K*H)*Pk_k_1;

		// Copy over
		Pkk.copyTo(Pk_1_k_1);
		xkk.copyTo(xk_1_k_1);
	}
};

// Kalman filter
class KalmanFinger : public KF
{
public:
    Point3D prev;
    KalmanFinger()
    {
        // Initialize
        F = (Mat_<double>(6,6) <<  1,0,0,1,0,0,0,1,0,0,1,0,0,0,1,0,0,1,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,1);
        H = Mat::eye(6,6,CV_64F);
        Mat q = (Mat_<double>(6,1) << 1e-5,1e-5,1e-5,1e-4,1e-4,1e-4);
        Q = Mat::diag(q);
        R = Mat::eye(6,6,CV_64F);
        Pk_1_k_1 = Mat::eye(6,6,CV_64F);
        xk_1_k_1 = Mat::zeros(6,1,CV_64F);
    }
    KalmanFinger(const KalmanFinger & kal):KF(kal)
    {
        prev = kal.prev;
    }

    Point3D get_predict()
    {
        Point3D p;
        Mat xk_k_1 = F*xk_1_k_1;
        p.x = static_cast<float>(xk_k_1.at<double>(0,0));
        p.y = static_cast<float>(xk_k_1.at<double>(1,0));
        p.z = static_cast<float>(xk_k_1.at<double>(2,0));
        return p;
    }
    
    Point3D get_post()
    {
        Point3D p;
        Mat xk_k_1 = F*xk_1_k_1;
		p.x = static_cast<float>(xk_1_k_1.at<double>(0,0));
        p.y = static_cast<float>(xk_1_k_1.at<double>(1,0));
        p.z = static_cast<float>(xk_1_k_1.at<double>(2,0));
        return p;
    }
    
    void Point2State(const Point3D & p, Mat & x)
    {
        if(!prev.empty())
        {
            x.at<double>(0,0) = static_cast<double>(p.x);
            x.at<double>(1,0) = static_cast<double>(p.x);
            x.at<double>(2,0) = static_cast<double>(p.x);
            x.at<double>(3,0) = static_cast<double>(p.x-prev.x);
            x.at<double>(4,0) = static_cast<double>(p.x-prev.y);
            x.at<double>(5,0) = static_cast<double>(p.x-prev.z);
        }
    }
  
    void addPoint(const Point3D & p)
    {    
        if(prev.empty()) prev =p;
        else if(!init)
        {
            // Load point
            Point2State(p,xk_1_k_1);
            prev = p;
            init = true;
        }
        else
        {
            // Load point
            Mat zk(6,1,CV_64F);
            Point2State(p,zk); 
            computePost(zk);

            // Store for next iteration
            prev = p;

            // Ready
            ready = true;
        }
    }
};

// use 
//kal.addPoint(raw);
//if(kal.fully_initialized()) 
//smooth = kal.get_post();

#endif

