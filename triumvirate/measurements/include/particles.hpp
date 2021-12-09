#ifndef TRIUM_PARTICLES_H_INCLUDED_
#define TRIUM_PARTICLES_H_INCLUDED_

#ifndef TRIUM_PARAMETERS_H_INCLUDED_
#include "parameters.hpp"
#endif

/**
 * Container of particle data and summary information.
 *
 */
class ParticleCatalogue {
 public:
  struct ParticleData {
    double pos[3];  ///< particle position vector
    double w;  ///< particle weight
  }* particles;  ///< particle data

  int nparticles;  ///< total number of particles
  double pos_min[3];  ///< minimum values of particle positions
  double pos_max[3];  ///< maximum values of particle positions

  /**
   * Return individual particle information.
   *
   * @param id Particle ID/index.
   * @returns Particle information.
   */
  ParticleData& operator[](int id) {
    return this->particles[id];
  }

  /**
   * Initialise the particle container.
   */
  ParticleCatalogue () {
    this->particles = NULL;
    this->nparticles = 0;
    this->pos_min[0] = 0.; this->pos_max[0] = 0.;
    this->pos_min[1] = 0.; this->pos_max[1] = 0.;
    this->pos_min[2] = 0.; this->pos_max[2] = 0.;
  }

  /**
   * Destruct the particle container.
   */
  ~ParticleCatalogue() {
    finalise_particles();
  }

  /**
   * Initialise particle data and container information.
   *
   * @param num Number of particles.
   */
  void initialise_particles(const int num) {
    /// Check the total number of particles.
    if (num <= 0) {
      printf("[Warning] :: Number of particles is negative.\n");
      return;
    }
    this->nparticles = num;

    /// Renew particle data.
    delete[] this->particles; this->particles = NULL;
    this->particles = new ParticleData[this->nparticles];

    /// Determine memory usage.
    bytes += double(this->nparticles) * sizeof(struct ParticleData)
      / 1024. / 1024. / 1024.;

    /// Initialise particle data values.
    for (int id = 0; id < this->nparticles; id++) {
      this->particles[id].pos[0] = 0.;
      this->particles[id].pos[1] = 0.;
      this->particles[id].pos[2] = 0.;
      this->particles[id].w = 0.;
    }
  }

  /**
   * Finalise particle data and information.
   */
  void finalise_particles() {
    /// Free particle usage.
    if (this->particles != NULL) {
      delete[] this->particles; this->particles = NULL;
      bytes -= double(this->nparticles) * sizeof(struct ParticleData)
        / 1024. / 1024. / 1024.;
    }
  }

  /**
   * Read in particle data from a file.
   *
   * @param particles_file (Referene to) the input file path.
   * @returns Exit status.
   */
  int read_particles_catalogue(std::string& particles_file) {
    /// Initialise the counter for the number of lines/particles.
    int num_lines = 0;

    /// Check and size up data from the file.
    std::ifstream fin;
    fin.open(particles_file.c_str(), std::ios::in);

    if (fin.fail()) {
      printf("[Error] :: Cannot open file '%s'.\n", particles_file.c_str());
      fin.close();
      return -1;
    }

    std::string str_line;
    double x, y, z, w;
    while (getline(fin, str_line)) {
      /// Check if the line conforms to the expected format.
      if (sscanf(str_line.c_str(), "%lf %lf %lf %lf", &x, &y, &z, &w) != 4) {
        continue;
      }

      /// Count the line as a particle.
      num_lines++;
    }

    fin.close();

    /// Fill in particle data.
    this->initialise_particles(num_lines);

    fin.open(particles_file.c_str(), std::ios::in);

    num_lines = 0;
    while (getline(fin, str_line)) {
      /// Check if the line conforms to the expected format.
      if (sscanf(str_line.c_str(), "%lf %lf %lf %lf", &x, &y, &z, &w) != 4) {
        continue;
      }

      /// Add the current line as a particle.
      this->particles[num_lines].pos[0] = x;
      this->particles[num_lines].pos[1] = y;
      this->particles[num_lines].pos[2] = z;
      this->particles[num_lines].w = w;

      num_lines++;
    }

    fin.close();

    return 0;
  }

