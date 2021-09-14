# UNIVER AUDIO

Audio engine for Univer Game Engine in C++ using FMOD API.

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
