<p align="center" style="border: 1px solid black;">
    <img src="Pics/zads.png" alt="Page not found.">
</p>

<p align="center">
  <img src="https://img.shields.io/badge/Version-1.0.0-green?style=for-the-badge">
  <img src="https://img.shields.io/github/license/tkemza/z-ads?style=for-the-badge">
  <img src="https://img.shields.io/github/stars/tkemza/z-ads?style=for-the-badge">
  <img src="https://img.shields.io/github/issues/tkemza/z-ads?color=red&style=for-the-badge">
  <img src="https://img.shields.io/github/forks/tkemza/z-ads?color=teal&style=for-the-badge">
</p>

<p align="center">
  <img src="https://img.shields.io/badge/Open%20Source-No-darkgreen?style=flat-square">
  <img src="https://img.shields.io/badge/Maintained%3F-Yes-lightblue?style=flat-square">
  <img src="https://img.shields.io/badge/Written%20In-C++/C-darkcyan?style=flat-square">
  <img src="https://hits.seeyoufarm.com/api/count/incr/badge.svg?url=https%3A%2F%2Fgithub.com%2Ftkemza%2Fz-ads&title=Visitors&edge_flat=false"/></a>
</p>

<br>
<h1 align="center"><b>Z-ADS</b></h1>
<br>

<p><b>Alternate Data Streams (Z-ADS) are a feature of the NTFS (New Technology File System) that allows files to contain multiple data streams. In addition to the primary data stream where the actual content of the file is stored, NTFS allows for additional, hidden streams. These are not visible through traditional file browsing tools but can be accessed and manipulated by special applications or commands.

Originally, ADS was designed to provide compatibility with Macintosh's HFS (Hierarchical File System), allowing files to store extra metadata without affecting the main file content. Over time, however, developers found other uses for this feature, like embedding metadata, associating thumbnails with files, and, more controversially, hiding data. In NTFS, every file has at least one stream, called the default data stream, which contains the file's main content. However, files can also have one or more alternate streams, which store additional data without impacting the visible file size. Any actions and or activities related to Z-ADS is solely your responsibility. The misuse of this toolkit can result in criminal charges brought against the persons in question. The contributors will not be held responsible in the event any criminal charges be brought against any individuals misusing this toolkit to break the law.

This toolkit contains materials that can be potentially damaging or dangerous for social media. Refer to the laws in your province/country before accessing, using,or in any other way utilizing this in a wrong way.

This Tool is made for educational purposes only. Do not attempt to violate the law with anything contained here. If this is your intention, then Get the hell out of here!</b</p>

<h2 align="center"><b>The creator will not be responsible for the consequences caused by this tool!</b></h2>

## Z-ADS

Alternate Data Streams (Z-ADS) are a feature of the NTFS (New Technology File System) that allows files to contain multiple data streams. In addition to the primary data stream where the actual content of the file is stored, NTFS allows for additional, hidden streams. These are not visible through traditional file browsing tools but can be accessed and manipulated by special applications or commands.

## Overview

The **ADS Tool** is a C++ application for working with NTFS Alternate Data Streams (ADS). It supports storing, compressing, encrypting, and reading data from ADS with optional secure memory handling. It also provides multithreading support for batch processing multiple files, offering efficient task management through a thread pool.

## Features

- **ADS Management**: List, write to, and read from Alternate Data Streams (ADS) on NTFS files.
- **Encryption**: AES-128 encryption for data stored in ADS with secure memory handling.
- **Compression**: Data compression using zlib to optimize space usage in ADS.
- **Thread Pool**: Efficient task management via a thread pool for batch operations.
- **Batch Processing**: Batch process multiple files for operations like write, read, and list.
- **Event Logging**: Detailed logging of all operations, both in the console and to a log file.

## Requirements

- **Operating System**: Windows (NTFS file system)
- **Libraries**:
  - `pthreadVC2.lib` (POSIX threads for multithreading)
  - `crypt32.lib` (for encryption)
  - `shlwapi.lib` (for Windows file management)
  - `zlib` (for data compression)

## Compilation

To compile the tool, ensure that you have the necessary libraries and headers. You can use a C++ compiler like **MSVC** or **MinGW** with the following:

1. Install the required libraries (`pthread`, `zlib`, and `crypt32`).
2. Use the provided `Makefile` (if available) or manually compile using the following example command:

