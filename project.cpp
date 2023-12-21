//developed by Taha Farooqui
//FAST-NU Karachi
//DSA project


#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <thread>
#include <string>
#include <stack>
#include <algorithm>
using namespace std;

// Structure to represent a file
struct File {
    string name;
    string content;

    File(const string& n, const string& c) : name(n), content(c) {}
};


class Folder {
private:
    static vector<Folder>::iterator findInsertionPosition(vector<Folder>& vec, const string& subfolderName) {
        return lower_bound(vec.begin(), vec.end(), subfolderName, [](const Folder& folder, const string& name) {
            return folder.name < name;
            });
    }

public:
    string name;
    vector<File> files;
    vector<Folder> subfolders;

    Folder() {}
    Folder(const string& n) : name(n) {}

    void addFile(const string& fileName, const string& content) {
        File newFile(fileName, content);
        auto position = lower_bound(files.begin(), files.end(), newFile, [](const File& file1, const File& file2) {
            return file1.name < file2.name;
            });
        files.insert(position, newFile);
        cout << "File added to folder: " << name << endl;
    }

    void addSubfolder(const string& subfolderName) {
        Folder newSubfolder(subfolderName);
        auto position = findInsertionPosition(subfolders, subfolderName);
        subfolders.insert(position, newSubfolder);
        cout << "Subfolder added to folder: " << name << endl;
    }

    void displayContents(int depth = 0) const {
        cout << string(depth, '\t') << "Contents of folder: " << name << "\n";

        // Display files
        for (const auto& file : files) {
            cout << string(depth + 1, '\t') << "File: " << file.name << "\n";
        }

        // Display subfolders
        for (const auto& subfolder : subfolders) {
            subfolder.displayContents(depth + 1);
        }
    }

    void deleteFile(const string& fileName) {
        auto it = find_if(files.begin(), files.end(), [fileName](const File& file) {
            return file.name == fileName;
            });

        if (it != files.end()) {
            files.erase(it);
            cout << "File deleted from folder: " << name << endl;
        }
        else {
            cout << "File not found in folder: " << name << endl;
        }
    }

    void deleteSubfolder(const string& subfolderName) {
        auto it = find_if(subfolders.begin(), subfolders.end(), [subfolderName](const Folder& folder) {
            return folder.name == subfolderName;
            });

        if (it != subfolders.end()) {
            subfolders.erase(it);
            cout << "Subfolder deleted from folder: " << name << endl;
        }
        else {
            cout << "Subfolder not found in folder: " << name << endl;
        }
    }

    Folder* findFolder(const string& folderName) {

        for (auto& subfolder : subfolders) {
            if (subfolder.name == folderName) {
                return &subfolder;
            }
        }

        return findFolderByName(folderName);
    }

private:
    Folder* findFolderByName(const string& folderName) {
        for (auto& subfolder : subfolders) {
            if (subfolder.name == folderName) {
                return &subfolder;
            }

            // Recursively search in subfolders
            Folder* result = subfolder.findFolderByName(folderName);
            if (result) {
                return result;
            }
        }

        return nullptr;
    }
};
class RecycleBin {
private:
    vector<File> deletedFiles;
    vector<Folder> deletedFolders;

public:
    void addDeletedFile(const File& file) {
        deletedFiles.push_back(file);
        cout << "File added to Recycle Bin: " << file.name << endl;
    }

    void addDeletedFolder(const Folder& folder) {
        deletedFolders.push_back(folder);
        cout << "Folder added to Recycle Bin: " << folder.name << endl;
    }

    void displayRecycleBinContents() const {
        cout << "Recycle Bin Contents:\n";

        // Display deleted files
        for (const auto& file : deletedFiles) {
            cout << "Deleted File: " << file.name << "\n";
        }

        // Display deleted folders
        for (const auto& folder : deletedFolders) {
            cout << "Deleted Folder: " << folder.name << "\n";
        }
    }

