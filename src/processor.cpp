#include "processor.h"

#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
  std::vector<std::string> cpu = LinuxParser::CpuUtilization();
  if (cpu.size() < 10) return 0.0;
  float user = std::stof(cpu[0]);
  float nice = std::stof(cpu[1]);
  float system = std::stof(cpu[2]);
  float idle = std::stof(cpu[3]);
  float iowait = std::stof(cpu[4]);
  float irq = std::stof(cpu[5]);
  float softirq = std::stof(cpu[6]);
  float steal = std::stof(cpu[7]);
//   float guest = std::stof(cpu[8]);
//   float guest_nice = std::stof(cpu[9]);

  // Current jiffies
  float idle_jiffies = idle + iowait;
  float non_idle_jiffies = user + nice + system + irq + softirq + steal;
  float total_jiffies = idle_jiffies + non_idle_jiffies;

  // Compute deltas from previous jiffies
  long totald = total_jiffies - prev_total_;
  long idled = idle_jiffies - prev_idle_;

  prev_total_ = total_jiffies;
  prev_idle_ = idle_jiffies;

  return totald > 0 ? float(totald - idled) / static_cast<float>(totald) : 0.0;
}