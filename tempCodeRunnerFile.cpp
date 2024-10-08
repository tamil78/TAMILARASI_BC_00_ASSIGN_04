#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <map>
#include <ctime>
#include <limits>

// Struct to store sale information
struct Sale {
    std::string date;
    int saleID;
    std::string description;
    std::string item;
    int quantity;
    double unitPrice;

    double salesAmount() const {
        return quantity * unitPrice;
    }
};

// Function prototypes
std::vector<Sale> loadSales(const std::string& filename);
void saveSales(const std::string& filename, const std::vector<Sale>& sales);
void displaySales(const std::vector<Sale>& sales);
int validateIntegerInput(const std::string& prompt);
double validateDoubleInput(const std::string& prompt);
void createSale(std::vector<Sale>& sales);
void updateSale(std::vector<Sale>& sales);
void deleteSale(std::vector<Sale>& sales);
void sortAndSaveSales(std::vector<Sale>& sales);
void generateReport(const std::string& reportFilename);

// Function to validate integer input
int validateIntegerInput(const std::string& prompt) {
    int value;
    while (true) {
        std::cout << prompt;
        std::cin >> value;
        if (std::cin.fail() || value < 0) {
            std::cin.clear(); // Clear the error flag
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignore the invalid input
            std::cerr << "Invalid input. Please enter a positive integer.\n";
        } else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignore any extra input
            return value;
        }
    }
}

// Function to validate double input
double validateDoubleInput(const std::string& prompt) {
    double value;
    while (true) {
        std::cout << prompt;
        std::cin >> value;
        if (std::cin.fail() || value < 0.0) {
            std::cin.clear(); // Clear the error flag
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignore the invalid input
            std::cerr << "Invalid input. Please enter a positive number.\n";
        } else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignore any extra input
            return value;
        }
    }
}

// Function to load sales from the CSV file into a vector
std::vector<Sale> loadSales(const std::string& filename) {
    std::vector<Sale> sales;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << ".\n";
        return sales;
    }

    std::string line;
    while (std::getline(file, line)) {
        Sale sale;
        std::stringstream ss(line);
        std::string value;

        std::getline(ss, sale.date, ',');
        std::getline(ss, value, ',');
        sale.saleID = std::stoi(value);
        std::getline(ss, sale.description, ',');
        std::getline(ss, sale.item, ',');
        std::getline(ss, value, ',');
        sale.quantity = std::stoi(value);
        std::getline(ss, value, ',');
        sale.unitPrice = std::stod(value);

        sales.push_back(sale);
    }

    file.close();
    return sales;
}

// Function to save sales to a CSV file
void saveSales(const std::string& filename, const std::vector<Sale>& sales) {
    std::ofstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << " for writing.\n";
        return;
    }

    for (const auto& sale : sales) {
        file << sale.date << "," << sale.saleID << "," << sale.description << "," 
             << sale.item << "," << sale.quantity << "," << sale.unitPrice << "\n";
    }

    file.close();
}

// Function to display sales
void displaySales(const std::vector<Sale>& sales) {
    for (const auto& sale : sales) {
        std::cout << "Date: " << sale.date 
                  << ", Sale ID: " << sale.saleID 
                  << ", Description: " << sale.description 
                  << ", Item: " << sale.item 
                  << ", Quantity: " << sale.quantity 
                  << ", Unit Price: " << sale.unitPrice 
                  << ", Sales Amount: " << sale.salesAmount() 
                  << "\n";
    }
}

// Function to add a new sale
void createSale(std::vector<Sale>& sales) {
    Sale newSale;
    std::cout << "Enter sale date (YYYY-MM-DD): ";
    std::cin >> newSale.date;
    newSale.saleID = validateIntegerInput("Enter sale ID: ");
    std::cout << "Enter description: ";
    std::cin.ignore();
    std::getline(std::cin, newSale.description);
    std::cout << "Enter item name: ";
    std::getline(std::cin, newSale.item);
    newSale.quantity = validateIntegerInput("Enter quantity: ");
    newSale.unitPrice = validateDoubleInput("Enter unit price: ");

    sales.push_back(newSale);
    saveSales("input.csv", sales);
    sortAndSaveSales(sales);

    std::cout << "Sale added successfully!\n";
}

