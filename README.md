<h3 align="center"><img src="img/logo.png" alt="logo" height="100px"></h3>

<p align="center">A tiny image to ascii tool written in C.</p>
  
<p align="center">
<a href="./LICENSE"><img src="https://img.shields.io/badge/license-MIT-blue.svg"></a>
</p>

Akscii is a tool that converts JPEG images into ASCII art. The name comes from the Farsi word "aksi" (meaning "picture") combined with ASCII, reflecting the tool’s purpose of transforming images into text. This project demonstrates low-level image processing by decompressing JPEG files entirely from scratch and logging detailed image data in a human-readable format.

## Table of Contents
- [Development](#Development)
- [Features](#Features)
- [Architecture](#Architecture)
- [Installation](#Installation)
- [Usage](#Usage)
- [Development](#Development)
- [License](#License)

## Development

Akscii is designed to be both a practical utility and a showcase of advanced programming skills. The tool performs the following tasks:

- [ ] JPEG Decompression: Reads and decompresses JPEG images from scratch.
  - [x] Decompress JPEG format into segments
  - [x] Collect Huffman tables
  - [x] Collect Quantization tables
  - [ ] Recontruct quantized data
  - [ ] Fill in color
  - [ ] Convert to RBG (or grayscale)
- [ ] ASCII Conversion: Translates the decompressed image data into ASCII art.
  - [ ] Basic
    - [ ] Convert grayscale to ascii
  - [ ] Advanced (tbd)
- [ ] Data Logging: Provides detailed, human-readable logs of the image data for debugging and analysis.
  - [x] Output basic information from decompressed Jpeg
  - [ ] Output verbose information during recontruction process

## Architecture

Akscii is organized into several key components:
- Image Loader: Reads the JPEG file and extracts raw image data.
- Decompression Engine: Implements the JPEG decompression algorithm from scratch.
- ASCII Converter: Maps image pixels to ASCII characters to create the final art.
- Logging Module: Provides comprehensive logging of the image data, offering insights into the decompression and conversion processes.

## Installation
### Prerequisites

- A C/C++ compiler (e.g., GCC, Clang)
- Standard build tools (e.g., Make)

### Build Instructions

Clone the repository and navigate into the project directory:

    git clone https://github.com/aniknia/Akscii.git
    cd Akscii

Compile the project using the provided Makefile:

    make

This will generate the akscii executable in the project directory.
Usage

To convert a JPEG image into ASCII art, run the executable with the image file as an argument:

    ./akscii path/to/image.jpg

The tool will output the ASCII art directly to the terminal. For detailed logging information, you can enable verbose mode (if implemented):

    ./akscii path/to/image.jpg --verbose

## Development

Akscii is an evolving project. Current development highlights include:

- Image Data Extraction: Successfully pulling raw JPEG data.
- Logging Functionality: In-progress work on a comprehensive logging module to present data in a human-readable format.

## License

This project is licensed under the MIT License. See the LICENSE file for details.

Thank you for exploring Akscii. Your feedback and contributions are greatly appreciated!
