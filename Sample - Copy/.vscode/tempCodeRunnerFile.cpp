#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <iomanip>
#include <numeric>
#include <stdexcept>
#include <limits>
#include <regex>

using namespace std;

// Function to validate the date format (YYYY-MM-DD)
bool isValidDate(const string& date) {
    const regex pattern(R"(\d{4}-\d{2}-\d{2})");
    return regex_match(date, pattern);
}

// Function to validate that SaleID is an integer
bool isValidSaleID(const string& saleID) {
    return all_of(saleID.begin(), saleID.end(), ::isdigit);
}

void loadCSV(const string& filename, vector<string>& dates, vector<string>& saleIDs, vector<string>& descriptions, 
             vector<string>& items, vector<int>& quantities, vector<double>& unitPrices) {
    ifstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Error: Could not open file " + filename);
    }

    string line, date, saleID, description, item;
    int quantity;
    double unitPrice;

    getline(file, line); 

    while (getline(file, line)) {
        istringstream stream(line);
        if (!getline(stream, date, ',') || !getline(stream, saleID, ',') || !getline(stream, description, ',') ||
            !getline(stream, item, ',') || !(stream >> quantity) || !stream.ignore(1) || !(stream >> unitPrice)) {
            throw runtime_error("Error: Corrupted data format in file " + filename);
        }

        dates.push_back(date);
        saleIDs.push_back(saleID);
        descriptions.push_back(description);
        items.push_back(item);
        quantities.push_back(quantity);
        unitPrices.push_back(unitPrice);
    }

    if (file.bad()) {
        throw runtime_error("Error: Failed while reading the file " + filename);
    }
}

void saveCSV(const string& filename, const vector<string>& dates, const vector<string>& saleIDs, 
             const vector<string>& descriptions, const vector<string>& items, 
             const vector<int>& quantities, const vector<double>& unitPrices) {
    ofstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Error: Could not open file " + filename);
    }

    file << "Date,SaleID,Description,Item,Quantity,TotalPrice" << endl;

    for (size_t i = 0; i < dates.size(); ++i) {
        double totalPrice = quantities[i] * unitPrices[i];
        file << dates[i] << "," << saleIDs[i] << "," << descriptions[i] << "," 
             << items[i] << "," << quantities[i] << "," << totalPrice << endl;
    }

    if (file.bad()) {
        throw runtime_error("Error: Failed while writing to the file " + filename);
    }
}

void generateReport(const string& filename, const vector<string>& dates, const vector<string>& saleIDs, 
                    const vector<string>& items, const vector<int>& quantities, const vector<double>& unitPrices) {
    ofstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Error: Could not open file " + filename);
    }

    file << "Date of Report: " << __DATE__ << endl << endl;
    file << "Sales Report: Stationary Items Sold" << endl;
    file << "--------------------------------------------------------------------------------------------------------------------" << endl;
    file << "Date                  SaleID                Item                 Quantity          Price            SalesAmount" << endl;
    file << "--------------------------------------------------------------------------------------------------------------------" << endl;

    double grandTotal = 0.0;
    unordered_map<string, double> dateSubtotals;
    string currentDate = "";

    for (size_t i = 0; i < dates.size(); ++i) {
        if (dates[i] != currentDate && !currentDate.empty()) {
            file << "--------------------------------------------------------------------------------------------------------------------" << endl;
            file << right << setw(85) << "Subtotal for " << currentDate << " is: " << fixed << setprecision(2) << dateSubtotals[currentDate] << endl;
            file << "--------------------------------------------------------------------------------------------------------------------" << endl;
        }

        currentDate = dates[i];
        double salesAmount = quantities[i] * unitPrices[i];
        file << left << setw(22) << dates[i]
             << setw(23) << saleIDs[i]
             << setw(23) << items[i]
             << setw(18) << quantities[i]
             << setw(18) << fixed << setprecision(2) << unitPrices[i]
             << salesAmount << endl;

                dateSubtotals[dates[i]] += salesAmount;
        grandTotal += salesAmount;
    }

    file << "--------------------------------------------------------------------------------------------------------------------" << endl;
    file << right << setw(85) << "Subtotal for " << currentDate << " is: " << fixed << setprecision(2) << dateSubtotals[currentDate] << endl;
    file << "--------------------------------------------------------------------------------------------------------------------" << endl;

    file << right << setw(85) << "Grand Total: " << fixed << setprecision(2) << grandTotal << endl;

    if (file.bad()) {
        throw runtime_error("Error: Failed while writing the report to the file " + filename);
    }
}

