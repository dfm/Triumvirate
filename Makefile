# @file Makefile
# @brief `make` instructions for building and testing Triumvirate.
# @authors Mike S Wang (https://github.com/MikeSWang)
#

# ========================================================================
# Configuration
# ========================================================================

REPONAME := Triumvirate
PKGNAME := triumvirate
PROGNAME := triumvirate
LIBNAME := trv

SCM_VER_SCHEME ?= no-guess-dev
SCM_LOC_SCHEME ?= node-and-date


# ------------------------------------------------------------------------
# Directories
# ------------------------------------------------------------------------

# Repository root
DIR_ROOT := $(shell pwd)

# Package, build, test and distribution directories
DIR_PKG := ${DIR_ROOT}/src/${PKGNAME}
DIR_BUILD := ${DIR_ROOT}/build
DIR_TESTS := ${DIR_ROOT}/tests
DIR_DIST := ${DIR_ROOT}/dist

# Package subdirectories
DIR_PKG_INCLUDE := ${DIR_PKG}/include
DIR_PKG_SRC := ${DIR_PKG}/src
DIR_PKG_SRCPROG := ${DIR_PKG}/main

# Build subdirectories
DIR_BUILDOBJ := ${DIR_BUILD}/obj
DIR_BUILDLIB := ${DIR_BUILD}/lib
DIR_BUILDBIN := ${DIR_BUILD}/bin

# Test subdirectories
DIR_TESTBUILD := ${DIR_TESTS}/test_build
DIR_TESTOUT := ${DIR_TESTS}/test_output


# ------------------------------------------------------------------------
# Options
# ------------------------------------------------------------------------

# Extract the '-j' or '--jobs' option (possibly empty).
# Note the space added after ``${MAKEFLAGS}``.
PATTERN_JOBS = "\-j[[:digit:][:space:]]*[^a-z[:punct:]]"
MAKEFLAGS_JOBS = $(shell echo "${MAKEFLAGS} " | grep -Eo ${PATTERN_JOBS})


# ------------------------------------------------------------------------
# Compilation
# ------------------------------------------------------------------------

# -- OS ------------------------------------------------------------------

OS := $(shell uname -s)


# -- Compiler ------------------------------------------------------------

# Assume explicitly GCC compiler by default. [adapt]
ifeq (${OS}, Linux)

CXX ?= g++

else ifeq (${OS}, Darwin)

# Use GCC compiler from Homebrew (brew formula 'gcc').
# The compiler binary may have suffix '-<version>';
# check the version number with ``brew info gcc``.
CXX ?= $(shell find $(brew --prefix gcc)/bin -type f -name 'g++*')

# # Use alternatively LLVM compiler from Homebrew (brew formula 'llvm').
# CXX ?= $(shell brew --prefix llvm)/bin/clang++

else  # OS

CXX ?= g++

endif  # OS

# Assume default archiver. [adapt]
AR ?= ar
ARFLAGS ?= -rcsv

# Assume default remover. [adapt]
RM ?= rm -f


# -- Dependencies --------------------------------------------------------

DEPS := gsl fftw3

# Dependencies are searched for by `pkg-config`.  Ensure the set-up of
# `pkg-config` matches that of the dependencies (e.g. both are installed
# by Conda in the same Conda environment).
DEP_INCLUDES := $(shell pkg-config --cflags-only-I ${DEPS})
DEP_CXXFLAGS := $(shell pkg-config --cflags-only-other ${DEPS})
DEP_LDFLAGS := $(shell pkg-config --libs-only-other --libs-only-L ${DEPS})
DEP_LDLIBS := $(shell pkg-config --libs-only-l ${DEPS})


# -- Options -------------------------------------------------------------

INCLUDES += -I${DIR_PKG_INCLUDE} ${DEP_INCLUDES}
CPPFLAGS += -MMD -MP
CXXFLAGS += -Wall -O3 ${DEP_CXXFLAGS}
LDFLAGS += ${DEP_LDFLAGS}
LDLIBS += $(if ${DEP_LDLIBS},${DEP_LDLIBS},$(-lgsl -lgslcblas -lm -lfftw3))

