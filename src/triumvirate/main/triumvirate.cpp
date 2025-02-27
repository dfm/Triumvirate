// Copyright (C) [GPLv3 Licence]
//
// This file is part of the Triumvirate program. See the COPYRIGHT
// and LICENCE files at the top-level directory of this distribution
// for details of copyright and licensing.
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.

/**
 * @file triumvirate.cpp
 * @authors Mike S Wang (https://github.com/MikeSWang),
 *          Naonori Sugiyama (https://github.com/naonori)
 * @brief Perform two- and three-point clustering statistic measurements.
 *
 */

#include <cstdio>
#include <string>

#include "monitor.hpp"
#include "parameters.hpp"
#include "particles.hpp"
#include "dataobjs.hpp"
#include "io.hpp"
#include "twopt.hpp"
#include "threept.hpp"

/**
 * @brief A 'black-box' program for measuring two- and three-point
 *        clustering statistics.
 *
 * @param argc Number of command-line arguments.
 * @param argv Command-line arguments.
 * @returns Exit status.
 */
int main(int argc, char* argv[]) {
#ifdef TRV_USE_LOGO
  trv::sys::display_prog_notice();
  // trv::sys::display_prog_licence();
#endif  // TRV_USE_LOGO

  if (trv::sys::currTask == 0) {
    std::printf("%s\n", std::string(80, '>').c_str());
  }

  // =====================================================================
  // A Initialisation
  // =====================================================================

  if (trv::sys::currTask == 0) {
    trv::sys::logger.stat(
      "[A] Parameters and source data are being initialised."
    );
  }

  // ---------------------------------------------------------------------
  // A.1 Parameter I/O
  // ---------------------------------------------------------------------

  if (trv::sys::currTask == 0) {
    trv::sys::logger.stat("[A.1] Reading parameters...");
  }

  if (argc < 2) {
    if (trv::sys::currTask == 0) {
      trv::sys::logger.error(
        "Failed to initialise program: missing parameter file."
      );
      throw trv::sys::IOError(
        "Failed to initialise program: missing parameter file.\n"
      );
    }
  }

  trv::ParameterSet params;  // program parameters
  if (params.read_from_file(argv[1])) {
    if (trv::sys::currTask == 0) {
      trv::sys::logger.error(
        "Failed to initialise program: invalidated parameters."
      );
      throw trv::sys::IOError(
        "Failed to initialise program: invalidated parameters.\n"
      );
    }
  }

  trv::sys::make_write_dir(params.measurement_dir);
  if (params.print_to_file()) {
    if (trv::sys::currTask == 0) {
      trv::sys::logger.warn(
        "Failed to print used parameters to file "
        "in the measurement output directory."
      );
    }
  }

  if (trv::sys::currTask == 0) {
    trv::sys::logger.stat("[A.1] ... read parameters.");
  }

  trv::sys::logger.reset_level(params.verbose);

  // ---------------------------------------------------------------------
  // A.2 Data I/O
  // ---------------------------------------------------------------------

  if (params.catalogue_type != "none") {
    if (trv::sys::currTask == 0) {
      trv::sys::logger.stat("[A.2] Reading catalogues...");
    }
  }

  trv::ParticleCatalogue catalogue_data; // data-source catalogue
  std::string flag_data = "false";       // data-source catalogue status
  if (params.catalogue_type == "survey" || params.catalogue_type == "sim") {
    if (!(trv::sys::if_filepath_is_set(params.data_catalogue_file))) {
      if (trv::sys::currTask == 0) {
        trv::sys::logger.error(
          "Failed to initialise program: "
          "unspecified data-source catalogue file."
        );
        throw trv::sys::IOError(
          "Failed to initialise program: "
          "unspecified data-source catalogue file.\n"
        );
      }
    }
    if (catalogue_data.load_catalogue_file(
      params.data_catalogue_file, params.catalogue_columns, params.volume
    )) {
      if (trv::sys::currTask == 0) {
        trv::sys::logger.error(
          "Failed to initialise program: "
          "unloadable data-source catalogue file."
        );
        throw trv::sys::IOError(
          "Failed to initialise program: "
          "unloadable data-source catalogue file.\n"
        );
      }
    }
    flag_data = "true";
  }

  trv::ParticleCatalogue catalogue_rand; // random-source catalogue
  std::string flag_rand = "false";       // random-source catalogue status
  if (params.catalogue_type == "survey" || params.catalogue_type == "random") {
    if (!(trv::sys::if_filepath_is_set(params.rand_catalogue_file))) {
      if (trv::sys::currTask == 0) {
        trv::sys::logger.error(
          "Failed to initialise program: "
          "unspecified random-source catalogue file."
        );
        throw trv::sys::IOError(
          "Failed to initialise program: "
          "unspecified random-source catalogue file.\n"
        );
      }
    }
    if (catalogue_rand.load_catalogue_file(
      params.rand_catalogue_file, params.catalogue_columns, params.volume
    )) {
      if (trv::sys::currTask == 0) {
        trv::sys::logger.error(
          "Failed to initialise program: "
          "unloadable random-source catalogue file."
        );
        throw trv::sys::IOError(
          "Failed to initialise program: "
          "unloadable random-source catalogue file.\n"
        );
      }
    }
    flag_rand = "true";
  }

  if (params.catalogue_type != "none") {
    if (trv::sys::currTask == 0) {
      trv::sys::logger.stat("[A.2] ... read catalogues.");
    }
  }

  // =====================================================================
  // B Measurements
  // =====================================================================

  if (trv::sys::currTask == 0) {
    trv::sys::logger.stat("[B] Clustering statistics are being measured.");
  }

  // ---------------------------------------------------------------------
  // B.1 Binning
  // ---------------------------------------------------------------------

  if (trv::sys::currTask == 0) {
    trv::sys::logger.stat("[B.1] Setting up binning...");
  }

  trv::Binning binning(params);  // binning
  binning.set_bins();

  if (trv::sys::currTask == 0) {
    trv::sys::logger.stat("[B.1] ... set up binning.");
  }

  // ---------------------------------------------------------------------
  // B.2 Line of sight
  // ---------------------------------------------------------------------

  if (params.catalogue_type != "none") {
    if (trv::sys::currTask == 0) {
      trv::sys::logger.stat("[B.2] Computing lines of sight...");
    }
  }

  trv::LineOfSight* los_data = nullptr;
  if (flag_data == "true") {
    // data-source LoS
    los_data = new trv::LineOfSight[catalogue_data.ntotal];
    trv::sys::gbytesMem +=
      trv::sys::size_in_gb<struct trv::LineOfSight>(catalogue_data.ntotal);
    trv::sys::update_maxmem();

#ifdef TRV_USE_OMP
#pragma omp parallel for
#endif  // TRV_USE_OMP
    for (int pid = 0; pid < catalogue_data.ntotal; pid++) {
      double los_mag =
        trv::maths::get_vec3d_magnitude(catalogue_data[pid].pos);

      if (los_mag == 0.) {
        trv::sys::logger.warn(
          "A data-catalogue particle coincides with the origin."
        );
        los_mag = 1.;
      }

      los_data[pid].pos[0] = catalogue_data[pid].pos[0] / los_mag;
      los_data[pid].pos[1] = catalogue_data[pid].pos[1] / los_mag;
      los_data[pid].pos[2] = catalogue_data[pid].pos[2] / los_mag;
    }
  }

  trv::LineOfSight* los_rand = nullptr;
  if (flag_rand == "true") {
    // random-source LoS
    los_rand = new trv::LineOfSight[catalogue_rand.ntotal];
    trv::sys::gbytesMem +=
      trv::sys::size_in_gb<struct trv::LineOfSight>(catalogue_rand.ntotal);
    trv::sys::update_maxmem();

#ifdef TRV_USE_OMP
#pragma omp parallel for
#endif  // TRV_USE_OMP
    for (int pid = 0; pid < catalogue_rand.ntotal; pid++) {
      double los_mag =
        trv::maths::get_vec3d_magnitude(catalogue_rand[pid].pos);

      if (los_mag == 0.) {
        trv::sys::logger.warn(
          "A random-catalogue particle coincides with the origin."
        );
        los_mag = 1.;
      }

      los_rand[pid].pos[0] = catalogue_rand[pid].pos[0] / los_mag;
      los_rand[pid].pos[1] = catalogue_rand[pid].pos[1] / los_mag;
      los_rand[pid].pos[2] = catalogue_rand[pid].pos[2] / los_mag;
    }
  }

  if (params.catalogue_type != "none") {
    if (trv::sys::currTask == 0) {
      trv::sys::logger.stat("[B.2] ... computed lines of sight.");
    }
  }

  // ---------------------------------------------------------------------
  // B.3 Box alignment
  // ---------------------------------------------------------------------

  if (params.catalogue_type != "none") {
    if (trv::sys::currTask == 0) {
      trv::sys::logger.stat(
        "[B.3] Aligning catalogues inside measurement box..."
      );
    }
  }

  if (params.catalogue_type == "survey") {
    if (params.alignment == "pad") {
      if (params.padscale == "grid") {
        double ngrid_pad[3] = {
          params.padfactor, params.padfactor, params.padfactor
        };
        trv::ParticleCatalogue::pad_grids(
          catalogue_data, catalogue_rand,
          params.boxsize, params.ngrid,
          ngrid_pad
        );
      } else
      if (params.padscale == "box") {
        double boxsize_pad[3] = {
          params.padfactor, params.padfactor, params.padfactor
        };
        trv::ParticleCatalogue::pad_in_box(
          catalogue_data, catalogue_rand,
          params.boxsize, boxsize_pad
        );
      }
    } else
    if (params.alignment == "centre") {
      trv::ParticleCatalogue::centre_in_box(
        catalogue_data, catalogue_rand, params.boxsize
      );
    }
  } else
  if (params.catalogue_type == "sim") {
    catalogue_data.offset_coords_for_periodicity(params.boxsize);
  } else
  if (params.catalogue_type == "random") {
    if (params.alignment == "pad") {
      if (params.padscale == "grid") {
        double ngrid_pad[3] = {
          params.padfactor, params.padfactor, params.padfactor
        };
        trv::ParticleCatalogue::pad_grids(
          catalogue_rand, params.boxsize, params.ngrid, ngrid_pad
        );
      } else
      if (params.padscale == "box") {
        double boxsize_pad[3] = {
          params.padfactor, params.padfactor, params.padfactor
        };
        trv::ParticleCatalogue::pad_in_box(
          catalogue_rand, params.boxsize, boxsize_pad
        );
      }
    } else
    if (params.alignment == "centre") {
      trv::ParticleCatalogue::centre_in_box(
        catalogue_rand, params.boxsize
      );
    }
  }

  if (params.catalogue_type != "none") {
    if (trv::sys::currTask == 0) {
      trv::sys::logger.stat(
        "[B.3] ... aligned catalogues inside measurement box."
      );
    }
  }

  // ---------------------------------------------------------------------
  // B.4 Constants
  // ---------------------------------------------------------------------

  double alpha;  // alpha contrast
  if (flag_data == "true" && flag_rand == "true") {
    alpha = catalogue_data.wstotal / catalogue_rand.wstotal;
  } else {
    alpha = 1.;
  }

  if (params.catalogue_type != "none") {
    if (trv::sys::currTask == 0) {
      trv::sys::logger.info("Alpha contrast: %.6e.", alpha);
    }
  }

  trv::ParticleCatalogue& catalogue_for_norm =
    (flag_rand == "true") ? catalogue_rand : catalogue_data;
  double alpha_for_norm = (flag_rand == "true") ? alpha : 1.;
  double norm_factor_part = 0., norm_factor_mesh = 0., norm_factor_meshes = 0.;

  if (params.npoint == "2pt") {
    norm_factor_part = trv::calc_powspec_normalisation_from_particles(
      catalogue_for_norm, alpha_for_norm
    );
    norm_factor_mesh = trv::calc_powspec_normalisation_from_mesh(
      catalogue_for_norm, params, alpha_for_norm
    );
    // Mixed-mesh normalisation is only implemented for
    // paired survey-like catalogues.
    if (params.catalogue_type == "survey") {
      // Use default parameters for mixed-mesh normalisation in `pypower`.
      const double PADDING = 0.1;
      const double CELLSIZE = 10.;
      const std::string ASSIGNMENT = "cic";
      // Box size for normalisation is internally set and as such,
      // the current alignment of the catalogues is not applicable, but
      // this should have no effect on the normalisation.
      norm_factor_meshes = trv::calc_powspec_normalisation_from_meshes(
        catalogue_data, catalogue_rand, params, alpha,
        PADDING, CELLSIZE, ASSIGNMENT
      );
    }
  } else
  if (params.npoint == "3pt") {
    norm_factor_part = trv::calc_bispec_normalisation_from_particles(
      catalogue_for_norm, alpha_for_norm
    );
    norm_factor_mesh = trv::calc_bispec_normalisation_from_mesh(
      catalogue_for_norm, params, alpha_for_norm
    );
  }

  double norm_factor = 0.;
  if (params.npoint != "none") {
    if (params.norm_convention == "none") {
      norm_factor = 1.;
      if (trv::sys::currTask == 0) {
        trv::sys::logger.info(
          "Normalisation factors: "
          "%.6e (particle), %.6e (mesh), %.6e (mesh-mixed) (none used).",
          norm_factor_part, norm_factor_mesh, norm_factor_meshes
        );
      }
    } else
    if (params.norm_convention == "particle") {
      norm_factor = norm_factor_part;
      if (trv::sys::currTask == 0) {
        trv::sys::logger.info(
          "Normalisation factors: "
          "%.6e (particle; used), %.6e (mesh), %.6e (mesh-mixed).",
          norm_factor, norm_factor_mesh, norm_factor_meshes
        );
      }
    } else
    if (params.norm_convention == "mesh") {
      norm_factor = norm_factor_mesh;
      if (trv::sys::currTask == 0) {
        trv::sys::logger.info(
          "Normalisation factors: "
          "%.6e (particle), %.6e (mesh; used), %.6e (mesh-mixed).",
          norm_factor_part, norm_factor, norm_factor_meshes
        );
      }
    } else
    if (params.norm_convention == "mesh-mixed") {
      norm_factor = norm_factor_meshes;
      if (trv::sys::currTask == 0) {
        trv::sys::logger.info(
          "Normalisation factors: "
          "%.6e (particle), %.6e (mesh), %.6e (mesh-mixed; used).",
          norm_factor_part, norm_factor_mesh, norm_factor
        );
      }
    }
  }

  // ---------------------------------------------------------------------
  // B.5 Clustering algorithms
  // ---------------------------------------------------------------------

  char save_filepath[1024];
  if (params.statistic_type == "powspec") {
    std::snprintf(
      save_filepath, sizeof(save_filepath), "%s/pk%d%s",
      params.measurement_dir.c_str(), params.ELL, params.output_tag.c_str()
    );
    std::FILE* save_fileptr = nullptr;
    trv::PowspecMeasurements meas_powspec;  // power spectrum
    if (params.catalogue_type == "survey") {
      meas_powspec = trv::compute_powspec(
        catalogue_data, catalogue_rand, los_data, los_rand,
        params, binning, norm_factor
      );
      save_fileptr = std::fopen(save_filepath, "w");
      trv::io::print_measurement_header_to_file(
        save_fileptr, params, catalogue_data, catalogue_rand,
        norm_factor_part, norm_factor_mesh, norm_factor_meshes
      );
    } else
    if (params.catalogue_type == "sim") {
      meas_powspec = trv::compute_powspec_in_gpp_box(
        catalogue_data, params, binning, norm_factor
      );
      save_fileptr = std::fopen(save_filepath, "w");
      trv::io::print_measurement_header_to_file(
        save_fileptr, params, catalogue_data,
        norm_factor_part, norm_factor_mesh, norm_factor_meshes
      );
    }
    trv::io::print_measurement_datatab_to_file(
      save_fileptr, params, meas_powspec
    );
    std::fclose(save_fileptr);
  } else
  if (params.statistic_type == "2pcf") {
    std::snprintf(
      save_filepath, sizeof(save_filepath), "%s/xi%d%s",
      params.measurement_dir.c_str(), params.ELL, params.output_tag.c_str()
    );
    std::FILE* save_fileptr = nullptr;
    trv::TwoPCFMeasurements meas_2pcf;  // two-point correlation function
    if (params.catalogue_type == "survey") {
      meas_2pcf = trv::compute_corrfunc(
        catalogue_data, catalogue_rand, los_data, los_rand,
        params, binning, norm_factor
      );
      save_fileptr = std::fopen(save_filepath, "w");
      trv::io::print_measurement_header_to_file(
        save_fileptr, params, catalogue_data, catalogue_rand,
        norm_factor_part, norm_factor_mesh, norm_factor_meshes
      );
    } else
    if (params.catalogue_type == "sim") {
      meas_2pcf = trv::compute_corrfunc_in_gpp_box(
        catalogue_data, params, binning, norm_factor
      );
      save_fileptr = std::fopen(save_filepath, "w");
      trv::io::print_measurement_header_to_file(
        save_fileptr, params, catalogue_data,
        norm_factor_part, norm_factor_mesh, norm_factor_meshes
      );
    }
    trv::io::print_measurement_datatab_to_file(
      save_fileptr, params, meas_2pcf
    );
    std::fclose(save_fileptr);
  } else
  if (params.statistic_type == "2pcf-win") {
    std::snprintf(
      save_filepath, sizeof(save_filepath), "%s/xiw%d%s",
      params.measurement_dir.c_str(), params.ELL, params.output_tag.c_str()
    );

    trv::TwoPCFWindowMeasurements meas_2pcf_win = trv::compute_corrfunc_window(
      catalogue_rand, los_rand, params, binning, alpha, norm_factor
    );  // two-point correlation function window
    std::FILE* save_fileptr = std::fopen(save_filepath, "w");
    trv::io::print_measurement_header_to_file(
      save_fileptr, params, catalogue_rand,
      norm_factor_part, norm_factor_mesh, norm_factor_meshes
    );
    trv::io::print_measurement_datatab_to_file(
      save_fileptr, params, meas_2pcf_win
    );
    std::fclose(save_fileptr);
  } else
  if (params.statistic_type == "bispec") {
    if (params.form == "full" || params.form == "diag") {
      std::snprintf(
        save_filepath, sizeof(save_filepath), "%s/bk%d%d%d_%s%s",
        params.measurement_dir.c_str(),
        params.ell1, params.ell2, params.ELL,
        params.form.c_str(),
        params.output_tag.c_str()
      );
    } else
    if (params.form == "off-diag") {
      std::snprintf(
        save_filepath, sizeof(save_filepath), "%s/bk%d%d%d_offdiag%d%s",
        params.measurement_dir.c_str(),
        params.ell1, params.ell2, params.ELL, params.idx_bin,
        params.output_tag.c_str()
      );
    } else
    if (params.form == "row") {
      std::snprintf(
        save_filepath, sizeof(save_filepath), "%s/bk%d%d%d_row%d%s",
        params.measurement_dir.c_str(),
        params.ell1, params.ell2, params.ELL, params.idx_bin,
        params.output_tag.c_str()
      );
    }
    std::FILE* save_fileptr = nullptr;
    trv::BispecMeasurements meas_bispec;  // bispectrum
    if (params.catalogue_type == "survey") {
      meas_bispec = trv::compute_bispec(
        catalogue_data, catalogue_rand, los_data, los_rand,
        params, binning, norm_factor
      );
      save_fileptr = std::fopen(save_filepath, "w");
      trv::io::print_measurement_header_to_file(
        save_fileptr, params, catalogue_data, catalogue_rand,
        norm_factor_part, norm_factor_mesh, norm_factor_meshes
      );
    } else
    if (params.catalogue_type == "sim") {
      meas_bispec = trv::compute_bispec_in_gpp_box(
        catalogue_data, params, binning, norm_factor
      );
      save_fileptr = std::fopen(save_filepath, "w");
      trv::io::print_measurement_header_to_file(
        save_fileptr, params, catalogue_data,
        norm_factor_part, norm_factor_mesh, norm_factor_meshes
      );
    }
    trv::io::print_measurement_datatab_to_file(
      save_fileptr, params, meas_bispec
    );
    std::fclose(save_fileptr);
  } else
  if (params.statistic_type == "3pcf") {
    if (params.form == "full" || params.form == "diag") {
      std::snprintf(
        save_filepath, sizeof(save_filepath), "%s/zeta%d%d%d_%s%s",
        params.measurement_dir.c_str(),
        params.ell1, params.ell2, params.ELL,
        params.form.c_str(),
        params.output_tag.c_str()
      );
    } else
    if (params.form == "off-diag") {
      std::snprintf(
        save_filepath, sizeof(save_filepath), "%s/zeta%d%d%d_offdiag%d%s",
        params.measurement_dir.c_str(),
        params.ell1, params.ell2, params.ELL, params.idx_bin,
        params.output_tag.c_str()
      );
    } else
    if (params.form == "row") {
      std::snprintf(
        save_filepath, sizeof(save_filepath), "%s/zeta%d%d%d_row%d%s",
        params.measurement_dir.c_str(),
        params.ell1, params.ell2, params.ELL, params.idx_bin,
        params.output_tag.c_str()
      );
    }
    std::FILE* save_fileptr = nullptr;
    trv::ThreePCFMeasurements meas_3pcf;  // three-point correlation function
    if (params.catalogue_type == "survey") {
      meas_3pcf = trv::compute_3pcf(
        catalogue_data, catalogue_rand, los_data, los_rand,
        params, binning, norm_factor
      );
      save_fileptr = std::fopen(save_filepath, "w");
      trv::io::print_measurement_header_to_file(
        save_fileptr, params, catalogue_data, catalogue_rand,
        norm_factor_part, norm_factor_mesh, norm_factor_meshes
      );
    } else
    if (params.catalogue_type == "sim") {
      meas_3pcf = trv::compute_3pcf_in_gpp_box(
        catalogue_data, params, binning, norm_factor
      );
      save_fileptr = std::fopen(save_filepath, "w");
      trv::io::print_measurement_header_to_file(
        save_fileptr, params, catalogue_data,
        norm_factor_part, norm_factor_mesh, norm_factor_meshes
      );
    }
    trv::io::print_measurement_datatab_to_file(
      save_fileptr, params, meas_3pcf
    );
    std::fclose(save_fileptr);
  } else
  if (params.statistic_type == "3pcf-win") {
    if (params.form == "full" || params.form == "diag") {
      std::snprintf(
        save_filepath, sizeof(save_filepath), "%s/zetaw%d%d%d_%s%s",
        params.measurement_dir.c_str(),
        params.ell1, params.ell2, params.ELL,
        params.form.c_str(),
        params.output_tag.c_str()
      );
    } else
    if (params.form == "off-diag") {
      std::snprintf(
        save_filepath, sizeof(save_filepath), "%s/zetaw%d%d%d_offdiag%d%s",
        params.measurement_dir.c_str(),
        params.ell1, params.ell2, params.ELL, params.idx_bin,
        params.output_tag.c_str()
      );
    } else
    if (params.form == "row") {
      std::snprintf(
        save_filepath, sizeof(save_filepath), "%s/zetaw%d%d%d_row%d%s",
        params.measurement_dir.c_str(),
        params.ell1, params.ell2, params.ELL, params.idx_bin,
        params.output_tag.c_str()
      );
    }
    bool wa = false;

    trv::ThreePCFWindowMeasurements meas_3pcf_win = trv::compute_3pcf_window(
      catalogue_rand, los_rand, params, binning, alpha, norm_factor, wa
    );  // three-point correlation function window
    std::FILE* save_fileptr = std::fopen(save_filepath, "w");
    trv::io::print_measurement_header_to_file(
      save_fileptr, params, catalogue_rand,
      norm_factor_part, norm_factor_mesh, norm_factor_meshes
    );
    trv::io::print_measurement_datatab_to_file(
      save_fileptr, params, meas_3pcf_win
    );
    std::fclose(save_fileptr);
  } else
  if (params.statistic_type == "3pcf-win-wa") {
    if (params.form == "full" || params.form == "diag") {
      std::snprintf(
        save_filepath, sizeof(save_filepath), "%s/zetaw%d%d%d_wa%d%d_%s%s",
        params.measurement_dir.c_str(),
        params.ell1, params.ell2, params.ELL, params.i_wa, params.j_wa,
        params.form.c_str(),
        params.output_tag.c_str()
      );
    } else
    if (params.form == "off-diag") {
      std::snprintf(
        save_filepath, sizeof(save_filepath),
        "%s/zetaw%d%d%d_wa%d%d_offdiag%d%s",
        params.measurement_dir.c_str(),
        params.ell1, params.ell2, params.ELL, params.i_wa, params.j_wa,
        params.idx_bin,
        params.output_tag.c_str()
      );
    } else
    if (params.form == "row") {
      std::snprintf(
        save_filepath, sizeof(save_filepath), "%s/zetaw%d%d%d_wa%d%d_row%d%s",
        params.measurement_dir.c_str(),
        params.ell1, params.ell2, params.ELL, params.i_wa, params.j_wa,
        params.idx_bin,
        params.output_tag.c_str()
      );
    }

    bool wa = true;

    trv::ThreePCFWindowMeasurements meas_3pcf_win_wa =
      trv::compute_3pcf_window(
        catalogue_rand, los_rand, params, binning, alpha, norm_factor, wa
      );  // three-point correlation function window wide-angle corrections
    std::FILE* save_fileptr = std::fopen(save_filepath, "w");
    trv::io::print_measurement_header_to_file(
      save_fileptr, params, catalogue_rand,
      norm_factor_part, norm_factor_mesh, norm_factor_meshes
    );
    trv::io::print_measurement_datatab_to_file(
      save_fileptr, params, meas_3pcf_win_wa
    );
    std::fclose(save_fileptr);
  }

  if (params.save_binned_vectors != "") {
    trv::FieldStats binning_meshgrid(params, false);
    trv::BinnedVectors binned_vectors = binning_meshgrid.record_binned_vectors(
      binning, params.save_binned_vectors
    );
    if (params.statistic_type == "modes" || params.statistic_type == "pairs") {
      std::snprintf(
        save_filepath, sizeof(save_filepath), "%s",
        params.save_binned_vectors.c_str()
      );
    }
  }

  if (trv::sys::currTask == 0) {
    trv::sys::logger.info("Measurements saved to %s.", save_filepath);
  }

  // =====================================================================
  // C Finalisation
  // =====================================================================

  if (trv::sys::currTask == 0) {
    trv::sys::logger.stat("[C] Data objects are being cleared.");
  }

  // Clear persistent and dynamic memory.
#if defined(TRV_USE_OMP) && defined(TRV_USE_FFTWOMP)
  fftw_cleanup_threads();
#else  // !TRV_USE_OMP || !TRV_USE_FFTWOMP
  fftw_cleanup();
#endif  // TRV_USE_OMP && TRV_USE_FFTWOMP

  catalogue_data.finalise_particles();
  catalogue_rand.finalise_particles();

  if (los_data != nullptr) {
    delete[] los_data; los_data = nullptr;
  }
  if (los_rand != nullptr) {
    delete[] los_rand; los_rand = nullptr;
  }
  trv::sys::gbytesMem -= trv::sys::size_in_gb<struct trv::LineOfSight>(
    (catalogue_data.ntotal + catalogue_rand.ntotal)
  );

  if (trv::sys::count_fft > 0 || trv::sys::count_ifft > 0) {
    trv::sys::logger.info(
      "Number of FFTs: %d forward, %d backward.",
      trv::sys::count_fft, trv::sys::count_ifft
    );
  }

  if (trv::sys::currTask == 0) {
    trv::sys::logger.info(
      "Minimal estimate of peak memory usage: %.1f gigabytes.",
      trv::sys::gbytesMaxMem
    );
  }
  if (trv::sys::gbytesMem > 0.) {
    if (trv::sys::currTask == 0) {
      trv::sys::logger.warn(
        "Uncleared dynamically allocated memory: %.1f gigabytes.",
        trv::sys::gbytesMem
      );
    }
  }

  if (trv::sys::currTask == 0) {
    std::printf("%s\n", std::string(80, '<').c_str());
  }

  return 0;
}
