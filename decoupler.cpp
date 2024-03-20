#include <iostream>
#include <fstream>
#include <string>
#include <dirent.h>
#include <openssl/md5.h>
#include <unordered_map>
#include <sstream>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>


using namespace std;

char getch() {
    char buf = 0;
    struct termios old = {0};
    fflush(stdout);
    if (tcgetattr(0, &old) < 0)
        perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0)
        perror("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0)
        perror ("tcsetattr ~ICANON");
    return (buf);
}

bool deleteFile(const std::string& filePath) {
    if (std::remove(filePath.c_str()) != 0) {
        std::cerr << "Error: Unable to delete file " << filePath << std::endl;
        return false;
    } else {
        //std::cout << "File " << filePath << " deleted successfully" << std::endl;
        return true;
    }
}

std::string calculateMD5(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        std::cerr << "Error: Unable to open file " << filePath << std::endl;
        return "";
    }

    MD5_CTX mdContext;
    MD5_Init(&mdContext);

    const int bufferSize = 1024;
    char buffer[bufferSize];
    while (file.good()) {
        file.read(buffer, bufferSize);
        MD5_Update(&mdContext, buffer, file.gcount());
    }
    file.close();

    unsigned char result[MD5_DIGEST_LENGTH];
    MD5_Final(result, &mdContext);

    std::string md5Hash;
    for (int i = 0; i < MD5_DIGEST_LENGTH; ++i) {
        char buf[3];
        snprintf(buf, sizeof(buf), "%02x", result[i]);
        md5Hash += buf;
    }

    return md5Hash;
}


void saveJPGFileThisPaths(const std::string& folderPath, const std::string& outputPath) {
    std::ofstream outputFile(outputPath);
    if (!outputFile.is_open()) {
        std::cerr << "Unable to open output file!" << std::endl;
        return;
    }

    DIR* dir;
    struct dirent* entry;
    if ((dir = opendir(folderPath.c_str())) != nullptr) {
        while ((entry = readdir(dir)) != nullptr) {
            std::string fileName = entry->d_name;
            if (fileName.length() >= 4 && fileName.substr(fileName.length() - 4) == ".jpg") {
		std::string fp = folderPath+'/'+fileName;
                outputFile <<"\""<< fp <<"\" \""<< calculateMD5(fp)<<"\""<<std::endl;
            }
        }
        closedir(dir);
    } else {
        std::cerr << "Unable to open directory!" << std::endl;
        return;
    }

    outputFile.close();
    //std::cout << "File paths saved to " << outputPath << std::endl;
}

void saveJPGFileRecurencyPaths(const std::string& folderPath, const std::string& outputPath) {
    std::ofstream outputFile(outputPath, std::ios::app);
    if (!outputFile.is_open()) {
        std::cerr << "Unable to open output file!" << std::endl;
        return;
    }

    DIR* dir;
    struct dirent* entry;
    if ((dir = opendir(folderPath.c_str())) != nullptr) {
        while ((entry = readdir(dir)) != nullptr) {
            std::string fileName = entry->d_name;
            if (fileName != "." && fileName != "..") {
                std::string filePath = folderPath + '/' + fileName;
                struct stat fileStat;
                if (stat(filePath.c_str(), &fileStat) == 0 && S_ISDIR(fileStat.st_mode)) {
                    // Jeśli to katalog, rekurencyjnie wywołujemy saveJPGFileRecurencyPaths
                    saveJPGFileRecurencyPaths(filePath, outputPath);
                } else if (fileName.length() >= 4 && fileName.substr(fileName.length() - 4) == ".jpg") {
                    // Jeśli to plik jpg, zapisujemy ścieżkę do pliku wraz z MD5
                    outputFile << "\"" << filePath << "\" \"" << calculateMD5(filePath) << "\"" << std::endl;
                }
            }
        }
        closedir(dir);
    } else {
        std::cerr << "Unable to open directory!" << std::endl;
        return;
    }

    outputFile.close();
    //std::cout << "File paths saved to " << outputPath << std::endl;
}


void findDuplicateMD5(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file) {
        std::cerr << "Error: Unable to open file " << filePath << std::endl;
        return;
    }

    std::unordered_map<std::string, std::string> md5Map;
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string filePath, md5Hash;

        // Manual parsing to handle quoted strings
        std::getline(iss, filePath, '"'); // Skip leading quote
        std::getline(iss, filePath, '"'); // Read until closing quote
        std::getline(iss, md5Hash, '"');  // Skip spaces and leading quote
        std::getline(iss, md5Hash, '"');  // Read until closing quote

        if (!md5Map.count(md5Hash)) {
            md5Map[md5Hash] = filePath;
        } else {
            std::cout << "\nDuplicated JPG found\nMD5:" << md5Hash << std::endl;
            std::cout << "File [1]: " << md5Map[md5Hash] << std::endl;
            std::cout << "File [2]: " << filePath << std::endl;
	    std::cout << "Press [1] or [2] to delete. [3] to cancel, [4] is exit" << std::endl;
	    char sel;
	    sel = getch();
	    if (sel == '1'){
		deleteFile(md5Map[md5Hash]);
		cout << "Deleted 1\n";
	    }
	    if (sel == '2'){
		deleteFile(filePath);
		cout << "Deleted 2\n";
	    }
	    if (sel == '3'){
		cout << "Canceled\n";
	    }
	    if (sel == '4'){
		cout << "Bye!\n";
		break;
	    }
        }
    }

    file.close();
}

std::string removeTrailingSlash(const std::string& folder) {
    if (!folder.empty() && folder.back() == '/') {
        return folder.substr(0, folder.size() - 1);
    }
    return folder;
}


int main(int argc, char* argv[]) {

    if (argc < 2) {
        std::cerr << "Using: " << argv[0] << " <path> [-r]" << std::endl;
        std::cerr << "<path> - path to folder with images" << std::endl;
        std::cerr << "-r - if you want recursive checking in subfolders" << std::endl;
	std::cerr << std::endl;
        return 1;
    }
    
    std::string folder = removeTrailingSlash(argv[1]);
    bool recursive = false;
    
    if (argc == 3 && std::string(argv[2]) == "-r") {
        recursive = true;
    }


    std::string folderPath = folder; // Ścieżka do folderu z plikami JPG
    std::string outputPath = folder+'/'+"decoupler_list.txt"; // Ścieżka do pliku wyjściowego, gdzie zostaną zapisane ścieżki

    std::remove(outputPath.c_str());

    if (recursive){
	    saveJPGFileRecurencyPaths(folderPath, outputPath);
    }
    else {
   	    saveJPGFileThisPaths(folderPath, outputPath);
    }

    findDuplicateMD5(outputPath);

    std::remove(outputPath.c_str());

    return 0;
}

