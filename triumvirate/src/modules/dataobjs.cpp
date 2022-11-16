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
 * @file dataobjs.cpp
 * @authors Mike S Wang (https://github.com/MikeSWang)
 *          Naonori Sugiyama (https://github.com/naonori)
 *
 */

#include "dataobjs.hpp"

namespace trv {

/// **********************************************************************
/// Binning schemes
/// **********************************************************************

Binning::Binning(double coord_min, double coord_max, int nbin) {
  if (coord_min < 0.) {
    throw trv::sys::InvalidParameter("Binning range must be non-negative.");
  }

  this->bin_min = coord_min;
  this->bin_max = coord_max;
  this->num_bins = nbin;
}

Binning::Binning(trv::ParameterSet& params) : Binning::Binning(
  params.bin_min, params.bin_max, params.num_bins
) {
  this->scheme = params.binning;
  this->space = params.space;

  /// Change default padding to grid scale.
  this->dbin_pad_config = (1 + 5.e-3)
    * *std::max_element(params.boxsize, params.boxsize + 3)
    / *std::min_element(params.ngrid, params.ngrid + 3);
  this->dbin_pad_fourier = (1 + 5.e-3)
    * (2 * M_PI)
    / *std::max_element(params.boxsize, params.boxsize + 3);
}

void Binning::set_bins(std::string scheme, std::string space) {
  /// Set up padding parameters.
  double dbin_pad;
  if (space == "fourier") {
    dbin_pad = this->dbin_pad_fourier;
  } else
  if (space == "config") {
    dbin_pad = this->dbin_pad_config;
  }

  /// Implement binning scheme.
  /// --------------------------------------------------------------------
  /// Customised binning
  /// --------------------------------------------------------------------
  if (scheme == "custom") {
    /// Insert customised binning code here.
    throw trv::sys::UnimplementedError(
      "Customed binning not implemented. "
      "Implement your own binning scheme here (\"dataobjs.cpp\")."
    );
  } else
  /// --------------------------------------------------------------------
  /// Linear binning
  /// --------------------------------------------------------------------
  if (scheme == "lin") {
    double dbin = (this->bin_max - this->bin_min) / double(this->num_bins);

    for (int ibin = 0; ibin < this->num_bins; ibin++) {
      double edge_left = this->bin_min + dbin * ibin;
      double centre = edge_left + dbin / 2.;

      this->bin_edges.push_back(edge_left);
      this->bin_centres.push_back(centre);
      this->bin_widths.push_back(dbin);
    }
    this->bin_edges.push_back(this->bin_max);
  } else
  /// --------------------------------------------------------------------
  /// Logarithmic binning
  /// --------------------------------------------------------------------
  if (scheme == "log")  {
    if (this->bin_min == 0.) {
      throw trv::sys::InvalidParameter(
        "Cannot use logarithmic binning when the lowest edge is zero."
      );
    }

    double dlnbin = (std::log(this->bin_max) - std::log(this->bin_min))
      / double(this->num_bins);

    for (int ibin = 0; ibin < this->num_bins; ibin++) {
      double edge_left = this->bin_min * std::exp(dlnbin * ibin);
      double edge_right = this->bin_min * std::exp(dlnbin * (ibin + 1));
      double centre = (edge_left + edge_right) / 2.;

      this->bin_edges.push_back(edge_left);
      this->bin_centres.push_back(centre);
      this->bin_widths.push_back(edge_right - edge_left);
    }
    this->bin_edges.push_back(this->bin_max);
  } else
  /// --------------------------------------------------------------------
  /// Padded linear binning
  /// --------------------------------------------------------------------
  if (scheme == "linpad") {
    for (int ibin = 0; ibin < this->nbin_pad; ibin++) {
      double edge_left = dbin_pad * ibin;
      double centre = edge_left + dbin_pad / 2.;

      this->bin_edges.push_back(edge_left);
      this->bin_centres.push_back(centre);
      this->bin_widths.push_back(dbin_pad);
    }

    double bin_min = dbin_pad * this->nbin_pad;

    double dbin = (this->bin_max - bin_min)
      / double(this->num_bins - this->nbin_pad);

    for (int ibin = this->nbin_pad; ibin < this->num_bins; ibin++) {
      double edge_left = bin_min + dbin * (ibin - this->nbin_pad);
      double centre = edge_left + dbin / 2.;

      this->bin_edges.push_back(edge_left);
      this->bin_centres.push_back(centre);
      this->bin_widths.push_back(dbin);
    }
    this->bin_edges.push_back(this->bin_max);
  } else
  /// --------------------------------------------------------------------
  /// Padded logarithmic binning
  /// --------------------------------------------------------------------
  if (scheme == "logpad") {
    for (int ibin = 0; ibin < this->nbin_pad; ibin++) {
      double edge_left = dbin_pad * ibin;
      double centre = edge_left + dbin_pad / 2.;

      this->bin_edges.push_back(edge_left);
      this->bin_centres.push_back(centre);
      this->bin_widths.push_back(dbin_pad);
    }

    double bin_min = dbin_pad * this->nbin_pad;

    double dlnbin = (std::log(this->bin_max) - std::log(bin_min))
      / double(this->num_bins - this->nbin_pad);

    for (int ibin = this->nbin_pad; ibin < this->num_bins; ibin++) {
      double edge_left =
        bin_min * std::exp(dlnbin * (ibin - this->nbin_pad));
      double edge_right =
        bin_min * std::exp(dlnbin * (ibin - this->nbin_pad + 1));
      double centre = (edge_left + edge_right) / 2.;

      this->bin_edges.push_back(edge_left);
      this->bin_centres.push_back(centre);
      this->bin_widths.push_back(edge_right - edge_left);
    }
    this->bin_edges.push_back(this->bin_max);
  } else {
    throw trv::sys::InvalidParameter("Invalid binning `scheme`: %s.", scheme);
  }
}

void Binning::set_bins() {Binning::set_bins(this->scheme, this->space);}


/// **********************************************************************
/// Line of sight
/// **********************************************************************

/// No definition.

/// **********************************************************************
/// Clustering statistics
/// **********************************************************************

}  // namespace trv
