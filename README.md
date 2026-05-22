# Assignment 4: String Processing
### Boyer-Moore & Rabin-Karp Algorithms

---

## Files

| File | Description |
|------|-------------|
| `string_processing.cpp` | Main source code |
| `input.txt` | Sample input file (Section 4) |
| `test_cases.txt` | All test cases with output |
| `README.md` | Documentation |

---

## Compile & Run

```bash
g++ -O2 -std=c++17 -o string_processing string_processing.cpp
./string_processing
```

---

## Menu

```
===== STRING PROCESSING MENU =====
1. Load Text File
2. Enter Text Manually
3. Search Using Boyer-Moore
4. Search Using Rabin-Karp
5. Compare Algorithms
6. Exit
==================================
```

---

## Algorithms

### Boyer-Moore
- Compares pattern from **right to left**
- Uses **bad character rule** to skip comparisons
- Time: O(n/m) best case — Space: O(1)

### Rabin-Karp
- Uses **rolling hash** to find matches
- Verifies character by character on hash match
- Time: O(n+m) average case — Space: O(1)

---

## Complexity Analysis

| Algorithm   | Time Complexity     | Space Complexity |
|-------------|---------------------|------------------|
| Boyer-Moore | O(n/m) best case    | O(1)             |
| Rabin-Karp  | O(n+m) average case | O(1)             |

---

## Features

- **Interactive menu** with 6 options
- **Single & multiple pattern search** (comma-separated)
- **Pattern highlighting** — `DATA [STRUCTURE] COURSE`
- **Overlapping match detection**
- **File loading** from external file
- **Statistics** after every search: comparisons, matches, time, algorithm

---

## Test Cases (Section 8)

| Text          | Pattern   | Expected Result             |
|---------------|-----------|-----------------------------|
| AAAAAAAAA     | AAA       | Multiple overlapping matches |
| DATA STRUCTURE| STRUCTURE | Match found                 |
| HELLO WORLD   | TEST      | No match                    |
| MISSISSIPPI   | ISSI      | Multiple matches             |
| ALGORITHM     | RITH      | match                    |

> Full output for all test cases in `test_cases.txt`

---

## Required Functions (Section 6)

```cpp
void loadFile();
void manualInput();
void boyerMooreSearch();
void rabinKarpSearch();
void compareAlgorithms();
void printStatistics(const Statistics &stats);
```
