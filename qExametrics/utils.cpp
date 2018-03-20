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
QString Utils::ccVector3ToString(const CCVector3 v)
{
	return QString("(" + QString::number(v.x) 
				  + ", " + QString::number(v.y)
				  + ", " + QString::number(v.z)
				  + ")");
}

/* Return a QString representing three numbers as "(x,y,z)" */
QString Utils::ccVector3ToString(const CCVector3d v)
{
	return Utils::ccVector3ToString(Utils::ccVectorDoubleToFloat(v));
}

/* Return a QString representing three numbers as "(x,y,z)" */
QString Utils::ccVector3ToString(const CCVector3* v)
{
	return QString("(" + QString::number(v->x) 
				  + ", " + QString::number(v->y)
				  + ", " + QString::number(v->z)
				  + ")");
}

/* Return a float Vector3 from a double Vector3 */
CCVector3 Utils::ccVectorDoubleToFloat(CCVector3d v)
{
	return CCVector3(v.x, v.y, v.z);
}

/* Return a float Vector3 from a double Vector3 */
CCVector3 Utils::ccVectorDoublePToFloat(CCVector3d* v)
{
	return CCVector3(v->x, v->y, v->z);
}