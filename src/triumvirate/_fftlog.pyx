"""
FFTLog algorithm (:mod:`~triumvirate._fftlog`)
==========================================================================

Implement the FFTLog algorithm for Hankel-related transforms.

"""
import numpy as np
cimport numpy as np

from ._arrayops import _check_1d_array
from ._fftlog cimport CppHankelTransform


cdef class HankelTransform:
    """Hankel transform.

    Parameters
    ----------
    mu : float
        Order of the Hankel transform.
    q : float
        Power-law bias index.
    x : array of float
        Pre-transform sample points.  Must be log-linearly spaced.
        Must be even in length if extrapolation is used.
    kr_c : float
        Pivot value for the transform.  When `lowring` is `True`, this is
        adjusted if it is non-zero, or otherwise directly calculated.
    lowring : bool, optional
        Low-ringing condition (default is `True`).
    extrap : int, optional
        Extrapolation method (default is 0) with the following
        options:

        - 0: none;
        - 1: extrapolate by constant padding;
        - 2: extrapolate linearly;
        - 3: extrapolate log-linearly.

        Any extrapolation results in a sample size for the transform that
        is the smallest power of 2 greater than or equal to `extrap_exp`
        times the original number of sample points; the pre-transform
        samples are assumed to be real.
    extrap_exp : float, optional
        Sample size expansion factor (default is 2.) for extrapolation.
        The smallest power of 2 greater than or equal to this times the
        original number of sample points is used as the sample size for
        the transform.
    threaded : bool, optional
        If `True` (default is `False`), use the multi-threaded FFTLog
        algorithm.

    Attributes
    ----------
    order : float
        Order of the transform.
    bias : float
        Power-law bias index.
    size : int
        Sample size of the transform.
    pivot : float
        Pivot value used.

    """

    def __cinit__(self, mu, q, x, kr_c, lowring=True, extrap=0, extrap_exp=2.,
                  threaded=False):
        self.thisptr = new CppHankelTransform(<double>mu, <double>q, threaded)

        cdef np.ndarray[double, ndim=1, mode='c'] _x = np.ascontiguousarray(
            _check_1d_array(x, check_loglin=True)
        )
        self.thisptr.initialise(
            _x, kr_c, lowring, 0 if extrap is None else extrap, extrap_exp
        )

        self.order = self.thisptr.order
        self.bias = self.thisptr.bias

        self._nsamp = self.thisptr.nsamp
        self._nsamp_trans = self.thisptr.nsamp_trans
        self._logres = self.thisptr.logres
        self._pivot = self.thisptr.pivot

        self._pre_sampts = self.thisptr.pre_sampts
        self._post_sampts = self.thisptr.post_sampts

    def __dealloc__(self):
        del self.thisptr

    @property
    def size(self):
        """Sample size.

        """
        return self._nsamp_trans

    @property
    def pivot(self):
        """Pivot value.

        """
        return self._pivot

    def transform(self, fx):
        """Transform samples at initialised sample points.

        Parameters
        ----------
        fx : array_like
            Pre-transform samples.  Must be even in length
            if extrapolation is used.  Assumed to be real if extrapolation
            is used.

        Returns
        -------
        y, gy : array_like
            Post-transform sample points and samples.

        """
        fx = np.ascontiguousarray(fx, dtype=np.complex128)
        y, gy = self._post_sampts, self._transform(fx)

        return np.asarray(y), np.asarray(gy)

    def _transform(
        self, np.ndarray[double complex, ndim=1, mode='c'] fx not None
    ):
        """Transform samples at initialised sample points.

        Parameters
        ----------
        fx : array of complex
            Pre-transform samples.  Must be even in length
            if extrapolation is used.  Assumed to be real if extrapolation
            is used.

        Returns
        -------
        gy : array of complex
            Post-transform samples.

        """
        cdef np.ndarray[double complex, ndim=1, mode='c'] gy = \
            np.zeros(self._nsamp, dtype=np.complex128)

        self.thisptr.biased_transform(&fx[0], &gy[0])

        return gy
