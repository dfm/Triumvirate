***************
Release History
***************

v0.4.0 (2023-1?-??)
===================

Bug fixes
---------

Fix various legacy issues in the non-public methods related to
Hankel-like transforms.

Features
--------

- Add public API for Hankel-like transforms using the FFTLog algorithm
  with enhanced extrapolation options.

- Add public API for window convolution.

- Expose installation validation as a public function.

Improvements
------------

- Refactor gamma function computations.
- Refactor array operation checks.
- Refactor FFTW plans.
- Add logs to `trv::MeshField` and `trv::FieldStats` operations.
- Add tracking of (I)FFTs.

Maintenance
-----------

Update syntax and fix typos.

Documentation
-------------

- Add version context to documentation.
- Add and update details to/in various documentation components.


v0.3.0 (2023-10-04)
===================

Bug fixes
---------

Fix reused bin statistics when duplicate recording is designed to be avoided
in |threept.cpp|_.

Features
--------

- Add the functionality to record binning details for wavevector modes
  and separation pairs from a mesh grid with the option to save the results
  to a file in C++.

- Add more forms of three-point statistics including the off-diagonal and
  full form (the original 'full' form is renamed to 'row')
  (`#22 <https://github.com/MikeSWang/Triumvirate/issues/22>`_).

Improvements
------------

- Implement further parallelisation and refactoring resulting in
  significant speed-up.

- Enhance parameter validation consistency.

Maintenance
-----------

- Update syntax and fix typos.
- Add and update test data after API changes.

Documentation
-------------

- Add and update details to/in various documentation components.
- Update and rerun tutorial notebooks after API changes.


v0.2.2 (2023-07-04)
===================

Improvements
------------

Enhance build process and update syntax.

Documentation
-------------

Update installation guide after build enhancement.


v0.2.1 (2023-06-20)
===================

Bug fixes
---------

Fix parity factor in three-point correlation functions in |threept.cpp|_.

Maintenance
-----------

- Remove reality-condition division in mode/pair counts for generality.
- Update test data.
- Update syntax.

Documentation
-------------

Rerun tutorial notebooks.


v0.2.0 (2023-06-01)
===================

Bug fixes
---------

Fix updating of derived parameters ``npoint`` and ``space`` in ``ParameterSet``
in |parameters.pyx|_.

Features
--------

- Add ``pypower``-like normalisation for two-point clustering statistics with
  the new value option 'mesh-mixed' for the ``norm_convention`` parameter.

- Separate ``wtotal`` and ``wstotal`` attributes for ``ParticleCatalogue``.
  This is also reflected in logging and output file headers.

- Add particle position spans as a new member ``pos_span``
  in ``trv::ParticleCatalogue`` as this is used for ``pypower``-like
  normalisation calculations.

Improvements
------------

Enhance logging and build and packaging processes.

Maintenance
-----------

Update syntax and fix typos.

Documentation
-------------

Rerun tutorial notebooks.


v0.1.2 (2023-04-12)
===================

Bug fixes
---------

Fix parsing of build environmental variables in |setup.py|_.

Improvements
------------

Upgrade build and packaging processes.

Maintenance
-----------

Update syntax and fix typos.

Documentation
-------------

Update installation guide based on the enhanced build process.


v0.1.1 (2023-04-07)
===================

Initial full public release (`major version 0 <https://semver.org/#spec-item-4>`_):
publish to the PyPI index and Anaconda repository.

.. Bug fixes
.. ---------

.. Sort loaded measurements files in ``application/tools/comb_data_vectors.py``.

Improvements
------------

Upgrade build and packaging processes.

Documentation
-------------

Update installation guide.


v0.1.0 (2023-03-30)
===================

Initial public release (`major version 0 <https://semver.org/#spec-item-4>`_):
publish to the PyPI Index (Anaconda repository pending).


.. |threept.cpp| replace:: ``threept.cpp``
.. _threept.cpp: https://github.com/MikeSWang/Triumvirate/blob/main/src/triumvirate/src/threept.cpp

.. |parameters.pyx| replace:: ``parameters.pyx``
.. _parameters.pyx: https://github.com/MikeSWang/Triumvirate/blob/main/src/triumvirate/parameters.pyx

.. |setup.py| replace:: ``setup.py``
.. _setup.py: https://github.com/MikeSWang/Triumvirate/blob/main/setup.py
