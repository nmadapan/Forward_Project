#ifndef __BASIC_H__
#define __BASIC_H__


#include <vector>
#include <iostream>

#define INCH_per_M 39.37 // 1 meter = 39.37 inches
#define M_per_INCH 0.0254  // 1 inch = 0.0254 meter
#define INCHES_PER_MICRON 3.937e-5  // 1 micron = 3.937*10^-5 inches
#define RAD_PER_MICRORAD 10e-6	// 1 micro radian = 10^-6 radian
#define DEG_PER_RAD 57.2958	// 1 radian = 57.2958 degrees
#define MEGA 1000000	// 1 mega = 10^6


inline void print_star()
{
	printf("**************************************************************\n");
}

inline void print_line()
{
	printf("--------------------------------------------------------------\n");
}

inline double my_max(double a, double b)
{
	return (a>=b?a:b);
}

inline double my_max(double a, double b, double c)
{
	double temp;
	temp = a>=b?a:b;
	return (temp>=c?temp:c);
}

class Point3D {
public:
	double x;
	double y;
	double z;
	bool init;

	Point3D():init(false){}

	bool empty()
	{
		return init;
	}

	void set_point(double px, double py, double pz)
	{
		x = px;
		y = py;
		z = pz;
		init = true;
	}

	void set_point (const Point3D _p)
	{
		x = _p.x;
		y = _p.y;
		z = _p.z;
		init = true;
	}

	Point3D operator=(const Point3D _p)
	{
		x = _p.x;
		y = _p.y;
		z = _p.z;
		init = true;
		return *this;
	}

	Point3D operator+=(const Point3D _p)
	{
		x += _p.x;
		y += _p.y;
		z += _p.z;
		init = true;
		return *this;
	}

	Point3D operator-(const Point3D _p)
	{
		Point3D result;
		result.x = x - _p.x;
		result.y = y - _p.y;
		result.z = z - _p.z;
		return result;
	}

	Point3D operator+(const Point3D _p)
	{
		Point3D result;
		result.x = x + _p.x;
		result.y = y + _p.y;
		result.z = z + _p.z;
		return result;
	}

	Point3D operator/ (const double _p)
	{
		Point3D result; 
		result.x = x / _p;
		result.y = y / _p;
		result.z = z / _p;
		return result;
	}

	Point3D operator/(const int _p)
	{
		Point3D result; 
		result.x = x / _p;
		result.y = y / _p;
		result.z = z / _p;
		return result;
	}


	Point3D operator*(const double _p)
	{
		Point3D result; 
		result.x = x * _p;
		result.y = y * _p;
		result.z = z * _p;
		return result;
	}

	friend std::ostream& operator<<(std::ostream& os, const Point3D& _p);
};

inline std::ostream& operator<<(std::ostream& os, const Point3D& _p)
{
    os <<"( "<<_p.x << ", " << _p.y << " ," << _p.z <<" )";
    return os;
}

class tAngleArray{
public:
	double x_angle;
	double y_angle;
	double z_angle;
};
class tLengthArray{
public:
	double x_length;
	double y_length;
	double z_length;
};

class LPF{
private:
	Point3D sum;
	int length;
	int counter;
	bool done_flag;
public:
	LPF()
	{
		length = 10;
		sum.set_point(0,0,0);
		counter = 0;
		done_flag = false;
	}
	void add_point(const Point3D& _p)
	{
		sum += _p;
		counter++;
		if (counter == length)
		{
			counter = 0;
			done_flag = true;
		}
	}

	bool check_done()
	{
		return done_flag;
	}
	void set_length(int _p){
		length = _p;
	}

	Point3D get_value(){
		if (done_flag)
		{
			Point3D result = sum/length;
			sum.set_point(0,0,0);
			done_flag = false;
			return result;
		}
			
	}



};

#endif