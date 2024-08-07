# UNIVER AUDIO

Audio module developed in C++ for Univer Game Engine using FMOD API.

[![CMake-CI](https://github.com/VgTajdd/univer_audio/actions/workflows/cmake.yml/badge.svg)](https://github.com/VgTajdd/univer_audio/actions/workflows/cmake.yml)

![univer_logo](https://user-images.githubusercontent.com/51887591/218179773-13fd9fa4-677b-4306-ac1b-a27c4935f82b.png)

## Usage

Clone the repository using one of the following commands:

```bash
git clone https://github.com/vgtajdd/univer_audio.git       # HTTPS
git clone git@github.com:VgTajdd/univer_audio.git           # SSH
```

Configure the project (CMake):

```bash
cd univer_audio
cmake -S . -B build
```

Build the targets (CMake):

```bash
cmake --build build --config Debug -j
```

Run the example:

- For ```macOS``` and ```Linux```:

```bash
cd build/bin
./example
```

- For ```Windows```:

```bash
cd build/bin/Debug
example.exe
```

## Contributing

Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

## License

[Apache License 2.0](https://choosealicense.com/licenses/apache-2.0/)
