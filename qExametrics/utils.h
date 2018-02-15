#ifndef Q_EXAMETRICS_UTILS_H
#define Q_EXAMETRICS_UTILS_H

#include <math.h>
#include <CCGeom.h> // ccvector3
#include <QString>

class Utils
{
    public:

        static bool double_equals(double a, double b, double epsilon = 0.01);

        static double frand_a_b(double a, double b);

        static QString ccVector3ToString(CCVector3 v);
};


#endif
