/**
 * \file convolution.h
 */

#ifndef HRTFCONVOLVER_H_
#define HRTFCONVOLVER_H_

#include <cstring>
#include <memory>
#include <stk/Stk.h>

// AVRS headers
#include "common.hpp"

//#define KERNEL_LENGTH 		183
//#define N_FFT 				694 // for output of 512 samples
//#define IN_SEGMENT_LENGTH 	(N_FFT - KERNEL_LENGTH + 1)
//#define OUT_SEGMENT_LENGTH 	IN_SEGMENT_LENGTH

#define KERNEL_LENGTH 		183  // in time
#define N_FFT 				694 // for output of 512 samples
#define SEGMENT_LENGTH 		(N_FFT - KERNEL_LENGTH + 1)

namespace avrs
{

// Convolution by FFT method
class HrtfConvolver
{
public:
	typedef std::auto_ptr<HrtfConvolver> ptr_t;

	virtual ~HrtfConvolver();
	/// Static factory function for HrtfConvolver objects
	static ptr_t create(uint nFft);

	void setKernelLength(uint n);
	void setSegmentLength(uint n);
	void setTKernel(float *tKernel, uint n);
	void setSKernel(complex_t *sKernel, uint n);
	void setTSegment(float *tSegment, uint n);
	uint getConvolutionLength();
	void convolve(float *out);
	void flush(float *out);

private:
	HrtfConvolver(uint nFft);

	uint nFft_;  // number of samples of FFT
	complex_t *sSegment_;
	complex_t *sKernel_;
	uint nSpectrum_;
	fftwf_plan planFwdSegment_;
	fftwf_plan planBwdSegment_;
	fftwf_plan planFwdKernel_;
	bool isSKernelSet_;  // flag for avoid unnecessary calculus of FFT

	float *tSegment_;  // input segment to convolve
	uint nTSegment_;  // input number of samples
	float *tKernel_;  // filter kernel
	uint nTKernel_;  // filter kernel number of samples
	float *olap_; // holds the overlapping samples from segment to segment
	uint nOlap_;
	float *zeros_;  // hold zeros values (for pad with memcpy)
};

inline void HrtfConvolver::setKernelLength(uint n)
{
	assert(n != 0);

	nTKernel_ = n;
	nOlap_ = nTKernel_ - 1;
	olap_ = (float *) fftwf_malloc(sizeof(float) * nOlap_);

	for (uint i = 0; i < nOlap_; i++)
		olap_[i] = 0.0;
}

inline void HrtfConvolver::setSegmentLength(uint n)
{
	assert(n != 0);

	nTSegment_ = n;
	zeros_ = (float *) fftwf_malloc(sizeof(float) * (nFft_ - nTSegment_));

	for (uint i = 0; i < (nFft_ - nTSegment_); i++)
		zeros_[i] = 0.0;
}

inline void HrtfConvolver::setTKernel(float *tKernel, uint n)
{
	assert(n == nTKernel_);
	memcpy(tKernel_, tKernel, sizeof(float) * nTKernel_);
	isSKernelSet_ = false;  // must be do FFT
}

inline void HrtfConvolver::setSKernel(complex_t *sKernel, uint n)
{
	assert(n == (2 * nSpectrum_ - 2));
	memcpy(sKernel_, sKernel, sizeof(complex_t) * nSpectrum_);
	isSKernelSet_ = true;
}

inline void HrtfConvolver::setTSegment(float *tSegment, uint n)
{
	assert(n == nTSegment_);
	memcpy(tSegment_, tSegment, sizeof(float) * nTSegment_);
}

inline uint HrtfConvolver::getConvolutionLength()
{
	return nTSegment_;
}

inline void HrtfConvolver::flush(float *out)
{
	memcpy(out, olap_, sizeof(float) * nOlap_);  // output all samples in olap_[]
	memcpy(&out[nOlap_], zeros_, sizeof(float) * (nTSegment_ - nOlap_));
}

}  // namespace avrs

#endif // HRTFCONVOLVER_H_
