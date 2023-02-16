<p align="center">
<img src="docs/source/_static/Triumvirate.png" alt=Triumvirate-Logo width=67%>
</p>

# Three-Point Clustering Measurements in LSS

[![Build](
https://img.shields.io/github/actions/workflow/status/MikeSWang/Triumvirate/build.yml?branch=main&logo=GitHub%20Actions
)](https://github.com/MikeSWang/Triumvirate/actions/workflows/build.yml)
[![Tests](
https://img.shields.io/github/actions/workflow/status/MikeSWang/Triumvirate/tests.yml?branch=main&logo=GitHub%20Actions&label=tests
)](https://github.com/MikeSWang/Triumvirate/actions/workflows/tests.yml)
[![Docs](
https://img.shields.io/readthedocs/triumvirate/latest?logo=Read%20the%20Docs
)](https://triumvirate.readthedocs.io/en/latest/)
[![Release](
https://img.shields.io/github/v/release/MikeSWang/Triumvirate?logo=Git
)](https://github.com/MikeSWang/Triumvirate/releases/latest)

``Triumvirate`` is a Python/C++ software package for measuring three-point (and
two-point) clustering statistics in large-scale structure (LSS) cosmological
analyses.


## Documentation

Comprehensive documentation including the [scientific background](
https://triumvirate.readthedocs.io/en/latest/background.html),
[installation instructions](
https://triumvirate.readthedocs.io/en/latest/installation.html),
[tutorials](https://triumvirate.readthedocs.io/en/latest/tutorials.html) and
[API reference](https://triumvirate.readthedocs.io/en/latest/apiref.html) can
be found at [triumvirate.readthedocs.io](https://triumvirate.readthedocs.io).


## Installation

### Python package

[![PyPI](
https://img.shields.io/pypi/v/Triumvirate?logo=PyPI&color=informational
)](https://pypi.org/project/Triumvirate)
[![conda](
https://img.shields.io/conda/vn/conda-forge/triumvirate?logo=conda-forge
)](https://anaconda.org/conda-forge/triumvirate)

``Triumvirate`` as a Python package is distributed through [PyPI](
https://pypi.org/project/Triumvirate) and [conda-forge](
https://anaconda.org/conda-forge/triumvirate). Instructions for installation
can be found on the [Installation](
https://triumvirate.readthedocs.io/en/latest/installation.html#python-package)
page in the documentation.

### C++ library & program

``Triumvirate`` as either a C++ library or a 'black-box' C++ program can be
built using `make`. Instructions for compilation can be found on the
[Installation](
https://triumvirate.readthedocs.io/en/latest/installation.html#c-program)
page in the documentation.

### Development mode

Both the Python package and the C++ library/program can be set up in
development mode with `make`. First `git clone` this repository and
`git checkout` the branch/release you would like to edit:
```bash
git clone git@github.com:MikeSWang/Triumvirate.git --branch <branch-or-release>
cd Triumvirate
```
Then at the repository directory root run
```bash
make clean
make [py|cpp]install [useomp=[true|1]]
```
where ``install`` builds both and ``pyinstall``/``cppinstall`` is for
Python/C++ build only; you may also replace this with ``cpplibinstall`` or
``cppappbuild`` to compile the C++ static library or binary executable only.
To enable OpenMP parallelisation, append ``useomp=true`` or ``useomp=1`` to
the end of the second line as shown above.

The latest release is on the ``main`` branch. The default ``Makefile``
(located at the repository diretory root) suits most use cases, but you may
modify it as appropriate for your need.


## Attribution

[![JOSS](
https://img.shields.io/badge/JOSS-doi-brightgreen
)](https://joss.theoj.org/papers/?/status.svg)
[![arXiv](
https://img.shields.io/badge/arXiv-yymm.%3F-b31b1b
)](https://arxiv.org/abs/?.?)
[![MNRAS](
https://img.shields.io/badge/doi-10.1093%2Fmnras%2Fsty3249-informational
)](https://doi.org/10.1093/mnras/sty3249)
[![MNRAS](
https://img.shields.io/badge/doi-10.1093%2Fmnras%2Fstx2333-informational
)](https://doi.org/10.1093/mnras/stx2333)

To acknowledge the use of ``Triumvirate`` in your published research, please
cite the publications linked above which contain the relevant information
in the BibTeX format.


## Acknowledgement

<img src="docs/source/_static/ERC-Logo-Flag.png" alt="ERC" width="40%">

This project has received funding from the European Research Council (ERC)
under the European Union's Horizon 2020 research and innovation programme
(Grant agreement No. [853291](https://doi.org/10.3030/853291)).

Key underlying numerical algorithms were originally developed by
Naonori S Sugiyama, and are available in the GitHub repository [``hitomi``](
https://github.com/naonori/hitomi).


## Contributing

User feedback and contributions are highly valued and very welcome. Please
refer to the [contribution guidelines](CONTRIBUTING.md).


## Releases

Changes in recent releases are listed in the [change log](CHANGELOG.md).


## Licence

[![Licence](
https://img.shields.io/github/license/MikeSWang/Triumvirate?label=licence&style=flat-square&color=informational
)](https://github.com/MikeSWang/Triumvirate/blob/main/LICENCE)

``Triumvirate`` is made freely available under the [GPL-3.0 licence](
https://www.gnu.org/licenses/gpl-3.0.en.html). Please see
[``LICENCE``](LICENCE) for full terms and conditions.

&copy; 2023 MS Wang & NS Sugiyama
