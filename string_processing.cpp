#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>
#include <limits>
#include <algorithm>

using namespace std;
using namespace std::chrono;

// ─── Global Text ──────────────────────────────────────────────────────────────
string textData = "";

// ─── Statistics Struct ────────────────────────────────────────────────────────
struct Statistics {
    int    comparisons = 0;
    int    matches = 0;
    double executionTime = 0.0;
    string algorithm = "";
};

// ─── Function Declarations ────────────────────────────────────────────────────
void loadFile();
void manualInput();
void boyerMooreSearch();
void rabinKarpSearch();
void compareAlgorithms();
void printStatistics(const Statistics& stats);
void highlightMatches(const string& text, const string& pattern, const vector<int>& matches);

vector<int> boyerMoore(const string& text, const string& pattern, Statistics& stats);
vector<int> rabinKarp(const string& text, const string& pattern, Statistics& stats);

// ─── Main ─────────────────────────────────────────────────────────────────────
int main() {

    int choice;

    do {
        cout << "\n===== STRING PROCESSING MENU =====\n";
        cout << "1. Load Text File\n";
        cout << "2. Enter Text Manually\n";
        cout << "3. Search Using Boyer-Moore\n";
        cout << "4. Search Using Rabin-Karp\n";
        cout << "5. Compare Algorithms\n";
        cout << "6. Exit\n";
        cout << "==================================\n";
        cout << "Enter choice: ";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
        case 1: loadFile();           break;
        case 2: manualInput();        break;
        case 3: boyerMooreSearch();   break;
        case 4: rabinKarpSearch();    break;
        case 5: compareAlgorithms();  break;
        case 6: cout << "Exiting...\n"; break;
        default: cout << "Invalid choice.\n";
        }

    } while (choice != 6);

    return 0;
}

// ─── Load File ────────────────────────────────────────────────────────────────
void loadFile() {

    string filename;
    cout << "Enter file name: ";
    getline(cin, filename);

    ifstream file(filename);
    if (!file) {
        cout << "Error: Cannot open file.\n";
        return;
    }

    textData = "";
    string line;
    while (getline(file, line))
        textData += line + "\n";

    file.close();
    cout << "File loaded successfully. (" << textData.size() << " characters)\n";
}

// ─── Manual Input ─────────────────────────────────────────────────────────────
void manualInput() {

    cout << "Enter text:\n> ";
    getline(cin, textData);
    cout << "Text stored successfully.\n";
}

// ─── Highlight Matches ────────────────────────────────────────────────────────
void highlightMatches(const string& text, const string& pattern, const vector<int>& matches) {

    if (matches.empty()) {
        cout << "No matches to highlight.\n";
        return;
    }

    int n = (int)text.length();
    int m = (int)pattern.length();

    vector<bool> marked(n, false);
    for (int pos : matches)
        for (int j = 0; j < m && pos + j < n; j++)
            marked[pos + j] = true;

    bool inside = false;
    for (int i = 0; i < n; i++) {
        if (marked[i] && !inside) { cout << "["; inside = true; }
        if (!marked[i] && inside) { cout << "]"; inside = false; }
        cout << text[i];
    }
    if (inside) cout << "]";
    cout << "\n";
}

// ─── Boyer-Moore Algorithm ────────────────────────────────────────────────────
//
//  Uses the bad-character heuristic.
//  Compares right-to-left; on mismatch shifts by max(1, j - badChar[text[s+j]]).
//  shift += 1 on match so overlapping occurrences are detected.
//
vector<int> boyerMoore(const string& text, const string& pattern, Statistics& stats) {

    vector<int> matches;
    int n = (int)text.length();
    int m = (int)pattern.length();

    if (m == 0 || n == 0 || m > n)
        return matches;

    // Build bad-character table
    vector<int> badChar(256, -1);
    for (int i = 0; i < m; i++)
        badChar[(unsigned char)pattern[i]] = i;

    int shift = 0;
    while (shift <= n - m) {

        int j = m - 1;

        // Compare right to left
        while (j >= 0) {
            stats.comparisons++;
            if (pattern[j] == text[shift + j]) j--;
            else break;
        }

        if (j < 0) {
            // Full match found
            matches.push_back(shift);
            stats.matches++;
            shift += 1;  // advance by 1 to allow overlapping matches
        }
        else {
            int bc = badChar[(unsigned char)text[shift + j]];
            shift += max(1, j - bc);
        }
    }

    return matches;
}

