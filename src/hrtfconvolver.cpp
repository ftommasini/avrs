#include "hrtfconvolver.hpp"
#include "common.hpp"

namespace avrs
{

// FftConvolution
HrtfConvolver::HrtfConvolver(uint nFft)
	: nFft_(nFft)
{
	uint i;

	nTKernel_ = 0;  // zero until setting
	nTSegment_ = 0;  // zero until setting

	nSpectrum_ = (nFft_ / 2) + 1;
	tSegment_ = (float *) fftwf_malloc(sizeof(float) * nFft_);
	sSegment_ = (complex_t *) fftwf_malloc(sizeof(complex_t) * nSpectrum_);
	tKernel_ = (float *) fftwf_malloc(sizeof(float) * nFft_);  // same size of tSegment_
	sKernel_ = (complex_t *) fftwf_malloc(sizeof(complex_t) * nSpectrum_);
	isSKernelSet_ = false;

	for (i = 0; i < nFft_; i++) {
		tSegment_[i] = 0.0;
		tKernel_[i] = 0.0;
	}

	// Create forward FFT plan from in_ into sIn_
	planFwdSegment_ = fftwf_plan_dft_r2c_1d(nFft_, tSegment_, sSegment_, FFTW_ESTIMATE);
	// Create inverse FFT (IFFT) plan from sIn_ into in_
	planBwdSegment_ = fftwf_plan_dft_c2r_1d(nFft_, sSegment_, tSegment_, FFTW_ESTIMATE);
	// Create forward FFT plan from kernel_ into sKernel_
	planFwdKernel_ = fftwf_plan_dft_r2c_1d(nFft_, tKernel_, sKernel_, FFTW_ESTIMATE);
}

HrtfConvolver::~HrtfConvolver()
{
	fftwf_destroy_plan(planFwdSegment_);
    fftwf_destroy_plan(planBwdSegment_);
    fftwf_destroy_plan(planFwdKernel_);
    fftwf_free(tSegment_);
    fftwf_free(sSegment_);
    fftwf_free(tKernel_);
    fftwf_free(sKernel_);

    if (nTKernel_ != 0)
    	fftwf_free(olap_);

    if (nTSegment_ != 0)
    	fftwf_free(zeros_);
}

HrtfConvolver::ptr_t HrtfConvolver::create(uint nFft)
{
	ptr_t p_tmp(new HrtfConvolver(nFft));
	return p_tmp;
}

void HrtfConvolver::convolve(float *out)
{
	static uint i;
	static float tmp;

	if (!isSKernelSet_)
		fftwf_execute(planFwdKernel_);  // FFT from tKernel_ into sKernel_

	isSKernelSet_ = false;
	memcpy(&tSegment_[nTSegment_], zeros_, sizeof(float) * (nFft_ - nTSegment_));  // complete with zeros
	fftwf_execute(planFwdSegment_);  // FFT from tSegment_ into sSegment_

	// Complex multiplication an pre-normalization
    for (i = 0; i < nSpectrum_; i++) {
    	tmp = (sSegment_[i][0] * sKernel_[i][0] - sSegment_[i][1] * sKernel_[i][1]) / nFft_;  // real
    	sSegment_[i][1] = (sSegment_[i][0] * sKernel_[i][1] + sSegment_[i][1] * sKernel_[i][0]) / nFft_;  // imaginary
    	sSegment_[i][0] = tmp;
    }

    fftwf_execute(planBwdSegment_);  // IFFT from sSegment_ into tSegment_ (unnormalized)

    // Add the last segment's overlap to this segment
    for (i = 0; i < nOlap_; i++)
    	tSegment_[i] += olap_[i];

    memcpy(olap_, &tSegment_[nTSegment_], sizeof(float) * nOlap_);  // save the samples that will overlap the next segment
    memcpy(out, tSegment_, sizeof(float) * nTSegment_);  // output samples
}

}  // namespace avrs
