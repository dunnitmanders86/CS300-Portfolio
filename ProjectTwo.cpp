//============================================================================
// Program     : ABCU Course Planner
// Author      : Amanda Dunn
// Date        : June 21, 2026
// Description : This program loads course data from a file into a binary search tree.
//               It allows users to print an alphanumeric course list and search
//               for individual course information and prerequisites.
//============================================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <algorithm>

using namespace std;

// Course structure stores course number, title, and prerequisites
struct Course {
    string courseNumber;
    string courseTitle;
    vector<string> prerequisites;
};

// Node used by the binary search tree
struct TreeNode {
    Course course;
    TreeNode* left;
    TreeNode* right;

    TreeNode(Course aCourse) {
        course = aCourse;
        left = nullptr;
        right = nullptr;
    }
};

// Converts text to uppercase
string toUpperCase(string input) {
    transform(input.begin(), input.end(), input.begin(), ::toupper);
    return input;
}

// Removes spaces from the beginning and end of a string
string trim(string input) {
    size_t start = input.find_first_not_of(" \t\r\n");
    size_t end = input.find_last_not_of(" \t\r\n");

    if (start == string::npos) {
        return "";
    }

    return input.substr(start, end - start + 1);
}

// Inserts a course into the binary search tree
TreeNode* insertCourse(TreeNode* root, Course course) {
    if (root == nullptr) {
        return new TreeNode(course);
    }

    if (course.courseNumber < root->course.courseNumber) {
        root->left = insertCourse(root->left, course);
    }
    else if (course.courseNumber > root->course.courseNumber) {
        root->right = insertCourse(root->right, course);
    }

    return root;
}

// Searches the binary search tree for a course number
TreeNode* searchCourse(TreeNode* root, string courseNumber) {
    if (root == nullptr) {
        return nullptr;
    }

    if (root->course.courseNumber == courseNumber) {
        return root;
    }

    if (courseNumber < root->course.courseNumber) {
        return searchCourse(root->left, courseNumber);
    }

    return searchCourse(root->right, courseNumber);
}

// Splits a comma-separated line from the input file
vector<string> splitLine(string line) {
    vector<string> tokens;
    string token;
    stringstream ss(line);

    while (getline(ss, token, ',')) {
        tokens.push_back(trim(token));
    }

    return tokens;
}

// Loads course data from the input file into the binary search tree
TreeNode* loadCourses(string fileName, bool& dataLoaded) {
    ifstream file(fileName);

    if (!file.is_open()) {
        cout << "Error: File could not be opened." << endl;
        dataLoaded = false;
        return nullptr;
    }

    vector<vector<string>> rawCourseData;
    set<string> courseNumbers;
    string line;

    // Read each line from the file
    while (getline(file, line)) {
        vector<string> tokens = splitLine(line);

        if (tokens.size() < 2) {
            cout << "Error: Invalid file format." << endl;
            dataLoaded = false;
            return nullptr;
        }

        if (tokens[0].empty() || tokens[1].empty()) {
            cout << "Error: Missing course number or course title." << endl;
            dataLoaded = false;
            return nullptr;
        }

        tokens[0] = toUpperCase(tokens[0]);

        rawCourseData.push_back(tokens);
        courseNumbers.insert(tokens[0]); // Store course numbers for prerequisite validation
    }

    file.close();

    TreeNode* root = nullptr;

    // Create course objects and validate prerequisite course numbers
    for (vector<string> tokens : rawCourseData) {
        Course course;
        course.courseNumber = tokens[0];
        course.courseTitle = tokens[1];

        for (unsigned int i = 2; i < tokens.size(); i++) {
            string prerequisite = toUpperCase(trim(tokens[i]));

            // Ignore blank prerequisite fields caused by extra commas
            if (prerequisite.empty()) {
                continue;
            }

            // Verify that each prerequisite exists in the file
            if (courseNumbers.find(prerequisite) == courseNumbers.end()) {
                cout << "Error: Prerequisite " << prerequisite << " does not exist." << endl;
                dataLoaded = false;
                return nullptr;
            }

            course.prerequisites.push_back(prerequisite);
        }

        // Insert the completed course into the tree
        root = insertCourse(root, course);
    }

    cout << "Courses loaded successfully." << endl;
    dataLoaded = true;
    return root;
}

// In-order traversal prints courses in alphanumeric order
void printCourseList(TreeNode* root) {
    if (root == nullptr) {
        return;
    }

    printCourseList(root->left);
    cout << root->course.courseNumber << ", " << root->course.courseTitle << endl;
    printCourseList(root->right);
}

// Displays a course and its prerequisites
void printCourseInformation(TreeNode* root, string courseNumber) {
    courseNumber = toUpperCase(trim(courseNumber));

    TreeNode* courseNode = searchCourse(root, courseNumber);

    if (courseNode == nullptr) {
        cout << "Course not found." << endl;
        return;
    }

    cout << courseNode->course.courseNumber << ", " << courseNode->course.courseTitle << endl;

    if (courseNode->course.prerequisites.empty()) {
        cout << "Prerequisites: None" << endl;
    }
    else {
        cout << "Prerequisites: ";

        for (unsigned int i = 0; i < courseNode->course.prerequisites.size(); i++) {
            cout << courseNode->course.prerequisites[i];

            if (i < courseNode->course.prerequisites.size() - 1) {
                cout << ", ";
            }
        }

        cout << endl;
    }
}

// Displays the main menu options
void displayMenu() {
    cout << endl;
    cout << "1. Load Data Structure." << endl;
    cout << "2. Print Course List." << endl;
    cout << "3. Print Course." << endl;
    cout << "9. Exit." << endl;
    cout << "What would you like to do? ";
}

// Program entry point
int main() {
    TreeNode* courseTree = nullptr;
    bool dataLoaded = false;
    int choice = 0;
    string fileName;
    string courseNumber;

    cout << "Welcome to the course planner." << endl;

    while (choice != 9) {
        displayMenu();
        cin >> choice;

        // Process the user's menu selection
        switch (choice) {
        case 1:
            cout << "Enter the file name: ";
            cin.ignore();
            getline(cin, fileName);
            courseTree = loadCourses(fileName, dataLoaded);
            break;

        case 2:
            if (!dataLoaded) {
                cout << "Please load the data first." << endl;
            }
            else {
                cout << "Here is a sample schedule:" << endl;
                printCourseList(courseTree);
            }
            break;

        case 3:
            if (!dataLoaded) {
                cout << "Please load the data first." << endl;
            }
            else {
                cout << "What course do you want to know about? ";
                cin >> courseNumber;
                printCourseInformation(courseTree, courseNumber);
            }
            break;

        case 9:
            cout << "Thank you for using the course planner!" << endl;
            break;

        default:
            cout << choice << " is not a valid option." << endl;
            break;
        }
    }

    return 0;
}