# Ride distributor
<img src="https://travis-ci.org/ob-algdatii-ss18/leistungsnachweis-ride-distributor.svg?branch=master" style="float: left;margin-right:1em;">
<img src="https://coveralls.io/repos/github/ob-algdatii-ss18/leistungsnachweis-ride-distributor/badge.svg?branch=master" style="float: left;">
<br>

Solution to the [problem statement](https://hashcode.withgoogle.com/2018/tasks/hashcode2018_qualification_task.pdf) of the HashCode online quallification round 2018.

## Setup
* Create Workspace
```
cmake . -B".build" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Debug
```

* Create Executable (and documentation)
```
cmake --build .build --target all
cmake --build .build --target doc
```

* Unit Tests (first create executable)
```
cd .build && ctest -VV
```