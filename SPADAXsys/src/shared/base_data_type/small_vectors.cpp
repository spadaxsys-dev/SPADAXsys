#include "small_vectors.h"

namespace SPH {
	//===========================================================//
	/** 2x2 matrix,
	  * It is only works for kernel corection not reverse other matrices */
	Mat2d GeneralizedInverse(Mat2d &A)
	{
		// Find U such that U*A*A�*U� = diag
		Mat2d Su = A * ~A;
		SimTK::Real phi = 0.5*atan2(Su(0, 1) + Su(1, 0), Su(0, 0) - Su(1, 1));
		SimTK::Real Cphi = cos(phi);
		SimTK::Real Sphi = sin(phi);
		Mat2d U(Cphi, -Sphi, Sphi, Cphi);

		// Find W such that W�*A�*A*W = diag
		Mat2d Sw = ~A*A;
		SimTK::Real theta = 0.5*atan2(Sw(0, 1) + Sw(1, 0), Sw(0, 0) - Sw(1, 1));
		SimTK::Real Ctheta = cos(theta);
		SimTK::Real Stheta = sin(theta);
		Mat2d W(Ctheta, -Stheta, Stheta, Ctheta);

		// Find the singular values from U
		SimTK::Real SUsum = Su(0, 0) + Su(1, 1);
		SimTK::Real SUdif = sqrt((Su(0, 0) - Su(1, 1))*(Su(0, 0) - Su(1, 1)) + 4.0 * Su(0, 1)*Su(1, 0));
		Vec2d svals(sqrt((SUsum + SUdif) / 2), sqrt((SUsum - SUdif) / 2));
		Mat2d SIG(svals(0), 0.0, 0.0, svals(1));

		// Find the correction matrix for the right side
		Mat2d S = ~U*A*W;
		Mat2d C(SimTK::sign(S(0, 0)), 0.0, 0.0, SimTK::sign(S(1, 1)));
		Mat2d V = W * C;

		//regularization
		SimTK::Real strength = 0.1;
		Mat2d r_SIG(svals(0) / (svals(0)*svals(0) + strength *(svals(0) - 1.0)*(svals(0) - 1.0)),
			0.0, 0.0,
			svals(1) / (svals(1)*svals(1) + strength *(svals(1) - 1.0)*(svals(1) - 1.0)));

		return V * r_SIG*~U;
	}
	/** 3x3 matrix,
	  * It is only works for kernel corection not reverse other matrices */
	  //===========================================================//
	Mat3d GeneralizedInverse(Mat3d &A)
	{
		SimTK::Real det = A(0, 0) * (A(1, 1) * A(2, 2) - A(2, 1) * A(1, 2)) -
             A(0, 1) * (A(1, 0) * A(2, 2) - A(1, 2) * A(2, 0)) +
             A(0, 2) * (A(1, 0) * A(2, 1) - A(1, 1) * A(2, 0));

        SimTK::Real invdet = 1 / det;
        Mat3d minv; // inverse of matrix m
		minv(0, 0) = (A(1, 1) * A(2, 2) - A(2, 1) * A(1, 2)) * invdet;
		minv(0, 1) = (A(0, 2) * A(2, 1) - A(0, 1) * A(2, 2)) * invdet;
		minv(0, 2) = (A(0, 1) * A(1, 2) - A(0, 2) * A(1, 1)) * invdet;
		minv(1, 0) = (A(1, 2) * A(2, 0) - A(1, 0) * A(2, 2)) * invdet;
		minv(1, 1) = (A(0, 0) * A(2, 2) - A(0, 2) * A(2, 0)) * invdet;
		minv(1, 2) = (A(1, 0) * A(0, 2) - A(0, 0) * A(1, 2)) * invdet;
		minv(2, 0) = (A(1, 0) * A(2, 1) - A(2, 0) * A(1, 1)) * invdet;
		minv(2, 1) = (A(2, 0) * A(0, 1) - A(0, 0) * A(2, 1)) * invdet;
		minv(2, 2) = (A(0, 0) * A(1, 1) - A(1, 0) * A(0, 1)) * invdet;

		return minv;
	}
	//===========================================================//
	SimTK::Real TensorDoubleDotProduct(Mat2d &A, Mat2d &B)
	{
		return  (A*~B).trace();
	}
	//===========================================================//
	SimTK::Real TensorDoubleDotProduct(Mat3d &A, Mat3d &B)
	{
		return  (A*~B).trace();
	}
	//===========================================================//
}
