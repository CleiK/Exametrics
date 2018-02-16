 #include "utils.h"

/* Return true if a is equal to b with a epsilon precision */
bool Utils::double_equals(double a, double b, double epsilon)
{
    return std::abs(a - b) < epsilon;
}

/* Return a random number between a and b */
double Utils::frand_a_b(double a, double b)
{
    return ( rand()/(double)RAND_MAX ) * (b-a) + a;
}

/* Return a QString representing three numbers as "(x,y,z)" */
QString Utils::ccVector3ToString(CCVector3 v)
{
	return QString("(" + QString::number(v.x) 
				  + ", " + QString::number(v.y)
				  + ", " + QString::number(v.z)
				  + ")");
}

QString Utils::ccVector3ToString(CCVector3d v)
{
	return Utils::ccVector3ToString(Utils::ccVectorDoubleToFloat(v));
}

CCVector3 Utils::ccVectorDoubleToFloat(CCVector3d v)
{
	return CCVector3(v.x, v.y, v.z);
}