bool fileExists(const string& filename) {
    ifstream file(filename);
    return file.good();
}

void updateRecord(vector<string>& saleIDs, vector<string>& dates, vector<string>& descriptions,
                  vector<string>& items, vector<int>& quantities, vector<double>& unitPrices) {
    string saleID;
    cout << "Enter SaleID of the record to update: ";
    cin >> saleID;

    auto it = find(saleIDs.begin(), saleIDs.end(), saleID);
    if (it == saleIDs.end()) {
        cout << "SaleID not found. Update operation aborted." << endl;
        return;
    }

    size_t index = distance(saleIDs.begin(), it);

    string newDate, newDescription, newItem;
    int newQuantity;
    double newUnitPrice;

    cout << "Enter new Sale Date (YYYY-MM-DD): ";
    cin >> newDate;
    while (!isValidDate(newDate)) {
        cout << "Invalid date format. Please enter the date in YYYY-MM-DD format: ";
        cin >> newDate;
    }

    cout << "Enter new Description: ";
    cin.ignore();
    getline(cin, newDescription);

    cout << "Enter new Item: ";
    cin >> newItem;

    cout << "Enter new Quantity: ";
    while (!(cin >> newQuantity) || newQuantity <= 0) {
        cout << "Invalid quantity. Please enter a positive integer: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    cout << "Enter new Unit Price: ";
    while (!(cin >> newUnitPrice) || newUnitPrice <= 0) {
        cout << "Invalid unit price. Please enter a positive number: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    dates[index] = newDate;
    descriptions[index] = newDescription;
    items[index] = newItem;
    quantities[index] = newQuantity;
    unitPrices[index] = newUnitPrice;

    cout << "Record updated successfully." << endl;
}

void deleteRecord(vector<string>& saleIDs, vector<string>& dates, vector<string>& descriptions,
                  vector<string>& items, vector<int>& quantities, vector<double>& unitPrices) {
    string saleID;
    cout << "Enter SaleID of the record to delete: ";
    cin >> saleID;

    auto it = find(saleIDs.begin(), saleIDs.end(), saleID);
    if (it == saleIDs.end()) {
        cout << "SaleID not found. Delete operation aborted." << endl;
        return;
    }

    size_t index = distance(saleIDs.begin(), it);

    saleIDs.erase(saleIDs.begin() + index);
    dates.erase(dates.begin() + index);
    descriptions.erase(descriptions.begin() + index);
    items.erase(items.begin() + index);
    quantities.erase(quantities.begin() + index);
    unitPrices.erase(unitPrices.begin() + index);

    cout << "Record deleted successfully." << endl;
}

void sortDataByDate(vector<string>& dates, vector<string>& saleIDs, vector<string>& descriptions,
                    vector<string>& items, vector<int>& quantities, vector<double>& unitPrices) {
    vector<size_t> indices(dates.size());
    iota(indices.begin(), indices.end(), 0);

    sort(indices.begin(), indices.end(), [&dates](size_t i1, size_t i2) {
        return dates[i1] < dates[i2];
    });

    vector<string> sortedDates, sortedSaleIDs, sortedDescriptions, sortedItems;
    vector<int> sortedQuantities;
    vector<double> sortedUnitPrices;

    for (size_t i : indices) {
        sortedDates.push_back(dates[i]);
        sortedSaleIDs.push_back(saleIDs[i]);
        sortedDescriptions.push_back(descriptions[i]);
        sortedItems.push_back(items[i]);
        sortedQuantities.push_back(quantities[i]);
        sortedUnitPrices.push_back(unitPrices[i]);
    }

    dates = move(sortedDates);
    saleIDs = move(sortedSaleIDs);
    descriptions = move(sortedDescriptions);
    items = move(sortedItems);
    quantities = move(sortedQuantities);
    unitPrices = move(sortedUnitPrices);
}

int main() {
    try {
        string salesFilename = "sales.csv";
        string tempFilename = "temp.csv";
        string reportFilename = "report.txt";
        
        if (fileExists(salesFilename)) {
            vector<string> dates, saleIDs, descriptions, items;
            vector<int> quantities;
            vector<double> unitPrices;

            loadCSV(salesFilename, dates, saleIDs, descriptions, items, quantities, unitPrices);

            cout << "Choose an operation:" << endl;
            cout << "1. Copy data" << endl;
            cout << "2. Add new data" << endl;
            cout << "3. Update data" << endl;
            cout << "4. Delete data" << endl;
            int choice;
            cin >> choice;

            if (choice == 1) {
                cout << "Data will be copied and stored as sorted by date." << endl;
            } else if (choice == 2) {
                string newDate, newSaleID, newDescription, newItem;
                int newQuantity;
                double newUnitPrice;

                cout << "Enter Sale Date (YYYY-MM-DD): ";
                cin >> newDate;
                while (!isValidDate(newDate)) {
                    cout << "Invalid date format. Please enter the date in YYYY-MM-DD format: ";
                    cin >> newDate;
                }

                cout << "Enter SaleID: ";
                cin >> newSaleID;
                while (!isValidSaleID(newSaleID)) {
                    cout << "Invalid SaleID. Please enter an integer: ";
                    cin >> newSaleID;
                }

                if (find(saleIDs.begin(), saleIDs.end(), newSaleID) != saleIDs.end()) {
                    cout << "SaleID already exists. Aborting operation." << endl;
                    return 1;
                }

                cout << "Enter Description: ";
                cin.ignore(); 
                getline(cin, newDescription);

                cout << "Enter Item: ";
                cin >> newItem;

                cout << "Enter Quantity: ";
                while (!(cin >> newQuantity) || newQuantity <= 0) {
                    cout << "Invalid quantity. Please enter a positive integer: ";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }

                cout << "Enter Unit Price: ";
                while (!(cin >> newUnitPrice) || newUnitPrice <= 0) {
                    cout << "Invalid unit price. Please enter a positive number: ";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }

                dates.push_back(newDate);
                saleIDs.push_back(newSaleID);
                descriptions.push_back(newDescription);
                items.push_back(newItem);
                quantities.push_back(newQuantity);
                unitPrices.push_back(newUnitPrice);
            } else if (choice == 3) {
                updateRecord(saleIDs, dates, descriptions, items, quantities, unitPrices);
            } else if (choice == 4) {
                deleteRecord(saleIDs, dates, descriptions, items, quantities, unitPrices);
            } else {
                cout << "Invalid choice. Exiting..." << endl;
                return 1;
            }

            saveCSV(salesFilename, dates, saleIDs, descriptions, items, quantities, unitPrices);
            
            sortDataByDate(dates, saleIDs, descriptions, items, quantities, unitPrices);

            saveCSV(tempFilename, dates, saleIDs, descriptions, items, quantities, unitPrices);
            
            generateReport(reportFilename, dates, saleIDs, items, quantities, unitPrices);
        } else {
            cout << "File " << salesFilename << " does not exist. Please create the file first." << endl;
            return 1;
        }
    } catch (const exception& ex) {
        cerr << ex.what() << endl;
        return 1;
    }

    return 0;
}