    void restoreDeletedFile(const string& fileName, Folder& targetFolder) {
        auto it = find_if(deletedFiles.begin(), deletedFiles.end(), [fileName](const File& file) {
            return file.name == fileName;
            });

        if (it != deletedFiles.end()) {
            targetFolder.addFile(it->name, it->content);
            deletedFiles.erase(it);
            cout << "File restored from Recycle Bin: " << fileName << endl;
        }
        else {
            cout << "File not found in Recycle Bin: " << fileName << endl;
        }
    }

    void emptyRecycleBin() {
        deletedFiles.clear();
        deletedFolders.clear();
    }
    void restoreDeletedFolder(const string& folderName, Folder& targetFolder) {
        auto it = find_if(deletedFolders.begin(), deletedFolders.end(), [folderName](const Folder& folder) {
            return folder.name == folderName;
            });

        if (it != deletedFolders.end()) {
            targetFolder.subfolders.push_back(*it);
            deletedFolders.erase(it);
            cout << "Folder restored from Recycle Bin: " << folderName << endl;
        }
        else {
            cout << "Folder not found in Recycle Bin: " << folderName << endl;
        }
    }
};
class TextEditor;
class TextFileManager;
void openTextFileInFolder(Folder& currentFolder, TextFileManager& textFileManager, string fileName);
class TextFileManager {
private:
    Folder* currentFolder;
    RecycleBin recycleBin;

public:
    vector<Folder> folders;

    TextFileManager() : currentFolder(nullptr) {
        // Create a default root folder
        folders.emplace_back("Root");
        currentFolder = &folders.front();
    }

    void setCurrentFolder(Folder* folder) {
        currentFolder = folder;
    }

    Folder* getCurrentFolder() const {
        return currentFolder;
    }

    void createFolder(const string& folderName, Folder& parentFolder) {
        parentFolder.addSubfolder(folderName);
        cout << "Folder created: " << folderName << " in " << parentFolder.name << endl;
    }

    Folder* findFolder(const string& folderName) {
        for (auto& folder : folders) {
            if (folder.name == folderName) {
                return &folder;
            }

            // Search recursively in subfolders
            Folder* subfolderResult = folder.findFolder(folderName);
            if (subfolderResult) {
                return subfolderResult;
            }
        }

        return nullptr;  // Folder not found
    }

    Folder* navigateIntoFolder(Folder& currentFolder, TextFileManager& textFileManager, TextEditor& textEditor) {
        Folder* current = &currentFolder; // Use a pointer to maintain the correct currentFolder

        while (true) {
            current->displayContents();
            cout << "\nEnter the folder or file name to open (or '.' to go back): ";
            string folderName;
            cin >> folderName;
            if (folderName == ".") {
                return nullptr; // Go back
            }

            // Open the folder or file
            openTextFileInFolder(*current, textFileManager, folderName);

            auto it = find_if(current->subfolders.begin(), current->subfolders.end(),
                [folderName](const Folder& folder) {
                    return folder.name == folderName;
                });

            if (it != current->subfolders.end()) {
                current = &(*it);
            }
            else {
                return nullptr;
            }
        }
    }


    RecycleBin& getRecycleBin() {
        return recycleBin;
    }
};

class TextEditor {
private:
    Folder* currentFolder;

public:
    stack<string> txt, backup, backup1, rev;
    string reff, alt;

    TextEditor() : currentFolder(nullptr) {}

    void setCurrentFolder(Folder* folder) {
        currentFolder = folder;
    }

    void enter() {
        string s;
        cout << "Enter the Text:\n\n";
        cin.ignore();
        getline(cin, s);

        for (int i = 0, n = s.size(); i < n; i++) {
            alt += s[i];
            if (s[i] == ' ') {
                rev.push(alt);
                alt = "";
            }
            else if (i == n - 1) {
                rev.push(alt);
                alt = "";
            }
        }

        while (!rev.empty()) {
            txt.push(rev.top());
            rev.pop();
        }
        cout << "\n";
    }