// ─── Rabin-Karp Algorithm ─────────────────────────────────────────────────────
//
//  Uses a rolling polynomial hash (base 256, modulus 101).
//  On a hash match, verifies character-by-character.
//  Advances by 1 each step so overlapping occurrences are detected.
//
vector<int> rabinKarp(const string& text, const string& pattern, Statistics& stats) {

    vector<int> matches;
    int n = (int)text.length();
    int m = (int)pattern.length();

    if (m == 0 || n == 0 || m > n)
        return matches;

    const int d = 256;   // alphabet size
    const int q = 101;   // a prime modulus

    // h = d^(m-1) % q  (used to drop the leading digit)
    long long h = 1;
    for (int i = 0; i < m - 1; i++)
        h = (h * d) % q;

    long long patHash = 0, winHash = 0;
    for (int i = 0; i < m; i++) {
        patHash = (d * patHash + (unsigned char)pattern[i]) % q;
        winHash = (d * winHash + (unsigned char)text[i]) % q;
    }

    for (int i = 0; i <= n - m; i++) {

        stats.comparisons++;  // hash comparison

        if (patHash == winHash) {
            // Verify character by character to rule out spurious hits
            bool match = true;
            for (int j = 0; j < m; j++) {
                stats.comparisons++;
                if (text[i + j] != pattern[j]) { match = false; break; }
            }
            if (match) {
                matches.push_back(i);
                stats.matches++;
            }
        }

        // Roll the hash: remove text[i], add text[i+m]
        if (i < n - m) {
            winHash = (d * (winHash - (unsigned char)text[i] * h) + (unsigned char)text[i + m]) % q;
            if (winHash < 0) winHash += q;
        }
    }

    return matches;
}

// ─── Print Statistics ─────────────────────────────────────────────────────────
void printStatistics(const Statistics& stats) {

    cout << "\n========== Statistics ==========\n";
    cout << "Algorithm      : " << stats.algorithm << "\n";
    cout << "Comparisons    : " << stats.comparisons << "\n";
    cout << "Matches        : " << stats.matches << "\n";
    cout << fixed << setprecision(4);
    cout << "Execution Time : " << stats.executionTime << " ms\n";
    cout << "================================\n";
}

// ─── Helper: parse comma-separated patterns ───────────────────────────────────
static vector<string> splitPatterns(const string& input) {

    vector<string> patterns;
    string cur;

    for (char c : input) {
        if (c == ',') {
            // trim leading/trailing spaces
            while (!cur.empty() && cur.front() == ' ') cur.erase(cur.begin());
            while (!cur.empty() && cur.back() == ' ') cur.pop_back();
            if (!cur.empty()) patterns.push_back(cur);
            cur.clear();
        }
        else {
            cur += c;
        }
    }
    while (!cur.empty() && cur.front() == ' ') cur.erase(cur.begin());
    while (!cur.empty() && cur.back() == ' ') cur.pop_back();
    if (!cur.empty()) patterns.push_back(cur);

    return patterns;
}

// ─── Boyer-Moore Search (menu handler) ───────────────────────────────────────
void boyerMooreSearch() {

    if (textData.empty()) {
        cout << "No text loaded. Please load a file or enter text first.\n";
        return;
    }

    cout << "Enter pattern (or multiple patterns separated by commas): ";
    string input;
    getline(cin, input);

    vector<string> patterns = splitPatterns(input);
    if (patterns.empty()) { cout << "No valid pattern entered.\n"; return; }

    for (const string& pattern : patterns) {

        Statistics stats;
        stats.algorithm = "Boyer-Moore";

        auto t0 = high_resolution_clock::now();
        vector<int> matches = boyerMoore(textData, pattern, stats);
        auto t1 = high_resolution_clock::now();
        stats.executionTime = duration<double, milli>(t1 - t0).count();

        cout << "\n--- Pattern: \"" << pattern << "\" ---\n";
        if (matches.empty()) {
            cout << "No matches found.\n";
        }
        else {
            for (int pos : matches)
                cout << "Match at index: " << pos << "\n";
            cout << "\nHighlighted:\n";
            highlightMatches(textData, pattern, matches);
        }
        printStatistics(stats);
    }
}