// Function to update an existing sale
void updateSale(std::vector<Sale>& sales) {
    int saleID = validateIntegerInput("Enter the sale ID to update: ");

    for (auto& sale : sales) {
        if (sale.saleID == saleID) {
            std::cout << "Enter new sale date (YYYY-MM-DD): ";
            std::cin >> sale.date;
            std::cout << "Enter new description: ";
            std::cin.ignore();
            std::getline(std::cin, sale.description);
            std::cout << "Enter new item name: ";
            std::getline(std::cin, sale.item);
            sale.quantity = validateIntegerInput("Enter new quantity: ");
            sale.unitPrice = validateDoubleInput("Enter new unit price: ");

            saveSales("input.csv", sales);
            sortAndSaveSales(sales);

            std::cout << "Sale updated successfully!\n";
            return;
        }
    }

    std::cerr << "Error: Sale ID not found.\n";
}

// Function to delete an existing sale
void deleteSale(std::vector<Sale>& sales) {
    int saleID = validateIntegerInput("Enter the sale ID to delete: ");

    auto it = std::remove_if(sales.begin(), sales.end(), [&](const Sale& sale) {
        return sale.saleID == saleID;
    });

    if (it != sales.end()) {
        sales.erase(it, sales.end());
        saveSales("input.csv", sales);
        sortAndSaveSales(sales);

        std::cout << "Sale deleted successfully!\n";
    } else {
        std::cerr << "Error: Sale ID not found.\n";
    }
}

// Function to sort sales by date and save to temp.csv
void sortAndSaveSales(std::vector<Sale>& sales) {
    std::sort(sales.begin(), sales.end(), [](const Sale& a, const Sale& b) {
        return a.date < b.date;
    });
    saveSales("temp.csv", sales);
    std::cout << "Sales sorted by date and saved to temp.csv.\n";
}

// Function to generate a report from temp.csv
void generateReport(const std::string& reportFilename) {
    std::vector<Sale> sales = loadSales("temp.csv");

    std::ofstream report(reportFilename);

    if (!report.is_open()) {
        std::cerr << "Error: Could not open report file " << reportFilename << ".\n";
        return;
    }

    // Get the current date and time
    std::time_t now = std::time(nullptr);
    char dateBuffer[100];
    std::strftime(dateBuffer, sizeof(dateBuffer), "%Y-%m-%d", std::localtime(&now));

    std::map<std::string, double> subtotals;
    double grandTotal = 0;

    report << "Sales Report : Stationary Items sold\n";
    report << "Date of Report : " << dateBuffer << "\n";
    report << "----------------------------------------------------------------------------\n";
    report << std::left
           << std::setw(12) << "Date"
           << std::setw(12) << "Sales ID"
           << std::setw(20) << "Item Name"
           << std::setw(10) << "Quantity"
           << std::setw(10) << "Price"
           << std::setw(15) << "SalesAmount" << "\n";
    report << "----------------------------------------------------------------------------\n";

    for (const auto& sale : sales) {
        report << std::left
               << std::setw(12) << sale.date
               << std::setw(12) << sale.saleID
               << std::setw(20) << sale.item
               << std::setw(10) << sale.quantity
               << std::setw(10) << std::fixed << std::setprecision(2) << sale.unitPrice
               << std::setw(15) << std::fixed << std::setprecision(2) << sale.salesAmount()
               << "\n";

        subtotals[sale.date] += sale.salesAmount();
        grandTotal += sale.salesAmount();
    }

    report << "----------------------------------------------------------------------------\n";
    for (const auto& [date, subtotal] : subtotals) {
        report << "Subtotal for " << date << " is :" << std::setw(10) << std::fixed << std::setprecision(2) << subtotal << "\n";
    }
    report << "----------------------------------------------------------------------------\n";
    report << "Grand Total : " << std::setw(10) << std::fixed << std::setprecision(2) << grandTotal << "\n";
    report << "----------------------------------------------------------------------------\n";

    report.close();
    std::cout << "Report generated successfully in " << reportFilename << "!\n";
}


int main() {
    std::vector<Sale> sales = loadSales("input.csv");

    int choice;
    do {
        std::cout << "\n1. Display Sales\n";
        std::cout << "\n2. Add New Sale\n";
        std::cout << "\n3. Update Sale\n";
        std::cout << "\n4. Delete Sale\n";
        std::cout << "\n5. Generate Report\n";
        std::cout << "\n6. Exit\n";
        std::cout << "Choose an option: ";
        std::cin >> choice;

        switch (choice) {
            case 1:
                displaySales(sales);
                break;
            case 2:
                createSale(sales);
                break;
            case 3:
                updateSale(sales);
                break;
            case 4:
                deleteSale(sales);
                break;
            case 5:
                generateReport("report.txt");
                break;
            case 6:
                std::cout << "Exiting program.\n";
                break;
            default:
                std::cerr << "Invalid choice. Please choose a valid option.\n";
        }
    } while (choice != 6);

    return 0;
}
