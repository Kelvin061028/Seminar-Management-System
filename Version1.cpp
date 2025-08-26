#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <vector>
#include <regex>
#include <algorithm>
#include <limits>
#include <map>
#include <unordered_map>

using namespace std;

/*==============================================================================
   DATA MODELS
==============================================================================*/
struct Session {
    int sessionID{};
    string title;
    string date;
    string time;
    string participants[30]; // kept for compatibility (not strictly needed)
};

struct Address {
    string street;
    string city;
    string state;
    string country;
};

struct User {
    string userID;
    string name;
    string gender;
    string ic;
    string email;
    string contactNum;
    string passwd;
    int age{};
    string bookedSessions[3];  // kept for compatibility (not strictly needed)
    Address address;
};

/*==============================================================================
   FORWARD DECLARATIONS
==============================================================================*/
// Visual headers / static screens
void displayIntroScreen();
void eventDescription();
void displaySeminarInfo();
void displayMainHeader();
void displayAdminHeader();
void displayUserHeader();
void displayProfileHeader1();
void displayProfileHeader2();
void displayBookingSummaryHeader();

// Core lists
void displaySessionMenu(const vector<Session>& sessions);

// Persistence
void loadSessionsList(vector<Session>& sessions, const string& txtfilename, int& nextSessionID);
void saveSessionsList(const vector<Session>& sessions, const string& txtfilename);
void loadUserDetails(vector<User>& users);
void saveUserDetails(vector<User>& users);

// Auth
bool adminLogin();
bool validateUserIdPassw(vector<User>& users, const string& userIDEnter, const string& userPasswEnter);
string userLogin(vector<User>& users);
string generateUserID();
void userRegister(vector<User>& users);

// Validators
bool cancelRegisterYN(const string& choice);
bool validateEmail(vector<User>& users, string& email);
bool validateContactNum(vector<User>& users, string& contact);
bool validateICNum(vector<User>& users, string& icNum);

// User profile
void displayUserProfile(vector<User> users, const string& userIDEnter);

// Booking matrix
void loadBookingStatus(const string& filename, vector<vector<int>>& bookingStatus, int totUser, int totSessions);
void saveBookingStatus(const string& filename, const vector<vector<int>>& bookingStatus);
bool checkSessionIDExist(const vector<Session>& sessions, int sessionIDbook);
int  countNumOfParticipants(const vector<vector<int>>& bookingStatus, int sessionIndex);
int  findUserIndex(const vector<User>& users, const string& userIDEnter);

// Booking flow
void displayBookingScreen(vector<User>& users, const string& userIDEnter);

// Screens
void displayMainMenu(vector<User>& users);
void displayUserMainMenu(vector<User>& users);
void userScreen(const string& userIDEnter);

/*========================= Kelvin module prototypes =========================*/
void displayAdminScreen(vector<User>& users);
void viewParticipantSummary(const vector<User>& users,
                            const vector<Session>& sessions,
                            const vector<vector<int>>& bookingStatus);
void viewSessionSummary(const vector<Session>& sessions,
                        const vector<vector<int>>& bookingStatus);
void adminFilterMenu(const vector<User>& users,
                     const vector<Session>& sessions,
                     const vector<vector<int>>& bookingStatus);
void editSessions(vector<Session>& sessions,
                  int& nextSessionID,
                  const string& sessionsFile,
                  vector<vector<int>>& bookingStatus);
void viewFeedback();
void writeFeedback(const string& userIDEnter,
                   const vector<User>& users,
                   const vector<Session>& sessions,
                   const vector<vector<int>>& bookingStatus);
void displayUserBookedSessions(const vector<User>& users,
                               const vector<Session>& sessions,
                               const vector<vector<int>>& bookingStatus,
                               const string& userIDEnter);

