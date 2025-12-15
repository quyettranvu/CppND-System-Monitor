#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  string line;
  string key;
  string value;
  float memTotal = 0.0;
  float memFree = 0.0;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "MemTotal:") {
        memTotal = stof(value);
      } else if (key == "MemFree:") {
        memFree = stof(value);
      } else if (key == "Buffers:") {
        memFree += stof(value);
      } else if (key == "Cached:") {
        memFree += stof(value);
      } else if (key == "SReclaimable:") {
        memFree += stof(value);
      } else if (key == "Shmem:") {
        memFree += stof(value);
      } else if (key == "MemAvailable:") {
        return (memTotal - memFree) / memTotal;
      } else if (key == "SwapTotal:") {
        return 0.0;
      } else if (key == "SwapFree:") {
        return 0.0;
      } else {
        continue;
      }
    }
  }
  return 0.0; }

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
  string line;
  long uptime = 0;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
    return static_cast<long>(uptime);
  }
  return 0; 
}

// TODO: Read and return the number of jiffies for the system: tương ứng với số tick của timer từ khi hệ thống boot
long LinuxParser::Jiffies() { 
  string line;
  long jiffies = 0;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> jiffies;
    return static_cast<long>(jiffies);
  }
  return 0; }

// TODO: Read and return the number of active jiffies for a PID
// Active Jiffies for a PID = utime + stime + cutime + cstime
long LinuxParser::ActiveJiffies(int pid) {
  string line;
  string value;
  long activeJiffies = 0;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    for (int i = 0; i < 13; i++) {
      linestream >> value;
    }
    long utime, stime, cutime, cstime;
    linestream >> utime >> stime >> cutime >> cstime;
    activeJiffies = utime + stime + cutime + cstime;
    return static_cast<long>(activeJiffies);
  }
  return 0; 
}

// TODO: Read and return the number of active jiffies for the system
// Active Jiffies = user + nice + system + irq + softirq + steal
long LinuxParser::ActiveJiffies() { 
  string line;
  string value;
  long activeJiffies = 0;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::string cpu;
    long user, nice, system, irq, softirq, steal;
    filestream >> cpu >> user >> nice >> system >> irq >> softirq >> steal;
    activeJiffies = user + nice + system + irq + softirq + steal;
    return static_cast<long>(activeJiffies);
  }
  return 0; 
}

// TODO: Read and return the number of idle jiffies for the system
// Idle jiffies = irq + softirq + steal
long LinuxParser::IdleJiffies() { 
  string line;
  string value;
  long idleJiffies = 0;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::string cpu;
    long user, nice, system, irq, softirq, steal;
    filestream >> cpu >> user >> nice >> system >> irq >> softirq >> steal;
    idleJiffies = irq + softirq + steal;
    return static_cast<long>(idleJiffies);
  }
  return 0; 
}

// TODO: Read and return CPU utilization: skip cpu, take the rest in first line of /proc/stat
vector<string> LinuxParser::CpuUtilization() { 
  string line;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line); 
    std::istringstream linestream(line);
    string cpu;
    vector<string> cpu_values;
    linestream >> cpu;
    string value;
    while (linestream >> value) {
      cpu_values.push_back(value);
    }
    return cpu_values;
  }
  return {}; 
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "processes") {
        return stoi(value);;
      }
    }
  }
  return 0; 
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "procs_running") {
        return stoi(value);
      }
    }
  }
  return 0; 
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) { 
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (filestream.is_open()) {
    std::string command;
    std::getline(filestream, command);
    return command;
  }
  return string(); 
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid[[maybe_unused]]) { 
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "VmSize:") {
        return value;
      }
    }
  }
  return string();
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid[[maybe_unused]]) { return 0; }
