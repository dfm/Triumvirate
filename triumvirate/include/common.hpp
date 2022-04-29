/**
 * @file common.hpp
 * @brief Program global variables.
 *
 */

#ifndef TRIUMVIRATE_INCLUDE_COMMON_HPP_INCLUDED_
#define TRIUMVIRATE_INCLUDE_COMMON_HPP_INCLUDED_

#include <chrono>
#include <cstdarg>
#include <cstring>
#include <ctime>
#include <stdexcept>
#include <string>

/// Initialise program trackers.
/// RFE: Sort out MPI tasks.

int currTask = 0;  ///< current task
int numTasks = 1;  ///< number of tasks (in a batch)

const double BYTES_PER_GBYTES = 1024. * 1024. * 1024.;  ///< bytes per gibibyte
double gbytesMem = 0.;  ///< memory usage in gibibytes

double clockStart;  ///< program start clock
double clockElapsed;  ///< program elapsed clock

/// Provide program utilities.

class InvalidParameter: public std::invalid_argument {
 public:
  std::string err_mesg;

  InvalidParameter(const char* fmt_string, ...): std::invalid_argument(
    "Invalid parameter error."  // default error message; essential
  ) {
    va_list args;
    char err_mesg_buf[4096];

    va_start(args, fmt_string);
    vsprintf(err_mesg_buf, fmt_string, args);
    va_end(args);

    this->err_mesg = std::string(err_mesg_buf);
  }

  virtual const char* what() const noexcept {
    return err_mesg.c_str();
  }
};

/**
 * Return string showing the current date-time in "YYYY-MM-DD HH:MM:SS"
 * format.
 *
 * @returns timestamp Timestamp string.
 */
std::string show_current_datetime() {
  auto now = std::chrono::system_clock::now();
  auto timenow = std::chrono::system_clock::to_time_t(now);

  char buffer[64];
  std::strftime(
    buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&timenow)
  );

  std::string timestamp = std::string(buffer);

  return timestamp;
}

/**
 * Return string showing elapsed time in "HH:MM:SS" format.
 *
 * @param clock_duration Clock time duration.
 * @returns elapsed_time Elapsed-time string.
 */
std::string show_elapsed_time(double clock_duration) {
  int time = int(clock_duration / CLOCKS_PER_SEC);

  std::string h = std::to_string(time / 3600);
  std::string m = std::to_string((time % 3600) / 60);
  std::string s = std::to_string(time % 60);

  std::string hh;
  if (h.length() < 2) {
    hh = std::string(2 - h.length(), '0') + h;
  } else {
    hh = h;
  }

  std::string mm = std::string(2 - m.length(), '0') + m;
  std::string ss = std::string(2 - s.length(), '0') + s;

  std::string elapsed_time = hh + ":" + mm + ":" + ss;

  return elapsed_time;
}

/**
 * Return logging string showing the timestamp including the elapsed time.
 *
 * @param elapsed_time Elapsed time.
 * @returns timestamp Timestamp string.
 */
std::string show_timestamp() {
  double elapsed_time = double(clock() - clockStart);

  char timestamp_[128];
  sprintf(
    timestamp_, "%s (+%s)",
    show_current_datetime().c_str(), show_elapsed_time(elapsed_time).c_str()
  );

  std::string timestamp(timestamp_);

  return timestamp;
}

/**
 * Check if a path is set (after trimming).
 *
 * This is used for checking whether a path is set.
 *
 * @param pathstr
 * @returns true, false
 */
bool if_path_is_set(std::string pathstr){
  /// Check if the string is empty.
  if (pathstr.empty()) {return false;}

  /// Check if the string contains non-whitespace characters.  If so,
  /// the path is set, otherwise not.
  for (int ichar = 0; ichar < pathstr.length(); ichar++){
    if (!std::isspace(pathstr[ichar])) {return true;}
  }
  return false;
}

#endif  // TRIUMVIRATE_INCLUDE_COMMON_HPP_INCLUDED_
