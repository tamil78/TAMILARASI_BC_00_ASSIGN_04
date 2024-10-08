include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip> // For std::setw
#include <ctime>   // For time-related functions
 
using namespace std;
 
// Define a structure to hold each record
struct SaleRecord {
    string date;
    string salesid;
    string description;
    string item;
    int quantity;       // Change quantity to int
    int unit_price;     // Change unit_price to int
};
 
// Helper function to validate a date in DD-MM-YYYY format
bool isValidDate(const string& dateStr) {
    if (dateStr.size() != 10 || dateStr[2] != '-' || dateStr[5] != '-') {
        return false;
    }
    for (size_t hypenCheck = 0; hypenCheck < dateStr.size(); ++hypenCheck) {
        if (hypenCheck == 2 || hypenCheck == 5)
        continue;
        if (!isdigit(dateStr[hypenCheck]))
        return false;
    }
    return true;
}
 
// Helper function to convert a date from DD-MM-YYYY to YYYYMMDD for sorting
string sortDate(const string& dateStr) {
    if (dateStr.size() != 10 || dateStr[2] != '-' || dateStr[5] != '-') {
        return ""; // Invalid date format
    }
    return dateStr.substr(6, 4) + dateStr.substr(3, 2) + dateStr.substr(0, 2);
}
 
// Helper function to compare two dates
bool compareDate(const SaleRecord& a, const SaleRecord& b) {
return sortDate(a.date) < sortDate(b.date);
}
 
// Function to read all records from the sales.csv file
vector<SaleRecord> readRecords() {
    ifstream file("salesnp.csv");
    vector<SaleRecord> records;
    string line;
 
    while (getline(file, line)) {
        stringstream ss(line);
        SaleRecord record;
        
getline(ss, record.date, ',');
if (!isValidDate(record.date)) {
            continue; // Skip invalid date records
        }
        getline(ss, record.salesid, ',');
        getline(ss, record.description, ',');
        getline(ss, record.item, ',');
        
        string quantityStr, unitPriceStr;
        getline(ss, quantityStr, ',');
        getline(ss, unitPriceStr, ',');
 
        // Convert quantity and unit_price to integers
        try {
            record.quantity = stoi(quantityStr);
        } catch (const invalid_argument&) {
            record.quantity = 0;
        }
 
        try {
            record.unit_price = stoi(unitPriceStr);
        } catch (const invalid_argument&) {
            record.unit_price = 0;
        }
 
        records.push_back(record);
    }
    file.close();
    return records;
}
 
//remove
 
string getCurrentDate() {
    time_t t = time(nullptr);
    tm* tmPtr = localtime(&t);
    char buffer[11];
    strftime(buffer, sizeof(buffer), "%d-%m-%Y", tmPtr);
    return string(buffer);
}
 
 
 
// Function to copy the sorted data from tempnp.csv to np.txt
void copyToTextFile(const vector<SaleRecord>& records) {
    ofstream txtFile("np.txt");
 
    //remove
    string currentDate = getCurrentDate();
 
    // Adding header
    txtFile << "Date of Report: " << currentDate << endl;
    txtFile << "Sales Report : Stationary Items Sold" << endl;
    txtFile << "---------------------------------------" << endl;
    txtFile << setw(10) << "Date"
            << setw(10) << "SaleID"
            << setw(15) << "ItemName"
            << setw(10) << "Quantity"
            << setw(10) << "Price"
            << setw(15) << "SalesAmount" << endl;
    txtFile << "---------------------------------------" << endl;
 
    int currentSubtotal = 0;
    int grandTotal = 0;
 
    for (const auto& record : records) {
if (record.date != currentDate) {
            if (!currentDate.empty()) {
                txtFile << "---------------------------------------" << endl;
                txtFile << "SubTotal: " << currentSubtotal << endl;
                txtFile << "---------------------------------------" << endl;
            }
currentDate = record.date;
            currentSubtotal = 0;
        }
        int salesAmount = record.quantity * record.unit_price;
        currentSubtotal += salesAmount;
        grandTotal += salesAmount;
 
txtFile << setw(10) << record.date
                << setw(10) << record.salesid
                << setw(15) << record.item
                << setw(10) << record.quantity
                << setw(10) << record.unit_price
                << setw(15) << salesAmount << endl;
    }
 
    txtFile << "---------------------------------------" << endl;
    txtFile << "SubTotal: " << currentSubtotal << endl;
    txtFile << "---------------------------------------" << endl;
    txtFile << "Grand Total: " << grandTotal << endl;
 
    txtFile.close();
}
 
// Function to write all records to the temp.csv file
void writeRecords(const vector<SaleRecord>& records) {
    ofstream file("tempnp.csv");
    file << "Date,SalesID,Description,Item,Quantity,Unit Price" << endl;
    
    for (const auto& record : records) {
file << record.date << ","
             << record.salesid << ","
             << record.description << ","
             << record.item << ","
             << record.quantity << ","
             << record.unit_price << endl;
    }
    file.close();
 
    // Call the function to copy to np.txt after writing to temp.csv
    copyToTextFile(records);
}
 