    void printText() {
        if (txt.empty()) {
            cout << "Nothing to print. Please Enter Text\n\n";
            enter();
        }
        else {
            //system("cls");
            while (!txt.empty()) {
                reff = txt.top();
                backup.push(reff);
                cout << reff << " ";
                txt.pop();
            }
            cout << flush;
            while (!backup.empty()) {
                txt.push(backup.top());
                backup.pop();
            }
            cout << "\n\n";
            this_thread::sleep_for(chrono::seconds(5));
        }
    }

    void undo() {
        if (txt.empty()) {
            cout << "Nothing to Undo\n\n";
            enter();
        }
        else {
            while (!txt.empty()) {
                rev.push(txt.top());
                txt.pop();
            }

            backup1.push(rev.top());
            rev.pop();
            cout << "Undo performed\n";

            while (!rev.empty()) {
                txt.push(rev.top());
                rev.pop();
            }
        }
    }

    void redo() {
        if (backup1.empty()) {
            cout << "Nothing to Redo\n\n";
        }
        else {
            while (!txt.empty()) {
                rev.push(txt.top());
                txt.pop();
            }

            rev.push(backup1.top());
            backup1.pop();
            cout << "Redo performed\n";

            while (!rev.empty()) {
                txt.push(rev.top());
                rev.pop();
            }
        }
    }

    void erase() {
        if (txt.empty()) {
            cout << "Nothing to Erase\n\n";
        }
        else {
            while (!txt.empty()) {
                txt.pop();
            }
            while (!backup.empty()) {
                backup.pop();
            }
            cout << "Text has been Erased\n\n";
        }
    }

    void saveToFile(const string& filename) {
        ofstream file(filename);
        while (!txt.empty()) {
            reff = txt.top();
            backup.push(reff);
            file << reff << " "; // Save each word separated by space
            txt.pop();
        }
        file.close();
    }
    void saveToFolder(string filename, string foldername, TextFileManager& textFileManager) {
        string content = getContent();

        // Find the folder by name
        Folder* targetFolder = textFileManager.findFolder(foldername);

        if (targetFolder) {
            targetFolder->addFile(filename, content);
            cout << "File saved to folder: " << targetFolder->name << endl;
        }
        else {
            cout << "Folder not found: " << foldername << endl;
        }
    }
    void exitEditor(TextFileManager& textFileManager) {
        string response;
        cout << "Do you want to save the file before exiting? (yes/no): ";
        cin >> response;

        if (response == "yes") {
            string filename;
            cout << "Enter the filename to save: ";
            cin >> filename;

            string foldername; // You may want to get the folder name from the user
            cout << "Enter the folder name to save the file: ";
            cin >> foldername;

            saveToFile(filename + ".txt");
            saveToFolder(filename, foldername, textFileManager);
            cout << "File saved successfully.\n";
        }
        cout << "\nThank you for using the Text Editor.\n";
    }

    string getContent() {
        string content;
        while (!txt.empty()) {
            content += txt.top() + " ";
            txt.pop();
        }
        return content;
    }
};

void createFolder(Folder& currentFolder, TextFileManager& textFileManager) {
    string folderName;
    cout << "Enter folder name: ";
    cin >> folderName;

    textFileManager.createFolder(folderName, currentFolder);
}

void deleteFolder(Folder& currentFolder, TextFileManager& textFileManager) {
    string folderName;
    cout << "Enter folder name to delete: ";
    cin >> folderName;

    auto it = find_if(currentFolder.subfolders.begin(), currentFolder.subfolders.end(), [folderName](const Folder& folder) {
        return folder.name == folderName;
        });

    if (it != currentFolder.subfolders.end()) {
        // Move deleted folder to Recycle Bin
        textFileManager.getRecycleBin().addDeletedFolder(*it);

        currentFolder.subfolders.erase(it);
        cout << "Folder deleted: " << folderName << endl;
    }
    else {
        cout << "Folder not found: " << folderName << endl;
    }
}