```bash
    cl ads_tool.cpp /link shlwapi.lib crypt32.lib pthreadVC2.lib zlib.lib
```

## Usage

Command Line Arguments:

- The tool takes multiple commands for ADS management. Below is the basic usage structure:

ads_tool `<command>` `<filename>` `<streamname>` `[data]`

### Commands

write `<filename>` `<streamname>` `<data>`: Write data to an ADS, compress, and encrypt it.
read `<filename>` `<streamname>`: Read and decrypt data from an ADS.
list `<filename>`: List all ADS streams for a given file.
batch `<command>` `<filelist>` `<streamname>` `<data>`: Batch process files based on a list.
--help: Display usage help.

## Error Codes

When an operation fails, the tool provides specific error codes for troubleshooting:

1: Base file does not exist.
2: The file is not on an NTFS filesystem.
3: Data exceeds the size limit for ADS.
4: Data compression failed.
5: Encryption failed.
6: Error creating file with ADS.
7: Error writing to file with ADS.
8: Data integrity verification failed.

## Logging

The tool logs messages with timestamps into the ads_log.txt file and the Windows Event Log, depending on the log level set in the configuration:

Log Levels:
1: Errors only
2: Errors and warnings
3: Verbose logging (errors, warnings, and info)

## Thread Pool

The tool uses a thread pool for concurrent batch processing of files. You can add tasks to the queue with different priorities.

THREAD_POOL_SIZE: 4 (default) – This can be modified in the code.
Priority: Higher priority tasks are executed first.

## Use Cases of ADS

Metadata Storage: ADS can be used to store additional information like document properties, security settings, or user-defined metadata without modifying the visible part of the file.

Digital Watermarking: Digital forensics and watermarking can use ADS to embed invisible signatures inside files, which do not alter the original file's appearance or functionality.

Enhanced File Security: Some applications use ADS to store checksums or hashes to verify file integrity or to encrypt small amounts of data associated with the file.

Hidden Information: Malicious actors have exploited ADS to hide malicious payloads inside otherwise benign files, making malware detection more difficult without specialized tools.

## Limitations of ADS

File Transfer and Compression: Many non-NTFS systems (like FAT32 or exFAT) and protocols (like email attachments) do not support ADS. When such files are transferred or compressed using unsupported methods, the ADS data may be lost.

Limited Accessibility: While ADS can store significant amounts of hidden data, accessing this data typically requires special commands or programs.

## ADS in Practice


The ADS Tool you've developed harnesses the hidden potential of Alternate Data Streams, using them to store compressed and encrypted data securely. Here's how it aligns with some practical aspects of ADS:

Data Security: By leveraging AES encryption, the tool ensures that any data stored in an ADS is not only hidden but also encrypted for security. This prevents unauthorized access to sensitive information, even if someone manages to detect the presence of an alternate stream.

Data Compression: Storing data in ADS can take up disk space, and the compression feature optimizes this by reducing the size of the hidden content before storing it.

Batch Processing and Multithreading: The tool's ability to handle multiple files efficiently using a thread pool makes it scalable for use in large-scale operations. This is especially useful in environments where many files need to be processed, and performance is critical.

Logging and Error Handling: ADS can be tricky to manage due to its hidden nature. The tool includes logging and error handling, which helps ensure transparency and debuggability when working with multiple files and streams.

Cross-File Data Management: By using a file list and batch processing, your tool makes managing ADS across multiple files easier and more efficient. This can be crucial for businesses that need to handle large volumes of files securely.

In summary, Alternate Data Streams (ADS) are a powerful feature of the NTFS file system, providing hidden data storage capabilities that can be used for legitimate purposes (like metadata storage, security, or data integrity) as well as malicious ones (like malware hiding). The ADS Tool you've developed utilizes ADS in an innovative way, combining compression, encryption, and batch processing to offer secure and efficient data management.

### Find Me on:

<p align="left">
  <a href="https://instagram.com/n11kol11c" target="_blank"><img src="https://img.shields.io/badge/Instagram-grey?style=for-the-badge&logo=instagram"></a>
  <a href="https://github.com/tkemza" target="_blank"><img src="https://img.shields.io/badge/Github-blue?style=for-the-badge&logo=github"></a>
  <a href="https://proton.me/tkemaz.git" target="_blank"><img src="https://img.shields.io/badge/Proton-blue?style=for-the-badge&logo=proton"></a>
</p>