PIPOPTS ?= --user


# -- Environment ---------------------------------------------------------

# NERSC computer cluster: an example environment [adapt]
ifdef NERSC_HOST

# GSL library
ifdef GSL_ROOT
INCLUDES += -I${GSL_ROOT}/include
LDFLAGS += -L${GSL_ROOT}/lib
endif  # GSL_ROOT

# FFTW library
ifdef FFTW_ROOT
INCLUDES += -I${FFTW_INC}
LDFLAGS += -L${FFTW_DIR}
endif  # FFTW_ROOT

endif  # NERSC_HOST


# -- Customisation -------------------------------------------------------

# OpenMP: enabled with ``useomp=(true|1)``; disabled otherwise
ifdef useomp

ifeq ($(strip ${useomp}), $(filter $(strip ${useomp}), true 1))

# Assume GCC OpenMP implementation by default. [adapt]
ifeq (${OS}, Linux)

CXXFLAGS_OMP ?= -fopenmp
LDFLAGS_OMP ?= -fopenmp
# LDLIBS_OMP ?= -lgomp

# # Use alternatively Intel OpenMP implementation.
# CXXFLAGS_OMP ?= -qopenmp
# LDFLAGS_OMP ?= -qopenmp
# # LDLIBS_OMP ?= -liomp5

else ifeq (${OS}, Darwin)

CXXFLAGS_OMP ?= -fopenmp
LDFLAGS_OMP ?= -fopenmp
# LDLIBS_OMP ?= -lgomp

# # Use alternatively LLVM OpenMP implementation from Homebrew
# # (brew formula 'libomp').
# CXXFLAGS_OMP ?= -I$(shell brew --prefix libomp)/include -Xpreprocessor -fopenmp
# LDFLAGS_OMP ?= -L$(shell brew --prefix libomp)/lib
# LDLIBS_OMP ?= -lomp

else  # OS

CXXFLAGS_OMP ?= -fopenmp
LDFLAGS_OMP ?= -fopenmp

endif  # OS

CPPFLAGS += -DTRV_USE_OMP -DTRV_USE_FFTWOMP
CXXFLAGS += ${CXXFLAGS_OMP}
LDFLAGS += ${LDFLAGS_OMP}
LDLIBS += -lfftw3_omp ${LDLIBS_OMP}

WOMP := with

else  # useomp!=(true|1)

# NOTE: Use `undefine` for make>=3.82.
unexport useomp

WOMP := without

endif  # useomp==(true|1)

else  # !useomp

WOMP := without

endif  # useomp

# Visual enhancements: enabled with `uselogo=(true|1)`; disabled otherwise
ifdef uselogo
ifeq ($(strip ${uselogo}), $(filter $(strip ${uselogo}), true 1))
CPPFLAGS += -DTRV_USE_LOGO
endif  # uselogo==(true|1)
endif  # uselogo

# Profiler flags: enabled with `useprof=(true|1)`; disabled otherwise
ifdef useprof
ifeq ($(strip ${useprof}), $(filter $(strip ${useprof}), true 1))
# Linaro MAP profiler
CXXFLAGS += -g1 -O3 -fno-inline -fno-optimize-sibling-calls
endif  # useprof==(true|1)
endif  # useprof

# Parameter debugging: enabled with `dbgpars=(true|1)`; disabled otherwise
ifdef dbgpars
ifeq ($(strip ${dbgpars}), $(filter $(strip ${dbgpars}), true 1))
CPPFLAGS += -DDBG_MODE -DDBG_PARS
endif  # dbgpars==(true|1)
endif  # dbgpars

# Add options: e.g. {-DTRV_USE_LEGACY_CODE, -DDBG_MODE, -DDBG_FLAG_NOAC, ...}.


# -- Parsing -------------------------------------------------------------

