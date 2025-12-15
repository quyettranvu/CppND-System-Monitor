#include "system.h"

#include <unistd.h>

#include <cstddef>
#include <fstream>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "processor.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;
/*You need to complete the mentioned TODOs in order to satisfy the rubric
criteria "The student will be able to extract and display basic data about the
system."

You need to properly format the uptime. Refer to the comments mentioned in
format. cpp for formatting the uptime.*/

// TODO: Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// TODO: Return a container composed of the system's processes
vector<Process>& System::Processes() { return processes_; }

// TODO: Return the system's kernel identifier (string)
std::string System::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream("/proc/version");
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// TODO: Return the system's memory utilization
float System::MemoryUtilization() {
  string key, value, unit;
  string line;
  float total_memory = 0.0, free_memory = 0.0, buffers = 0.0, cached = 0.0;
  std::ifstream stream("/proc/meminfo");
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value >> unit;
      if (key == "MemTotal:")
        total_memory = std::stof(value);
      else if (key == "MemFree:")
        free_memory = std::stof(value);
      else if (key == "Buffers:")
        buffers += std::stof(value);
      else if (key == "Cached:")
        cached += std::stof(value);
    }
  }
  return (total_memory - free_memory - buffers - cached) / total_memory;
}

// TODO: Return the operating system name
std::string System::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream("/etc/os-release");
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), '\"', ' ');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return string();
}

// TODO: Return the number of processes actively running on the system
int System::RunningProcesses() {
  string line;
  string key;
  string value;
  std::ifstream stream("/proc/stat");
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") return std::stoi(value);
      }
    }
  }

  return 0;
}

// TODO: Return the total number of processes on the system
int System::TotalProcesses() {
  string line;
  string key;
  string value;
  std::ifstream stream("/proc/stat");
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") return std::stoi(value);
      }
    }
  }
  return 0;
}

// TODO: Return the number of seconds since the system started running
long int System::UpTime() {
  string line;
  long int uptime = 0;
  std::ifstream stream("/proc/uptime");
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
    return uptime;
  }
  return 0;
}