/*==============================================================================
   GENERIC HELPERS
==============================================================================*/
int getIntInRange(const string& prompt, int lo, int hi) {
    while (true) {
        cout << prompt;
        string s;
        if (!(cin >> s)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        if (all_of(s.begin(), s.end(), ::isdigit)) {
            long long v = stoll(s);
            if (v >= lo && v <= hi) return (int)v;
        }
        cout << "Invalid input. Enter an integer in [" << lo << ", " << hi << "].\n";
    }
}

int askYesNo(const string& prompt) {
    while (true) {
        cout << prompt << " (YES[1]/NO[0]): ";
        string s;
        if (!(cin >> s)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        if (s == "1" || s == "0") return stoi(s);
        cout << "Please enter 1 or 0.\n";
    }
}

void ensureBookingMatrixSize(vector<vector<int>>& bookingStatus, int userCount, int sessionCount) {
    if ((int)bookingStatus.size() < userCount) {
        bookingStatus.resize(userCount);
    }
    for (auto& row : bookingStatus) {
        if ((int)row.size() < sessionCount) row.resize(sessionCount, 0);
    }
}

void removeBookingColumn(vector<vector<int>>& bookingStatus, int col) {
    for (auto& row : bookingStatus) {
        if (col >= 0 && col < (int)row.size()) {
            row.erase(row.begin() + col);
        }
    }
}

/*==============================================================================
   STATIC/DECORATION SCREENS
==============================================================================*/
void displayIntroScreen() {
    cout << endl << endl;
    cout << setfill(' ') << setw(25) << " ";
    cout << setfill('*') << setw(103) << "*" << endl;
    cout << setfill(' ');
    cout << setw(26) << "*" << setw(102) << "*" << endl;
    cout << setw(26) << "*" << setw(102) << "*" << endl;
    cout << setw(26) << "*" << setw(102) << "                 /\\                                             *" << endl;
    cout << setw(26) << "*" << setw(102) << "    /\\         /  \\                                            *" << endl;
    cout << setw(26) << "*" << setw(102) << "___/  \\___    /    \\____                                       *" << endl;
    cout << setw(26) << "*" << setw(102) << "          \\  /                                                 *" << endl;
    cout << setw(26) << "*" << setw(102) << "           \\/                                                  *" << endl;
    cout << setw(26) << "*" << setw(102) << "*" << endl;
    cout << setw(26) << "*" << setw(102) << " H E A L T H   S E M I N A R                                      *" << endl;
    cout << setw(26) << "*" << setw(102) << "*" << endl;
    cout << setw(26) << "*" << setw(102) << "\"Awareness is the First Step to Wellness\"                                *" << endl;
    cout << setw(26) << "*" << setw(102) << "*" << endl;
    cout << setw(26) << "*" << setw(102) << "*" << endl;
    cout << setfill(' ') << setw(25) << " ";
    cout << setfill('*') << setw(103) << "*" << endl;
    cout << setfill(' ');
    cout << endl;
}

void eventDescription() {
    cout << endl;
    cout << "Join us for an exciting, multi-day health seminar covering mental health, "
            "nutrition, physical activity, stress management, and more!\n\n"
            " Venue   : Online Google \n"
            " Open To : Students, Staff \n"
            " Price   : RM 20 per session (Discounts Available)";
    cout << endl << endl << endl;
}

void displaySeminarInfo() {
    cout << endl;
    cout << "Welcome to Health Seminar Event System !!\nWe appreciate you coming along to start down the path to improved health!" << endl;

    cout << "\n\n-----------------------------------------------------------------------" << endl;
    cout << "             I n f o r m a t i o n   O n   P r i c e s " << endl;
    cout << "-----------------------------------------------------------------------" << endl;
    cout << " 1. The cost per session is RM20." << endl;
    cout << " 2. Combo Offer: RM50 for Sessions 1, 6, and 15." << endl;
    cout << " 3. Reserve more than two sessions. Save RM5 on your entire purchase." << endl;

    cout << "\n\n-----------------------------------------------------------------------" << endl;
    cout << "                  R e s e r v a t i o n   C a p  " << endl;
    cout << "-----------------------------------------------------------------------" << endl;
    cout << " 1. Each participant may only attend three sessions." << endl;
    cout << " 2. 30 people is the maximum number of participants per session." << endl;
    cout << endl << endl;
}

void displayMainHeader() {
    cout << endl << endl;
    cout << setfill('=') << setw(157) << " ";
    cout << setfill(' ');
    cout << setw(123) << " H E A L T H   S E M I N A R   S Y S T E M                           " << endl;
    cout << setfill('=') << setw(157) << " ";
    cout << setfill(' ');
    cout << endl << endl;
}

void displayAdminHeader() {
    cout << endl << endl << endl;
    cout << setfill('=') << setw(156) << " ";
    cout << setfill(' ') << endl;
    cout << right << setw(123) << " H E A L T H   S E M I N A R  S Y S T E M                            \n" << endl;
    cout << setw(128) << "\" Awareness is the First Step to Wellness \"                              \n" << endl;
    cout << setfill('=') << setw(157) << " ";
    cout << setfill(' ');
    cout << setw(123) << " A D M I N   S C R E E N                                     " << endl;
    cout << setfill('=') << setw(157) << " ";
    cout << setfill(' ');
    cout << endl << endl;
}

void displayUserHeader() {
    cout << endl << endl << endl;
    cout << setfill('=') << setw(156) << " ";
    cout << setfill(' ') << endl;
    cout << right << setw(123) << " H E A L T H   S E M I N A R  S Y S T E M                            \n" << endl;
    cout << setw(128) << "\" Awareness is the First Step to Wellness \"                              \n" << endl;
    cout << setfill('=') << setw(157) << " ";
    cout << setfill(' ');
    cout << setw(123) << " U S E R   S C R E E N                                      " << endl;
    cout << setfill('=') << setw(157) << " ";
    cout << setfill(' ');
    cout << endl << endl << endl;
}

void displayProfileHeader1() {
    cout << endl << endl << endl;
    cout << setfill('=') << setw(156) << " ";
    cout << setfill(' ') << endl;
    cout << right << setw(123) << " H E A L T H   S E M I N A R  S Y S T E M                            \n" << endl;
    cout << setw(128) << "\" Awareness is the First Step to Wellness \"                              \n" << endl;
    cout << setfill('=') << setw(157) << " ";
    cout << setfill(' ');
    cout << setw(123) << " P R O F I L E                                           " << endl;
    cout << setfill('=') << setw(157) << " ";
    cout << setfill(' ');
    cout << endl << endl << endl;
}

void displayProfileHeader2() {
    cout << endl << endl << endl;
    cout << setfill('=') << setw(156) << " ";
    cout << setfill(' ') << endl;
    cout << right << setw(123) << " H E A L T H   S E M I N A R  S Y S T E M                            \n" << endl;
    cout << setw(128) << "\" Awareness is the First Step to Wellness \"                              \n" << endl;
    cout << setfill('=') << setw(157) << " ";
    cout << setfill(' ');
    cout << setw(123) << " P e r s o n a l   I n f o r m a t i o n                              " << endl;
    cout << setfill('=') << setw(157) << " ";
    cout << setfill(' ');
    cout << endl << endl << endl;
}

void displayBookingSummaryHeader() {
    cout << endl << endl << endl;
    cout << setfill('=') << setw(156) << " ";
    cout << setfill(' ') << endl;
    cout << right << setw(123) << " H E A L T H   S E M I N A R  S Y S T E M                            \n" << endl;
    cout << setw(128) << "\" Awareness is the First Step to Wellness \"                              \n" << endl;
    cout << setfill('=') << setw(157) << " ";
    cout << setfill(' ');
    cout << setw(123) << " B o o k i n g    S u m m a r y                                       " << endl;
    cout << setfill('=') << setw(157) << " ";
    cout << setfill(' ');
    cout << endl << endl << endl;
}

/*==============================================================================
   SESSION LIST R/W + LIST UI
==============================================================================*/
void loadSessionsList(vector<Session>& sessions, const string& txtfilename, int& nextSessionID) {
    ifstream txtFile(txtfilename);
    if (!txtFile) {
        cerr << "Warning: File not found. Starting with an empty session list.\n";
        return;
    }
    sessions.clear();
    string line;
    while (getline(txtFile, line)) {
        stringstream ss(line);
        string id, title, date, time;
        if (getline(ss, id, '|') && getline(ss, title, '|') && getline(ss, date, '|') && getline(ss, time)) {
            Session session;
            session.sessionID = stoi(id);
            session.title = title;
            session.date = date;
            session.time = time;
            sessions.push_back(session);
            if (session.sessionID >= nextSessionID)
                nextSessionID = session.sessionID + 1;
        }
    }
    txtFile.close();
}

void saveSessionsList(const vector<Session>& sessions, const string& txtfilename) {
    ofstream txtFile(txtfilename);
    if (!txtFile) {
        cerr << "Error: Unable to open file for saving.\n";
        return;
    }
    for (const auto& session : sessions) {
        txtFile << session.sessionID << "|" << session.title << "|"
                << session.date << "|" << session.time << '\n';
    }
    txtFile.close();
}

void displaySessionMenu(const vector<Session>& sessions) {
    if (sessions.empty()) {
        cout << "No seminar sessions available.\n";
        return;
    }
    cout << string(155, '-') << "\n";
    cout << left << setw(15) << " SESSION  ID" << setw(80) << " SESSION  TITLE " << setw(30) << " DATE " << setw(30) << " TIME " << "\n";
    cout << string(155, '-') << "\n";
    for (const auto& s : sessions) {
        cout << left << setw(15) << s.sessionID
             << setw(80) << (s.title.size() > 78 ? s.title.substr(0,75)+"..." : s.title)
             << setw(30) << s.date << setw(30) << s.time << "\n";
    }
}

/*==============================================================================
   USER R/W + AUTH + VALIDATION + PROFILE
==============================================================================*/
void loadUserDetails(vector<User>& users) {
    users.clear();
    ifstream existUser("users.txt");
    if (!existUser.is_open()) {
        cerr << "\nWarning: File not found. Starting with an empty user list.\n" << endl;
        return;
    }
    string line;
    while (getline(existUser, line)) {
        stringstream ss(line);
        User user;
        getline(ss, user.userID, '|');
        getline(ss, user.name, '|');
        getline(ss, user.gender, '|');
        ss >> user.age;
        ss.ignore();
        getline(ss, user.ic, '|');
        getline(ss, user.email, '|');
        getline(ss, user.contactNum, '|');
        getline(ss, user.address.street, '|');
        getline(ss, user.address.city, '|');
        getline(ss, user.address.state, '|');
        getline(ss, user.address.country, '|');
        getline(ss, user.passwd);
        users.push_back(user);
    }
    existUser.close();
}

void saveUserDetails(vector<User>& users) {
    ofstream saveUserDetail("users.txt");
    for (const auto& user : users) {
        saveUserDetail << user.userID << "|" << user.name << "|" << user.gender << "|" << user.age << "|" << user.ic
            << "|" << user.email << "|" << user.contactNum << "|" << user.address.street << "|" << user.address.city
            << "|" << user.address.state << "|" << user.address.country << "|" << user.passwd << '\n';
    }
    saveUserDetail.close();
}

string generateUserID() {
    int pervoiusLastUserId = 0;
    ifstream userIdNumFile("pervoiusLastUserId.txt");
    if (userIdNumFile >> pervoiusLastUserId)
        userIdNumFile.close();
    pervoiusLastUserId++;
    ofstream saveUserLastID("pervoiusLastUserId.txt");
    saveUserLastID << pervoiusLastUserId;
    saveUserLastID.close();
    stringstream ss;
    ss << "U" << setw(4) << setfill('0') << pervoiusLastUserId;
    return ss.str();
}

bool cancelRegisterYN(const string& choice) {
    string checkInput = choice;
    for (auto& c : checkInput) c = tolower(c);
    return (checkInput == "q");
}

bool validateEmail(vector<User>& /*users*/, string& email) {
    const regex pattern(R"(^[A-Za-z0-9._%+\-]+@[A-Za-z0-9.\-]+\.[A-Za-z]{2,}$)");
    return regex_match(email, pattern);
}

bool validateContactNum(vector<User>& /*users*/, string& contact) {
    contact.erase(remove(contact.begin(), contact.end(), '-'), contact.end());
    contact.erase(remove(contact.begin(), contact.end(), ' '), contact.end());
    const regex pattern(R"(^\d{10,11}$)");
    return regex_match(contact, pattern);
}

bool validateICNum(vector<User>& /*users*/, string& icNum) {
    const regex pattern(R"(^\d{12}$)");
    return regex_match(icNum, pattern);
}

void userRegister(vector<User>& users) {
    User user;
    int age;
    string email, contactNum, icNum;

    cout << "\n\n----------------------------------------------------------";
    cout << "\n         S i g n   U p   A s   N e w   U s e r";
    cout << "\n----------------------------------------------------------";
    cin.ignore();
    cout << "\n** You may enter 'Q' or 'q' to CANCEL register ** \n";
    cout << "\nPlease fill in the following fields :\n";

    do {
        cout << "Name                       : ";
        getline(cin, user.name);
        if (cancelRegisterYN(user.name)) {
            cout << "\nRegistration Cancelled.\n";
            return;
        }
        if (user.name.empty())
            cout << "Do not leave it empty !\n\n";
    } while (user.name.empty());

   bool isValid = false;

    do {
        cout << "Gender ( Female/F ,Male/M ): ";
        getline(cin, user.gender);
        if (cancelRegisterYN(user.gender)) {
            cout << "\nRegistration Cancelled.\n";
            return;
        }
        for (char& inputChar : user.gender) inputChar = tolower(inputChar);
        if (user.gender == "female" || user.gender == "f") {
            user.gender = "Female";
            isValid = true;
        } else if (user.gender == "male" || user.gender == "m") {
            user.gender = "Male";
            isValid = true;
        } else {
            cout << "Invalid input ! Please enter Female/F or Male/M !!\n\n";
        }
    } while (!isValid);

    bool isAgeValid = false;
    do {
        string ageInput;
        cout << "Age                        : ";
        getline(cin, ageInput);
        if (cancelRegisterYN(ageInput)) {
            cout << "\nRegistration Cancelled.\n";
            return;
        }
        if (ageInput.empty()) {
            cout << "Age cannot be empty!\n\n";
            continue;
        }
        if (all_of(ageInput.begin(), ageInput.end(), ::isdigit)) {
            age = stoi(ageInput);
            if (age > 0 && age <= 120) {
                user.age = age;
                isAgeValid =true;
            } else {
                cout << "Please enter an age between 1 and 120.\n\n";
            }
        } else {
            cout << "Invalid input. Please enter numeric digits only.\n\n";
        }
    } while (!isAgeValid);

    do {
        cout << "IC                         : ";
        getline(cin, icNum);
        if (cancelRegisterYN(icNum)) {
            cout << "\nRegistration Cancelled.\n";
            return;
        }
        if (validateICNum(users, icNum)) {
            user.ic = icNum;
            break;
        } else {
            cout << "Invalid input! Wrong ic format! \n\n";
        }
    } while (!(validateICNum(users, icNum)));

    do {
        cout << "Email                      : ";
        getline(cin, email);
        if (cancelRegisterYN(email)) {
            cout << "\nRegistration Cancelled.\n";
            return;
        }
        if (validateEmail(users, email)) {
            user.email = email;
            break;
        } else {
            cout << "Invalid input! Wrong email format! \n\n";
        }
    } while (!(validateEmail(users, email)));

    do {
        cout << "Contact Number  (XXX-XXX-XXXX) : ";
        getline(cin, contactNum);
        if (cancelRegisterYN(contactNum)) {
            cout << "\nRegistration Cancelled.\n";
            return;
        }
        if (validateContactNum(users, contactNum)) {
            user.contactNum = contactNum;
            break;
        } else {
            cout << "Invalid Input! Wrong contact number format! \n\n";
        }
    } while (!(validateContactNum(users, contactNum)));

    cout << "\n\n=======================================================";
    cout << "\n                  A d d r e s s ";
    cout << "\n=======================================================";
    do {
        cout << "\nStreet                     : ";
        getline(cin, user.address.street);
        if (cancelRegisterYN(user.address.street)) {
            cout << "\nRegistration Cancelled.\n";
            return;
        }
        if (user.address.street.empty())
            cout << "Do not leave it empty !\n\n";
    } while (user.address.street.empty());

    do {
        cout << "City                       : ";
        getline(cin, user.address.city);
        if (cancelRegisterYN(user.address.city)) {
            cout << "\nRegistration Cancelled.\n";
            return;
        }
        if (user.address.city.empty())
            cout << "Do not leave it empty !\n\n";
    } while (user.address.city.empty());

    do {
        cout << "State                      : ";
        getline(cin, user.address.state);
        if (cancelRegisterYN(user.address.state)) {
            cout << "\nRegistration Cancelled.\n";
            return;
        }
        if (user.address.state.empty())
            cout << "Do not leave it empty !\n\n";
    } while (user.address.state.empty());

    do {
        cout << "Country                    : ";
        getline(cin, user.address.country);
        if (cancelRegisterYN(user.address.country)) {
            cout << "\nRegistration Cancelled.\n";
            return;
        }
        if (user.address.country.empty())
            cout << "Do not leave it empty !\n\n";
    } while (user.address.country.empty());

    string password1st, password2nd;
    do {
        cout << "\n\nPlease enter a password for your account : ";
        cout << "\nSet Password > ";
        getline(cin, password1st);
        cout << "Confirm Password > ";
        getline(cin, password2nd);
        if (password1st != password2nd) {
            cout << "\nNot match ! Invalid! Please try again!";
            password2nd.clear();
            cout << "\nConfirm Password > ";
            getline(cin, password2nd);
            if (password1st != password2nd)
                cout << "\nNot match !!! Please RESET again your password!\n";
            else
                cout << "\nPassword matched!! Generating your unique userID.\n";
        }
    } while (password1st != password2nd);

    user.passwd = password1st;
    user.userID = generateUserID();

    users.push_back(user);
    saveUserDetails(users);
    cout << "\nYour unique user ID are generated >> [ " << user.userID << " ] << \n\n";
}

bool adminLogin() {
    string adminIDEnter, adminPasswEnter;
    const string adminID = "abc123";
    const string adminPassw = "abc123$$d";
    int adminAttempt = 0;

    while (adminAttempt < 3) {
        cout << "\nPlease enter the following information: " << endl;
        cout << "Admin ID  : ";
        cin >> adminIDEnter;
        cout << "Password  : ";
        cin >> adminPasswEnter;
        if (adminIDEnter == adminID && adminPasswEnter == adminPassw) {
            cout << "\n Successfully Logged in....\n\n\n";
            return true;
        } else {
            cout << "\n Invalid admin ID or Password!" << endl;
            adminAttempt += 1;
            cout << " Attempt(s) left : " << 3 - adminAttempt << "/3\n\n" << endl;
        }
    }
    cout << " Reached the permitted number of attempts.Returning to main menu ....\n\n\n";
    return false;
}

bool validateUserIdPassw(vector<User>& users, const string& userIDEnter, const string& userPasswEnter) {
    for (const auto& u : users) {
        if (u.userID == userIDEnter && u.passwd == userPasswEnter)
            return true;
    }
    return false;
}

string userLogin(vector<User>& users) {
    cout << "\n\n------------------------------------------------------------";
    cout << "\n       L o g   I n   A s   E x i s t i n g   U s e r ";
    cout << "\n------------------------------------------------------------";
    int userAttempt = 0;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    while (userAttempt < 3) {
        string userIDEnter = " ";
        string userPasswEnter = " ";
        cout << "\n\nPlease enter your userID and password :" << endl;
        cout << "User ID  : ";
        getline(cin, userIDEnter);
        cout << "Password : ";
        getline(cin, userPasswEnter);
        if (validateUserIdPassw(users, userIDEnter, userPasswEnter)) {
            cout << "\n\nLogin successfully !!\n";
            userAttempt = 3;
            return userIDEnter;
        } else {
            cout << "\n\nLogin unsuccessfully!! Invalid userID or password!\n";
            cout << "Please check again your userID or password!!Thank You!\n";
            userAttempt += 1;
            cout << " Attempt(s) left : " << 3 - userAttempt << "/3 \n";
        }
    }
    return "";
}

void displayUserProfile(vector<User> users, const string& userIDEnter) {
    displayProfileHeader2();
    cout << "-----------------------------------------------------------------------";
    cout << "\n       A C C O U N T   P E R S O N A L   I N F O R M A T I O N";
    cout << "\n-----------------------------------------------------------------------\n";
    bool found = false;
    for (User user : users) {
        if (user.userID == userIDEnter) {
            cout << " User ID          : " << user.userID << endl;
            cout << " Name             : " << user.name << endl;
            cout << " Gender           : " << user.gender << endl;
            cout << " Age              : " << user.age << endl;
            cout << " IC               : " << user.ic << endl;
            cout << " Email            : " << user.email << endl;
            cout << " Contact number   : " << user.contactNum << endl;
            cout << "\n\n=======================================================";
            cout << "\n                  A d d r e s s ";
            cout << "\n=======================================================\n";
            cout << " Street           : " << user.address.street << endl;
            cout << " City             : " << user.address.city << endl;
            cout << " State            : " << user.address.state << endl;
            cout << " Country          : " << user.address.country << endl;
            found = true;
            break;
        }
    }
    if (!found) {
        cout << "User not found!\n";
    }
    cout << "\n\nPress any key to return back...";
    cin.ignore();
    cin.get();
}

/*==============================================================================
   BOOKING MATRIX & BOOKING FLOW
==============================================================================*/
void loadBookingStatus(const string& filename, vector<vector<int>>& bookingStatus, int totUser, int totSessions) {
    bookingStatus.assign(totUser, vector<int>(totSessions, 0));
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "File not found !";
        return;
    }
    for (int i = 0; i < totUser; i++) {
        for (int j = 0; j < totSessions; j++) {
            file >> bookingStatus[i][j];
        }
    }
    file.close();
}

void saveBookingStatus(const string& filename, const vector<vector<int>>& bookingStatus) {
    ofstream file(filename);
    for (const auto& row : bookingStatus) {
        for (size_t j = 0; j < row.size(); j++) {
            file << row[j] << (j + 1 < row.size() ? " " : "");
        }
        file << "\n";
    }
}

bool checkSessionIDExist(const vector<Session>& sessions, int sessionIDbook) {
    for (const auto& session : sessions) {
        if (session.sessionID == sessionIDbook) {
            return true;
        }
    }
    return false;
}

int countNumOfParticipants(const vector<vector<int>>& bookingStatus, int sessionIndex) {
    int count = 0;
    for (const auto& row : bookingStatus) {
        if (sessionIndex >= 0 && sessionIndex < (int)row.size() && row[sessionIndex] == 1) count++;
    }
    return count;
}

int findUserIndex(const vector<User>& users, const string& userIDEnter) {
    for (int i = 0; i < (int)users.size(); i++) {
        if (users[i].userID == userIDEnter) return i;
    }
    return -1;
}

void displayBookingScreen(vector<User>& users, const string& userIDEnter) {
    vector<Session> sessions;
    int nextSessionID = 1;
    const string txtfilename = "sessionsList.txt";

    vector<vector<int>> bookingStatus;
    loadUserDetails(users);
    loadSessionsList(sessions, txtfilename, nextSessionID);
    loadBookingStatus("bookings.txt", bookingStatus, (int)users.size(), (int)sessions.size());
    ensureBookingMatrixSize(bookingStatus, (int)users.size(), (int)sessions.size());

    int userIndex = findUserIndex(users, userIDEnter);
    if (userIndex == -1) {
        cout << "User not found!\n";
        return;
    }

    vector<int> currentBookingMade;
    int addOnYesNo = 1;

    while (addOnYesNo == 1) {
        string checkInput;
        int sessionIDbook = -1;
        bool isValidSessionID = false;

        while (!isValidSessionID) {
            displayUserHeader();
            displaySessionMenu(sessions);

            cout << "\n\n\n----------------------------------------------------------------";
            cout << "\n             B o o k i n g   P l a c e m e n t  ";
            cout << "\n----------------------------------------------------------------";
            cout << "\n **You are able to enter 'q' or 'Q' to exit booking placement** ";
            cout << "\n----------------------------------------------------------------";
            cout << "\n Please enter the session ID to book >> ";
            cin >> checkInput;

            if (checkInput == "q" || checkInput == "Q") {
                cout << "Exiting Booking Placement...\n";
                cin.ignore();
                userScreen(userIDEnter);
                return;
            }

            if (all_of(checkInput.begin(), checkInput.end(), ::isdigit)) {
                sessionIDbook = stoi(checkInput);
                if (sessionIDbook > 0) {
                    isValidSessionID = true;
                } else {
                    cout << "Invalid input. Please enter the valid session ID !!!\n\n";
                }
            } else {
                cout << "Invalid input. Please enter the valid session ID !!!\n\n";
                cin.clear();
                cout << "\n\nPress any key to continue order...";
                cin.ignore();
                cin.get();
            }
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        if (!checkSessionIDExist(sessions, sessionIDbook)) {
            cout << "\nPlease enter the valid session ID that are provided! \n";
            cout << "\n\nPress any key to return back...";
            cin.ignore();
            cin.get();
            continue;
        }

        int sessionIndex = sessionIDbook - 1;
        if (sessionIndex < 0 || sessionIndex >= (int)sessions.size()) {
            cout << "\nInvalid session index!\n";
            cout << "\n\nPress any key to return back...";
            cin.ignore();
            cin.get();
            continue;
        }

        if (bookingStatus[userIndex][sessionIndex] == 1) {
            cout << "\n\nYou have already booked this session!";
            cout << "\n\nPress any key to return back...";
            cin.ignore();
            cin.get();
            continue;
        }
        if (countNumOfParticipants(bookingStatus, sessionIndex) >= 30) {
            cout << "\n\nSo sorry for that, this session is FULL.";
            cout << "\n\nPress any key to return back...";
            cin.ignore();
            cin.get();
            continue;
        }
        int sessionBookedCount = 0;
        for (int booked : bookingStatus[userIndex]) {
            if (booked == 1) sessionBookedCount++;
        }
        if (sessionBookedCount >= 3) {
            cout << "You have reached the maximum of 3 bookings.\n";
            cout << "\n\nPress any key to continue...";
            cin.ignore();
            cin.get();
            break;
        }

        bookingStatus[userIndex][sessionIndex] = 1;
        currentBookingMade.push_back(sessionIndex);
        cout << "\nSession Booked Successfully !!\n";

        for (string& b : users[userIndex].bookedSessions) {
            if (b.empty()) {
                b = sessions[sessionIndex].title; // store session title (compat)
                break;
            }
        }
        for (string& p : sessions[sessionIndex].participants) {
            if (p.empty()) {
                p = userIDEnter;
                break;
            }
        }
        bool isValidInputYN = false;
        while (!isValidInputYN) {
            cout << "\nDo you want to add another booking? (YES[1]/NO[0]): ";
            if (cin >> addOnYesNo) {
                if (addOnYesNo == 1 || addOnYesNo == 0)
                    isValidInputYN= true;
                else
                    cout << "Invalid input. Please make sure the input is integer!! (0/1) !!\n\n";
            } else {
                cout << "Invalid input. Please make sure the input is integer!! (0/1) !!\n\n";
                cin.clear();
            }
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }

    if (!currentBookingMade.empty()) {
        displayBookingSummaryHeader();
        cout << "-----------------------------------------------\n";
        cout << "      S E S S I O N      B O O K E D  \n";

        int bookingCount = 1;
        for (int sessionID : currentBookingMade) {
            cout << "-----------------------------------------------\n";
            cout << "  B O O K I N G  " << bookingCount++ << "  :\n";
            cout << "-----------------------------------------------\n";
            cout << "  Session Name : " << sessions[sessionID].title << "\n";
            cout << "  Session Date : " << sessions[sessionID].date << "\n";
            cout << "  Session Time : " << sessions[sessionID].time << "\n";
        }

        int confirmBooking;
        bool validChoice1 = false;

        while (!validChoice1) {
            cout << "\n\nConfirm your booking >> YES[1] or NO[0]\n";
            cout << "\nEnter your choice > ";
            if (cin >> confirmBooking && (confirmBooking == 1 || confirmBooking == 0))
                validChoice1 = true;
            cout << "\nInvalid input.Please enter 1/0 only\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        int doubleConfirmBooking;
        bool validChoice2= false;

        while (!validChoice2) {
            if (confirmBooking == 1) {
                cout << "\nAre you sure CONFIRM all the booking above (YES[1]/NO[0]) >> ";
            } else {
                cout << "\nAre you sure CANCEL all the booking above (YES[0]/NO[1]) >> ";
            }
            if (cin >> doubleConfirmBooking && (doubleConfirmBooking == 1 || doubleConfirmBooking == 0))
                validChoice2 = true;
            cout << "\nInvalid input.Please enter 1/0 only\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        if (doubleConfirmBooking == 1) {
            saveBookingStatus("bookings.txt", bookingStatus);
            cout << "\nAll bookings confirmed and saved!\n";
        } else {
            for (int sessionID : currentBookingMade) {
                bookingStatus[userIndex][sessionID] = 0;
            }
            cout << "\nAll bookings cancelled!\n";
        }
    }
}

/*==============================================================================
    (ADMIN + USER EXTRAS)
==============================================================================*/
void displayAdminScreen(vector<User>& users) {
    vector<Session> sessions;
    int nextSessionID = 1;
    const string sessionsFile = "sessionsList.txt";

    loadSessionsList(sessions, sessionsFile, nextSessionID);
    loadUserDetails(users);

    vector<vector<int>> bookingStatus;
    loadBookingStatus("bookings.txt", bookingStatus, (int)users.size(), (int)sessions.size());
    ensureBookingMatrixSize(bookingStatus, (int)users.size(), (int)sessions.size());

    while (true) {
        displayAdminHeader();
        cout << "Below are actions available to be taken :\n"
             << "1. View Participant Summary\n"
             << "2. View Session Summary / Booking\n"
             << "3. Filter\n"
             << "4. Edit session\n"
             << "5. View Feedback\n"
             << "6. Exit\n\n";

        int c = getIntInRange("Please select an option (1-6): ", 1, 6);
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (c == 1) {
            viewParticipantSummary(users, sessions, bookingStatus);
        } else if (c == 2) {
            viewSessionSummary(sessions, bookingStatus);
        } else if (c == 3) {
            adminFilterMenu(users, sessions, bookingStatus);
        } else if (c == 4) {
            editSessions(sessions, nextSessionID, sessionsFile, bookingStatus);
            saveBookingStatus("bookings.txt", bookingStatus);
        } else if (c == 5) {
            viewFeedback();
        } else {
            break;
        }
    }
}

void viewParticipantSummary(const vector<User>& users,
                            const vector<Session>& sessions,
                            const vector<vector<int>>& bookingStatus) {
    displayAdminHeader();
    cout << "================== Participant Summary ==================\n\n";
    if (users.empty()) {
        cout << "No registered users yet.\n\n";
    } else if (sessions.empty()) {
        cout << "No sessions available.\n\n";
    } else {
        for (size_t ui = 0; ui < users.size(); ++ui) {
            const User& u = users[ui];
            cout << "USER ID: " << u.userID << " | Name: " << u.name
                 << " | Gender: " << u.gender << " | Age: " << u.age << "\n";
            cout << "  Sessions booked:\n";
            bool any = false;
            if (ui < bookingStatus.size()) {
                for (size_t si = 0; si < sessions.size() && si < bookingStatus[ui].size(); ++si) {
                    if (bookingStatus[ui][si] == 1) {
                        cout << "   - [" << sessions[si].sessionID << "] " << sessions[si].title
                             << " | " << sessions[si].date << " | " << sessions[si].time << "\n";
                        any = true;
                    }
                }
            }
            if (!any) cout << "   (none)\n";
            cout << string(60, '-') << "\n";
        }
    }
    cout << "\nPress ENTER to return...";
    cin.get();
}

void viewSessionSummary(const vector<Session>& sessions,
                        const vector<vector<int>>& bookingStatus) {
    displayAdminHeader();
    cout << "==================== Sessions Summary ====================\n\n";
    if (sessions.empty()) {
        cout << "No sessions available.\n\n";
    } else {
        cout << left << setw(5) << "ID" << setw(40) << "TITLE" << setw(20) << "DATE"
             << setw(15) << "TIME" << setw(12) << "TAKEN" << setw(12) << "AVAIL" << "\n";
        cout << string(110, '-') << "\n";
        for (size_t si = 0; si < sessions.size(); ++si) {
            int taken = countNumOfParticipants(bookingStatus, (int)si);
            int avail = max(0, 30 - taken);
            cout << left << setw(5) << sessions[si].sessionID
                 << setw(40) << (sessions[si].title.size() > 38 ? sessions[si].title.substr(0,35)+"..." : sessions[si].title)
                 << setw(20) << sessions[si].date
                 << setw(15) << sessions[si].time
                 << setw(12) << taken
                 << setw(12) << avail << "\n";
        }
    }
    cout << "\nPress ENTER to return...";
    cin.get();
}

int monthNameToNum(const string& month) {
    static map<string,int> m = {
        {"january",1},{"february",2},{"march",3},{"april",4},
        {"may",5},{"june",6},{"july",7},{"august",8},
        {"september",9},{"october",10},{"november",11},{"december",12}
    };
    string lower = month;
    for (auto &c : lower) c = tolower(c);
    return m[lower];
}
string toLowerStr(string s) {
    for (auto &c : s) c = tolower(c);
    return s;
}

void trim(string &s) {
    while (!s.empty() && isspace(s.front())) s.erase(s.begin());
    while (!s.empty() && isspace(s.back())) s.pop_back();
}

tm parseDate(string dateStr) {
    // Normalize
    trim(dateStr);

    tm t = {};
    string monthStr, dayStr, yearStr;
    stringstream ss(dateStr);
    ss >> monthStr >> dayStr >> yearStr;

    // remove comma from day (e.g., "25,")
    if (!dayStr.empty() && dayStr.back() == ',')
        dayStr.pop_back();

    // lower month for mapping
    int monthNum = monthNameToNum(toLowerStr(monthStr));
    if (monthNum == 0) { // fail safe
        cerr << "[DEBUG] Invalid month: " << monthStr << endl;
        t.tm_mon = 0; t.tm_mday = 1; t.tm_year = 120; // Jan 1, 2020 fallback
        return t;
    }

    t.tm_mday = stoi(dayStr);
    t.tm_mon = monthNum - 1;     // 0-based
    t.tm_year = stoi(yearStr) - 1900; // years since 1900
    return t;
}


time_t toTimeT(const tm& t) {
    tm copy = t;
    return mktime(&copy);
}

void adminFilterMenu(const vector<User>& users,
                     const vector<Session>& sessions,
                     const vector<vector<int>>& bookingStatus) {
    displayAdminHeader();
    cout << "====================== Filter Function ====================\n\n"
         << "Filter Type:\n"
         << "1. Filter participants by gender\n"
         << "2. Filter participants by age >= X\n"
         << "3. Filter sessions by name keyword\n"
         << "4. Filter sessions by date\n"
         << "0. Back\n\n";
    int c = getIntInRange("Enter choice (0-4): ", 0, 4);
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    if (c == 0) return;

    if (c == 1) {
    string g;
    cout << "Enter gender to match (Male/Female): ";
    getline(cin, g);
    for (auto& ch : g) ch = (char)tolower(ch);

    displayAdminHeader();
    cout << "Result of Filter by participant gender (" << g << ")\n";
    cout << string(60, '-') << "\n";

    for (const auto& u : users) {
        string ug = u.gender;
        for (auto& ch : ug) ch = (char)tolower(ch);
        if (ug == g) {   // <-- Fixed to exact match
            cout << "ID: " << u.userID << " | Name: " << u.name << " | Age: " << u.age << "\n";
        }
    }
    cout << "\nPress ENTER to return...";
    cin.get();
}
    else if (c == 2) {
        string ageInput;
        cout << "Enter age or range (e.g., 25 or 20-30): ";
        getline(cin, ageInput);

        // Remove spaces
        ageInput.erase(remove_if(ageInput.begin(), ageInput.end(), ::isspace), ageInput.end());

        int minAge = -1, maxAge = -1;
        size_t dash = ageInput.find('-');

        if (dash != string::npos) {
            // Range input
            minAge = stoi(ageInput.substr(0, dash));
            maxAge = stoi(ageInput.substr(dash + 1));
        } else {
            // Single number input
            minAge = stoi(ageInput);
            maxAge = INT_MAX; // effectively ">= minAge"
        }

        displayAdminHeader();
        cout << "Result of Filter by participant age (" << ageInput << ")\n";
        cout << string(60, '-') << "\n";

        for (const auto& u : users) {
            if (u.age >= minAge && u.age <= maxAge) {
                cout << "ID: " << u.userID
                     << " | Name: " << u.name
                     << " | Age: " << u.age << "\n";
            }
        }

        cout << "\nPress ENTER to return...";
        cin.get();
    }

    else if (c == 3) {
        string key;
        cout << "Enter session name keyword: ";
        getline(cin, key);
        string keyL = key; for (auto& ch : keyL) ch = (char)tolower(ch);
        displayAdminHeader();
        cout << "Result of Filter by session name (\"" << key << "\")\n";
        cout << string(60, '-') << "\n";
        for (const auto& s : sessions) {
            string t = s.title; for (auto& ch : t) ch = (char)tolower(ch);
            if (t.find(keyL) != string::npos) {
                cout << "[" << s.sessionID << "] " << s.title << " | " << s.date << " | " << s.time << "\n";
            }
        }
        cout << "\nPress ENTER to return..."; cin.get();
    }
    else if (c == 4) {
        string dateInput;
        cout << "Enter date or range (e.g., November 25, 2025 OR November 25, 2025 - November 27, 2025): ";
        getline(cin, dateInput);
        trim(dateInput);

        // Split into start and end
        size_t dash = dateInput.find('-');
        string startStr, endStr;
        if (dash != string::npos) {
            startStr = dateInput.substr(0, dash);
            endStr   = dateInput.substr(dash + 1);
            trim(startStr);
            trim(endStr);
        } else {
            startStr = dateInput;
            endStr   = startStr;
        }

        tm startTm = parseDate(startStr);
        tm endTm   = parseDate(endStr);

        time_t startTime = toTimeT(startTm);
        time_t endTime   = toTimeT(endTm);

        displayAdminHeader();
        cout << "Sessions scheduled between " << startStr << " and " << endStr << "\n";
        cout << string(80, '-') << "\n";

        // Load sessions
        vector<Session> sessions;
        int nextSessionID = 1;
        loadSessionsList(sessions, "sessionsList.txt", nextSessionID);

        // Show only sessions whose date matches
        for (const auto& s : sessions) {
            tm d = parseDate(s.date);
            time_t sessionTime = toTimeT(d);

            if (sessionTime >= startTime && sessionTime <= endTime) {
                cout << "[" << s.sessionID << "] " << s.title
                     << " | " << s.date
                     << " | " << s.time << "\n";
            }
        }

        cout << "\nPress ENTER to return...";
        cin.get();
    }



}

void editSessions(vector<Session>& sessions,
                  int& nextSessionID,
                  const string& sessionsFile,
                  vector<vector<int>>& bookingStatus) {
    while (true) {
        displayAdminHeader();
        cout << "======================== Edit Session ======================\n\n"
             << "1. ADD EVENT\n"
             << "2. DELETE EVENT\n"
             << "3. EXIT\n\n";
        int c = getIntInRange("Enter your choice (1-3): ", 1, 3);
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        if (c == 3) break;

        if (c == 1) {
            Session s;
            s.sessionID = nextSessionID++;
            cout << "Title    : "; getline(cin, s.title);
            cout << "Date     : "; getline(cin, s.date);
            cout << "Time     : "; getline(cin, s.time);

            cout << "\nAre you sure to ADD this session?\n";
            cout << "  " << s.title << " | " << s.date << " | " << s.time << "\n";
            if (askYesNo("Confirm")) {
                sessions.push_back(s);
                ensureBookingMatrixSize(bookingStatus, (int)bookingStatus.size(), (int)sessions.size());
                saveSessionsList(sessions, sessionsFile);
                cout << "Session added.\n";
            } else {
                cout << "Cancelled.\n";
            }
            cout << "Press ENTER to continue..."; cin.get();
        }
        else if (c == 2) {
            displayAdminHeader();
            displaySessionMenu(sessions);
            cout << "\nEnter SESSION ID to delete (or 0 to cancel): ";
            string sID; getline(cin, sID);
            if (!all_of(sID.begin(), sID.end(), ::isdigit)) {
                cout << "Invalid ID.\nPress ENTER to continue..."; cin.get(); continue;
            }
            int id = stoi(sID);
            if (id == 0) continue;

            int idx = -1;
            for (int i = 0; i < (int)sessions.size(); ++i)
                if (sessions[i].sessionID == id) { idx = i; break; }
            if (idx < 0) {
                cout << "Session ID not found.\nPress ENTER to continue..."; cin.get(); continue;
            }

            cout << "Delete: [" << sessions[idx].sessionID << "] " << sessions[idx].title
                 << " | " << sessions[idx].date << " | " << sessions[idx].time << "\n";
            if (!askYesNo("Are you sure to DELETE this session")) {
                cout << "Cancelled.\nPress ENTER to continue..."; cin.get(); continue;
            }

            removeBookingColumn(bookingStatus, idx);
            sessions.erase(sessions.begin() + idx);
            saveSessionsList(sessions, sessionsFile);
            cout << "Session deleted.\nPress ENTER to continue..."; cin.get();
        }
    }
}

void viewFeedback() {
    displayAdminHeader();
    cout << "========================== FEEDBACK ========================\n\n";
    ifstream in("feedback.txt");
    if (!in.is_open()) {
        cout << "No feedback found yet.\n\nPress ENTER to return..."; cin.get(); return;
    }
    string line; int count = 0;
    while (getline(in, line)) {
        cout << line << "\n";
        ++count;
    }
    if (count == 0) cout << "No feedback found yet.\n";
    cout << "\nPress ENTER to return..."; cin.get();
}

void writeFeedback(const string& userIDEnter,
                   const vector<User>& users,
                   const vector<Session>& sessions,
                   const vector<vector<int>>& bookingStatus) {
    int ui = findUserIndex(users, userIDEnter);
    if (ui < 0) { cout << "User not found.\n"; return; }

    vector<int> mySessionIdx;
    cout << "\nAvailable events you can comment on:\n";
    if (ui < (int)bookingStatus.size()) {
        int no = 1;
        for (int si = 0; si < (int)sessions.size() && si < (int)bookingStatus[ui].size(); ++si) {
            if (bookingStatus[ui][si] == 1) {
                cout << " " << no << ". [" << sessions[si].sessionID << "] "
                     << sessions[si].title << " | " << sessions[si].date << " | " << sessions[si].time << "\n";
                mySessionIdx.push_back(si);
                ++no;
            }
        }
    }
    if (mySessionIdx.empty()) {
        cout << "(You have no bookings, nothing to comment.)\n";
        cout << "Press ENTER to return..."; cin.ignore(numeric_limits<streamsize>::max(), '\n'); cin.get();
        return;
    }

    int pick = getIntInRange("Please select one to comment (1-" + to_string((int)mySessionIdx.size()) + "): ",
                             1, (int)mySessionIdx.size());
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    int si = mySessionIdx[pick - 1];

    cout << "Please write down your comment:\n> ";
    string comment; getline(cin, comment);

    int rating = getIntInRange("Give a rating 1-5: ", 1, 5);
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (!askYesNo("Submit feedback now")) {
        cout << "Cancelled.\nPress ENTER to return..."; cin.get(); return;
    }

    ofstream out("feedback.txt", ios::app);
    if (!out.is_open()) {
        cerr << "Error: cannot open feedback.txt for writing.\n";
    } else {
        out << "User " << userIDEnter << " -> [" << sessions[si].sessionID << "] "
            << sessions[si].title << " | " << sessions[si].date << " | " << sessions[si].time
            << " | Rating: " << rating << " | Comment: " << comment << "\n";
        cout << "Feedback submitted. Thank you!\n";
    }
    cout << "Press ENTER to return..."; cin.get();
}

void displayUserBookedSessions(const vector<User>& users,
                               const vector<Session>& sessions,
                               const vector<vector<int>>& bookingStatus,
                               const string& userIDEnter) {
    displayProfileHeader2();
    cout << "-------------------------- SESSION(S) BOOKED -------------------------\n";
    int ui = findUserIndex(users, userIDEnter);
    if (ui < 0) { cout << "User not found!\n\nPress ENTER to return..."; cin.ignore(numeric_limits<streamsize>::max(), '\n'); cin.get(); return; }

    bool any = false;
    if (ui < (int)bookingStatus.size()) {
        for (int si = 0; si < (int)sessions.size() && si < (int)bookingStatus[ui].size(); ++si) {
            if (bookingStatus[ui][si] == 1) {
                cout << "  - [" << sessions[si].sessionID << "] " << sessions[si].title
                     << " | " << sessions[si].date << " | " << sessions[si].time << "\n";
                any = true;
            }
        }
    }
    if (!any) cout << "(No sessions booked yet.)\n";
    cout << "\nPress ENTER to return..."; cin.ignore(numeric_limits<streamsize>::max(), '\n'); cin.get();
}

/*==============================================================================
   USER & MAIN SCREENS
==============================================================================*/
void userScreen(const string& userIDEnter) {
    vector<User> users;
    vector<Session> sessions;
    int nextSessionID = 1;
    const string txtfilename = "sessionsList.txt";

    loadSessionsList(sessions, txtfilename, nextSessionID);
    loadUserDetails(users);

    displayUserHeader();
    int choice = -1;
    bool validAction = false;

    while (!validAction) {
        cout << "\nChoose an action to continue :";
        cout << "\n 1. View menu & booking seminar sessions\n 2. View profile \n 3. Filter \n 4. Feedback \n 0. Exit";
        cout << "\n\nEnter your choice (integer)(0 - 4) : ";
        if (cin >> choice) {
            if (choice == 0 || choice == 1 || choice == 2 || choice == 3 || choice == 4) {
                validAction = true;
            } else {
                cout << "Invalid input. Please enter integer only!! (0-4)!\n\n";
            }
        } else {
            cout << "Invalid input. Please enter integer only!! (0-4)!\n\n";
            cin.clear();
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    switch (choice) {
    case 1: {
        cout << "\nDisplaying menu ....";
        displayBookingScreen(users, userIDEnter);
        break;
    }
    case 2: {
        int userChoice;
        cout << "\nDisplaying profile page....";
        displayProfileHeader1();
        bool validOption = false;

        while (!validOption) {
            cout << "Options available : \n 1. Personal information \n 2. Session booked \n 0. Return to previous page...\n";
            cout << "\nEnter your choice (1/2/0):";
            if (cin >> userChoice) {
                if (userChoice == 0 || userChoice == 1 || userChoice == 2)
                    validOption = true;
                else
                    cout << "Invalid input. Please enter integer only!! (0-2)!!\n\n\n";
            } else {
                cout << "Invalid input. Please enter integer only!! (0-2)!!\n\n\n";
                cin.clear();
            }
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        switch (userChoice) {
        case 1: {
            displayUserProfile(users, userIDEnter);
            userScreen(userIDEnter);
            break;
        }
        case 2: {
            vector<Session> sessions2;
            int nextSessionID2 = 1;
            const string txtfilename2 = "sessionsList.txt";
            loadSessionsList(sessions2, txtfilename2, nextSessionID2);
            vector<vector<int>> bookingStatus2;
            loadUserDetails(users);
            loadBookingStatus("bookings.txt", bookingStatus2, (int)users.size(), (int)sessions2.size());
            ensureBookingMatrixSize(bookingStatus2, (int)users.size(), (int)sessions2.size());
            displayUserBookedSessions(users, sessions2, bookingStatus2, userIDEnter);
            userScreen(userIDEnter);
            break;
        }
        case 0: {
            userScreen(userIDEnter);
            break;
        }
        default: {
            cout << "Invalid input! Please try again (0-2)!";
        }
        }
        break;
    }
    case 3: {
        
        break;
    }
    case 4: {
        vector<Session> sessions2;
        int nextSessionID2 = 1;
        const string txtfilename2 = "sessionsList.txt";
        loadSessionsList(sessions2, txtfilename2, nextSessionID2);
        vector<vector<int>> bookingStatus2;
        loadUserDetails(users);
        loadBookingStatus("bookings.txt", bookingStatus2, (int)users.size(), (int)sessions2.size());
        ensureBookingMatrixSize(bookingStatus2, (int)users.size(), (int)sessions2.size());
        writeFeedback(userIDEnter, users, sessions2, bookingStatus2);
        userScreen(userIDEnter);
        break;
    }
    case 0: {
        displayMainMenu(users);
        break;
    }
    }
}

void displayUserMainMenu(vector<User>& users) {
    int userChoice;
    do {
        bool validInt = false;
        while (!validInt) {
            displayUserHeader();
            cout << "Login as USER : ";
            cout << "\n 1. Sign up as new user\n 2. Login as existing user ";
            cout << "\nEnter an integer (1/2 , 0 to return back )  : ";
            if (cin >> userChoice) {
                if (userChoice == 0 || userChoice == 1 || userChoice == 2) {
                    validInt = true;
                } else {
                    cout << "Invalid input. Please enter 0-2 only!!\n\n";
                }
            } else {
                cout << "Invalid input. Please enter 0-2 only!!\n\n";
                cin.clear();
            }
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        switch (userChoice) {
        case 1: {
            cout << "\nMoving to Registration Page...\n\n";
            userRegister(users);
            break;
        }
        case 2: {
            cout << "\nMoving to Log In Page...\n\n";
            string succLoginUserID = userLogin(users);
            if (!succLoginUserID.empty()) {
                userScreen(succLoginUserID);
            } else {
                cout << "Login failed. Please try again.\n";
            }
            break;
        }
        case 0: {
            cout << "\nReturning to previous page...\n\n";
            displayMainMenu(users);
            break;
        }
        default: {
            cout << "\n\nInvalid input .Please enter integer > 1,2,0 !!\n";
        }
        }
    } while (userChoice != 0);
}

void displayMainMenu(vector<User>& users) {
    loadUserDetails(users);
    displayIntroScreen();
    displayMainHeader();
    eventDescription();
    displaySeminarInfo();

    int userChoice;
    do {
        bool validUserType = false;

        while (!validUserType) {
            displayMainHeader();
            cout << "Welcome to the Health Seminar System !" << endl;
            cout << "Please choose an option below to get started. " << endl;
            cout << "\nPlease select an user type :" << endl;
            cout << "1. Login as Admin" << endl;
            cout << "2. Login as User" << endl;
            cout << "0. EXIT" << endl;
            cout << "\nEnter an integer(1 or 2) :";
            if (cin >> userChoice) {
                if (userChoice == 0 || userChoice == 1 || userChoice == 2) {
                    validUserType = true;
                } else {
                    cout << "Invalid input !! Please enter only 0 ~2.\n\n";
                }
            } else {
                cout << "Invalid input !! Please enter integer only (0~2).\n\n ";
                cin.clear();
            }
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        if (userChoice == 1) {
            displayAdminHeader();
            bool successLogIn = adminLogin();
            if (successLogIn) {
                displayAdminHeader();
                displayAdminScreen(users); // Kelvin admin menu
            }
        } else if (userChoice == 2) {
            displayUserMainMenu(users);
        } else if (userChoice == 0) {
            cout << "\n\nThank you for using our System !! Have a nice day !! \n\n";
        } else {
            cout << "\n\nInvalid input !! Please try again ! (0-2)! \n\n";
        }
    } while (userChoice != 0);
}

/*==============================================================================
   ENTRY POINT
==============================================================================*/
int main() {
    vector<Session> sessions;
    vector<User> users;
    int nextSessionID = 1;
    const string txtfilename = "sessionsList.txt";
    loadSessionsList(sessions, txtfilename, nextSessionID);
    loadUserDetails(users);
    displayMainMenu(users);
    return 0;

}

