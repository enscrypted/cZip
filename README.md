# cZip

cZip is designed to provide a simple, portable, effective, C++ based method for preparing files to be transferred/stored undetected. cZip offers a collection of compression, encryption, and steganography algorithms which can be used together (or individually) to produce an image carrying the bit information of an encrypted file. The program is delivered to the user in a simple, easy to read GUI provided by the Qt framework (Qt5 specifically). 

## Installation

#### Windows
To run on windows, simply download the [cZipWin folder](https://github.com/enscrypted/cZip/tree/main/cZipWin) and extract. Then, navigate into the release folder and run czip.exe

#### Linux
A prebuild AppImage is available to download in the [cZipLin folder](https://github.com/enscrypted/cZip/tree/main/cZipLin). Download czip.AppImage, then run "chmod a+x czip.AppImage" from a terminal in the directory the AppImage is downloaded in (note, installing FUSE might be necessary prior to this). If building the AppImage is preferable, an unbuilt version is available in cZippApp.zip. Extracting this file provides a folder architecture suitable for AppImage creation.

#### From Source
To build from source, download the src folder. cZip requires both Qt 5.15 and QCA. Qt is available from [their website](https://qt.io), and QCA is available [here](https://api.kde.org/qca/html/). Both requirements contain excellent documentation for installation. Once everything necessary is installed, simply compile the source.

## Usage

Six main option checkboxes are present at startup: compress, decompress, encrypt, decrypt, conceal, and reveal. The options are split into two rows, and only one row of options can be selected at a time. The options in the top row are executed in left to right order, and the bottom row are in reverse. Upon choosing any selection, any relevant options hidden are revealed, such as algorithm selection for encryption. A user need only browse their computer for an input file and output directory using the given input fields. If concealing a file into an image, one must also input a name for the output image, as well as a source image if that option is desired.

## Technical Details

The three main pairs of instructions are:

#### 1. Compress/Decompress:
File compression for cZip uses the qCompress function built into Qt. qCompress uses gZip for compression, offering 9 different compression rates. The bulk of files run through cZip will likely use the highest rate (9), but support for large files (over 15GB) sees the rate drop down incrementally to 3 in order to ensure acceptable program speeds.
#### 2.Encrypt/Decrypt:
The user is given two options for encryption: SimpleCrypt and AES256. SimpleCrypt is designed for low security files, offering protection with a 64 bit key. The user can enter a key themself or generate one with the provided button. Keys must be provided in hexadecimal format. Upon program start, the file is first compressed (to minimize encryption time, all files must be compressed first), then ran through SimpleCrypt to produce a .pcrypt file of the same name. AES uses a standard AES256 algorithm provided by the Qt Cryptographic Architecture (QCA) library to encrypt desired files. A user enters or generates a 96 character long hex string, which is decoded into a 256 bit key and 128 bit initialization vector. Using these values, the input file is then parsed in 14 byte blocks, which are given 2 bytes of padding and encrypted. These encrypted blocks are then chained back together to form a single output. Due to the algorithm requiring two bytes of padding per 14 bytes, the output file is roughly 14% larger than the initial file. This is slightly worse in size than SimpleCrypt (1:1 input to output), but considerably more secure.
#### 3. Conceal/Reveal:
The user is given multiple different options for concealing a file. The first option is for the algorithm style. The two options are LSB (least significant bit) and seeded, a variation of LSB. For LSB, a source image is loaded into the QImage class provided by Qt. The image is then traversed pixel by pixel, and the least significant bit of each color value is loaded with a bit from the input file. If the pixel is a black pixel (255 RGBA) it is slightly altered before loading in the file. This is due to the stopping mechanism for revealing a file being the first black pixel in the image, which is placed at the end of the concealing process. The seeded algorithm works in the same manner, except the order which bits are overwritten into color values is determined by a 64 character long hex string. The user is also given an option for image selection. If desired, a pre-existing image can be selected for concealing, given it contains enough pixels to hide the data. Otherwise, an image of randomly generated pixels will be created. The image generation algorithm creates an image containing 2 times the amount of pixels as the input file has bytes.


## License


    cZip is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.

    cZip is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with MySoft. If not, see http://www.gnu.org/licenses/.
