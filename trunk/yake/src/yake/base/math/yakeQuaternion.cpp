/**------------------------------------------------------------------------------------
   This file is part of YAKE
   Copyright © 2004 The YAKE Team
   For the latest information visit http://www.yake.org 
   ------------------------------------------------------------------------------------
   This program is free software; you can redistribute it and/or modify it under
   the terms of the GNU Lesser General Public License as published by the Free Software
   Foundation; either version 2 of the License, or (at your option) any later
   version.

   This program is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
   FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License along with
   this program; if not, write to the Free Software Foundation, Inc., 59 Temple
   Place - Suite 330, Boston, MA 02111-1307, USA, or go to
   http://www.gnu.org/copyleft/lesser.txt.
   ------------------------------------------------------------------------------------
*/
//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
#include <yake/base/yakePCH.h>
#include <yake/base/math/yakeQuaternion.h>
#include <yake/base/math/yakeMatrix3.h>


//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake
{
namespace base
{
namespace math
{

    const real Quaternion::ms_fEpsilon = (real)1e-03;
    const Quaternion Quaternion::kZero(0.0,0.0,0.0,0.0);
    const Quaternion Quaternion::kIdentity(1.0,0.0,0.0,0.0);

    //-----------------------------------------------------------------------
    void Quaternion::FromRotationMatrix (const Matrix3& kRot)
    {
        // Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
        // article "Quaternion Calculus and Fast Animation".

        real fTrace = kRot[0][0]+kRot[1][1]+kRot[2][2];
        real fRoot;

        if ( fTrace > 0.0 )
        {
            // |w| > 1/2, may as well choose w > 1/2
            fRoot = (real) Math::Sqrt(fTrace + (real)1.0);  // 2w
            w = (real) 0.5*fRoot;
            fRoot = (real) 0.5/fRoot;  // 1/(4w)
            x = (kRot[2][1]-kRot[1][2])*fRoot;
            y = (kRot[0][2]-kRot[2][0])*fRoot;
            z = (kRot[1][0]-kRot[0][1])*fRoot;
        }
        else
        {
            // |w| <= 1/2
            static int s_iNext[3] = { 1, 2, 0 };
            int i = 0;
            if ( kRot[1][1] > kRot[0][0] )
                i = 1;
            if ( kRot[2][2] > kRot[i][i] )
                i = 2;
            int j = s_iNext[i];
            int k = s_iNext[j];

            fRoot = (real) Math::Sqrt(kRot[i][i]-kRot[j][j]-kRot[k][k] + (real)1.0);
            real* apkQuat[3] = { &x, &y, &z };
            *apkQuat[i] = (real) 0.5*fRoot;
            fRoot = (real) 0.5/fRoot;
            w = (kRot[k][j]-kRot[j][k])*fRoot;
            *apkQuat[j] = (kRot[j][i]+kRot[i][j])*fRoot;
            *apkQuat[k] = (kRot[k][i]+kRot[i][k])*fRoot;
        }
    }
/*
	//-----------------------------------------------------------------------
    void Quaternion::ToRotationMatrix (Matrix3& kRot) const
    {
        real fTx  = 2.0*x;
        real fTy  = 2.0*y;
        real fTz  = 2.0*z;
        real fTwx = fTx*w;
        real fTwy = fTy*w;
        real fTwz = fTz*w;
        real fTxx = fTx*x;
        real fTxy = fTy*x;
        real fTxz = fTz*x;
        real fTyy = fTy*y;
        real fTyz = fTz*y;
        real fTzz = fTz*z;

        kRot[0][0] = 1.0-(fTyy+fTzz);
        kRot[0][1] = fTxy-fTwz;
        kRot[0][2] = fTxz+fTwy;
        kRot[1][0] = fTxy+fTwz;
        kRot[1][1] = 1.0-(fTxx+fTzz);
        kRot[1][2] = fTyz-fTwx;
        kRot[2][0] = fTxz-fTwy;
        kRot[2][1] = fTyz+fTwx;
        kRot[2][2] = 1.0-(fTxx+fTyy);
    }*/

	//-----------------------------------------------------------------------
    void Quaternion::FromAngleAxis (const real& rfAngle,
        const Vector3& rkAxis)
    {
        // assert:  axis[] is unit length
        //
        // The quaternion representing the rotation is
        //   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

        real fHalfAngle = (real) 0.5*rfAngle;
        real fSin = Math::Sin(fHalfAngle);
        w = Math::Cos(fHalfAngle);
        x = fSin*rkAxis.x;
        y = fSin*rkAxis.y;
        z = fSin*rkAxis.z;
    }
    //-----------------------------------------------------------------------
    void Quaternion::ToAngleAxis (real& rfAngle, Vector3& rkAxis) const
    {
        // The quaternion representing the rotation is
        //   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

        real fSqrLength = x*x+y*y+z*z;
        if ( fSqrLength > 0.0 )
        {
            rfAngle = (real) 2.0*Math::ACos(w);
            real fInvLength = Math::InvSqrt(fSqrLength);
            rkAxis.x = x*fInvLength;
            rkAxis.y = y*fInvLength;
            rkAxis.z = z*fInvLength;
        }
        else
        {
            // angle is 0 (mod 2*pi), so any axis will do
            rfAngle = 0.0;
            rkAxis.x = 1.0;
            rkAxis.y = 0.0;
            rkAxis.z = 0.0;
        }
    }
    //-----------------------------------------------------------------------
    void Quaternion::FromAxes (const Vector3* akAxis)
    {
        Matrix3 kRot;

        for (int iCol = 0; iCol < 3; iCol++)
        {
            kRot[0][iCol] = akAxis[iCol].x;
            kRot[1][iCol] = akAxis[iCol].y;
            kRot[2][iCol] = akAxis[iCol].z;
        }

        FromRotationMatrix(kRot);
    }
    //-----------------------------------------------------------------------
    void Quaternion::FromAxes (const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis)
    {
        Matrix3 kRot;

        kRot[0][0] = xAxis.x;
        kRot[1][0] = xAxis.y;
        kRot[2][0] = xAxis.z;

        kRot[0][1] = yAxis.x;
        kRot[1][1] = yAxis.y;
        kRot[2][1] = yAxis.z;

        kRot[0][2] = zAxis.x;
        kRot[1][2] = zAxis.y;
        kRot[2][2] = zAxis.z;

        FromRotationMatrix(kRot);

    }
	/*
    //-----------------------------------------------------------------------
    void Quaternion::ToAxes (Vector3* akAxis) const
    {
        Matrix3 kRot;

        ToRotationMatrix(kRot);

        for (int iCol = 0; iCol < 3; iCol++)
        {
            akAxis[iCol].x = kRot[0][iCol];
            akAxis[iCol].y = kRot[1][iCol];
            akAxis[iCol].z = kRot[2][iCol];
        }
    }
    //-----------------------------------------------------------------------
    void Quaternion::ToAxes (Vector3& xAxis, Vector3& yAxis, Vector3& zAxis) const
    {
        Matrix3 kRot;

        ToRotationMatrix(kRot);

        xAxis.x = kRot[0][0];
        xAxis.y = kRot[1][0];
        xAxis.z = kRot[2][0];

        yAxis.x = kRot[0][1];
        yAxis.y = kRot[1][1];
        yAxis.z = kRot[2][1];

        zAxis.x = kRot[0][2];
        zAxis.y = kRot[1][2];
        zAxis.z = kRot[2][2];
    }
	*/

    //-----------------------------------------------------------------------
    Quaternion Quaternion::operator+ (const Quaternion& rkQ) const
    {
        return Quaternion(w+rkQ.w,x+rkQ.x,y+rkQ.y,z+rkQ.z);
    }
    //-----------------------------------------------------------------------
    Quaternion Quaternion::operator- (const Quaternion& rkQ) const
    {
        return Quaternion(w-rkQ.w,x-rkQ.x,y-rkQ.y,z-rkQ.z);
    }
    //-----------------------------------------------------------------------
    Quaternion Quaternion::operator* (const Quaternion& rkQ) const
    {
        // NOTE:  Multiplication is not generally commutative, so in most
        // cases p*q != q*p.

        return Quaternion
        (
            w * rkQ.w - x * rkQ.x - y * rkQ.y - z * rkQ.z,
            w * rkQ.x + x * rkQ.w + y * rkQ.z - z * rkQ.y,
            w * rkQ.y + y * rkQ.w + z * rkQ.x - x * rkQ.z,
            w * rkQ.z + z * rkQ.w + x * rkQ.y - y * rkQ.x
        );
    }
    //-----------------------------------------------------------------------
    Quaternion Quaternion::operator* (real fScalar) const
    {
        return Quaternion(fScalar*w,fScalar*x,fScalar*y,fScalar*z);
    }
    //-----------------------------------------------------------------------
    Quaternion operator* (real fScalar, const Quaternion& rkQ)
    {
        return Quaternion(fScalar*rkQ.w,fScalar*rkQ.x,fScalar*rkQ.y,
            fScalar*rkQ.z);
    }
    //-----------------------------------------------------------------------
    Quaternion Quaternion::operator- () const
    {
        return Quaternion(-w,-x,-y,-z);
    }
    //-----------------------------------------------------------------------
    real Quaternion::Dot (const Quaternion& rkQ) const
    {
        return w*rkQ.w+x*rkQ.x+y*rkQ.y+z*rkQ.z;
    }
    //-----------------------------------------------------------------------
    real Quaternion::Norm () const
    {
        return w*w+x*x+y*y+z*z;
    }
    //-----------------------------------------------------------------------
    Quaternion Quaternion::Inverse () const
    {
        real fNorm = w*w+x*x+y*y+z*z;
        if ( fNorm > 0.0 )
        {
            real fInvNorm = (real) 1.0/fNorm;
            return Quaternion(w*fInvNorm,-x*fInvNorm,-y*fInvNorm,-z*fInvNorm);
        }
        else
        {
            // return an invalid result to flag the error
            return kZero;
        }
    }
    //-----------------------------------------------------------------------
    Quaternion Quaternion::UnitInverse () const
    {
        // assert:  'this' is unit length
        return Quaternion(w,-x,-y,-z);
    }
    //-----------------------------------------------------------------------
	/*
    Quaternion Quaternion::Exp () const
    {
        // If q = A*(x*i+y*j+z*k) where (x,y,z) is unit length, then
        // exp(q) = cos(A)+sin(A)*(x*i+y*j+z*k).  If sin(A) is near zero,
        // use exp(q) = cos(A)+A*(x*i+y*j+z*k) since A/sin(A) has limit 1.

        real fAngle = Math::Sqrt(x*x+y*y+z*z);
        real fSin = Math::Sin(fAngle);

        Quaternion kResult;
        kResult.w = Math::Cos(fAngle);

        if ( Math::Abs(fSin) >= ms_fEpsilon )
        {
            real fCoeff = fSin/fAngle;
            kResult.x = fCoeff*x;
            kResult.y = fCoeff*y;
            kResult.z = fCoeff*z;
        }
        else
        {
            kResult.x = x;
            kResult.y = y;
            kResult.z = z;
        }

        return kResult;
    }
    //-----------------------------------------------------------------------
    Quaternion Quaternion::Log () const
    {
        // If q = cos(A)+sin(A)*(x*i+y*j+z*k) where (x,y,z) is unit length, then
        // log(q) = A*(x*i+y*j+z*k).  If sin(A) is near zero, use log(q) =
        // sin(A)*(x*i+y*j+z*k) since sin(A)/A has limit 1.

        Quaternion kResult;
        kResult.w = 0.0;

        if ( Math::Abs(w) < 1.0 )
        {
            real fAngle = Math::ACos(w);
            real fSin = Math::Sin(fAngle);
            if ( Math::Abs(fSin) >= ms_fEpsilon )
            {
                real fCoeff = fAngle/fSin;
                kResult.x = fCoeff*x;
                kResult.y = fCoeff*y;
                kResult.z = fCoeff*z;
                return kResult;
            }
        }

        kResult.x = x;
        kResult.y = y;
        kResult.z = z;

        return kResult;
    }
	*/
    //-----------------------------------------------------------------------
    Vector3 Quaternion::operator* (const Vector3& v) const
    {
		// nVidia SDK implementation
		Vector3 uv, uuv; 
		Vector3 qvec(x, y, z);
		uv = qvec.crossProduct(v); 
		uuv = qvec.crossProduct(uv); 
		uv *= (2.0f * w); 
		uuv *= 2.0f; 
		
		return v + uv + uuv;

    }
    //-----------------------------------------------------------------------
	/*
    Quaternion Quaternion::Slerp (real fT, const Quaternion& rkP,
        const Quaternion& rkQ, bool shortestPath)
    {
        real fCos = rkP.Dot(rkQ);
        real fAngle = Math::ACos(fCos);

        if ( Math::Abs(fAngle) < ms_fEpsilon )
            return rkP;

        real fSin = Math::Sin(fAngle);
        real fInvSin = 1.0/fSin;
        real fCoeff0 = Math::Sin((1.0-fT)*fAngle)*fInvSin;
        // Do we need to invert rotation?
        if (fCos < 0.0f && shortestPath)
        {
            fCoeff0 = -fCoeff0;
        }
        real fCoeff1 = Math::Sin(fT*fAngle)*fInvSin;
        return fCoeff0*rkP + fCoeff1*rkQ;
    }
    //-----------------------------------------------------------------------
    Quaternion Quaternion::SlerpExtraSpins (real fT,
        const Quaternion& rkP, const Quaternion& rkQ, int iExtraSpins)
    {
        real fCos = rkP.Dot(rkQ);
        real fAngle = Math::ACos(fCos);

        if ( Math::Abs(fAngle) < ms_fEpsilon )
            return rkP;

        real fSin = Math::Sin(fAngle);
        real fPhase = Math::PI*iExtraSpins*fT;
        real fInvSin = 1.0/fSin;
        real fCoeff0 = Math::Sin((1.0-fT)*fAngle - fPhase)*fInvSin;
        real fCoeff1 = Math::Sin(fT*fAngle + fPhase)*fInvSin;
        return fCoeff0*rkP + fCoeff1*rkQ;
    }
    //-----------------------------------------------------------------------
    void Quaternion::Intermediate (const Quaternion& rkQ0,
        const Quaternion& rkQ1, const Quaternion& rkQ2,
        Quaternion& rkA, Quaternion& rkB)
    {
        // assert:  q0, q1, q2 are unit quaternions

        Quaternion kQ0inv = rkQ0.UnitInverse();
        Quaternion kQ1inv = rkQ1.UnitInverse();
        Quaternion rkP0 = kQ0inv*rkQ1;
        Quaternion rkP1 = kQ1inv*rkQ2;
        Quaternion kArg = 0.25*(rkP0.Log()-rkP1.Log());
        Quaternion kMinusArg = -kArg;

        rkA = rkQ1*kArg.Exp();
        rkB = rkQ1*kMinusArg.Exp();
    }
    //-----------------------------------------------------------------------
    Quaternion Quaternion::Squad (real fT,
        const Quaternion& rkP, const Quaternion& rkA,
        const Quaternion& rkB, const Quaternion& rkQ, bool shortestPath)
    {
        real fSlerpT = 2.0*fT*(1.0-fT);
        Quaternion kSlerpP = Slerp(fT, rkP, rkQ, shortestPath);
        Quaternion kSlerpQ = Slerp(fT, rkA, rkB);
        return Slerp(fSlerpT, kSlerpP ,kSlerpQ);
    }
	*/
    //-----------------------------------------------------------------------
    bool Quaternion::operator== (const Quaternion& rhs) const
    {
        return (rhs.x == x) && (rhs.y == y) &&
            (rhs.z == z) && (rhs.w == w);
    }
    //-----------------------------------------------------------------------
    bool Quaternion::operator!= (const Quaternion& rhs) const
    {
        return (rhs.x != x) || (rhs.y != y) ||
            (rhs.z != z) || (rhs.w != w);
    }


} // math
} // base
} // yake