// ─── Rabin-Karp Search (menu handler) ────────────────────────────────────────
void rabinKarpSearch() {

    if (textData.empty()) {
        cout << "No text loaded. Please load a file or enter text first.\n";
        return;
    }

    cout << "Enter pattern (or multiple patterns separated by commas): ";
    string input;
    getline(cin, input);

    vector<string> patterns = splitPatterns(input);
    if (patterns.empty()) { cout << "No valid pattern entered.\n"; return; }

    for (const string& pattern : patterns) {

        Statistics stats;
        stats.algorithm = "Rabin-Karp";

        auto t0 = high_resolution_clock::now();
        vector<int> matches = rabinKarp(textData, pattern, stats);
        auto t1 = high_resolution_clock::now();
        stats.executionTime = duration<double, milli>(t1 - t0).count();

        cout << "\n--- Pattern: \"" << pattern << "\" ---\n";
        if (matches.empty()) {
            cout << "No matches found.\n";
        }
        else {
            for (int pos : matches)
                cout << "Match at index: " << pos << "\n";
            cout << "\nHighlighted:\n";
            highlightMatches(textData, pattern, matches);
        }
        printStatistics(stats);
    }
}

// ─── Compare Algorithms (menu handler) ───────────────────────────────────────
void compareAlgorithms() {

    if (textData.empty()) {
        cout << "No text loaded. Please load a file or enter text first.\n";
        return;
    }

    string pattern;
    cout << "Enter pattern: ";
    getline(cin, pattern);

    if (pattern.empty()) { cout << "No pattern entered.\n"; return; }

    // ── Boyer-Moore ──────────────────────────────────────────────────────────
    Statistics bmStats;
    bmStats.algorithm = "Boyer-Moore";
    auto s1 = high_resolution_clock::now();
    vector<int> bmMatches = boyerMoore(textData, pattern, bmStats);
    auto e1 = high_resolution_clock::now();
    bmStats.executionTime = duration<double, milli>(e1 - s1).count();

    // ── Rabin-Karp ───────────────────────────────────────────────────────────
    Statistics rkStats;
    rkStats.algorithm = "Rabin-Karp";
    auto s2 = high_resolution_clock::now();
    vector<int> rkMatches = rabinKarp(textData, pattern, rkStats);
    auto e2 = high_resolution_clock::now();
    rkStats.executionTime = duration<double, milli>(e2 - s2).count();

    // ── Print matches ────────────────────────────────────────────────────────
    cout << "\n--- Pattern: \"" << pattern << "\" ---\n";
    if (bmMatches.empty()) {
        cout << "No matches found.\n";
    }
    else {
        for (int pos : bmMatches)
            cout << "Match at index: " << pos << "\n";
        cout << "\nHighlighted:\n";
        highlightMatches(textData, pattern, bmMatches);
    }

    // ── Side-by-side comparison table ────────────────────────────────────────
    cout << "\n======== ALGORITHM COMPARISON ========\n";
    cout << left
        << setw(20) << "Metric"
        << setw(15) << "Boyer-Moore"
        << setw(15) << "Rabin-Karp" << "\n";
    cout << string(50, '-') << "\n";
    cout << setw(20) << "Comparisons"
        << setw(15) << bmStats.comparisons
        << setw(15) << rkStats.comparisons << "\n";
    cout << setw(20) << "Matches"
        << setw(15) << bmStats.matches
        << setw(15) << rkStats.matches << "\n";
    cout << fixed << setprecision(4);
    cout << setw(20) << "Time (ms)"
        << setw(15) << bmStats.executionTime
        << setw(15) << rkStats.executionTime << "\n";
    cout << string(50, '-') << "\n";
    cout << "Fewer Comparisons : "
        << (bmStats.comparisons <= rkStats.comparisons ? "Boyer-Moore" : "Rabin-Karp") << "\n";
    cout << "Faster Execution  : "
        << (bmStats.executionTime <= rkStats.executionTime ? "Boyer-Moore" : "Rabin-Karp") << "\n";
    cout << "======================================\n";
}