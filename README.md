

# Decaf-22

This project is an implementation of the lexer portion to tokenize files written with decaf syntax. The current iteration has very limited error logging.

## Installation

### Dependencies
 - cmake (>=3.2.0) 
    - Provided in source release is cmake-3.26.1-linux-x86_64


Use the provided build script to build the project. 

```bash
./workdir/build.sh
```

Build script will attempt to see if cmake is avilable and if not unpackage provided cmake tar binary to then use for subsequent build. The cmake linux-x86_64 binary has been tested on fox01 system on the UTSA network.

If provided cmake binary does fail to either be extracted, run, or configure project a fallback manual build is run during build script to attempt to build project, this process has also been tested on fox01 system on the UTSA network.

Binary is installed by CMake to both base source directory as well as into work dir to make `exec.sh` script run easily. Fallback build will build binary into source tree and copy it to workdir as well.

## Usage

Use the provide execution script to run, expects source file as parameter and outputs all information to stdout, can use redirection to write to file
```
# Run with output on stdout
./workdir/exec.sh <source_file>

# Run and save output to file
./workdir/exec.sh <source_file> > <output_file>

# Run binary directly with output on stdout
./decaf-22 <source_file>
```