  /**
   * Test the read-in of particle data from a file.
   *
   * @param particles_file (Reference to) the nput file path.
   * @returns Exit status.
   */
  int read_particles_test(std::string& particles_file) {
    /// Initialise the counter for the number of lines/particles.
    int num_lines = 0;

    /// Read in data from the file.
    std::ifstream fin;
    fin.open(particles_file.c_str(), std::ios::in);

    if (fin.fail()) {
      printf("[Error] :: Cannot open file '%s'.\n", particles_file.c_str());
      fin.close();
      return -1;
    }

    std::string str_line;
    double x, y, z;
    while (getline(fin, str_line)) {
      if (sscanf(str_line.c_str(), "%lf %lf %lf", &x, &y, &z) != 3) {
        continue;
      }
      num_lines++;
    }

    fin.close();

    /// Fill in particle data.
    this->initialise_particles(num_lines);

    fin.open(particles_file.c_str(), std::ios::in);

    num_lines = 0;
    while (getline(fin, str_line)) {
      if (sscanf(str_line.c_str(), "%lf %lf %lf", &x, &y, &z) != 3) {
        continue;
      }

      this->particles[num_lines].pos[0] = x;
      this->particles[num_lines].pos[1] = y;
      this->particles[num_lines].pos[2] = z;
      this->particles[num_lines].w = 1.;

      num_lines++;
    }

    fin.close();

    /// Calculate extreme data values.
    this->calc_min_and_max();

    return 0;
  }

  /**
   * Calculate extreme data values.
   *
   * @returns Exit status.
   */
  int calc_min_and_max() {
    if (this->particles == NULL) {
      return -1;
    }

    /// Initialise minimum and maximum values with the first
    /// data entry/row.
    double min[3], max[3];

    min[0] = this->particles[0].pos[0]; max[0] = this->particles[0].pos[0];
    min[1] = this->particles[0].pos[1]; max[1] = this->particles[0].pos[1];
    min[2] = this->particles[0].pos[2]; max[2] = this->particles[0].pos[2];

    /// Find/update minimum and maximum values line-by-line.
    for (int id = 0; id < this->nparticles; id++) {
      if (min[0] > this->particles[id].pos[0]) {
        min[0] = this->particles[id].pos[0];
      }
      if (min[1] > this->particles[id].pos[1]) {
        min[1] = this->particles[id].pos[1];
      }
      if (min[2] > this->particles[id].pos[2]) {
        min[2] = this->particles[id].pos[2];
      }

      if (max[0] < this->particles[id].pos[0]) {
        max[0] = this->particles[id].pos[0];
      }
      if (max[1] < this->particles[id].pos[1]) {
        max[1] = this->particles[id].pos[1];
      }
      if (max[2] < this->particles[id].pos[2]) {
        max[2] = this->particles[id].pos[2];
      }
    }

    this->pos_min[0] = min[0]; this->pos_max[0] = max[0];
    this->pos_min[1] = min[1]; this->pos_max[1] = max[1];
    this->pos_min[2] = min[2]; this->pos_max[2] = max[2];

    return 0;
  }

  /**
   * Calculate the alpha ratio (of weighted number counts or
   * number densities).
   *
   * @param particles_data (Reference to) the data-source particle container.
   * @param particles_rand (Reference to) the random-source particle container.
   * @returns alpha Alpha ratio.
   */
  static double calc_alpha_ratio(
      ParticleCatalogue& particles_data, ParticleCatalogue& particles_rand
    ) {
    double num_wgt_data = 0.;
    for (int id = 0; id < particles_data.nparticles; id++) {
      num_wgt_data += particles_data[id].w;
    }

    double num_wgt_rand = 0.;
    for (int id = 0; id < particles_rand.nparticles; id++) {
      num_wgt_rand += particles_rand[id].w;
    }

    double alpha = num_wgt_data / num_wgt_rand;

    return alpha;
  }

  /**
   * Calculate power spectrum normalisation in the Sugiyama convention.
   *
   * @param particles_data (Reference to) the data-source particle container.
   * @param survey_vol_norm Survey volume normalisation constant.
   * @returns norm Normalisation factor.
   */
  static double calc_norm_for_power_spec_sugiyama(
      ParticleCatalogue& particles_data, double survey_vol_norm
    ) {
    double num_wgt_data = 0.;
    for (int id = 0; id < particles_data.nparticles; id++) {
      num_wgt_data += particles_data[id].w;
    }

    double norm = survey_vol_norm / num_wgt_data / num_wgt_data;
      // equivalent to 1/I_2 where I_2 = ∫d^3x \bar{n}(x)^2

    return norm;
  }

  /**
   * Calculate power spectrum normalisation in the 'nbodykit' convention.
   *
   * @param particles_data (Reference to) the data-source particle container.
   * @param survey_vol_norm Survey volume normalisation constant.
   * @returns norm Normalisation factor.
   */
  static double calc_norm_for_power_spec_nbodykit(
      ParticleCatalogue& particles_rand,
      ParameterSet& params
    ) {

    if (thisTask == 0) {
      printf(
        "[WARN] :: 'nbodykit' normalisation convention not yet implemented.\n"
      );
    }

    return -1;
  }

