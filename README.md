This program is a simple utility written in C++ that scans a folder for JPG image files, calculates their MD5 hashes, and checks for duplicates based on these hashes. Here's a breakdown of its main functionalities:

__File Input/Output Operations:__ 
The program utilizes file input/output operations to read from and write to files. It includes functions to save file paths along with their MD5 hashes to a text file.

__MD5 Hash Calculation:__ 
The program calculates the MD5 hash of each JPG file in the specified folder using the OpenSSL library.

__Duplicate Detection:__ 
After generating the list of file paths along with their MD5 hashes, the program identifies duplicate images by comparing their MD5 hashes. If duplicate images are found, it prompts the user to select which duplicate to delete.

__Folder Traversal:__
It can optionally traverse subfolders recursively to find JPG files.

__User Interaction:__
The program interacts with the user through the command line interface, allowing them to select which duplicate file to delete or to cancel the operation.

__Error Handling:__ 
It includes error handling mechanisms to deal with issues such as file opening failures and directory traversal errors.

To use the program, you provide the path to the folder containing JPG files as a command-line argument. Optionally, you can specify the "-r" flag to enable recursive searching within subfolders. After scanning and detecting duplicates, it generates a file named "decoupler_list.txt" containing the paths of JPG files and their MD5 hashes for reference.

Overall, this program offers a straightforward way to manage duplicate JPG files within a folder structure.
