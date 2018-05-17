#include <iostream>
#include <string>
#include <vector>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

using std::string;
using std::cout;
using std::cerr;
using std::cin;
using std::endl;
using std::vector;

typedef int (*func)(string);

struct configuration {
    bool recursive = false;
    bool force = false;
    bool all = false;
    bool cancel = false;
};

int help_text() {
    cout << "Usage: myrm [-h|--help] [-f] [-R] <file>... \n\
    Remove (unlink) the FILE(s).\n\
    \n\
    -f, --force         ignore nonexistent files and arguments, never prompt\n\
    -R, --recursive     remove directories and their contents recursively\n\
    -h, --help          display this help and exit\n\
    \n\
    By default, rm does not remove directories.  Use the --recursive (-r or -R)\n\
    option to remove each listed directory, too, along with all of its contents.\n" << endl;
    
    return 0;
}

int remove_all(string filename) {
    auto path = boost::filesystem::system_complete(filename);
    if (boost::filesystem::exists(path)) {
        try {
            boost::filesystem::remove_all(path);
        } catch (boost::filesystem::filesystem_error const &e) {
            cerr << e.what() << endl;
            exit(-1);
        }
    } else {
        cerr << "Error: there is no file with name: " + filename << endl;
        exit(-1);
    }
    return 0;
}

int force_remove(string filename) {
    auto path = boost::filesystem::system_complete(filename);
    
    if (boost::filesystem::exists(path)) {
        if (!boost::filesystem::is_directory(path)) {
            try {
                boost::filesystem::remove(path);
            } catch (boost::filesystem::filesystem_error &e) {
                cerr << e.what() << endl;
                exit(-1);
            }
        } else {
            cerr << "Eror: this file is a directory, use option -R for remowing it, or -h for more info." << endl;
            exit(-1);
        }
        
    } else {
        cerr << "Error: there is no file with name: " + filename << endl;
        exit(-1);
    }
    
    return 0;
}

int params(func remove, string filename, configuration &conf) {
    string input;
    cout << "Are you sure? The file: " + filename + " will be removed. [Y/N]: ";
    cin >> input;
    
    if (input == "Y" || input == "y" || input == "yes" || input == "YES") {
        remove(filename);
    } else if (input == "A" || input == "a" || input == "all" || input == "ALL") {
        conf.all = true;
        remove(filename);
    } else if (input == "C" || input == "c" || input == "cancel" || input == "CANCEL") {
        conf.cancel = true;
    } else {
        cerr << "No such option, usage: Y[es]/N[o]/A[ll]/C[ancel]" << endl;
        exit(-1);
    }
    
    return 0;
}

int read_args(vector<string> &args, configuration &conf, int argc, const char * argv[]) {
    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
            help_text();
        } else if (!strcmp(argv[i], "-R") || !strcmp(argv[i], "--recursive")) {
            conf.recursive = true;
        } else if (!strcmp(argv[i], "-f") || !strcmp(argv[i], "--force")) {
            conf.force = true;
        } else {
            args.push_back(argv[i]);
        }
    }
    
    return 0;
}

int params_no_opts(func remove, vector<string> &args, configuration &conf) {
    for (auto &name : args) {
        if (!conf.all) {
            params(remove, name, conf);
        } else if (conf.cancel) {
            break;
        } else {
            remove(name);
        }
    }
    
    return 0;
}

int launch(vector<string> &args, configuration &conf) {
    if (conf.force && !conf.recursive) {
        for (auto &name : args) {
            force_remove(name);
        }
    } else if (conf.recursive) {
        if (conf.force) {
            for (auto &name : args) {
                remove_all(name);
            }
        } else {
            params_no_opts(remove_all, args, conf);
        }
    } else {
        params_no_opts(force_remove, args, conf);
    }
    
    return 0;
}

int main(int argc, const char * argv[]) {
    vector<string> args;
    configuration conf;
    read_args(args, conf, argc, argv);
    launch(args, conf);
    
    return 0;
}