  /**
   * Calculate bispectrum normalisation.
   *
   * @param particles_data (Reference to) the data-source particle container.
   * @param survey_vol_norm Survey volume normalisation constant.
   * @returns norm Normalisation factor.
   */
  static double calc_norm_for_bispec(
      ParticleCatalogue& particles_data, double survey_vol_norm
    ) {
    double num_wgt_data = 0.;
    for (int id = 0; id < particles_data.nparticles; id++) {
      num_wgt_data += particles_data[id].w;
    }

    double norm = survey_vol_norm / num_wgt_data / num_wgt_data;

    norm *= survey_vol_norm / num_wgt_data;
      // equivalent to I_3 = ∫d^3x \bar{n}(x)^3

    return norm;
  }

  /**
   * Offset particle positions by a given vector to subtract.
   *
   * @param dpos Position offset vector (subtractive).
   * @returns Exit status.
   */
  int offset_particles(const double* dpos) {
    if (this->particles == NULL) {
      return -1;
    }

    for (int id = 0; id < this->nparticles; id++) {
      this->particles[id].pos[0] -= dpos[0];
      this->particles[id].pos[1] -= dpos[1];
      this->particles[id].pos[2] -= dpos[2];
    }

    return 0;
  }

  /**
   * Offset particle positions for FFT (by grid adjustment).
   *
   * @param particles_data (Reference to) the data-source particle container.
   * @param particles_rand (Reference to) the random-soruce particle data.
   * @param params (Reference to) the input parameter set.
   * @param factor Offset grid adjustment factor (default is 3.).
   * @returns Exit status.
   */
  static int offset_particles_for_fft(
      ParticleCatalogue& particles_data,
      ParticleCatalogue& particles_rand,
      ParameterSet& params,
      double factor=3.  // NOTE: discretionary choice
    ) {
    particles_data.calc_min_and_max();
    particles_rand.calc_min_and_max();

    /// Re-adjust the grid.
    double dpos[3] = {
      particles_rand.pos_min[0],
      particles_rand.pos_min[1],
      particles_rand.pos_min[2]
    };

    dpos[0] -= factor * params.boxsize[0] / double(params.nmesh[0]);
    dpos[1] -= factor * params.boxsize[1] / double(params.nmesh[1]);
    dpos[2] -= factor * params.boxsize[2] / double(params.nmesh[2]);

    particles_data.offset_particles(dpos);
    particles_rand.offset_particles(dpos);

    /// Recalculate extreme data values.
    particles_data.calc_min_and_max();
    particles_rand.calc_min_and_max();

    return 0;
  }

  /**
   * Offset particle positions for window function calculations (by
   * centring them inside the specified box).
   *
   * @param params (Reference to) the input parameter set.
   * @returns Exit status.
   */
  int offset_particles_for_window(ParameterSet& params) {
    double xmid = this->pos_min[0]
      + (this->pos_max[0] - this->pos_min[0]) / 2.;
    double ymid = this->pos_min[1]
      + (this->pos_max[1] - this->pos_min[1]) / 2.;
    double zmid = this->pos_min[2]
      + (this->pos_max[2] - this->pos_min[2]) / 2.;

    double dx[3] = {0., 0., 0.};
    dx[0] = params.boxsize[0]/2. - xmid;
    dx[1] = params.boxsize[1]/2. - ymid;
    dx[2] = params.boxsize[2]/2. - zmid;

    for (int id = 0; id < this->nparticles; id++) {
      for (int axis = 0; axis < 3; axis++) {
        this->particles[id].pos[axis] += dx[axis];
      }
    }

    /// Recalculate extreme data values.
    this->calc_min_and_max();

    return 0;
  }

  /**
   * Offset particle positions for periodic boundary conditions.
   *
   * @param params (Reference to) the input parameter set.
   * @returns Exit status.
   */
  int offset_particles_for_periodicity(ParameterSet& params) {
    for (int id = 0; id < this->nparticles; id++) {
      for (int axis = 0; axis < 3; axis++) {
        if (this->particles[id].pos[axis] >= params.boxsize[axis]) {
          this->particles[id].pos[axis] -= params.boxsize[axis];
        } else if (this->particles[id].pos[axis] < 0.) {
          this->particles[id].pos[axis] += params.boxsize[axis];
        }
      }
    }

    /// Recalculate extreme data values.
    this->calc_min_and_max();

    return 0;
  }
};

#endif