void addFileToFolder(Folder& currentFolder, TextEditor& textEditor, TextFileManager& textFileManager) {
    textEditor.enter();
    string content = textEditor.getContent();

    string fileName;
    cout << "Enter file name: ";
    cin >> fileName;

    currentFolder.addFile(fileName, content);
    textEditor.saveToFile(fileName + ".txt");
    cout << "File added to folder: " << currentFolder.name << endl;
}

void deleteFile(Folder& currentFolder, TextFileManager& textFileManager) {
    string fileName;
    cout << "Enter file name to delete: ";
    cin >> fileName;

    auto it = find_if(currentFolder.files.begin(), currentFolder.files.end(), [fileName](const File& file) {
        return file.name == fileName;
        });

    if (it != currentFolder.files.end()) {
        // Move deleted file to Recycle Bin
        textFileManager.getRecycleBin().addDeletedFile(*it);

        currentFolder.files.erase(it);
        cout << "File deleted from folder: " << currentFolder.name << endl;
    }
    else {
        cout << "File not found in folder: " << currentFolder.name << endl;
    }
}

void openFolder(Folder& currentFolder, TextFileManager& textFileManager, TextEditor& texteditor) {
    while (true) {
        Folder* selectedFolder = textFileManager.navigateIntoFolder(currentFolder, textFileManager, texteditor);
        if (selectedFolder) {
            textFileManager.setCurrentFolder(selectedFolder);
            currentFolder = *selectedFolder;
        }
        else {
            // Update the currentFolder pointer to the parent folder when navigating back
            textFileManager.setCurrentFolder(&currentFolder);
            break;
        }
    }
}
void displayTextFileContent(const File& file) {
    cout << "Content of file " << file.name << ":\n";
    cout << file.content << "\n\n";
}
void openTextFileInFolder(Folder& currentFolder, TextFileManager& textFileManager, string fileName) {
    // Check if the entered name is not a file or folder name
    if (fileName == "." || currentFolder.name == fileName) {
        return;
    }
    auto fileIt = find_if(currentFolder.files.begin(), currentFolder.files.end(), [fileName](const File& file) {
        return file.name == fileName;
        });

    if (fileIt != currentFolder.files.end()) {
        ifstream file(fileName + ".txt");

        if (file.is_open()) {
            // Read and display the content of the file
            string content;
            getline(file, content);
            file.close();

            cout << "Content of file " << fileIt->name << ":\n";
            cout << content << "\n\n";
        }
        else {
            cout << "Unable to open file: " << fileName << ".txt\n";
        }
    }
    else {
        cout << "File not found in folder: " << currentFolder.name << endl;
    }
}
Folder* binarySearchFolder(vector<Folder>& folders, const string& folderName) {
    int low = 0;
    int high = folders.size() - 1;

    while (low <= high) {
        int mid = low + (high - low) / 2;

        if (folders[mid].name == folderName) {
            return &folders[mid];
        }
        else if (folders[mid].name < folderName) {
            low = mid + 1;
        }
        else {
            high = mid - 1;
        }
    }

    return nullptr; // Folder not found
}

// Binary search function to find a file by name
File* binarySearchFile(vector<File>& files, const string& fileName) {
    int low = 0;
    int high = files.size() - 1;

    while (low <= high) {
        int mid = low + (high - low) / 2;

        if (files[mid].name == fileName) {
            return &files[mid];
        }
        else if (files[mid].name < fileName) {
            low = mid + 1;
        }
        else {
            high = mid - 1;
        }
    }

    return nullptr; // File not found
}

