# Branchboozle

This repository contains all the source code used in the paper "**Branchboozle**: A Side-Channel Within a Hidden Pattern History Table of Modern Branch Prediction Units", which has been accepted by The 36th ACM/SIGAPP Symposium on Applied Computing.


## Directories
- `Branchboozle`: Directory that contains all the corresponding code to target a hidden 3-bit PHT in modern CPUs.
- `BranchScope`: Directory with experiments regarding the 2-bit PHT.
- `kernel-pmc`: Simple kernel module that configures Performance Monitoring Counters for experimentation.
- `libpht2b`:  Auxiliary library to implement **BranchScope**'s `randomize_pht` function.
- `libfr`: Auxiliary library to implement **Flush+Reload**, primarily used to demonstrate **Branchboozle**-based speculation attacks.

## Building the project

From the project's root directory, simply running `make` will compile all the experiments targeting Intel's Skylake micro-architecture. If you want to compile the project for another micro-architecture, simply specify it via `make`'s first argument, e.g., `make zen`. Supported micro-architectures are:
- Intel's `ivybridge`, `sandybridge`, `haswell`, `skylake`, `kabylake`, `coffeelake`.
- AMD's `zen`.

## Usage

Most experiments from both **BranchScope** and **Branchboozle** use PMCs that need to be configured from the kernel-space. From the `kernel-pmc` directory, one must first run the `./install-module.sh` script with root privileges to install the kernel module, which will automatically configure the corresponding PMCs.

Each directory in `BranchScope` and `Branchboozle` contains an individual experiment. To run a experiment simply run `./start.sh` from the experiment's folder.

After experimenting, from the `kernel-pmc` directory simply run `./uninstall-module.sh` to remove the kernel module from your system.
