![Build status](https://travis-ci.org/ob-algdatii-ss18/leistungsnachweis-ride-distributor.svg?branch=master)

# Ride distributor

Solution to the [problem statement](https://hashcode.withgoogle.com/2018/tasks/hashcode2018_qualification_task.pdf) of the HashCode online quallification round 2018.

*You can also check the output using this [tool](https://nikosk93.github.io/hashcode2018_grader/)*

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

## Results

The following table shows benchmarks from all versions of this project:
*(each of the columns links to the commit of that version)*

|                     |[Heuristic Beam Search]|[Best-first search (using points)]|[Best-first search (using times left + points)]|
|:--------------------|:---------------------:|:-------------------------------:|:--------------------------------------------:|
| a_example.in        |          10           |                8                |                      10                      |
| b_should_be_easy.in |        124373         |              168815             |                    176877                    |
| c_no_hurry.in       |        8747443        |              9133444            |                   14825711                   |
| d_metropolis.in     |        1708208        |              4403978            |                   3015856                    |
| e_high_bonus.in     |        2503182        |              3363963            |                   21465945                   |
| Total               |       13,083,216      |             17,070,208          |                  39,484,399                  |

[Heuristic Beam Search]: https://github.com/ob-algdatii-ss18/leistungsnachweis-ride-distributor/tree/f7e62f75816e76f720a8996a9e8c475ab3343053
[Best-first search (using points)]: https://github.com/ob-algdatii-ss18/leistungsnachweis-ride-distributor/tree/159ce4224aa3974086f4b4e08e330543555b7478
[Best-first search (using times left + points)]: https://github.com/ob-algdatii-ss18/leistungsnachweis-ride-distributor/tree/eb81ece48d004fe83487926999c8ef1079212033
