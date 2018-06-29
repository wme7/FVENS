/** @file agradientschemes.hpp
 * @brief Classes for different gradient estimation schemes.
 * @author Aditya Kashi
 * @date February 3, 2016
 */

#ifndef AGRADIENTSCHEMES_H
#define AGRADIENTSCHEMES_H 1

#include "mesh/amesh2dh.hpp"

namespace fvens
{

/// Abstract class for solution gradient computation schemes
/** For this, we need ghost cell-centered values of flow variables.
 */
template<short nvars>
class GradientScheme
{
protected:
	const UMesh2dh *const m;                             ///< Mesh context
	const amat::Array2d<a_real>& rc;                     ///< All cell-centres' coordinates

public:
	GradientScheme(const UMesh2dh *const mesh,        ///< Mesh context
	               const amat::Array2d<a_real>& _rc);        ///< Cell centers 
	
	virtual ~GradientScheme();

	/// Computes gradients corresponding to a state vector
	virtual void compute_gradients(
			const MVector& unk,                         ///< [in] Solution multi-vector
			const amat::Array2d<a_real>& unkg,          ///< [in] Ghost cell states 
			GradArray<nvars>& grads ) const = 0;
};

/// Simply sets the gradient to zero
template<short nvars>
class ZeroGradients : public GradientScheme<nvars>
{
public:
	ZeroGradients(const UMesh2dh *const mesh, 
	              const amat::Array2d<a_real>& _rc);

	void compute_gradients(const MVector& unk, 
	                       const amat::Array2d<a_real>& unkg, 
	                       GradArray<nvars>& grads ) const;

protected:
	using GradientScheme<nvars>::m;
	using GradientScheme<nvars>::rc;
};

/**
 * @brief Implements linear reconstruction using the Green-Gauss theorem over elements.
 * 
 * An inverse-distance weighted average is used to obtain the conserved variables at the faces.
 */
template<short nvars>
class GreenGaussGradients : public GradientScheme<nvars>
{
public:
	GreenGaussGradients(const UMesh2dh *const mesh, 
	                    const amat::Array2d<a_real>& _rc);

	void compute_gradients(const MVector& unk, 
	                       const amat::Array2d<a_real>& unkg,
	                       GradArray<nvars>& grads ) const;

protected:
	using GradientScheme<nvars>::m;
	using GradientScheme<nvars>::rc;
};

/// Class implementing linear weighted least-squares reconstruction
template<short nvars>
class WeightedLeastSquaresGradients : public GradientScheme<nvars>
{
public:
	WeightedLeastSquaresGradients(const UMesh2dh *const mesh, 
	                              const amat::Array2d<a_real>& _rc);

	void compute_gradients(const MVector& unk, 
	                       const amat::Array2d<a_real>& unkg, 
	                       GradArray<nvars>& grads ) const;

protected:
	using GradientScheme<nvars>::m;
	using GradientScheme<nvars>::rc;

private:
	/// The least squares LHS matrix
	DimMatrixArray V;
};


} // end namespace
#endif
