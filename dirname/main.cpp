#include <stdio.h>
#include <string>
#include <unistd.h>
#include <dirent.h>

std::string findDirName(std::string path) {
    if (path == "") return path;
    if (opendir(path.c_str())) return path;
    int len = path.length();
    int i = 0;
    for (i = len - 1; i >=0; i--) {
        if (path.at(i) == '/') break;
    } 
    path.erase(i, len-i);
    return path;
}

int main(int argc, char *argv[]) {
    std::string dir = findDirName("/usr/local/bin/rek");
    printf("%s\n", dir.c_str());
}