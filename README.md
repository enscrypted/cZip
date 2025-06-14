# cZip

**cZip** is a C++ and Qt5-based file utility that provides a multi-stage pipeline for preparing files for secure transfer and storage. It combines compression, robust encryption, and steganographic concealment into a single, user-friendly graphical application. `cZip` is designed with performance and efficiency in mind, making it a powerful tool for privacy-conscious users.

## Core Features & Algorithms

The application provides three primary pairs of operations:

* **1. Compress / Decompress**:
    * Utilizes the zlib-based `qCompress` function integrated into the Qt framework.
    * To ensure optimal performance, the compression level is intelligently tiered based on file size: level 9 (maximum) is used for most files, which is reduced for files larger than 15GB to ensure speed.

* **2. Encrypt / Decrypt**:
    * **AES-256 (Recommended)**: A secure, industry-standard implementation provided by the Qt Cryptographic Architecture (QCA) framework. A 256-bit key and a 128-bit initialization vector are derived from a user-provided 96-character hex string.
    * **SimpleCrypt**: A custom, lightweight 64-bit key algorithm for basic, non-critical obfuscation.

* **3. Conceal / Reveal**:
    * Hides the processed file within the pixels of a PNG image. The application can either use a pre-existing image or generate a new one of an appropriate size.
    * **LSB (Least Significant Bit)**: A standard method that overwrites the least significant bit of each color value with the file's data.
    * **Seeded**: A proprietary variation of LSB where the order of bit placement within a pixel's color channels is determined by a user-provided key, preventing trivial data extraction.

## Getting Started: A Quick Example

The `cZip` workflow is sequential. The top-row options (**Compress, Encrypt, Conceal**) are executed from left to right. The bottom-row options (**Decompress, Decrypt, Reveal**) are executed in reverse order (right to left).

Here is a common use case: **Compressing, Encryping, and Concealing a file.**

1.  **Select Workflow**: Check the boxes for `Compress`, `Encrypt`, and `Conceal`. The relevant option panels will appear.
2.  **Choose Encryption**: In the "Encrypt" panel, select the `AES` radio button. Click the `Generate` button to create a new, secure key.
3.  **Choose Steganography**: In the "Conceal" panel, select the `LSB` radio button.
4.  **Select Files**:
    * In the main panel, click `Browse` next to "input file" to select the file you want to hide.
    * Click `Browse` next to "output folder" to choose where the final image will be saved.
5.  **Set Image Options**: In the "Conceal" panel, enter a name for the output image (e.g., `secret-image`). You can either generate a new image or select an existing one.
6.  **Execute**: Click the `Start` button. `cZip` will perform the operations and save the resulting image to your specified output folder.

## Installation

### From GitHub Releases (Recommended)

The easiest way to use `cZip` is to download the latest official release. Portable binaries for Windows, macOS, and Linux are available on the project's GitHub Releases page.

### Building from Source

If you wish to build the application yourself, you will need **Qt 5.15** and the **Qt Cryptographic Architecture (QCA)** library installed. Detailed build instructions for each platform can be found in their respective `INSTALL.md` files:

* **Windows**: See the instructions in `docker/windows/INSTALL.md`.
* **Linux**: See the instructions in `docker/linux/INSTALL.md`.
* **macOS**: See the instructions in `macos/INSTALL.md`.

## Repository Structure

For developers contributing to `cZip`, here is a brief overview of the repository layout:

* `src/`: Contains all C++ source code (`.cpp`, `.h`) and Qt UI files (`.ui`).
* `docker/`: Contains Dockerfiles and build scripts for creating Linux and Windows binaries.
* `macos/`: Contains build scripts specifically for compiling on macOS.
* `assets/`: Holds graphical assets, icons, and other resources used by the application.
* `README.md`: This file.

## License

`cZip` is free software distributed under the terms of the GNU Lesser General Public License. For more details, see the `LICENSE` file.