# Python: export build options as environmental variables.
export PY_CXX=${CXX}
export PY_INCLUDES=${INCLUDES}
export PY_CXXFLAGS=${CXXFLAGS}
export PY_LDFLAGS=${LDFLAGS}

ifndef useomp
export PY_NO_OMP
else  # useomp
export PY_CXXFLAGS_OMP=${CXXFLAGS_OMP}
export PY_LDFLAGS_OMP=${LDFLAGS_OMP}
endif  # !useomp

export PY_BUILD_PARALLEL=${MAKEFLAGS_JOBS}

export PY_SCM_VER_SCHEME=${SCM_VER_SCHEME}
export PY_SCM_LOC_SCHEME=${SCM_LOC_SCHEME}

# C++: strip whitespace.
CPPFLAGS := $(strip ${CPPFLAGS}) $(strip ${INCLUDES})
CXXFLAGS := $(strip ${CXXFLAGS})
LDFLAGS := $(strip ${LDFLAGS})
LDLIBS := $(strip ${LDLIBS})


# ========================================================================
# Recipes
# ========================================================================

# ------------------------------------------------------------------------
# Building
# ------------------------------------------------------------------------

SRCS := $(wildcard ${DIR_PKG_SRC}/*.cpp)
OBJS := $(SRCS:${DIR_PKG_SRC}/%.cpp=${DIR_BUILDOBJ}/%.o)
DEPS := $(OBJS:.o=.d)

PROGSRC := ${DIR_PKG_SRCPROG}/${PROGNAME}.cpp
PROGOBJ := ${DIR_BUILDOBJ}/${PROGNAME}.o

PROGEXE := ${DIR_BUILDBIN}/${PROGNAME}
PROGLIB := ${DIR_BUILDLIB}/lib${LIBNAME}.a


# -- Installation --------------------------------------------------------

.PHONY: install cppinstall pyinstall cpplibinstall cppappbuild \
        uninstall cppuninstall pyuninstall

install: cppinstall pyinstall

cppinstall: cppinstall_ cpplibinstall cppappbuild

cppinstall_:
	@echo "Installing Triumvirate C++ library/program..."

cpplibinstall: library

cppappbuild: executable

pyinstall:
	@echo "Installing Triumvirate Python package ${WOMP} OpenMP (in pip dev mode)..."
	python -m pip install ${PIPOPTS} --editable . -vvv

uninstall: cppuninstall pyuninstall

cppuninstall:
	@echo "Uninstalling Triumvirate C++ library/program..."
	@echo "  ... removing builds..."
	@find ${DIR_BUILD} -mindepth 1 -maxdepth 1 ! -name ".gitignore" -exec rm -r {} +

pyuninstall:
	@echo "Uninstalling Triumvirate Python package (in pip mode)..."
	python -m pip uninstall -y ${PKGNAME}


# -- Components ----------------------------------------------------------

.PHONY: executable library OBJS_

executable: OBJS_ ${PROGEXE}

${PROGEXE}: $(OBJS) ${PROGOBJ}
	@echo "Compiling Triumvirate C++ program ${WOMP} OpenMP..."
	@if [ ! -d ${DIR_BUILDBIN} ]; then \
	    echo "  making bin subdirectory in build directory..."; \
	    mkdir -p ${DIR_BUILDBIN}; \
	fi
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS) $(LDLIBS)

library: OBJS_ ${PROGLIB}

${PROGLIB}: $(OBJS)
	@echo "Creating Triumvirate C++ library ${WOMP} OpenMP..."
	@if [ ! -d ${DIR_BUILDLIB} ]; then \
	    echo "  making lib subdirectory in build directory..."; \
	    mkdir -p ${DIR_BUILDLIB}; \
	fi
	$(AR) $(ARFLAGS) $@ $^

OBJS_:
	@echo "Creating Triumvirate C++ object files..."
	@if [ ! -d ${DIR_BUILDOBJ} ]; then \
	    echo "  making obj subdirectory in build directory..."; \
	    mkdir -p ${DIR_BUILDOBJ}; \
	fi

${PROGOBJ}: ${PROGSRC}
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

$(OBJS): ${DIR_BUILDOBJ}/%.o: ${DIR_PKG_SRC}/%.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

-include $(DEPS)


# -- Configuration -------------------------------------------------------

.PHONY: checkopts

checkopts:
	@echo "Checking options parsed by Makefile..."
	@echo "  MAKEFLAGS=${MAKEFLAGS}"
	@echo "  MAKEFLAGS_JOBS=${MAKEFLAGS_JOBS}"
	@echo "  SCM_VER_SCHEME=${SCM_VER_SCHEME}"
	@echo "  SCM_LOC_SCHEME=${SCM_LOC_SCHEME}"
	@echo "  CXX=${CXX}"
	@echo "  INCLUDES=${INCLUDES}"
	@echo "  CPPFLAGS=${CPPFLAGS}"
	@echo "  CXXFLAGS=${CXXFLAGS}"
	@echo "  LDFLAGS=${LDFLAGS}"
	@echo "  LDLIBS=${LDLIBS}"
	@echo "  CXXFLAGS_OMP=${CXXFLAGS_OMP}"
	@echo "  LDFLAGS_OMP=${LDFLAGS_OMP}"
	@echo "  LDLIBS_OMP=${LDLIBS_OMP}"
	@echo "  AR=${AR}"
	@echo "  ARFLAGS=${ARFLAGS}"
	@echo "  RM=${RM}"
	@echo "  PIPOPTS=${PIPOPTS}"


# ------------------------------------------------------------------------
# Testing
# ------------------------------------------------------------------------

.PHONY: test pytest

test: pytest

pytest:
	@echo "Peforming Triumvirate Python tests..."
	@if [ ! -d ${DIR_TESTOUT} ]; then \
	    echo "  making output subdirectory in test directory..."; \
	    mkdir -p ${DIR_TESTOUT}; \
	fi
	pytest


# ------------------------------------------------------------------------
# Cleaning
# ------------------------------------------------------------------------

.PHONY: clean buildclean testclean distclean runclean cppclean pyclean

clean: buildclean testclean distclean runclean

buildclean: cppclean pyclean

cppclean:
	@echo "Cleaning up Triumvirate C++ build..."
	@echo "  removing builds..."
	@find ${DIR_BUILD} -mindepth 1 -maxdepth 1 ! -name ".gitignore" -exec rm -r {} +

pyclean:
	@echo "Cleaning up Triumvirate Python build..."
	@echo "  removing Cythonised C/C++ scripts..."
	@find ${DIR_PKG} -maxdepth 1 -name "*.cpp" -exec rm {} +
	@echo "  removing Cythonised extensions..."
	@find ${DIR_PKG} -maxdepth 1 -name "*.so" -exec rm {} +
	@echo "  removing compiled bytecode..."
	@find . -type d -name "__pycache__" -exec rm -r {} +
	@echo "  removing Jupyter notebook checkpoints..."
	@find . -type d -name ".ipynb_checkpoints" -exec rm -r {} +

testclean:
	@echo "Cleaning up Triumvirate tests..."
	@echo "  removing test builds and outputs..."
	@$(RM) -r ${DIR_TESTBUILD}/* ${DIR_TESTOUT}/*
	@echo "  removing pytest cache..."
	@find . -type d -name ".pytest_cache" -exec rm -r {} +
	@echo "  removing core dumps..."
	@$(RM) -r core

distclean:
	@echo "Cleaning up Triumvirate distributions..."
	@echo "  removing distribution outputs..."
	@$(RM) -r ${DIR_DIST}/
	@echo "  removing wheels..."
	@find . -type d -name "wheelhouse" -exec rm -r {} +
	@echo "  removing eggs..."
	@find . -name "*.egg-info" -exec rm -r {} +

runclean:
	@echo "Cleaning up Triumvirate runs..."
	@echo "  removing core dumps..."
	@$(RM) -r core