// Function to search for a file or folder by name
void searchFileOrFolder(TextFileManager& textFileManager) {
    while (1)
    {
        cout << "Enter the name of the folder to search\nor '.' to go back: ";
        string folderName;
        cin >> folderName;
        if (folderName == ".")
        {
            break;
        }
        // Search for the folder in the root
        Folder* foundFolder = binarySearchFolder(textFileManager.folders, folderName);

        if (foundFolder) {
            cout << "Folder found: " << foundFolder->name << "\n";

            // Now ask the user to search for a filename within the found folder
            cout << "Enter the name of the file to search in the folder\nor '.' to go back: ";
            string fileName;
            cin >> fileName;
            if (fileName == ".")
            {
                break;
            }
            // Search for the file in the found folder
            File* foundFile = binarySearchFile(foundFolder->files, fileName);

            if (foundFile) {
                cout << "File found in folder: " << foundFolder->name << "\n";
                ifstream file(fileName + ".txt");

                if (file.is_open()) {
                    // Read and display the content of the file
                    string content;
                    getline(file, content);
                    file.close();

                    cout << "Content of file " << foundFile->name << ":\n";
                    cout << content << "\n\n";
                }
                else {
                    cout << "Unable to open file: " << fileName << ".txt\n";
                }
            }
            else {
                cout << "File not found in folder: " << foundFolder->name << "\n";
            }
        }
        else {
            // Search for the file or folder in the current folder
            File* foundFile = binarySearchFile(textFileManager.getCurrentFolder()->files, folderName);

            if (foundFile) {
                cout << "File found in current folder: " << foundFile->name << "\n";
                ifstream file(foundFile->name + ".txt");

                if (file.is_open()) {
                    // Read and display the content of the file
                    string content;
                    getline(file, content);
                    file.close();

                    cout << "Content of file " << foundFile->name << ":\n";
                    cout << content << "\n\n";
                }
                else {
                    cout << "Unable to open file: " << foundFile->name << ".txt\n";
                }
            }
            else {
                // Search recursively in subfolders
                foundFolder = textFileManager.getCurrentFolder()->findFolder(folderName);

                if (foundFolder) {
                    cout << "Folder found: " << foundFolder->name << "\n";

                    // Now ask the user to search for a filename within the found folder
                    cout << "Enter the name of the file to search in the folder: ";
                    string fileName;
                    cin >> fileName;

                    // Search for the file in the found folder
                    File* foundFile = binarySearchFile(foundFolder->files, fileName);

                    if (foundFile) {
                        cout << "File found in folder: " << foundFolder->name << "\n";
                        ifstream file(fileName + ".txt");

                        if (file.is_open()) {
                            // Read and display the content of the file
                            string content;
                            getline(file, content);
                            file.close();

                            cout << "Content of file " << foundFile->name << ":\n";
                            cout << content << "\n\n";
                        }
                        else {
                            cout << "Unable to open file: " << fileName << ".txt\n";
                        }
                    }
                    else {
                        cout << "File not found in folder: " << foundFolder->name << "\n";
                    }
                }
                else {
                    cout << "File or folder not found: " << folderName << "\n";
                }
            }
        }
    }
}
int main() {
    TextFileManager textFileManager;
    TextEditor textEditor;

    Folder* currentFolder = textFileManager.getCurrentFolder();
    cout<<"\t\t\t\t\tWelcome to CLoud Operating System\t\t\t"<<endl;

    while (true) {
        system("cls");

        cout << "\nSelect an option:\n";
        cout << "1. Text Editor\n";
        cout << "2. File Explorer\n";
        cout << "3. Search Functionality\n";
        cout << "4. Recycle Bin\n";
        cout << "5. Exit\n";

        int option;
        cout << "\nEnter your choice (1-5): ";
        cin >> option;
        bool exitfileexplorer = false;
        switch (option) {
        case 1:
            while (true) {
                system("cls");

                cout << "1. Enter - to enter input text\n";
                cout << "2. Display - to display output\n";
                cout << "3. Undo - to perform undo operation\n";
                cout << "4. Redo - to perform redo operation\n";
                cout << "5. Erase - to erase complete text\n";
                cout << "6. Exit - to exit text editor\n\n";

                int ch;
                cout << "Enter Your Choice:\n\n";
                cin >> ch;
                cout << "\n";

                if (ch == 1) {
                    textEditor.enter();
                }
                else if (ch == 2) {
                    textEditor.printText();
                }
                else if (ch == 3) {
                    textEditor.undo();
                }
                else if (ch == 4) {
                    textEditor.redo();
                }
                else if (ch == 5) {
                    textEditor.erase();
                }
                else if (ch == 6) {
                    textEditor.exitEditor(textFileManager);
                    break;
                }
                else {
                    cout << "Enter a valid choice\n\n";
                }
            }
            break;

        case 2:
            while (!exitfileexplorer) {
                cout << "File Explorer\n";
                cout << "1. Create a folder\n";
                cout << "2. Delete a folder\n";
                cout << "3. Open a folder\n";
                cout << "4. Add file to the current folder\n";
                cout << "5. Delete file in current folder\n";
                cout << "6. Add file to subfolder\n";
                cout << "7. Delete file in subfolder\n";
                cout << "8. Back to main menu\n\n";

                int fileExplorerChoice;
                cout << "Enter your choice (1-8): ";
                cin >> fileExplorerChoice;

                switch (fileExplorerChoice) {
                case 1:
                    createFolder(*currentFolder, textFileManager);
                    break;

                case 2:
                    deleteFolder(*currentFolder, textFileManager);
                    break;

                case 3:
                    openFolder(*currentFolder, textFileManager,textEditor);
                    
                    break;

                case 4:
                    addFileToFolder(*currentFolder, textEditor, textFileManager);
                    break;

                case 5:
                    deleteFile(*currentFolder, textFileManager);
                    break;

                case 6:
                {
                    // Add file to subfolder
                    string subfolderName;
                    cout << "Enter subfolder name: ";
                    cin >> subfolderName;

                    // Find the subfolder by name
                    Folder* subfolder = currentFolder->findFolder(subfolderName);

                    if (subfolder) {
                        addFileToFolder(*subfolder, textEditor, textFileManager);
                    }
                    else {
                        cout << "Subfolder not found: " << subfolderName << endl;
                    }
                }
                break;

                case 7:
                {
                    // Delete file in subfolder
                    string subfolderName;
                    cout << "Enter subfolder name: ";
                    cin >> subfolderName;

                    // Find the subfolder by name
                    Folder* subfolder = currentFolder->findFolder(subfolderName);

                    if (subfolder) {
                        deleteFile(*subfolder, textFileManager);
                    }
                    else {
                        cout << "Subfolder not found: " << subfolderName << endl;
                    }
                }
                break;

                case 8:
                    exitfileexplorer = true;
                    break;

                default:
                    cout << "Enter a valid choice (1-8)\n\n";
                }
            }
            break;


        case 3:
            searchFileOrFolder(textFileManager);
            break;

        case 4:
            while (true) {
                cout << "\nRecycle Bin\n";
                cout << "1. Display Recycle Bin contents\n";
                cout << "2. Restore a file from Recycle Bin\n";
                cout << "3. Restore a folder from Recycle Bin\n";
                cout << "4. Empty Recycle Bin\n";
                cout << "5. Back to main menu\n\n";

                int recycleBinChoice;
                cout << "Enter your choice (1-5): ";
                cin >> recycleBinChoice;

                switch (recycleBinChoice) {
                case 1:
                    textFileManager.getRecycleBin().displayRecycleBinContents();
                    break;

                case 2: {
                    string fileName;
                    cout << "Enter the filename to restore: ";
                    cin >> fileName;

                    textFileManager.getRecycleBin().restoreDeletedFile(fileName, *currentFolder);
                    break;
                }

                case 3: {
                    string folderName;
                    cout << "Enter the folder name to restore: ";
                    cin >> folderName;

                    textFileManager.getRecycleBin().restoreDeletedFolder(folderName, *currentFolder);
                    break;
                }

                case 4:
                    textFileManager.getRecycleBin().emptyRecycleBin();
                    cout << "Recycle Bin emptied.\n";
                    break;

                case 5:
                    exitfileexplorer = true;
                    break;

                default:
                    cout << "Enter a valid choice (1-5)\n\n";
                }

                if (exitfileexplorer) {
                    break;
                }
            }
            break;

        case 5:
            cout << "Exiting the Cloud Operating System. Goodbye!\n";
            return 0;

        default:
            cout << "Invalid choice. Please select a number between 1 and 5.\n";
            break;
        }
    }

    return 0;
}