// Function to sort records by date and save them to temp.csv
void sortRecordsByDate() {
    vector<SaleRecord> records = readRecords();
 
    // Sort the records by date
    sort(records.begin(), records.end(), compareDate);
 
    // Write sorted records to temp.csv
    writeRecords(records);
}
 
// Function to display records based on SalesID
void readSale() {
    // Read all records and sort by date
    sortRecordsByDate();
    
    // Ask for SalesID to filter
    string salesid;
    cout << "Enter SalesID to read records: ";
    cin >> salesid;
    cin.ignore(); // clear newline left in the input buffer
 
    vector<SaleRecord> records = readRecords();
    bool found = false;
 
    cout << "Date,SalesID,Description,Item,Quantity,Unit Price" << endl;
    for (const auto& record : records) {
        if (record.salesid == salesid) {
            found = true;
cout << record.date << ","
                 << record.salesid << ","
                 << record.description << ","
                 << record.item << ","
                 << record.quantity << ","
                 << record.unit_price << endl;
        }
    }
 
    if (!found) {
        cout << "No records found for SalesID: " << salesid << endl;
    }
}
 
 
// Function to add a new record to the sales.csv file
void createSale() {
    ofstream file("salesnp.csv", ios::app);
    string date, salesid, description, item;
    int quantity, unit_price;
 
    cout << "Enter Date (DD-MM-YYYY): ";
    cin >> date;
    while (!isValidDate(date)) {
        cout << "Invalid date format! Enter Date (DD-MM-YYYY): ";
        cin >> date;
    }
 
    cout << "Enter SalesID: ";
    cin >> salesid;
    cin.ignore(); // clear newline left in the input buffer
    cout << "Enter Description: ";
    getline(cin, description);
    cout << "Enter Item: ";
    getline(cin, item); // Use getline to handle multi-word items
    cout << "Enter Quantity: ";
    cin >> quantity;
    cout << "Enter Unit Price: ";
    cin >> unit_price;
 
    file << date << "," << salesid << "," << description << "," << item << "," << quantity << "," << unit_price << endl;
    file.close();
 
    sortRecordsByDate(); // Sort the records after adding
    cout << "Record added successfully!" << endl;
}
 
// Function to update a record in the sales.csv file
void updateSale() {
    ifstream file("salesnp.csv");
    ofstream tempFile("tempnp.csv");
    string line, salesid, date, description, item;
    int quantity, unit_price;
 
    cout << "Enter SalesID of the record you want to update: ";
    cin >> salesid;
    cin.ignore(); // clear newline left in the input buffer
 
    bool found = false;
    while (getline(file, line)) {
        stringstream ss(line);
        string id;
        getline(ss, date, ',');
        getline(ss, id, ',');
 
        if (id == salesid) {
            found = true;
            cout << "Enter new Date (DD-MM-YYYY): ";
            cin >> date;
            while (!isValidDate(date)) {
                cout << "Invalid date format! Enter Date (DD-MM-YYYY): ";
                cin >> date;
            }
            cin.ignore();
            cout << "Enter new Description: ";
            getline(cin, description);
            cout << "Enter new Item: ";
            getline(cin, item);
            cout << "Enter new Quantity: ";
            cin >> quantity;
            cout << "Enter new Unit Price: ";
            cin >> unit_price;
 
            tempFile << date << "," << salesid << "," << description << "," << item << "," << quantity << "," << unit_price << endl;
        } else {
            tempFile << line << endl;
        }
    }
 
    file.close();
    tempFile.close();
 
    remove("salesnp.csv");
    rename("tempnp.csv", "salesnp.csv");
 
    sortRecordsByDate(); // Sort the records after updating
 
    if (found)
        cout << "Record updated successfully!" << endl;
    else
        cout << "Record not found!" << endl;
}
 
// Function to delete a record from the sales.csv file
void deleteSale() {
    ifstream file("salesnp.csv");
    ofstream tempFile("tempnp.csv");
    string line, salesid;
 
    cout << "Enter SalesID of the record you want to delete: ";
    cin >> salesid;
    cin.ignore(); // clear newline left in the input buffer
 
    bool found = false;
    while (getline(file, line)) {
        stringstream ss(line);
        string id;
        getline(ss, id, ',');
 
        if (id == salesid) {
            found = true;
            cout << "Record deleted successfully!" << endl;
        } else {
            tempFile << line << endl;
        }
    }
 
    file.close();
    tempFile.close();
 
    remove("salesnp.csv");
    rename("tempnp.csv", "salesnp.csv");
 
    sortRecordsByDate(); // Sort the records after deleting
 
    if (!found)
        cout << "Record not found!" << endl;
}
 
int main() {
    int option;
 
    do {
        
        cout << "1. Create Sale\n";
        cout << "2. Read Sales\n";
        cout << "3. Update Sale\n";
        cout << "4. Delete Sale\n";
        cout << "5. Exit\n";
        cout << "Enter your choice: ";
        cin >> option;
 
        switch (option) {
            case 1:
                createSale();
                break;
            case 2:
                readSale();
                break;
            case 3:
                updateSale();
                break;
            case 4:
                deleteSale();
                break;
            case 5:
                cout << "Exit\n";
                break;
            default:
                cout << "Invalid choice! Please try again.\n";
        }
    } while (option != 5);
 
    return 0;
}
 