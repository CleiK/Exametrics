#ifndef Q_EXAMETRICS_UTILS_H
#define Q_EXAMETRICS_UTILS_H

#include <math.h>
#include <CCGeom.h> // ccvector3
#include <QString>
#include <QList>
#include <QPair>

class Utils
{
    public:

        static bool double_equals(double a, double b, double epsilon = 0.01);

        /* Return a random number between a and b */
        static double frand_a_b(double a, double b);

        /* Return a QString representing three numbers as "(x,y,z)" */
        static QString ccVector3ToString(const CCVector3 v);
        /* Return a QString representing three numbers as "(x,y,z)" */
        static QString ccVector3ToString(const CCVector3d v);
        /* Return a QString representing three numbers as "(x,y,z)" */
        static QString ccVector3ToString(const CCVector3* v);

        /* Return a float Vector3 from a double Vector3 */
        static CCVector3 ccVectorDoubleToFloat(CCVector3d v);
        /* Return a float Vector3 from a double Vector3 */
        static CCVector3 ccVectorDoublePToFloat(CCVector3d* v);

        
        struct QPairSecondComparer
        {
            template<typename T1, typename T2>
            bool operator()(const QPair<T1,T2> & a, const QPair<T1,T2> & b) const
            {
                return a.second < b.second;
            }
        };
};


#endif
